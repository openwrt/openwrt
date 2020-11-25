'use strict';

var rpcRequestID = 1,
    rpcSessionID = L.env.sessionid || '00000000000000000000000000000000',
    rpcBaseURL = L.url('admin/ubus'),
    rpcInterceptorFns = [];

/**
 * @class rpc
 * @memberof LuCI
 * @hideconstructor
 * @classdesc
 *
 * The `LuCI.rpc` class provides high level ubus JSON-RPC abstractions
 * and means for listing and invoking remove RPC methods.
 */
return L.Class.extend(/** @lends LuCI.rpc.prototype */ {
	/* privates */
	call: function(req, cb, nobatch) {
		var q = '';

		if (Array.isArray(req)) {
			if (req.length == 0)
				return Promise.resolve([]);

			for (var i = 0; i < req.length; i++)
				if (req[i].params)
					q += '%s%s.%s'.format(
						q ? ';' : '/',
						req[i].params[1],
						req[i].params[2]
					);
		}
		else if (req.params) {
			q += '/%s.%s'.format(req.params[1], req.params[2]);
		}

		return L.Request.post(rpcBaseURL + q, req, {
			timeout: (L.env.rpctimeout || 20) * 1000,
			nobatch: nobatch,
			credentials: true
		}).then(cb, cb);
	},

	parseCallReply: function(req, res) {
		var msg = null;

		if (res instanceof Error)
			return req.reject(res);

		try {
			if (!res.ok)
				L.raise('RPCError', 'RPC call to %s/%s failed with HTTP error %d: %s',
					req.object, req.method, res.status, res.statusText || '?');

			msg = res.json();
		}
		catch (e) {
			return req.reject(e);
		}

		/*
		 * The interceptor args are intentionally swapped.
		 * Response is passed as first arg to align with Request class interceptors
		 */
		Promise.all(rpcInterceptorFns.map(function(fn) { return fn(msg, req) }))
			.then(this.handleCallReply.bind(this, req, msg))
			.catch(req.reject);
	},

	handleCallReply: function(req, msg) {
		var type = Object.prototype.toString,
		    ret = null;

		try {
			/* verify message frame */
			if (!L.isObject(msg) || msg.jsonrpc != '2.0')
				L.raise('RPCError', 'RPC call to %s/%s returned invalid message frame',
					req.object, req.method);

			/* check error condition */
			if (L.isObject(msg.error) && msg.error.code && msg.error.message)
				L.raise('RPCError', 'RPC call to %s/%s failed with error %d: %s',
					req.object, req.method, msg.error.code, msg.error.message || '?');
		}
		catch (e) {
			return req.reject(e);
		}

		if (!req.object && !req.method) {
			ret = msg.result;
		}
		else if (Array.isArray(msg.result)) {
			ret = (msg.result.length > 1) ? msg.result[1] : msg.result[0];
		}

		if (req.expect) {
			for (var key in req.expect) {
				if (ret != null && key != '')
					ret = ret[key];

				if (ret == null || type.call(ret) != type.call(req.expect[key]))
					ret = req.expect[key];

				break;
			}
		}

		/* apply filter */
		if (typeof(req.filter) == 'function') {
			req.priv[0] = ret;
			req.priv[1] = req.params;
			ret = req.filter.apply(this, req.priv);
		}

		req.resolve(ret);
	},

	/**
	 * Lists available remote ubus objects or the method signatures of
	 * specific objects.
	 *
	 * This function has two signatures and is sensitive to the number of
	 * arguments passed to it:
	 *  - `list()` -
	 *    Returns an array containing the names of all remote `ubus` objects
	 *  - `list("objname", ...)`
	 *    Returns method signatures for each given `ubus` object name.
	 *
	 * @param {...string} [objectNames]
	 * If any object names are given, this function will return the method
	 * signatures of each given object.
	 *
	 * @returns {Promise<Array<string>|Object<string, Object<string, Object<string, string>>>>}
	 * When invoked without arguments, this function will return a promise
	 * resolving to an array of `ubus` object names. When invoked with one or
	 * more arguments, a promise resolving to an object describing the method
	 * signatures of each requested `ubus` object name will be returned.
	 */
	list: function() {
		var msg = {
			jsonrpc: '2.0',
			id:      rpcRequestID++,
			method:  'list',
			params:  arguments.length ? this.varargs(arguments) : undefined
		};

		return new Promise(L.bind(function(resolveFn, rejectFn) {
			/* store request info */
			var req = {
				resolve: resolveFn,
				reject:  rejectFn
			};

			/* call rpc */
			this.call(msg, this.parseCallReply.bind(this, req));
		}, this));
	},

	/**
	 * @typedef {Object} DeclareOptions
	 * @memberof LuCI.rpc
	 *
	 * @property {string} object
	 * The name of the remote `ubus` object to invoke.
	 *
	 * @property {string} method
	 * The name of the remote `ubus` method to invoke.
	 *
	 * @property {string[]} [params]
	 * Lists the named parameters expected by the remote `ubus` RPC method.
	 * The arguments passed to the resulting generated method call function
	 * will be mapped to named parameters in the order they appear in this
	 * array.
	 *
	 * Extraneous parameters passed to the generated function will not be
	 * sent to the remote procedure but are passed to the
	 * {@link LuCI.rpc~filterFn filter function} if one is specified.
	 *
	 * Examples:
	 *  - `params: [ "foo", "bar" ]` -
	 *    When the resulting call function is invoked with `fn(true, false)`,
	 *    the corresponding args object sent to the remote procedure will be
	 *    `{ foo: true, bar: false }`.
	 *  - `params: [ "test" ], filter: function(reply, args, extra) { ... }` -
	 *    When the resultung generated function is invoked with
	 *    `fn("foo", "bar", "baz")` then `{ "test": "foo" }` will be sent as
	 *    argument to the remote procedure and the filter function will be
	 *    invoked with `filterFn(reply, [ "foo" ], "bar", "baz")`
	 *
	 * @property {Object<string,*>} [expect]
	 * Describes the expected return data structure. The given object is
	 * supposed to contain a single key selecting the value to use from
	 * the returned `ubus` reply object. The value of the sole key within
	 * the `expect` object is used to infer the expected type of the received
	 * `ubus` reply data.
	 *
	 * If the received data does not contain `expect`'s key, or if the
	 * type of the data differs from the type of the value in the expect
	 * object, the expect object's value is returned as default instead.
	 *
	 * The key in the `expect` object may be an empty string (`''`) in which
	 * case the entire reply object is selected instead of one of its subkeys.
	 *
	 * If the `expect` option is omitted, the received reply will be returned
	 * as-is, regardless of its format or type.
	 *
	 * Examples:
	 *  - `expect: { '': { error: 'Invalid response' } }` -
	 *    This requires the entire `ubus` reply to be a plain JavaScript
	 *    object. If the reply isn't an object but e.g. an array or a numeric
	 *    error code instead, it will get replaced with
	 *    `{ error: 'Invalid response' }` instead.
	 *  - `expect: { results: [] }` -
	 *    This requires the received `ubus` reply to be an object containing
	 *    a key `results` with an array as value. If the received reply does
	 *    not contain such a key, or if `reply.results` points to a non-array
	 *    value, the empty array (`[]`) will be used instead.
	 *  - `expect: { success: false }` -
	 *    This requires the received `ubus` reply to be an object containing
	 *    a key `success` with a boolean value. If the reply does not contain
	 *    `success` or if `reply.success` is not a boolean value, `false` will
	 *    be returned as default instead.
	 *
	 * @property {LuCI.rpc~filterFn} [filter]
	 * Specfies an optional filter function which is invoked to transform the
	 * received reply data before it is returned to the caller.
	 *
	 */

	/**
	 * The filter function is invoked to transform a received `ubus` RPC call
	 * reply before returning it to the caller.
	 *
	 * @callback LuCI.rpc~filterFn
	 *
	 * @param {*} data
	 * The received `ubus` reply data or a subset of it as described in the
	 * `expect` option of the RPC call declaration. In case of remote call
	 * errors, `data` is numeric `ubus` error code instead.
	 *
	 * @param {Array<*>} args
	 * The arguments the RPC method has been invoked with.
	 *
	 * @param {...*} extraArgs
	 * All extraneous arguments passed to the RPC method exceeding the number
	 * of arguments describes in the RPC call declaration.
	 *
	 * @return {*}
	 * The return value of the filter function will be returned to the caller
	 * of the RPC method as-is.
	 */

	/**
	 * The generated invocation function is returned by
	 * {@link LuCI.rpc#declare rpc.declare()} and encapsulates a single
	 * RPC method call.
	 *
	 * Calling this function will execute a remote `ubus` HTTP call request
	 * using the arguments passed to it as arguments and return a promise
	 * resolving to the received reply values.
	 *
	 * @callback LuCI.rpc~invokeFn
	 *
	 * @param {...*} params
	 * The parameters to pass to the remote procedure call. The given
	 * positional arguments will be named to named RPC parameters according
	 * to the names specified in the `params` array of the method declaration.
	 *
	 * Any additional parameters exceeding the amount of arguments in the
	 * `params` declaration are passed as private extra arguments to the
	 * declared filter function.
	 *
	 * @return {Promise<*>}
	 * Returns a promise resolving to the result data of the remote `ubus`
	 * RPC method invocation, optionally substituted and filtered according
	 * to the `expect` and `filter` declarations.
	 */

	/**
	 * Describes a remote RPC call procedure and returns a function
	 * implementing it.
	 *
	 * @param {LuCI.rpc.DeclareOptions} options
	 * If any object names are given, this function will return the method
	 * signatures of each given object.
	 *
	 * @returns {LuCI.rpc~invokeFn}
	 * Returns a new function implementing the method call described in
	 * `options`.
	 */
	declare: function(options) {
		return Function.prototype.bind.call(function(rpc, options) {
			var args = this.varargs(arguments, 2);
			return new Promise(function(resolveFn, rejectFn) {
				/* build parameter object */
				var p_off = 0;
				var params = { };
				if (Array.isArray(options.params))
					for (p_off = 0; p_off < options.params.length; p_off++)
						params[options.params[p_off]] = args[p_off];

				/* all remaining arguments are private args */
				var priv = [ undefined, undefined ];
				for (; p_off < args.length; p_off++)
					priv.push(args[p_off]);

				/* store request info */
				var req = {
					expect:  options.expect,
					filter:  options.filter,
					resolve: resolveFn,
					reject:  rejectFn,
					params:  params,
					priv:    priv,
					object:  options.object,
					method:  options.method
				};

				/* build message object */
				var msg = {
					jsonrpc: '2.0',
					id:      rpcRequestID++,
					method:  'call',
					params:  [
						rpcSessionID,
						options.object,
						options.method,
						params
					]
				};

				/* call rpc */
				rpc.call(msg, rpc.parseCallReply.bind(rpc, req), options.nobatch);
			});
		}, this, this, options);
	},

	/**
	 * Returns the current RPC session id.
	 *
	 * @returns {string}
	 * Returns the 32 byte session ID string used for authenticating remote
	 * requests.
	 */
	getSessionID: function() {
		return rpcSessionID;
	},

	/**
	 * Set the RPC session id to use.
	 *
	 * @param {string} sid
	 * Sets the 32 byte session ID string used for authenticating remote
	 * requests.
	 */
	setSessionID: function(sid) {
		rpcSessionID = sid;
	},

	/**
	 * Returns the current RPC base URL.
	 *
	 * @returns {string}
	 * Returns the RPC URL endpoint to issue requests against.
	 */
	getBaseURL: function() {
		return rpcBaseURL;
	},

	/**
	 * Set the RPC base URL to use.
	 *
	 * @param {string} sid
	 * Sets the RPC URL endpoint to issue requests against.
	 */
	setBaseURL: function(url) {
		rpcBaseURL = url;
	},

	/**
	 * Translates a numeric `ubus` error code into a human readable
	 * description.
	 *
	 * @param {number} statusCode
	 * The numeric status code.
	 *
	 * @returns {string}
	 * Returns the textual description of the code.
	 */
	getStatusText: function(statusCode) {
		switch (statusCode) {
		case 0: return _('Command OK');
		case 1: return _('Invalid command');
		case 2: return _('Invalid argument');
		case 3: return _('Method not found');
		case 4: return _('Resource not found');
		case 5: return _('No data received');
		case 6: return _('Permission denied');
		case 7: return _('Request timeout');
		case 8: return _('Not supported');
		case 9: return _('Unspecified error');
		case 10: return _('Connection lost');
		default: return _('Unknown error code');
		}
	},

	/**
	 * Registered interceptor functions are invoked before the standard reply
	 * parsing and handling logic.
	 *
	 * By returning rejected promises, interceptor functions can cause the
	 * invocation function to fail, regardless of the received reply.
	 *
	 * Interceptors may also modify their message argument in-place to
	 * rewrite received replies before they're processed by the standard
	 * response handling code.
	 *
	 * A common use case for such functions is to detect failing RPC replies
	 * due to expired authentication in order to trigger a new login.
	 *
	 * @callback LuCI.rpc~interceptorFn
	 *
	 * @param {*} msg
	 * The unprocessed, JSON decoded remote RPC method call reply.
	 *
	 * Since interceptors run before the standard parsing logic, the reply
	 * data is not verified for correctness or filtered according to
	 * `expect` and `filter` specifications in the declarations.
	 *
	 * @param {Object} req
	 * The related request object which is an extended variant of the
	 * declaration object, allowing access to internals of the invocation
	 * function such as `filter`, `expect` or `params` values.
	 *
	 * @return {Promise<*>|*}
	 * Interceptor functions may return a promise to defer response
	 * processing until some delayed work completed. Any values the returned
	 * promise resolves to are ignored.
	 *
	 * When the returned promise rejects with an error, the invocation
	 * function will fail too, forwarding the error to the caller.
	 */

	/**
	 * Registers a new interceptor function.
	 *
	 * @param {LuCI.rpc~interceptorFn} interceptorFn
	 * The inteceptor function to register.
	 *
	 * @returns {LuCI.rpc~interceptorFn}
	 * Returns the given function value.
	 */
	addInterceptor: function(interceptorFn) {
		if (typeof(interceptorFn) == 'function')
			rpcInterceptorFns.push(interceptorFn);
		return interceptorFn;
	},

	/**
	 * Removes a registered interceptor function.
	 *
	 * @param {LuCI.rpc~interceptorFn} interceptorFn
	 * The inteceptor function to remove.
	 *
	 * @returns {boolean}
	 * Returns `true` if the given function has been removed or `false`
	 * if it has not been found.
	 */
	removeInterceptor: function(interceptorFn) {
		var oldlen = rpcInterceptorFns.length, i = oldlen;
		while (i--)
			if (rpcInterceptorFns[i] === interceptorFn)
				rpcInterceptorFns.splice(i, 1);
		return (rpcInterceptorFns.length < oldlen);
	}
});
