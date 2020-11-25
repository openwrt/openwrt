'use strict';
'require rpc';

/**
 * @typedef {Object} FileStatEntry
 * @memberof LuCI.fs

 * @property {string} name - Name of the directory entry
 * @property {string} type - Type of the entry, one of `block`, `char`, `directory`, `fifo`, `symlink`, `file`, `socket` or `unknown`
 * @property {number} size - Size in bytes
 * @property {number} mode - Access permissions
 * @property {number} atime - Last access time in seconds since epoch
 * @property {number} mtime - Last modification time in seconds since epoch
 * @property {number} ctime - Last change time in seconds since epoch
 * @property {number} inode - Inode number
 * @property {number} uid - Numeric owner id
 * @property {number} gid - Numeric group id
 */

/**
 * @typedef {Object} FileExecResult
 * @memberof LuCI.fs
 *
 * @property {number} code - The exit code of the invoked command
 * @property {string} [stdout] - The stdout produced by the command, if any
 * @property {string} [stderr] - The stderr produced by the command, if any
 */

var callFileList, callFileStat, callFileRead, callFileWrite, callFileRemove,
    callFileExec, callFileMD5;

callFileList = rpc.declare({
	object: 'file',
	method: 'list',
	params: [ 'path' ]
});

callFileStat = rpc.declare({
	object: 'file',
	method: 'stat',
	params: [ 'path' ]
});

callFileRead = rpc.declare({
	object: 'file',
	method: 'read',
	params: [ 'path' ]
});

callFileWrite = rpc.declare({
	object: 'file',
	method: 'write',
	params: [ 'path', 'data', 'mode' ]
});

callFileRemove = rpc.declare({
	object: 'file',
	method: 'remove',
	params: [ 'path' ]
});

callFileExec = rpc.declare({
	object: 'file',
	method: 'exec',
	params: [ 'command', 'params', 'env' ]
});

callFileMD5 = rpc.declare({
	object: 'file',
	method: 'md5',
	params: [ 'path' ]
});

var rpcErrors = [
	null,
	'InvalidCommandError',
	'InvalidArgumentError',
	'MethodNotFoundError',
	'NotFoundError',
	'NoDataError',
	'PermissionError',
	'TimeoutError',
	'UnsupportedError'
];

function handleRpcReply(expect, rc) {
	if (typeof(rc) == 'number' && rc != 0) {
		var e = new Error(rpc.getStatusText(rc)); e.name = rpcErrors[rc] || 'Error';
		throw e;
	}

	if (expect) {
		var type = Object.prototype.toString;

		for (var key in expect) {
			if (rc != null && key != '')
				rc = rc[key];

			if (rc == null || type.call(rc) != type.call(expect[key])) {
				var e = new Error(_('Unexpected reply data format')); e.name = 'TypeError';
				throw e;
			}

			break;
		}
	}

	return rc;
}

function handleCgiIoReply(res) {
	if (!res.ok || res.status != 200) {
		var e = new Error(res.statusText);
		switch (res.status) {
		case 400:
			e.name = 'InvalidArgumentError';
			break;

		case 403:
			e.name = 'PermissionError';
			break;

		case 404:
			e.name = 'NotFoundError';
			break;

		default:
			e.name = 'Error';
		}
		throw e;
	}

	return res.text();
}

/**
 * @class fs
 * @memberof LuCI
 * @hideconstructor
 * @classdesc
 *
 * Provides high level utilities to wrap file system related RPC calls.
 * To import the class in views, use `'require fs'`, to import it in
 * external JavaScript, use `L.require("fs").then(...)`.
 */
var FileSystem = L.Class.extend(/** @lends LuCI.fs.prototype */ {
	/**
	 * Obtains a listing of the specified directory.
	 *
	 * @param {string} path
	 * The directory path to list.
	 *
	 * @returns {Promise<LuCI.fs.FileStatEntry[]>}
	 * Returns a promise resolving to an array of stat detail objects or
	 * rejecting with an error stating the failure reason.
	 */
	list: function(path) {
		return callFileList(path).then(handleRpcReply.bind(this, { entries: [] }));
	},

	/**
	 * Return file stat information on the specified path.
	 *
	 * @param {string} path
	 * The filesystem path to stat.
	 *
	 * @returns {Promise<LuCI.fs.FileStatEntry>}
	 * Returns a promise resolving to a stat detail object or
	 * rejecting with an error stating the failure reason.
	 */
	stat: function(path) {
		return callFileStat(path).then(handleRpcReply.bind(this, { '': {} }));
	},

	/**
	 * Read the contents of the given file and return them.
	 * Note: this function is unsuitable for obtaining binary data.
	 *
	 * @param {string} path
	 * The file path to read.
	 *
	 * @returns {Promise<string>}
	 * Returns a promise resolving to a string containing the file contents or
	 * rejecting with an error stating the failure reason.
	 */
	read: function(path) {
		return callFileRead(path).then(handleRpcReply.bind(this, { data: '' }));
	},

	/**
	 * Write the given data to the specified file path.
	 * If the specified file path does not exist, it will be created, given
	 * sufficient permissions.
	 *
	 * Note: `data` will be converted to a string using `String(data)` or to
	 * `''` when it is `null`.
	 *
	 * @param {string} path
	 * The file path to write to.
	 *
	 * @param {*} [data]
	 * The file data to write. If it is null, it will be set to an empty
	 * string.
	 *
	 * @param {number} [mode]
	 * The permissions to use on file creation. Default is 420 (0644).
	 *
	 * @returns {Promise<number>}
	 * Returns a promise resolving to `0` or rejecting with an error stating
	 * the failure reason.
	 */
	write: function(path, data, mode) {
		data = (data != null) ? String(data) : '';
		mode = (mode != null) ? mode : 420; // 0644
		return callFileWrite(path, data, mode).then(handleRpcReply.bind(this, { '': 0 }));
	},

	/**
	 * Unlink the given file.
	 *
	 * @param {string}
	 * The file path to remove.
	 *
	 * @returns {Promise<number>}
	 * Returns a promise resolving to `0` or rejecting with an error stating
	 * the failure reason.
	 */
	remove: function(path) {
		return callFileRemove(path).then(handleRpcReply.bind(this, { '': 0 }));
	},

	/**
	 * Execute the specified command, optionally passing params and
	 * environment variables.
	 *
	 * Note: The `command` must be either the path to an executable,
	 * or a basename without arguments in which case it will be searched
	 * in $PATH. If specified, the values given in `params` will be passed
	 * as arguments to the command.
	 *
	 * The key/value pairs in the optional `env` table are translated to
	 * `setenv()` calls prior to running the command.
	 *
	 * @param {string} command
	 * The command to invoke.
	 *
	 * @param {string[]} [params]
	 * The arguments to pass to the command.
	 *
	 * @param {Object.<string, string>} [env]
	 * Environment variables to set.
	 *
	 * @returns {Promise<LuCI.fs.FileExecResult>}
	 * Returns a promise resolving to an object describing the execution
	 * results or rejecting with an error stating the failure reason.
	 */
	exec: function(command, params, env) {
		if (!Array.isArray(params))
			params = null;

		if (!L.isObject(env))
			env = null;

		return callFileExec(command, params, env).then(handleRpcReply.bind(this, { '': {} }));
	},

	/**
	 * Read the contents of the given file, trim leading and trailing white
	 * space and return the trimmed result. In case of errors, return an empty
	 * string instead.
	 *
	 * Note: this function is useful to read single-value files in `/sys`
	 * or `/proc`.
	 *
	 * This function is guaranteed to not reject its promises, on failure,
	 * an empty string will be returned.
	 *
	 * @param {string} path
	 * The file path to read.
	 *
	 * @returns {Promise<string>}
	 * Returns a promise resolving to the file contents or the empty string
	 * on failure.
	 */
	trimmed: function(path) {
		return L.resolveDefault(this.read(path), '').then(function(s) {
			return s.trim();
		});
	},

	/**
	 * Read the contents of the given file, split it into lines, trim
	 * leading and trailing white space of each line and return the
	 * resulting array.
	 *
	 * This function is guaranteed to not reject its promises, on failure,
	 * an empty array will be returned.
	 *
	 * @param {string} path
	 * The file path to read.
	 *
	 * @returns {Promise<string[]>}
	 * Returns a promise resolving to an array containing the stripped lines
	 * of the given file or `[]` on failure.
	 */
	lines: function(path) {
		return L.resolveDefault(this.read(path), '').then(function(s) {
			var lines = [];

			s = s.trim();

			if (s != '') {
				var l = s.split(/\n/);

				for (var i = 0; i < l.length; i++)
					lines.push(l[i].trim());
			}

			return lines;
		});
	},

	/**
	 * Read the contents of the given file and return them, bypassing ubus.
	 *
	 * This function will read the requested file through the cgi-io
	 * helper applet at `/cgi-bin/cgi-download` which bypasses the ubus rpc
	 * transport. This is useful to fetch large file contents which might
	 * exceed the ubus message size limits or which contain binary data.
	 *
	 * The cgi-io helper will enforce the same access permission rules as
	 * the ubus based read call.
	 *
	 * @param {string} path
	 * The file path to read.
	 *
	 * @returns {Promise<string>}
	 * Returns a promise resolving to a string containing the file contents or
	 * rejecting with an error stating the failure reason.
	 */
	read_direct: function(path) {
		var postdata = 'sessionid=%s&path=%s'
			.format(encodeURIComponent(L.env.sessionid), encodeURIComponent(path));

		return L.Request.post('/cgi-bin/cgi-download', postdata, {
			headers: { 'Content-Type': 'application/x-www-form-urlencoded' }
		}).then(handleCgiIoReply);
	},

	/**
	 * Execute the specified command, bypassing ubus.
	 *
	 * Note: The `command` must be either the path to an executable,
	 * or a basename without arguments in which case it will be searched
	 * in $PATH. If specified, the values given in `params` will be passed
	 * as arguments to the command.
	 *
	 * This function will invoke the requested commands through the cgi-io
	 * helper applet at `/cgi-bin/cgi-exec` which bypasses the ubus rpc
	 * transport. This is useful to fetch large command outputs which might
	 * exceed the ubus message size limits or which contain binary data.
	 *
	 * The cgi-io helper will enforce the same access permission rules as
	 * the ubus based exec call.
	 *
	 * @param {string} command
	 * The command to invoke.
	 *
	 * @param {string[]} [params]
	 * The arguments to pass to the command.
	 *
	 * @returns {Promise<string>}
	 * Returns a promise resolving to the gathered command stdout output or
	 * rejecting with an error stating the failure reason.
	 */
	exec_direct: function(command, params) {
		var cmdstr = String(command)
			.replace(/\\/g, '\\\\').replace(/(\s)/g, '\\$1');

		if (Array.isArray(params))
			for (var i = 0; i < params.length; i++)
				cmdstr += ' ' + String(params[i])
					.replace(/\\/g, '\\\\').replace(/(\s)/g, '\\$1');

		var postdata = 'sessionid=%s&command=%s'
			.format(encodeURIComponent(L.env.sessionid), encodeURIComponent(cmdstr));

		return L.Request.post('/cgi-bin/cgi-exec', postdata, {
			headers: { 'Content-Type': 'application/x-www-form-urlencoded' }
		}).then(handleCgiIoReply);
	}
});

return FileSystem;
