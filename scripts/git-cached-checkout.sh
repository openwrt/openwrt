#!/bin/sh
gitdir="$1"
giturl="$2"
gitversion="$3"
checkoutdir="$4"
if [ ! -d "$gitdir" ]; then
	mkdir "$gitdir" || exit $?
	cd "$gitdir" || exit $?
	git init --bare || exit $?
	git config --unset core.bare
	git remote add origin "dummy"
	git config --unset core.worktree
	git config --unset-all remote.origin.fetch
	git config --add remote.origin.fetch "+refs/heads/*:refs/remotes/origin/*"
	git config --add remote.origin.fetch "+refs/tags/*:refs/tags/*"
fi

set -e
cd "$gitdir"
git remote set-url origin "$giturl"
git rev-parse origin/$gitversion \
	|| git show $gitversion >/dev/null \
	|| git fetch --progress origin $gitversion \
	|| git fetch --tags --progress origin

cd "$checkoutdir"
echo "gitdir: $gitdir" > .git
git reset --hard origin/$gitversion || git reset --hard $gitversion

echo git submodule
git submodule update --init --recursive
