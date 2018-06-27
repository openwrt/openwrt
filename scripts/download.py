#!/usr/bin/env python
#
# Copyright (c) 2018 Yousong Zhou <yszhou4tech@gmail.com>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

import argparse
import calendar
import datetime
import errno
import fcntl
import json
import os
import os.path
import re
import shutil
import ssl
import subprocess
import sys
import time
import urllib2

TMPDIR = os.environ.get('TMP_DIR') or '/tmp'
TMPDIR_DL = os.path.join(TMPDIR, 'dl')
DOWNLOAD_METHODS = []

class PathException(Exception): pass
class DownloadException(Exception): pass


class Path(object):
    """Context class for preparing and cleaning up directories.

    If ``path`` ``isdir``, then it will be created on context enter.

    If ``keep`` is True, then ``path`` will NOT be removed on context exit
    """

    def __init__(self, path, isdir=True, keep=False):
        self.path = path
        self.isdir = isdir
        self.keep = keep

    def __enter__(self):
        if self.isdir:
            self.mkdir_all(self.path)
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        if not self.keep:
            self.rm_all(self.path)

    @staticmethod
    def mkdir_all(path):
        """Same as mkdir -p."""
        names = os.path.split(path)
        p = ''
        for name in names:
            p = os.path.join(p, name)
            Path._mkdir(p)

    @staticmethod
    def _rmdir_all(dir_):
        names = Path._listdir(dir_)
        for name in names:
            p = os.path.join(dir_, name)
            if os.path.isdir(p):
                Path._rmdir_all(p)
            else:
                Path._remove(p)
        Path._rmdir(dir_)

    @staticmethod
    def _mkdir(path):
        Path._os_func(os.mkdir, path, errno.EEXIST)

    @staticmethod
    def _rmdir(path):
        Path._os_func(os.rmdir, path, errno.ENOENT)

    @staticmethod
    def _remove(path):
        Path._os_func(os.remove, path, errno.ENOENT)

    @staticmethod
    def _listdir(path):
        return Path._os_func(os.listdir, path, errno.ENOENT, default=[])

    @staticmethod
    def _os_func(func, path, errno, default=None):
        """Call func(path) in an idempotent way.

        On exception ``ex``, if the type is OSError and ``ex.errno == errno``,
        return ``default``, otherwise, re-raise
        """
        try:
            return func(path)
        except OSError as e:
            if e.errno == errno:
                return default
            else:
                raise

    @staticmethod
    def rm_all(path):
        """Same as rm -r."""
        if os.path.isdir(path):
            Path._rmdir_all(path)
        else:
            Path._remove(path)

    @staticmethod
    def untar(path, into=None):
        """Extract tarball at ``path`` into subdir ``into``.

        return subdir name if and only if there exists one, otherwise raise PathException
        """
        args = ('tar', '-C', into, '-xzf', path, '--no-same-permissions')
        subprocess.check_call(args, preexec_fn=lambda: os.umask(0o22))
        dirs = os.listdir(into)
        if len(dirs) == 1:
            return dirs[0]
        else:
            raise PathException('untar %s: expecting a single subdir, got %s' % (path, dirs))

    @staticmethod
    def tar(path, subdir, into=None, ts=None):
        """Pack ``path`` into tarball ``into``."""
        # --sort=name requires a recent build of GNU tar
        args = ['tar', '--numeric-owner', '--owner=0', '--group=0', '--sort=name']
        args += ['-C', path, '-cf', into, subdir]
        envs = os.environ.copy()
        if ts is not None:
            args.append('--mtime=@%d' % ts)
        if into.endswith('.xz'):
            envs['XZ_OPT'] = '-7e'
            args.append('-J')
        elif into.endswith('.bz2'):
            args.append('-j')
        elif into.endswith('.gz'):
            args.append('-z')
            envs['GZIP'] = '-n'
        else:
            raise PathException('unknown compression type %s' % into)
        subprocess.check_call(args, env=envs)


class GitHubCommitTsCache(object):
    __cachef = 'github.commit.ts.cache'
    __cachen = 2048

    def __init__(self):
        Path.mkdir_all(TMPDIR_DL)
        self.cachef = os.path.join(TMPDIR_DL, self.__cachef)
        self.cache = {}

    def get(self, k):
        """Get timestamp with key ``k``."""
        fileno = os.open(self.cachef, os.O_RDONLY | os.O_CREAT)
        with os.fdopen(fileno) as fin:
            try:
                fcntl.lockf(fileno, fcntl.LOCK_SH)
                self._cache_init(fin)
                if k in self.cache:
                    ts = self.cache[k][0]
                    return ts
            finally:
                fcntl.lockf(fileno, fcntl.LOCK_UN)
        return None

    def set(self, k, v):
        """Update timestamp with ``k``."""
        fileno = os.open(self.cachef, os.O_RDWR | os.O_CREAT)
        with os.fdopen(fileno, 'wb+') as f:
            try:
                fcntl.lockf(fileno, fcntl.LOCK_EX)
                self._cache_init(f)
                self.cache[k] = (v, int(time.time()))
                self._cache_flush(f)
            finally:
                fcntl.lockf(fileno, fcntl.LOCK_UN)

    def _cache_init(self, fin):
        for line in fin:
            k, ts, updated = line.split()
            ts = int(ts)
            updated = int(updated)
            self.cache[k] = (ts, updated)

    def _cache_flush(self, fout):
        cache = sorted(self.cache.iteritems(), cmp=lambda a, b: b[1][1] - a[1][1])
        cache = cache[:self.__cachen]
        self.cache = {}
        os.ftruncate(fout.fileno(), 0)
        fout.seek(0, os.SEEK_SET)
        for k, ent in cache:
            ts = ent[0]
            updated = ent[1]
            line = '{0} {1} {2}\n'.format(k, ts, updated)
            fout.write(line)


class DownloadMethod(object):
    """Base class of all download method."""

    def __init__(self, args):
        self.args = args
        self.urls = args.urls
        self.url = self.urls[0]
        self.dl_dir = args.dl_dir

    @classmethod
    def resolve(cls, args):
        """Resolve download method to use.

        return instance of subclass of DownloadMethod
        """
        for c in DOWNLOAD_METHODS:
            if c.match(args):
                return c(args)

    @staticmethod
    def match(args):
        """Return True if it can do the download."""
        return NotImplemented

    def download(self):
        """Do the download and put it into the download dir."""
        return NotImplemented


class DownloadMethodGitHubTarball(DownloadMethod):
    """Download and repack archive tarabll from GitHub."""

    __repo_url_regex = re.compile(r'^(?:https|git)://github.com/(?P<owner>[^/]+)/(?P<repo>[^/]+)')

    def __init__(self, args):
        super(DownloadMethodGitHubTarball, self).__init__(args)
        self._init_owner_repo()
        self.version = args.version
        self.subdir = args.subdir
        self.source = args.source
        self.commit_ts = None           # lazy load commit timestamp
        self.commit_ts_cache = GitHubCommitTsCache()
        self.name = 'github-tarball'

    @staticmethod
    def match(args):
        """Match if it's a GitHub clone url."""
        url = args.urls[0]
        proto = args.proto
        if proto == 'git' and isinstance(url, basestring) \
                and (url.startswith('https://github.com/') or url.startswith('git://github.com/')):
            return True
        return False

    def download(self):
        """Download and repack GitHub archive tarball."""
        self._init_commit_ts()
        with Path(TMPDIR_DL, keep=True) as dir_dl:
            # fetch tarball from GitHub
            tarball_path = os.path.join(dir_dl.path, self.subdir + '.tar.gz.dl')
            with Path(tarball_path, isdir=False):
                self._fetch(tarball_path)
                # unpack
                d = os.path.join(dir_dl.path, self.subdir + '.untar')
                with Path(d) as dir_untar:
                    tarball_prefix = Path.untar(tarball_path, into=dir_untar.path)
                    dir0 = os.path.join(dir_untar.path, tarball_prefix)
                    dir1 = os.path.join(dir_untar.path, self.subdir)
                    # submodules check
                    if self._has_submodule(dir0):
                        raise DownloadException('unable to fetch submodules\' source code')
                    # rename subdir
                    os.rename(dir0, dir1)
                    # repack
                    into=os.path.join(TMPDIR_DL, self.source)
                    Path.tar(dir_untar.path, self.subdir, into=into, ts=self.commit_ts)
                    # move to target location
                    file1 = os.path.join(self.dl_dir, self.source)
                    if into != file1:
                        shutil.move(into, file1)

    def _has_submodule(self, dir_):
        m = os.path.join(dir_, '.gitmodules')
        try:
            st = os.stat(m)
            return st.st_size > 0
        except OSError as e:
            return e.errno != errno.ENOENT

    def _init_owner_repo(self):
        url = self.url
        m = self.__repo_url_regex.search(url)
        if m is None:
            raise DownloadException('invalid github url: %s' % url)
        owner = m.group('owner')
        repo = m.group('repo')
        if repo.endswith('.git'):
            repo = repo[:-4]
        self.owner = owner
        self.repo = repo

    def _init_commit_ts(self):
        if self.commit_ts is not None:
            return
        url = self._make_repo_url_path('git', 'commits', self.version)
        ct = self.commit_ts_cache.get(url)
        if ct is not None:
            self.commit_ts = ct
            return
        resp = self._make_request(url)
        data = resp.read()
        data = json.loads(data)
        date = data['committer']['date']
        date = datetime.datetime.strptime(date, '%Y-%m-%dT%H:%M:%SZ')
        date = date.timetuple()
        ct = calendar.timegm(date)
        self.commit_ts = ct
        self.commit_ts_cache.set(url, ct)

    def _fetch(self, path):
        """Fetch tarball of the specified version ref."""
        ref = self.version
        url = self._make_repo_url_path('tarball', ref)
        resp = self._make_request(url)
        with open(path, 'wb') as fout:
            while True:
                d = resp.read(4096)
                if not d:
                    break
                fout.write(d)

    def _make_repo_url_path(self, *args):
        url = '/repos/{0}/{1}'.format(self.owner, self.repo)
        if args:
            url += '/' + '/'.join(args)
        return url

    def _make_request(self, path):
        """Request GitHub API endpoint on ``path``."""
        url = 'https://api.github.com' + path
        headers = {
            'Accept': 'application/vnd.github.v3+json',
            'User-Agent': 'OpenWrt',
        }
        req = urllib2.Request(url, headers=headers)
        sslcontext = ssl._create_unverified_context()
        fileobj = urllib2.urlopen(req, context=sslcontext)
        return fileobj


class DownloadMethodCatchall(DownloadMethod):
    """Dummy method that knows names but not ways of download."""

    def __init__(self, args):
        super(DownloadMethodCatchall, self).__init__(args)
        self.args = args
        self.proto = args.proto
        self.name = self._resolve_name()

    def _resolve_name(self):
        if self.proto:
            return self.proto
        methods_map = (
            ('default', ('@APACHE/', '@GITHUB/', '@GNOME/', '@GNU/',
                         '@KERNEL/', '@SF/', '@SAVANNAH/', 'ftp://', 'http://',
                         'https://', 'file://')),
            ('git', ('git://', )),
            ('svn', ('svn://', )),
            ('cvs', ('cvs://', )),
            ('bzr', ('sftp://', )),
            ('bzr', ('sftp://', )),
            ('unknown', ('', )),
        )
        for name, prefixes in methods_map:
            if any(url.startswith(prefix) for prefix in prefixes for url in self.urls):
                return name

    @staticmethod
    def match(args):
        """Return True."""
        return True

    def download(self):
        """Not implemented.

        raise DownloadException
        """
        raise DownloadException('download method for %s is not yet implemented' % self.name)

# order matters
DOWNLOAD_METHODS = [
    DownloadMethodGitHubTarball,
    DownloadMethodCatchall,
]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('action', choices=('dl_method', 'dl'), help='Action to take')
    parser.add_argument('--urls', nargs='+', metavar='URL', help='Download URLs')
    parser.add_argument('--proto', help='Download proto')
    parser.add_argument('--subdir', help='Source code subdir name')
    parser.add_argument('--version', help='Source code version')
    parser.add_argument('--source', help='Source tarball filename')
    parser.add_argument('--dl-dir', default=os.getcwd(), help='Download dir')
    args = parser.parse_args()
    if args.action == 'dl_method':
        method = DownloadMethod.resolve(args)
        sys.stdout.write(method.name + '\n')
    elif args.action == 'dl':
        method = DownloadMethod.resolve(args)
        try:
            method.download()
        except Exception:
            raise

if __name__ == '__main__':
    main()
