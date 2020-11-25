/**
 * @class LuCI
 * @classdesc
 *
 * This is the LuCI base class. It is automatically instantiated and
 * accessible using the global `L` variable.
 *
 * @param {Object} env
 * The environment settings to use for the LuCI runtime.
 */

(function(window, document, undefined) {
	'use strict';

	/* Object.assign polyfill for IE */
	if (typeof Object.assign !== 'function') {
		Object.defineProperty(Object, 'assign', {
			value: function assign(target, varArgs) {
				if (target == null)
					throw new TypeError('Cannot convert undefined or null to object');

				var to = Object(target);

				for (var index = 1; index < arguments.length; index++)
					if (arguments[index] != null)
						for (var nextKey in arguments[index])
							if (Object.prototype.hasOwnProperty.call(arguments[index], nextKey))
								to[nextKey] = arguments[index][nextKey];

				return to;
			},
			writable: true,
			configurable: true
		});
	}

	/* Promise.finally polyfill */
	if (typeof Promise.prototype.finally !== 'function') {
		Promise.prototype.finally = function(fn) {
			var onFinally = function(cb) {
				return Promise.resolve(fn.call(this)).then(cb);
			};

			return this.then(
				function(result) { return onFinally.call(this, function() { return result }) },
				function(reason) { return onFinally.call(this, function() { return Promise.reject(reason) }) }
			);
		};
	}

	/*
	 * Class declaration and inheritance helper
	 */

	var toCamelCase = function(s) {
		return s.replace(/(?:^|[\. -])(.)/g, function(m0, m1) { return m1.toUpperCase() });
	};

	/**
	 * @class Class
	 * @hideconstructor
	 * @memberof LuCI
	 * @classdesc
	 *
	 * `LuCI.Class` is the abstract base class all LuCI classes inherit from.
	 *
	 * It provides simple means to create subclasses of given classes and
	 * implements prototypal inheritance.
	 */
	var superContext = {}, classIndex = 0, Class = Object.assign(function() {}, {
		/**
		 * Extends this base class with the properties described in
		 * `properties` and returns a new subclassed Class instance
		 *
		 * @memberof LuCI.Class
		 *
		 * @param {Object<string, *>} properties
		 * An object describing the properties to add to the new
		 * subclass.
		 *
		 * @returns {LuCI.Class}
		 * Returns a new LuCI.Class sublassed from this class, extended
		 * by the given properties and with its prototype set to this base
		 * class to enable inheritance. The resulting value represents a
		 * class constructor and can be instantiated with `new`.
		 */
		extend: function(properties) {
			var props = {
				__id__: { value: classIndex },
				__base__: { value: this.prototype },
				__name__: { value: properties.__name__ || 'anonymous' + classIndex++ }
			};

			var ClassConstructor = function() {
				if (!(this instanceof ClassConstructor))
					throw new TypeError('Constructor must not be called without "new"');

				if (Object.getPrototypeOf(this).hasOwnProperty('__init__')) {
					if (typeof(this.__init__) != 'function')
						throw new TypeError('Class __init__ member is not a function');

					this.__init__.apply(this, arguments)
				}
				else {
					this.super('__init__', arguments);
				}
			};

			for (var key in properties)
				if (!props[key] && properties.hasOwnProperty(key))
					props[key] = { value: properties[key], writable: true };

			ClassConstructor.prototype = Object.create(this.prototype, props);
			ClassConstructor.prototype.constructor = ClassConstructor;
			Object.assign(ClassConstructor, this);
			ClassConstructor.displayName = toCamelCase(props.__name__.value + 'Class');

			return ClassConstructor;
		},

		/**
		 * Extends this base class with the properties described in
		 * `properties`, instantiates the resulting subclass using
		 * the additional optional arguments passed to this function
		 * and returns the resulting subclassed Class instance.
		 *
		 * This function serves as a convenience shortcut for
		 * {@link LuCI.Class.extend Class.extend()} and subsequent
		 * `new`.
		 *
		 * @memberof LuCI.Class
		 *
		 * @param {Object<string, *>} properties
		 * An object describing the properties to add to the new
		 * subclass.
		 *
		 * @param {...*} [new_args]
		 * Specifies arguments to be passed to the subclass constructor
		 * as-is in order to instantiate the new subclass.
		 *
		 * @returns {LuCI.Class}
		 * Returns a new LuCI.Class instance extended by the given
		 * properties with its prototype set to this base class to
		 * enable inheritance.
		 */
		singleton: function(properties /*, ... */) {
			return Class.extend(properties)
				.instantiate(Class.prototype.varargs(arguments, 1));
		},

		/**
		 * Calls the class constructor using `new` with the given argument
		 * array being passed as variadic parameters to the constructor.
		 *
		 * @memberof LuCI.Class
		 *
		 * @param {Array<*>} params
		 * An array of arbitrary values which will be passed as arguments
		 * to the constructor function.
		 *
		 * @param {...*} [new_args]
		 * Specifies arguments to be passed to the subclass constructor
		 * as-is in order to instantiate the new subclass.
		 *
		 * @returns {LuCI.Class}
		 * Returns a new LuCI.Class instance extended by the given
		 * properties with its prototype set to this base class to
		 * enable inheritance.
		 */
		instantiate: function(args) {
			return new (Function.prototype.bind.apply(this,
				Class.prototype.varargs(args, 0, null)))();
		},

		/* unused */
		call: function(self, method) {
			if (typeof(this.prototype[method]) != 'function')
				throw new ReferenceError(method + ' is not defined in class');

			return this.prototype[method].apply(self, self.varargs(arguments, 1));
		},

		/**
		 * Checks whether the given class value is a subclass of this class.
		 *
		 * @memberof LuCI.Class
		 *
		 * @param {LuCI.Class} classValue
		 * The class object to test.
		 *
		 * @returns {boolean}
		 * Returns `true` when the given `classValue` is a subclass of this
		 * class or `false` if the given value is not a valid class or not
		 * a subclass of this class'.
		 */
		isSubclass: function(classValue) {
			return (classValue != null &&
			        typeof(classValue) == 'function' &&
			        classValue.prototype instanceof this);
		},

		prototype: {
			/**
			 * Extract all values from the given argument array beginning from
			 * `offset` and prepend any further given optional parameters to
			 * the beginning of the resulting array copy.
			 *
			 * @memberof LuCI.Class
			 * @instance
			 *
			 * @param {Array<*>} args
			 * The array to extract the values from.
			 *
			 * @param {number} offset
			 * The offset from which to extract the values. An offset of `0`
			 * would copy all values till the end.
			 *
			 * @param {...*} [extra_args]
			 * Extra arguments to add to prepend to the resultung array.
			 *
			 * @returns {Array<*>}
			 * Returns a new array consisting of the optional extra arguments
			 * and the values extracted from the `args` array beginning with
			 * `offset`.
			 */
			varargs: function(args, offset /*, ... */) {
				return Array.prototype.slice.call(arguments, 2)
					.concat(Array.prototype.slice.call(args, offset));
			},

			/**
			 * Walks up the parent class chain and looks for a class member
			 * called `key` in any of the parent classes this class inherits
			 * from. Returns the member value of the superclass or calls the
			 * member as function and returns its return value when the
			 * optional `callArgs` array is given.
			 *
			 * This function has two signatures and is sensitive to the
			 * amount of arguments passed to it:
			 *  - `super('key')` -
			 *    Returns the value of `key` when found within one of the
			 *    parent classes.
			 *  - `super('key', ['arg1', 'arg2'])` -
			 *    Calls the `key()` method with parameters `arg1` and `arg2`
			 *    when found within one of the parent classes.
			 *
			 * @memberof LuCI.Class
			 * @instance
			 *
			 * @param {string} key
			 * The name of the superclass member to retrieve.
			 *
			 * @param {Array<*>} [callArgs]
			 * An optional array of function call parameters to use. When
			 * this parameter is specified, the found member value is called
			 * as function using the values of this array as arguments.
			 *
			 * @throws {ReferenceError}
			 * Throws a `ReferenceError` when `callArgs` are specified and
			 * the found member named by `key` is not a function value.
			 *
			 * @returns {*|null}
			 * Returns the value of the found member or the return value of
			 * the call to the found method. Returns `null` when no member
			 * was found in the parent class chain or when the call to the
			 * superclass method returned `null`.
			 */
			super: function(key, callArgs) {
				if (key == null)
					return null;

				var slotIdx = this.__id__ + '.' + key,
				    symStack = superContext[slotIdx],
				    protoCtx = null;

				for (protoCtx = Object.getPrototypeOf(symStack ? symStack[0] : Object.getPrototypeOf(this));
				     protoCtx != null && !protoCtx.hasOwnProperty(key);
				     protoCtx = Object.getPrototypeOf(protoCtx)) {}

				if (protoCtx == null)
					return null;

				var res = protoCtx[key];

				if (arguments.length > 1) {
					if (typeof(res) != 'function')
						throw new ReferenceError(key + ' is not a function in base class');

					if (typeof(callArgs) != 'object')
						callArgs = this.varargs(arguments, 1);

					if (symStack)
						symStack.unshift(protoCtx);
					else
						superContext[slotIdx] = [ protoCtx ];

					res = res.apply(this, callArgs);

					if (symStack && symStack.length > 1)
						symStack.shift(protoCtx);
					else
						delete superContext[slotIdx];
				}

				return res;
			},

			/**
			 * Returns a string representation of this class.
			 *
			 * @returns {string}
			 * Returns a string representation of this class containing the
			 * constructor functions `displayName` and describing the class
			 * members and their respective types.
			 */
			toString: function() {
				var s = '[' + this.constructor.displayName + ']', f = true;
				for (var k in this) {
					if (this.hasOwnProperty(k)) {
						s += (f ? ' {\n' : '') + '  ' + k + ': ' + typeof(this[k]) + '\n';
						f = false;
					}
				}
				return s + (f ? '' : '}');
			}
		}
	});


	/**
	 * @class
	 * @memberof LuCI
	 * @hideconstructor
	 * @classdesc
	 *
	 * The `Headers` class is an internal utility class exposed in HTTP
	 * response objects using the `response.headers` property.
	 */
	var Headers = Class.extend(/** @lends LuCI.Headers.prototype */ {
		__name__: 'LuCI.XHR.Headers',
		__init__: function(xhr) {
			var hdrs = this.headers = {};
			xhr.getAllResponseHeaders().split(/\r\n/).forEach(function(line) {
				var m = /^([^:]+):(.*)$/.exec(line);
				if (m != null)
					hdrs[m[1].trim().toLowerCase()] = m[2].trim();
			});
		},

		/**
		 * Checks whether the given header name is present.
		 * Note: Header-Names are case-insensitive.
		 *
		 * @instance
		 * @memberof LuCI.Headers
		 * @param {string} name
		 * The header name to check
		 *
		 * @returns {boolean}
		 * Returns `true` if the header name is present, `false` otherwise
		 */
		has: function(name) {
			return this.headers.hasOwnProperty(String(name).toLowerCase());
		},

		/**
		 * Returns the value of the given header name.
		 * Note: Header-Names are case-insensitive.
		 *
		 * @instance
		 * @memberof LuCI.Headers
		 * @param {string} name
		 * The header name to read
		 *
		 * @returns {string|null}
		 * The value of the given header name or `null` if the header isn't present.
		 */
		get: function(name) {
			var key = String(name).toLowerCase();
			return this.headers.hasOwnProperty(key) ? this.headers[key] : null;
		}
	});

	/**
	 * @class
	 * @memberof LuCI
	 * @hideconstructor
	 * @classdesc
	 *
	 * The `Response` class is an internal utility class representing HTTP responses.
	 */
	var Response = Class.extend({
		__name__: 'LuCI.XHR.Response',
		__init__: function(xhr, url, duration, headers, content) {
			/**
			 * Describes whether the response is successful (status codes `200..299`) or not
			 * @instance
			 * @memberof LuCI.Response
			 * @name ok
			 * @type {boolean}
			 */
			this.ok = (xhr.status >= 200 && xhr.status <= 299);

			/**
			 * The numeric HTTP status code of the response
			 * @instance
			 * @memberof LuCI.Response
			 * @name status
			 * @type {number}
			 */
			this.status = xhr.status;

			/**
			 * The HTTP status description message of the response
			 * @instance
			 * @memberof LuCI.Response
			 * @name statusText
			 * @type {string}
			 */
			this.statusText = xhr.statusText;

			/**
			 * The HTTP headers of the response
			 * @instance
			 * @memberof LuCI.Response
			 * @name headers
			 * @type {LuCI.Headers}
			 */
			this.headers = (headers != null) ? headers : new Headers(xhr);

			/**
			 * The total duration of the HTTP request in milliseconds
			 * @instance
			 * @memberof LuCI.Response
			 * @name duration
			 * @type {number}
			 */
			this.duration = duration;

			/**
			 * The final URL of the request, i.e. after following redirects.
			 * @instance
			 * @memberof LuCI.Response
			 * @name url
			 * @type {string}
			 */
			this.url = url;

			/* privates */
			this.xhr = xhr;

			if (content != null && typeof(content) == 'object') {
				this.responseJSON = content;
				this.responseText = null;
			}
			else if (content != null) {
				this.responseJSON = null;
				this.responseText = String(content);
			}
			else {
				this.responseJSON = null;
				this.responseText = xhr.responseText;
			}
		},

		/**
		 * Clones the given response object, optionally overriding the content
		 * of the cloned instance.
		 *
		 * @instance
		 * @memberof LuCI.Response
		 * @param {*} [content]
		 * Override the content of the cloned response. Object values will be
		 * treated as JSON response data, all other types will be converted
		 * using `String()` and treated as response text.
		 *
		 * @returns {LuCI.Response}
		 * The cloned `Response` instance.
		 */
		clone: function(content) {
			var copy = new Response(this.xhr, this.url, this.duration, this.headers, content);

			copy.ok = this.ok;
			copy.status = this.status;
			copy.statusText = this.statusText;

			return copy;
		},

		/**
		 * Access the response content as JSON data.
		 *
		 * @instance
		 * @memberof LuCI.Response
		 * @throws {SyntaxError}
		 * Throws `SyntaxError` if the content isn't valid JSON.
		 *
		 * @returns {*}
		 * The parsed JSON data.
		 */
		json: function() {
			if (this.responseJSON == null)
				this.responseJSON = JSON.parse(this.responseText);

			return this.responseJSON;
		},

		/**
		 * Access the response content as string.
		 *
		 * @instance
		 * @memberof LuCI.Response
		 * @returns {string}
		 * The response content.
		 */
		text: function() {
			if (this.responseText == null && this.responseJSON != null)
				this.responseText = JSON.stringify(this.responseJSON);

			return this.responseText;
		}
	});


	var requestQueue = [];

	function isQueueableRequest(opt) {
		if (!classes.rpc)
			return false;

		if (opt.method != 'POST' || typeof(opt.content) != 'object')
			return false;

		if (opt.nobatch === true)
			return false;

		var rpcBaseURL = Request.expandURL(classes.rpc.getBaseURL());

		return (rpcBaseURL != null && opt.url.indexOf(rpcBaseURL) == 0);
	}

	function flushRequestQueue() {
		if (!requestQueue.length)
			return;

		var reqopt = Object.assign({}, requestQueue[0][0], { content: [], nobatch: true }),
		    batch = [];

		for (var i = 0; i < requestQueue.length; i++) {
			batch[i] = requestQueue[i];
			reqopt.content[i] = batch[i][0].content;
		}

		requestQueue.length = 0;

		Request.request(rpcBaseURL, reqopt).then(function(reply) {
			var json = null, req = null;

			try { json = reply.json() }
			catch(e) { }

			while ((req = batch.shift()) != null)
				if (Array.isArray(json) && json.length)
					req[2].call(reqopt, reply.clone(json.shift()));
				else
					req[1].call(reqopt, new Error('No related RPC reply'));
		}).catch(function(error) {
			var req = null;

			while ((req = batch.shift()) != null)
				req[1].call(reqopt, error);
		});
	}

	/**
	 * @class
	 * @memberof LuCI
	 * @hideconstructor
	 * @classdesc
	 *
	 * The `Request` class allows initiating HTTP requests and provides utilities
	 * for dealing with responses.
	 */
	var Request = Class.singleton(/** @lends LuCI.Request.prototype */ {
		__name__: 'LuCI.Request',

		interceptors: [],

		/**
		 * Turn the given relative URL into an absolute URL if necessary.
		 *
		 * @instance
		 * @memberof LuCI.Request
		 * @param {string} url
		 * The URL to convert.
		 *
		 * @returns {string}
		 * The absolute URL derived from the given one, or the original URL
		 * if it already was absolute.
		 */
		expandURL: function(url) {
			if (!/^(?:[^/]+:)?\/\//.test(url))
				url = location.protocol + '//' + location.host + url;

			return url;
		},

		/**
		 * @typedef {Object} RequestOptions
		 * @memberof LuCI.Request
		 *
		 * @property {string} [method=GET]
		 * The HTTP method to use, e.g. `GET` or `POST`.
		 *
		 * @property {Object<string, Object|string>} [query]
		 * Query string data to append to the URL. Non-string values of the
		 * given object will be converted to JSON.
		 *
		 * @property {boolean} [cache=false]
		 * Specifies whether the HTTP response may be retrieved from cache.
		 *
		 * @property {string} [username]
		 * Provides a username for HTTP basic authentication.
		 *
		 * @property {string} [password]
		 * Provides a password for HTTP basic authentication.
		 *
		 * @property {number} [timeout]
		 * Specifies the request timeout in seconds.
		 *
		 * @property {boolean} [credentials=false]
		 * Whether to include credentials such as cookies in the request.
		 *
		 * @property {*} [content]
		 * Specifies the HTTP message body to send along with the request.
		 * If the value is a function, it is invoked and the return value
		 * used as content, if it is a FormData instance, it is used as-is,
		 * if it is an object, it will be converted to JSON, in all other
		 * cases it is converted to a string.
		 *
	     * @property {Object<string, string>} [header]
	     * Specifies HTTP headers to set for the request.
	     *
	     * @property {function} [progress]
	     * An optional request callback function which receives ProgressEvent
	     * instances as sole argument during the HTTP request transfer.
		 */

		/**
		 * Initiate an HTTP request to the given target.
		 *
		 * @instance
		 * @memberof LuCI.Request
		 * @param {string} target
		 * The URL to request.
		 *
		 * @param {LuCI.Request.RequestOptions} [options]
		 * Additional options to configure the request.
		 *
		 * @returns {Promise<LuCI.Response>}
		 * The resulting HTTP response.
		 */
		request: function(target, options) {
			var state = { xhr: new XMLHttpRequest(), url: this.expandURL(target), start: Date.now() },
			    opt = Object.assign({}, options, state),
			    content = null,
			    contenttype = null,
			    callback = this.handleReadyStateChange;

			return new Promise(function(resolveFn, rejectFn) {
				opt.xhr.onreadystatechange = callback.bind(opt, resolveFn, rejectFn);
				opt.method = String(opt.method || 'GET').toUpperCase();

				if ('query' in opt) {
					var q = (opt.query != null) ? Object.keys(opt.query).map(function(k) {
						if (opt.query[k] != null) {
							var v = (typeof(opt.query[k]) == 'object')
								? JSON.stringify(opt.query[k])
								: String(opt.query[k]);

							return '%s=%s'.format(encodeURIComponent(k), encodeURIComponent(v));
						}
						else {
							return encodeURIComponent(k);
						}
					}).join('&') : '';

					if (q !== '') {
						switch (opt.method) {
						case 'GET':
						case 'HEAD':
						case 'OPTIONS':
							opt.url += ((/\?/).test(opt.url) ? '&' : '?') + q;
							break;

						default:
							if (content == null) {
								content = q;
								contenttype = 'application/x-www-form-urlencoded';
							}
						}
					}
				}

				if (!opt.cache)
					opt.url += ((/\?/).test(opt.url) ? '&' : '?') + (new Date()).getTime();

				if (isQueueableRequest(opt)) {
					requestQueue.push([opt, rejectFn, resolveFn]);
					requestAnimationFrame(flushRequestQueue);
					return;
				}

				if ('username' in opt && 'password' in opt)
					opt.xhr.open(opt.method, opt.url, true, opt.username, opt.password);
				else
					opt.xhr.open(opt.method, opt.url, true);

				opt.xhr.responseType = 'text';

				if ('overrideMimeType' in opt.xhr)
					opt.xhr.overrideMimeType('application/octet-stream');

				if ('timeout' in opt)
					opt.xhr.timeout = +opt.timeout;

				if ('credentials' in opt)
					opt.xhr.withCredentials = !!opt.credentials;

				if (opt.content != null) {
					switch (typeof(opt.content)) {
					case 'function':
						content = opt.content(xhr);
						break;

					case 'object':
						if (!(opt.content instanceof FormData)) {
							content = JSON.stringify(opt.content);
							contenttype = 'application/json';
						}
						else {
							content = opt.content;
						}
						break;

					default:
						content = String(opt.content);
					}
				}

				if ('headers' in opt)
					for (var header in opt.headers)
						if (opt.headers.hasOwnProperty(header)) {
							if (header.toLowerCase() != 'content-type')
								opt.xhr.setRequestHeader(header, opt.headers[header]);
							else
								contenttype = opt.headers[header];
						}

				if ('progress' in opt && 'upload' in opt.xhr)
					opt.xhr.upload.addEventListener('progress', opt.progress);

				if (contenttype != null)
					opt.xhr.setRequestHeader('Content-Type', contenttype);

				try {
					opt.xhr.send(content);
				}
				catch (e) {
					rejectFn.call(opt, e);
				}
			});
		},

		handleReadyStateChange: function(resolveFn, rejectFn, ev) {
			var xhr = this.xhr,
			    duration = Date.now() - this.start;

			if (xhr.readyState !== 4)
				return;

			if (xhr.status === 0 && xhr.statusText === '') {
				if (duration >= this.timeout)
					rejectFn.call(this, new Error('XHR request timed out'));
				else
					rejectFn.call(this, new Error('XHR request aborted by browser'));
			}
			else {
				var response = new Response(
					xhr, xhr.responseURL || this.url, duration);

				Promise.all(Request.interceptors.map(function(fn) { return fn(response) }))
					.then(resolveFn.bind(this, response))
					.catch(rejectFn.bind(this));
			}
		},

		/**
		 * Initiate an HTTP GET request to the given target.
		 *
		 * @instance
		 * @memberof LuCI.Request
		 * @param {string} target
		 * The URL to request.
		 *
		 * @param {LuCI.Request.RequestOptions} [options]
		 * Additional options to configure the request.
		 *
		 * @returns {Promise<LuCI.Response>}
		 * The resulting HTTP response.
		 */
		get: function(url, options) {
			return this.request(url, Object.assign({ method: 'GET' }, options));
		},

		/**
		 * Initiate an HTTP POST request to the given target.
		 *
		 * @instance
		 * @memberof LuCI.Request
		 * @param {string} target
		 * The URL to request.
		 *
		 * @param {*} [data]
		 * The request data to send, see {@link LuCI.Request.RequestOptions} for details.
		 *
		 * @param {LuCI.Request.RequestOptions} [options]
		 * Additional options to configure the request.
		 *
		 * @returns {Promise<LuCI.Response>}
		 * The resulting HTTP response.
		 */
		post: function(url, data, options) {
			return this.request(url, Object.assign({ method: 'POST', content: data }, options));
		},

		/**
		 * Interceptor functions are invoked whenever an HTTP reply is received, in the order
		 * these functions have been registered.
		 * @callback LuCI.Request.interceptorFn
		 * @param {LuCI.Response} res
		 * The HTTP response object
		 */

		/**
		 * Register an HTTP response interceptor function. Interceptor
		 * functions are useful to perform default actions on incoming HTTP
		 * responses, such as checking for expired authentication or for
		 * implementing request retries before returning a failure.
		 *
		 * @instance
		 * @memberof LuCI.Request
		 * @param {LuCI.Request.interceptorFn} interceptorFn
		 * The interceptor function to register.
		 *
		 * @returns {LuCI.Request.interceptorFn}
		 * The registered function.
		 */
		addInterceptor: function(interceptorFn) {
			if (typeof(interceptorFn) == 'function')
				this.interceptors.push(interceptorFn);
			return interceptorFn;
		},

		/**
		 * Remove an HTTP response interceptor function. The passed function
		 * value must be the very same value that was used to register the
		 * function.
		 *
		 * @instance
		 * @memberof LuCI.Request
		 * @param {LuCI.Request.interceptorFn} interceptorFn
		 * The interceptor function to remove.
		 *
		 * @returns {boolean}
		 * Returns `true` if any function has been removed, else `false`.
		 */
		removeInterceptor: function(interceptorFn) {
			var oldlen = this.interceptors.length, i = oldlen;
			while (i--)
				if (this.interceptors[i] === interceptorFn)
					this.interceptors.splice(i, 1);
			return (this.interceptors.length < oldlen);
		},

		/**
		 * @class
		 * @memberof LuCI.Request
		 * @hideconstructor
		 * @classdesc
		 *
		 * The `Request.poll` class provides some convience wrappers around
		 * {@link LuCI.Poll} mainly to simplify registering repeating HTTP
		 * request calls as polling functions.
		 */
		poll: {
			/**
			 * The callback function is invoked whenever an HTTP reply to a
			 * polled request is received or when the polled request timed
			 * out.
			 *
			 * @callback LuCI.Request.poll~callbackFn
			 * @param {LuCI.Response} res
			 * The HTTP response object.
			 *
			 * @param {*} data
			 * The response JSON if the response could be parsed as such,
			 * else `null`.
			 *
			 * @param {number} duration
			 * The total duration of the request in milliseconds.
			 */

			/**
			 * Register a repeating HTTP request with an optional callback
			 * to invoke whenever a response for the request is received.
			 *
			 * @instance
			 * @memberof LuCI.Request.poll
			 * @param {number} interval
			 * The poll interval in seconds.
			 *
			 * @param {string} url
			 * The URL to request on each poll.
			 *
			 * @param {LuCI.Request.RequestOptions} [options]
			 * Additional options to configure the request.
			 *
			 * @param {LuCI.Request.poll~callbackFn} [callback]
			 * {@link LuCI.Request.poll~callbackFn Callback} function to
			 * invoke for each HTTP reply.
			 *
			 * @throws {TypeError}
			 * Throws `TypeError` when an invalid interval was passed.
			 *
			 * @returns {function}
			 * Returns the internally created poll function.
			 */
			add: function(interval, url, options, callback) {
				if (isNaN(interval) || interval <= 0)
					throw new TypeError('Invalid poll interval');

				var ival = interval >>> 0,
				    opts = Object.assign({}, options, { timeout: ival * 1000 - 5 });

				var fn = function() {
					return Request.request(url, options).then(function(res) {
						if (!Poll.active())
							return;

						try {
							callback(res, res.json(), res.duration);
						}
						catch (err) {
							callback(res, null, res.duration);
						}
					});
				};

				return (Poll.add(fn, ival) ? fn : null);
			},

			/**
			 * Remove a polling request that has been previously added using `add()`.
			 * This function is essentially a wrapper around
			 * {@link LuCI.Poll.remove LuCI.Poll.remove()}.
			 *
			 * @instance
			 * @memberof LuCI.Request.poll
			 * @param {function} entry
			 * The poll function returned by {@link LuCI.Request.poll#add add()}.
			 *
			 * @returns {boolean}
			 * Returns `true` if any function has been removed, else `false`.
			 */
			remove: function(entry) { return Poll.remove(entry) },

			/**
			  * Alias for {@link LuCI.Poll.start LuCI.Poll.start()}.
			  *
			  * @instance
			  * @memberof LuCI.Request.poll
			  */
			start: function() { return Poll.start() },

			/**
			  * Alias for {@link LuCI.Poll.stop LuCI.Poll.stop()}.
			  *
			  * @instance
			  * @memberof LuCI.Request.poll
			  */
			stop: function() { return Poll.stop() },

			/**
			  * Alias for {@link LuCI.Poll.active LuCI.Poll.active()}.
			  *
			  * @instance
			  * @memberof LuCI.Request.poll
			  */
			active: function() { return Poll.active() }
		}
	});

	/**
	 * @class
	 * @memberof LuCI
	 * @hideconstructor
	 * @classdesc
	 *
	 * The `Poll` class allows registering and unregistering poll actions,
	 * as well as starting, stopping and querying the state of the polling
	 * loop.
	 */
	var Poll = Class.singleton(/** @lends LuCI.Poll.prototype */ {
		__name__: 'LuCI.Poll',

		queue: [],

		/**
		 * Add a new operation to the polling loop. If the polling loop is not
		 * already started at this point, it will be implicitely started.
		 *
		 * @instance
		 * @memberof LuCI.Poll
		 * @param {function} fn
		 * The function to invoke on each poll interval.
		 *
		 * @param {number} interval
		 * The poll interval in seconds.
		 *
		 * @throws {TypeError}
		 * Throws `TypeError` when an invalid interval was passed.
		 *
		 * @returns {boolean}
		 * Returns `true` if the function has been added or `false` if it
		 * already is registered.
		 */
		add: function(fn, interval) {
			if (interval == null || interval <= 0)
				interval = window.L ? window.L.env.pollinterval : null;

			if (isNaN(interval) || typeof(fn) != 'function')
				throw new TypeError('Invalid argument to LuCI.Poll.add()');

			for (var i = 0; i < this.queue.length; i++)
				if (this.queue[i].fn === fn)
					return false;

			var e = {
				r: true,
				i: interval >>> 0,
				fn: fn
			};

			this.queue.push(e);

			if (this.tick != null && !this.active())
				this.start();

			return true;
		},

		/**
		 * Remove an operation from the polling loop. If no further operatons
		 * are registered, the polling loop is implicitely stopped.
		 *
		 * @instance
		 * @memberof LuCI.Poll
		 * @param {function} fn
		 * The function to remove.
		 *
		 * @throws {TypeError}
		 * Throws `TypeError` when the given argument isn't a function.
		 *
		 * @returns {boolean}
		 * Returns `true` if the function has been removed or `false` if it
		 * wasn't found.
		 */
		remove: function(fn) {
			if (typeof(fn) != 'function')
				throw new TypeError('Invalid argument to LuCI.Poll.remove()');

			var len = this.queue.length;

			for (var i = len; i > 0; i--)
				if (this.queue[i-1].fn === fn)
					this.queue.splice(i-1, 1);

			if (!this.queue.length && this.stop())
				this.tick = 0;

			return (this.queue.length != len);
		},

		/**
		 * (Re)start the polling loop. Dispatches a custom `poll-start` event
		 * to the `document` object upon successful start.
		 *
		 * @instance
		 * @memberof LuCI.Poll
		 * @returns {boolean}
		 * Returns `true` if polling has been started (or if no functions
		 * where registered) or `false` when the polling loop already runs.
		 */
		start: function() {
			if (this.active())
				return false;

			this.tick = 0;

			if (this.queue.length) {
				this.timer = window.setInterval(this.step, 1000);
				this.step();
				document.dispatchEvent(new CustomEvent('poll-start'));
			}

			return true;
		},

		/**
		 * Stop the polling loop. Dispatches a custom `poll-stop` event
		 * to the `document` object upon successful stop.
		 *
		 * @instance
		 * @memberof LuCI.Poll
		 * @returns {boolean}
		 * Returns `true` if polling has been stopped or `false` if it din't
		 * run to begin with.
		 */
		stop: function() {
			if (!this.active())
				return false;

			document.dispatchEvent(new CustomEvent('poll-stop'));
			window.clearInterval(this.timer);
			delete this.timer;
			delete this.tick;
			return true;
		},

		/* private */
		step: function() {
			for (var i = 0, e = null; (e = Poll.queue[i]) != null; i++) {
				if ((Poll.tick % e.i) != 0)
					continue;

				if (!e.r)
					continue;

				e.r = false;

				Promise.resolve(e.fn()).finally((function() { this.r = true }).bind(e));
			}

			Poll.tick = (Poll.tick + 1) % Math.pow(2, 32);
		},

		/**
		 * Test whether the polling loop is running.
		 *
		 * @instance
		 * @memberof LuCI.Poll
		 * @returns {boolean} - Returns `true` if polling is active, else `false`.
		 */
		active: function() {
			return (this.timer != null);
		}
	});


	var dummyElem = null,
	    domParser = null,
	    originalCBIInit = null,
	    rpcBaseURL = null,
	    sysFeatures = null,
	    classes = {};

	var LuCI = Class.extend(/** @lends LuCI.prototype */ {
		__name__: 'LuCI',
		__init__: function(env) {

			document.querySelectorAll('script[src*="/luci.js"]').forEach(function(s) {
				if (env.base_url == null || env.base_url == '') {
					var m = (s.getAttribute('src') || '').match(/^(.*)\/luci\.js(?:\?v=([^?]+))?$/);
					if (m) {
						env.base_url = m[1];
						env.resource_version = m[2];
					}
				}
			});

			if (env.base_url == null)
				this.error('InternalError', 'Cannot find url of luci.js');

			Object.assign(this.env, env);

			document.addEventListener('poll-start', function(ev) {
				document.querySelectorAll('[id^="xhr_poll_status"]').forEach(function(e) {
					e.style.display = (e.id == 'xhr_poll_status_off') ? 'none' : '';
				});
			});

			document.addEventListener('poll-stop', function(ev) {
				document.querySelectorAll('[id^="xhr_poll_status"]').forEach(function(e) {
					e.style.display = (e.id == 'xhr_poll_status_on') ? 'none' : '';
				});
			});

			var domReady = new Promise(function(resolveFn, rejectFn) {
				document.addEventListener('DOMContentLoaded', resolveFn);
			});

			Promise.all([
				domReady,
				this.require('ui'),
				this.require('rpc'),
				this.require('form'),
				this.probeRPCBaseURL()
			]).then(this.setupDOM.bind(this)).catch(this.error);

			originalCBIInit = window.cbi_init;
			window.cbi_init = function() {};
		},

		/**
		 * Captures the current stack trace and throws an error of the
		 * specified type as a new exception. Also logs the exception as
		 * error to the debug console if it is available.
		 *
		 * @instance
		 * @memberof LuCI
		 *
		 * @param {Error|string} [type=Error]
		 * Either a string specifying the type of the error to throw or an
		 * existing `Error` instance to copy.
		 *
		 * @param {string} [fmt=Unspecified error]
		 * A format string which is used to form the error message, together
		 * with all subsequent optional arguments.
		 *
		 * @param {...*} [args]
		 * Zero or more variable arguments to the supplied format string.
		 *
		 * @throws {Error}
		 * Throws the created error object with the captured stack trace
		 * appended to the message and the type set to the given type
		 * argument or copied from the given error instance.
		 */
		raise: function(type, fmt /*, ...*/) {
			var e = null,
			    msg = fmt ? String.prototype.format.apply(fmt, this.varargs(arguments, 2)) : null,
			    stack = null;

			if (type instanceof Error) {
				e = type;

				if (msg)
					e.message = msg + ': ' + e.message;
			}
			else {
				try { throw new Error('stacktrace') }
				catch (e2) { stack = (e2.stack || '').split(/\n/) }

				e = new (window[type || 'Error'] || Error)(msg || 'Unspecified error');
				e.name = type || 'Error';
			}

			stack = (stack || []).map(function(frame) {
				frame = frame.replace(/(.*?)@(.+):(\d+):(\d+)/g, 'at $1 ($2:$3:$4)').trim();
				return frame ? '  ' + frame : '';
			});

			if (!/^  at /.test(stack[0]))
				stack.shift();

			if (/\braise /.test(stack[0]))
				stack.shift();

			if (/\berror /.test(stack[0]))
				stack.shift();

			if (stack.length)
				e.message += '\n' + stack.join('\n');

			if (window.console && console.debug)
				console.debug(e);

			throw e;
		},

		/**
		 * A wrapper around {@link LuCI#raise raise()} which also renders
		 * the error either as modal overlay when `ui.js` is already loaed
		 * or directly into the view body.
		 *
		 * @instance
		 * @memberof LuCI
		 *
		 * @param {Error|string} [type=Error]
		 * Either a string specifying the type of the error to throw or an
		 * existing `Error` instance to copy.
		 *
		 * @param {string} [fmt=Unspecified error]
		 * A format string which is used to form the error message, together
		 * with all subsequent optional arguments.
		 *
		 * @param {...*} [args]
		 * Zero or more variable arguments to the supplied format string.
		 *
		 * @throws {Error}
		 * Throws the created error object with the captured stack trace
		 * appended to the message and the type set to the given type
		 * argument or copied from the given error instance.
		 */
		error: function(type, fmt /*, ...*/) {
			try {
				L.raise.apply(L, Array.prototype.slice.call(arguments));
			}
			catch (e) {
				if (!e.reported) {
					if (L.ui)
						L.ui.addNotification(e.name || _('Runtime error'),
							E('pre', {}, e.message), 'danger');
					else
						L.dom.content(document.querySelector('#maincontent'),
							E('pre', { 'class': 'alert-message error' }, e.message));

					e.reported = true;
				}

				throw e;
			}
		},

		/**
		 * Return a bound function using the given `self` as `this` context
		 * and any further arguments as parameters to the bound function.
		 *
		 * @instance
		 * @memberof LuCI
		 *
		 * @param {function} fn
		 * The function to bind.
		 *
		 * @param {*} self
		 * The value to bind as `this` context to the specified function.
		 *
		 * @param {...*} [args]
		 * Zero or more variable arguments which are bound to the function
		 * as parameters.
		 *
		 * @returns {function}
		 * Returns the bound function.
		 */
		bind: function(fn, self /*, ... */) {
			return Function.prototype.bind.apply(fn, this.varargs(arguments, 2, self));
		},

		/**
		 * Load an additional LuCI JavaScript class and its dependencies,
		 * instantiate it and return the resulting class instance. Each
		 * class is only loaded once. Subsequent attempts to load the same
		 * class will return the already instantiated class.
		 *
		 * @instance
		 * @memberof LuCI
		 *
		 * @param {string} name
		 * The name of the class to load in dotted notation. Dots will
		 * be replaced by spaces and joined with the runtime-determined
		 * base URL of LuCI.js to form an absolute URL to load the class
		 * file from.
		 *
		 * @throws {DependencyError}
		 * Throws a `DependencyError` when the class to load includes
		 * circular dependencies.
		 *
		 * @throws {NetworkError}
		 * Throws `NetworkError` when the underlying {@link LuCI.Request}
		 * call failed.
		 *
		 * @throws {SyntaxError}
		 * Throws `SyntaxError` when the loaded class file code cannot
		 * be interpreted by `eval`.
		 *
		 * @throws {TypeError}
		 * Throws `TypeError` when the class file could be loaded and
		 * interpreted, but when invoking its code did not yield a valid
		 * class instance.
		 *
		 * @returns {Promise<LuCI#Class>}
		 * Returns the instantiated class.
		 */
		require: function(name, from) {
			var L = this, url = null, from = from || [];

			/* Class already loaded */
			if (classes[name] != null) {
				/* Circular dependency */
				if (from.indexOf(name) != -1)
					L.raise('DependencyError',
						'Circular dependency: class "%s" depends on "%s"',
						name, from.join('" which depends on "'));

				return Promise.resolve(classes[name]);
			}

			url = '%s/%s.js%s'.format(L.env.base_url, name.replace(/\./g, '/'), (L.env.resource_version ? '?v=' + L.env.resource_version : ''));
			from = [ name ].concat(from);

			var compileClass = function(res) {
				if (!res.ok)
					L.raise('NetworkError',
						'HTTP error %d while loading class file "%s"', res.status, url);

				var source = res.text(),
				    requirematch = /^require[ \t]+(\S+)(?:[ \t]+as[ \t]+([a-zA-Z_]\S*))?$/,
				    strictmatch = /^use[ \t]+strict$/,
				    depends = [],
				    args = '';

				/* find require statements in source */
				for (var i = 0, off = -1, quote = -1, esc = false; i < source.length; i++) {
					var chr = source.charCodeAt(i);

					if (esc) {
						esc = false;
					}
					else if (chr == 92) {
						esc = true;
					}
					else if (chr == quote) {
						var s = source.substring(off, i),
						    m = requirematch.exec(s);

						if (m) {
							var dep = m[1], as = m[2] || dep.replace(/[^a-zA-Z0-9_]/g, '_');
							depends.push(L.require(dep, from));
							args += ', ' + as;
						}
						else if (!strictmatch.exec(s)) {
							break;
						}

						off = -1;
						quote = -1;
					}
					else if (quote == -1 && (chr == 34 || chr == 39)) {
						off = i + 1;
						quote = chr;
					}
				}

				/* load dependencies and instantiate class */
				return Promise.all(depends).then(function(instances) {
					var _factory, _class;

					try {
						_factory = eval(
							'(function(window, document, L%s) { %s })\n\n//# sourceURL=%s\n'
								.format(args, source, res.url));
					}
					catch (error) {
						L.raise('SyntaxError', '%s\n  in %s:%s',
							error.message, res.url, error.lineNumber || '?');
					}

					_factory.displayName = toCamelCase(name + 'ClassFactory');
					_class = _factory.apply(_factory, [window, document, L].concat(instances));

					if (!Class.isSubclass(_class))
					    L.error('TypeError', '"%s" factory yields invalid constructor', name);

					if (_class.displayName == 'AnonymousClass')
						_class.displayName = toCamelCase(name + 'Class');

					var ptr = Object.getPrototypeOf(L),
					    parts = name.split(/\./),
					    instance = new _class();

					for (var i = 0; ptr && i < parts.length - 1; i++)
						ptr = ptr[parts[i]];

					if (ptr)
						ptr[parts[i]] = instance;

					classes[name] = instance;

					return instance;
				});
			};

			/* Request class file */
			classes[name] = Request.get(url, { cache: true }).then(compileClass);

			return classes[name];
		},

		/* DOM setup */
		probeRPCBaseURL: function() {
			if (rpcBaseURL == null) {
				try {
					rpcBaseURL = window.sessionStorage.getItem('rpcBaseURL');
				}
				catch (e) { }
			}

			if (rpcBaseURL == null) {
				var rpcFallbackURL = this.url('admin/ubus');

				rpcBaseURL = Request.get('/ubus/').then(function(res) {
					return (rpcBaseURL = (res.status == 400) ? '/ubus/' : rpcFallbackURL);
				}, function() {
					return (rpcBaseURL = rpcFallbackURL);
				}).then(function(url) {
					try {
						window.sessionStorage.setItem('rpcBaseURL', url);
					}
					catch (e) { }

					return url;
				});
			}

			return Promise.resolve(rpcBaseURL);
		},

		probeSystemFeatures: function() {
			var sessionid = classes.rpc.getSessionID();

			if (sysFeatures == null) {
				try {
					var data = JSON.parse(window.sessionStorage.getItem('sysFeatures'));

					if (this.isObject(data) && this.isObject(data[sessionid]))
						sysFeatures = data[sessionid];
				}
				catch (e) {}
			}

			if (!this.isObject(sysFeatures)) {
				sysFeatures = classes.rpc.declare({
					object: 'luci',
					method: 'getFeatures',
					expect: { '': {} }
				})().then(function(features) {
					try {
						var data = {};
						    data[sessionid] = features;

						window.sessionStorage.setItem('sysFeatures', JSON.stringify(data));
					}
					catch (e) {}

					sysFeatures = features;

					return features;
				});
			}

			return Promise.resolve(sysFeatures);
		},

		/**
		 * Test whether a particular system feature is available, such as
		 * hostapd SAE support or an installed firewall. The features are
		 * queried once at the beginning of the LuCI session and cached in
		 * `SessionStorage` throughout the lifetime of the associated tab or
		 * browser window.
		 *
		 * @instance
		 * @memberof LuCI
		 *
		 * @param {string} feature
		 * The feature to test. For detailed list of known feature flags,
		 * see `/modules/luci-base/root/usr/libexec/rpcd/luci`.
		 *
		 * @param {string} [subfeature]
		 * Some feature classes like `hostapd` provide sub-feature flags,
		 * such as `sae` or `11w` support. The `subfeature` argument can
		 * be used to query these.
		 *
		 * @return {boolean|null}
		 * Return `true` if the queried feature (and sub-feature) is available
		 * or `false` if the requested feature isn't present or known.
		 * Return `null` when a sub-feature was queried for a feature which
		 * has no sub-features.
		 */
		hasSystemFeature: function() {
			var ft = sysFeatures[arguments[0]];

			if (arguments.length == 2)
				return this.isObject(ft) ? ft[arguments[1]] : null;

			return (ft != null && ft != false);
		},

		/* private */
		notifySessionExpiry: function() {
			Poll.stop();

			L.ui.showModal(_('Session expired'), [
				E('div', { class: 'alert-message warning' },
					_('A new login is required since the authentication session expired.')),
				E('div', { class: 'right' },
					E('div', {
						class: 'btn primary',
						click: function() {
							var loc = window.location;
							window.location = loc.protocol + '//' + loc.host + loc.pathname + loc.search;
						}
					}, _('To login…')))
			]);

			L.raise('SessionError', 'Login session is expired');
		},

		/* private */
		setupDOM: function(res) {
			var domEv = res[0],
			    uiClass = res[1],
			    rpcClass = res[2],
			    formClass = res[3],
			    rpcBaseURL = res[4];

			rpcClass.setBaseURL(rpcBaseURL);

			rpcClass.addInterceptor(function(msg, req) {
				if (!L.isObject(msg) || !L.isObject(msg.error) || msg.error.code != -32002)
					return;

				if (!L.isObject(req) || (req.object == 'session' && req.method == 'access'))
					return;

				return rpcClass.declare({
					'object': 'session',
					'method': 'access',
					'params': [ 'scope', 'object', 'function' ],
					'expect': { access: true }
				})('uci', 'luci', 'read').catch(L.notifySessionExpiry);
			});

			Request.addInterceptor(function(res) {
				var isDenied = false;

				if (res.status == 403 && res.headers.get('X-LuCI-Login-Required') == 'yes')
					isDenied = true;

				if (!isDenied)
					return;

				L.notifySessionExpiry();
			});

			return this.probeSystemFeatures().finally(this.initDOM);
		},

		/* private */
		initDOM: function() {
			originalCBIInit();
			Poll.start();
			document.dispatchEvent(new CustomEvent('luci-loaded'));
		},

		/**
		 * The `env` object holds environment settings used by LuCI, such
		 * as request timeouts, base URLs etc.
		 *
		 * @instance
		 * @memberof LuCI
		 */
		env: {},

		/**
		 * Construct a relative URL path from the given prefix and parts.
		 * The resulting URL is guaranteed to only contain the characters
		 * `a-z`, `A-Z`, `0-9`, `_`, `.`, `%`, `,`, `;`, and `-` as well
		 * as `/` for the path separator.
		 *
		 * @instance
		 * @memberof LuCI
		 *
		 * @param {string} [prefix]
		 * The prefix to join the given parts with. If the `prefix` is
		 * omitted, it defaults to an empty string.
		 *
		 * @param {string[]} [parts]
		 * An array of parts to join into an URL path. Parts may contain
		 * slashes and any of the other characters mentioned above.
		 *
		 * @return {string}
		 * Return the joined URL path.
		 */
		path: function(prefix, parts) {
			var url = [ prefix || '' ];

			for (var i = 0; i < parts.length; i++)
				if (/^(?:[a-zA-Z0-9_.%,;-]+\/)*[a-zA-Z0-9_.%,;-]+$/.test(parts[i]))
					url.push('/', parts[i]);

			if (url.length === 1)
				url.push('/');

			return url.join('');
		},

		/**
		 * Construct an URL  pathrelative to the script path of the server
		 * side LuCI application (usually `/cgi-bin/luci`).
		 *
		 * The resulting URL is guaranteed to only contain the characters
		 * `a-z`, `A-Z`, `0-9`, `_`, `.`, `%`, `,`, `;`, and `-` as well
		 * as `/` for the path separator.
		 *
		 * @instance
		 * @memberof LuCI
		 *
		 * @param {string[]} [parts]
		 * An array of parts to join into an URL path. Parts may contain
		 * slashes and any of the other characters mentioned above.
		 *
		 * @return {string}
		 * Returns the resulting URL path.
		 */
		url: function() {
			return this.path(this.env.scriptname, arguments);
		},

		/**
		 * Construct an URL path relative to the global static resource path
		 * of the LuCI ui (usually `/luci-static/resources`).
		 *
		 * The resulting URL is guaranteed to only contain the characters
		 * `a-z`, `A-Z`, `0-9`, `_`, `.`, `%`, `,`, `;`, and `-` as well
		 * as `/` for the path separator.
		 *
		 * @instance
		 * @memberof LuCI
		 *
		 * @param {string[]} [parts]
		 * An array of parts to join into an URL path. Parts may contain
		 * slashes and any of the other characters mentioned above.
		 *
		 * @return {string}
		 * Returns the resulting URL path.
		 */
		resource: function() {
			return this.path(this.env.resource, arguments);
		},

		/**
		 * Construct an URL path relative to the media resource path of the
		 * LuCI ui (usually `/luci-static/$theme_name`).
		 *
		 * The resulting URL is guaranteed to only contain the characters
		 * `a-z`, `A-Z`, `0-9`, `_`, `.`, `%`, `,`, `;`, and `-` as well
		 * as `/` for the path separator.
		 *
		 * @instance
		 * @memberof LuCI
		 *
		 * @param {string[]} [parts]
		 * An array of parts to join into an URL path. Parts may contain
		 * slashes and any of the other characters mentioned above.
		 *
		 * @return {string}
		 * Returns the resulting URL path.
		 */
		media: function() {
			return this.path(this.env.media, arguments);
		},

		/**
		 * Return the complete URL path to the current view.
		 *
		 * @instance
		 * @memberof LuCI
		 *
		 * @return {string}
		 * Returns the URL path to the current view.
		 */
		location: function() {
			return this.path(this.env.scriptname, this.env.requestpath);
		},


		/**
		 * Tests whether the passed argument is a JavaScript object.
		 * This function is meant to be an object counterpart to the
		 * standard `Array.isArray()` function.
		 *
		 * @instance
		 * @memberof LuCI
		 *
		 * @param {*} [val]
		 * The value to test
		 *
		 * @return {boolean}
		 * Returns `true` if the given value is of type object and
		 * not `null`, else returns `false`.
		 */
		isObject: function(val) {
			return (val != null && typeof(val) == 'object');
		},

		/**
		 * Return an array of sorted object keys, optionally sorted by
		 * a different key or a different sorting mode.
		 *
		 * @instance
		 * @memberof LuCI
		 *
		 * @param {object} obj
		 * The object to extract the keys from. If the given value is
		 * not an object, the function will return an empty array.
		 *
		 * @param {string} [key]
		 * Specifies the key to order by. This is mainly useful for
		 * nested objects of objects or objects of arrays when sorting
		 * shall not be performed by the primary object keys but by
		 * some other key pointing to a value within the nested values.
		 *
		 * @param {string} [sortmode]
		 * May be either `addr` or `num` to override the natural
		 * lexicographic sorting with a sorting suitable for IP/MAC style
		 * addresses or numeric values respectively.
		 *
		 * @return {string[]}
		 * Returns an array containing the sorted keys of the given object.
		 */
		sortedKeys: function(obj, key, sortmode) {
			if (obj == null || typeof(obj) != 'object')
				return [];

			return Object.keys(obj).map(function(e) {
				var v = (key != null) ? obj[e][key] : e;

				switch (sortmode) {
				case 'addr':
					v = (v != null) ? v.replace(/(?:^|[.:])([0-9a-fA-F]{1,4})/g,
						function(m0, m1) { return ('000' + m1.toLowerCase()).substr(-4) }) : null;
					break;

				case 'num':
					v = (v != null) ? +v : null;
					break;
				}

				return [ e, v ];
			}).filter(function(e) {
				return (e[1] != null);
			}).sort(function(a, b) {
				return (a[1] > b[1]);
			}).map(function(e) {
				return e[0];
			});
		},

		/**
		 * Converts the given value to an array. If the given value is of
		 * type array, it is returned as-is, values of type object are
		 * returned as one-element array containing the object, empty
		 * strings and `null` values are returned as empty array, all other
		 * values are converted using `String()`, trimmed, split on white
		 * space and returned as array.
		 *
		 * @instance
		 * @memberof LuCI
		 *
		 * @param {*} val
		 * The value to convert into an array.
		 *
		 * @return {Array<*>}
		 * Returns the resulting array.
		 */
		toArray: function(val) {
			if (val == null)
				return [];
			else if (Array.isArray(val))
				return val;
			else if (typeof(val) == 'object')
				return [ val ];

			var s = String(val).trim();

			if (s == '')
				return [];

			return s.split(/\s+/);
		},

		/**
		 * Returns a promise resolving with either the given value or or with
		 * the given default in case the input value is a rejecting promise.
		 *
		 * @instance
		 * @memberof LuCI
		 *
		 * @param {*} value
		 * The value to resolve the promise with.
		 *
		 * @param {*} defvalue
		 * The default value to resolve the promise with in case the given
		 * input value is a rejecting promise.
		 *
		 * @returns {Promise<*>}
		 * Returns a new promise resolving either to the given input value or
		 * to the given default value on error.
		 */
		resolveDefault: function(value, defvalue) {
			return Promise.resolve(value).catch(function() { return defvalue });
		},

		/**
		 * The request callback function is invoked whenever an HTTP
		 * reply to a request made using the `L.get()`, `L.post()` or
		 * `L.poll()` function is timed out or received successfully.
		 *
		 * @instance
		 * @memberof LuCI
		 *
		 * @callback LuCI.requestCallbackFn
		 * @param {XMLHTTPRequest} xhr
		 * The XMLHTTPRequest instance used to make the request.
		 *
		 * @param {*} data
		 * The response JSON if the response could be parsed as such,
		 * else `null`.
		 *
		 * @param {number} duration
		 * The total duration of the request in milliseconds.
		 */

		/**
		 * Issues a GET request to the given url and invokes the specified
		 * callback function. The function is a wrapper around
		 * {@link LuCI.Request#request Request.request()}.
		 *
		 * @deprecated
		 * @instance
		 * @memberof LuCI
		 *
		 * @param {string} url
		 * The URL to request.
		 *
		 * @param {Object<string, string>} [args]
		 * Additional query string arguments to append to the URL.
		 *
		 * @param {LuCI.requestCallbackFn} cb
		 * The callback function to invoke when the request finishes.
		 *
		 * @return {Promise<null>}
		 * Returns a promise resolving to `null` when concluded.
		 */
		get: function(url, args, cb) {
			return this.poll(null, url, args, cb, false);
		},

		/**
		 * Issues a POST request to the given url and invokes the specified
		 * callback function. The function is a wrapper around
		 * {@link LuCI.Request#request Request.request()}. The request is
		 * sent using `application/x-www-form-urlencoded` encoding and will
		 * contain a field `token` with the current value of `LuCI.env.token`
		 * by default.
		 *
		 * @deprecated
		 * @instance
		 * @memberof LuCI
		 *
		 * @param {string} url
		 * The URL to request.
		 *
		 * @param {Object<string, string>} [args]
		 * Additional post arguments to append to the request body.
		 *
		 * @param {LuCI.requestCallbackFn} cb
		 * The callback function to invoke when the request finishes.
		 *
		 * @return {Promise<null>}
		 * Returns a promise resolving to `null` when concluded.
		 */
		post: function(url, args, cb) {
			return this.poll(null, url, args, cb, true);
		},

		/**
		 * Register a polling HTTP request that invokes the specified
		 * callback function. The function is a wrapper around
		 * {@link LuCI.Request.poll#add Request.poll.add()}.
		 *
		 * @deprecated
		 * @instance
		 * @memberof LuCI
		 *
		 * @param {number} interval
		 * The poll interval to use. If set to a value less than or equal
		 * to `0`, it will default to the global poll interval configured
		 * in `LuCI.env.pollinterval`.
		 *
		 * @param {string} url
		 * The URL to request.
		 *
		 * @param {Object<string, string>} [args]
		 * Specifies additional arguments for the request. For GET requests,
		 * the arguments are appended to the URL as query string, for POST
		 * requests, they'll be added to the request body.
		 *
		 * @param {LuCI.requestCallbackFn} cb
		 * The callback function to invoke whenever a request finishes.
		 *
		 * @param {boolean} [post=false]
		 * When set to `false` or not specified, poll requests will be made
		 * using the GET method. When set to `true`, POST requests will be
		 * issued. In case of POST requests, the request body will contain
		 * an argument `token` with the current value of `LuCI.env.token` by
		 * default, regardless of the parameters specified with `args`.
		 *
		 * @return {function}
		 * Returns the internally created function that has been passed to
		 * {@link LuCI.Request.poll#add Request.poll.add()}. This value can
		 * be passed to {@link LuCI.Poll.remove Poll.remove()} to remove the
		 * polling request.
		 */
		poll: function(interval, url, args, cb, post) {
			if (interval !== null && interval <= 0)
				interval = this.env.pollinterval;

			var data = post ? { token: this.env.token } : null,
			    method = post ? 'POST' : 'GET';

			if (!/^(?:\/|\S+:\/\/)/.test(url))
				url = this.url(url);

			if (args != null)
				data = Object.assign(data || {}, args);

			if (interval !== null)
				return Request.poll.add(interval, url, { method: method, query: data }, cb);
			else
				return Request.request(url, { method: method, query: data })
					.then(function(res) {
						var json = null;
						if (/^application\/json\b/.test(res.headers.get('Content-Type')))
							try { json = res.json() } catch(e) {}
						cb(res.xhr, json, res.duration);
					});
		},

		/**
		 * Deprecated wrapper around {@link LuCI.Poll.remove Poll.remove()}.
		 *
		 * @deprecated
		 * @instance
		 * @memberof LuCI
		 *
		 * @param {function} entry
		 * The polling function to remove.
		 *
		 * @return {boolean}
		 * Returns `true` when the function has been removed or `false` if
		 * it could not be found.
		 */
		stop: function(entry) { return Poll.remove(entry) },

		/**
		 * Deprecated wrapper around {@link LuCI.Poll.stop Poll.stop()}.
		 *
		 * @deprecated
		 * @instance
		 * @memberof LuCI
		 *
		 * @return {boolean}
		 * Returns `true` when the polling loop has been stopped or `false`
		 * when it didn't run to begin with.
		 */
		halt: function() { return Poll.stop() },

		/**
		 * Deprecated wrapper around {@link LuCI.Poll.start Poll.start()}.
		 *
		 * @deprecated
		 * @instance
		 * @memberof LuCI
		 *
		 * @return {boolean}
		 * Returns `true` when the polling loop has been started or `false`
		 * when it was already running.
		 */
		run: function() { return Poll.start() },


		/**
		 * @class
		 * @memberof LuCI
		 * @hideconstructor
		 * @classdesc
		 *
		 * The `dom` class provides convenience method for creating and
		 * manipulating DOM elements.
		 */
		dom: Class.singleton(/* @lends LuCI.dom.prototype */ {
			__name__: 'LuCI.DOM',

			/**
			 * Tests whether the given argument is a valid DOM `Node`.
			 *
			 * @instance
			 * @memberof LuCI.dom
			 * @param {*} e
			 * The value to test.
			 *
			 * @returns {boolean}
			 * Returns `true` if the value is a DOM `Node`, else `false`.
			 */
			elem: function(e) {
				return (e != null && typeof(e) == 'object' && 'nodeType' in e);
			},

			/**
			 * Parses a given string as HTML and returns the first child node.
			 *
			 * @instance
			 * @memberof LuCI.dom
			 * @param {string} s
			 * A string containing an HTML fragment to parse. Note that only
			 * the first result of the resulting structure is returned, so an
			 * input value of `<div>foo</div> <div>bar</div>` will only return
			 * the first `div` element node.
			 *
			 * @returns {Node}
			 * Returns the first DOM `Node` extracted from the HTML fragment or
			 * `null` on parsing failures or if no element could be found.
			 */
			parse: function(s) {
				var elem;

				try {
					domParser = domParser || new DOMParser();
					elem = domParser.parseFromString(s, 'text/html').body.firstChild;
				}
				catch(e) {}

				if (!elem) {
					try {
						dummyElem = dummyElem || document.createElement('div');
						dummyElem.innerHTML = s;
						elem = dummyElem.firstChild;
					}
					catch (e) {}
				}

				return elem || null;
			},

			/**
			 * Tests whether a given `Node` matches the given query selector.
			 *
			 * This function is a convenience wrapper around the standard
			 * `Node.matches("selector")` function with the added benefit that
			 * the `node` argument may be a non-`Node` value, in which case
			 * this function simply returns `false`.
			 *
			 * @instance
			 * @memberof LuCI.dom
			 * @param {*} node
			 * The `Node` argument to test the selector against.
			 *
			 * @param {string} [selector]
			 * The query selector expression to test against the given node.
			 *
			 * @returns {boolean}
			 * Returns `true` if the given node matches the specified selector
			 * or `false` when the node argument is no valid DOM `Node` or the
			 * selector didn't match.
			 */
			matches: function(node, selector) {
				var m = this.elem(node) ? node.matches || node.msMatchesSelector : null;
				return m ? m.call(node, selector) : false;
			},

			/**
			 * Returns the closest parent node that matches the given query
			 * selector expression.
			 *
			 * This function is a convenience wrapper around the standard
			 * `Node.closest("selector")` function with the added benefit that
			 * the `node` argument may be a non-`Node` value, in which case
			 * this function simply returns `null`.
			 *
			 * @instance
			 * @memberof LuCI.dom
			 * @param {*} node
			 * The `Node` argument to find the closest parent for.
			 *
			 * @param {string} [selector]
			 * The query selector expression to test against each parent.
			 *
			 * @returns {Node|null}
			 * Returns the closest parent node matching the selector or
			 * `null` when the node argument is no valid DOM `Node` or the
			 * selector didn't match any parent.
			 */
			parent: function(node, selector) {
				if (this.elem(node) && node.closest)
					return node.closest(selector);

				while (this.elem(node))
					if (this.matches(node, selector))
						return node;
					else
						node = node.parentNode;

				return null;
			},

			/**
			 * Appends the given children data to the given node.
			 *
			 * @instance
			 * @memberof LuCI.dom
			 * @param {*} node
			 * The `Node` argument to append the children to.
			 *
			 * @param {*} [children]
			 * The childrens to append to the given node.
			 *
			 * When `children` is an array, then each item of the array
			 * will be either appended as child element or text node,
			 * depending on whether the item is a DOM `Node` instance or
			 * some other non-`null` value. Non-`Node`, non-`null` values
			 * will be converted to strings first before being passed as
			 * argument to `createTextNode()`.
			 *
			 * When `children` is a function, it will be invoked with
			 * the passed `node` argument as sole parameter and the `append`
			 * function will be invoked again, with the given `node` argument
			 * as first and the return value of the `children` function as
			 * second parameter.
			 *
			 * When `children` is is a DOM `Node` instance, it will be
			 * appended to the given `node`.
			 *
			 * When `children` is any other non-`null` value, it will be
			 * converted to a string and appened to the `innerHTML` property
			 * of the given `node`.
			 *
			 * @returns {Node|null}
			 * Returns the last children `Node` appended to the node or `null`
			 * if either the `node` argument was no valid DOM `node` or if the
			 * `children` was `null` or didn't result in further DOM nodes.
			 */
			append: function(node, children) {
				if (!this.elem(node))
					return null;

				if (Array.isArray(children)) {
					for (var i = 0; i < children.length; i++)
						if (this.elem(children[i]))
							node.appendChild(children[i]);
						else if (children !== null && children !== undefined)
							node.appendChild(document.createTextNode('' + children[i]));

					return node.lastChild;
				}
				else if (typeof(children) === 'function') {
					return this.append(node, children(node));
				}
				else if (this.elem(children)) {
					return node.appendChild(children);
				}
				else if (children !== null && children !== undefined) {
					node.innerHTML = '' + children;
					return node.lastChild;
				}

				return null;
			},

			/**
			 * Replaces the content of the given node with the given children.
			 *
			 * This function first removes any children of the given DOM
			 * `Node` and then adds the given given children following the
			 * rules outlined below.
			 *
			 * @instance
			 * @memberof LuCI.dom
			 * @param {*} node
			 * The `Node` argument to replace the children of.
			 *
			 * @param {*} [children]
			 * The childrens to replace into the given node.
			 *
			 * When `children` is an array, then each item of the array
			 * will be either appended as child element or text node,
			 * depending on whether the item is a DOM `Node` instance or
			 * some other non-`null` value. Non-`Node`, non-`null` values
			 * will be converted to strings first before being passed as
			 * argument to `createTextNode()`.
			 *
			 * When `children` is a function, it will be invoked with
			 * the passed `node` argument as sole parameter and the `append`
			 * function will be invoked again, with the given `node` argument
			 * as first and the return value of the `children` function as
			 * second parameter.
			 *
			 * When `children` is is a DOM `Node` instance, it will be
			 * appended to the given `node`.
			 *
			 * When `children` is any other non-`null` value, it will be
			 * converted to a string and appened to the `innerHTML` property
			 * of the given `node`.
			 *
			 * @returns {Node|null}
			 * Returns the last children `Node` appended to the node or `null`
			 * if either the `node` argument was no valid DOM `node` or if the
			 * `children` was `null` or didn't result in further DOM nodes.
			 */
			content: function(node, children) {
				if (!this.elem(node))
					return null;

				var dataNodes = node.querySelectorAll('[data-idref]');

				for (var i = 0; i < dataNodes.length; i++)
					delete this.registry[dataNodes[i].getAttribute('data-idref')];

				while (node.firstChild)
					node.removeChild(node.firstChild);

				return this.append(node, children);
			},

			/**
			 * Sets attributes or registers event listeners on element nodes.
			 *
			 * @instance
			 * @memberof LuCI.dom
			 * @param {*} node
			 * The `Node` argument to set the attributes or add the event
			 * listeners for. When the given `node` value is not a valid
			 * DOM `Node`, the function returns and does nothing.
			 *
			 * @param {string|Object<string, *>} key
			 * Specifies either the attribute or event handler name to use,
			 * or an object containing multiple key, value pairs which are
			 * each added to the node as either attribute or event handler,
			 * depending on the respective value.
			 *
			 * @param {*} [val]
			 * Specifies the attribute value or event handler function to add.
			 * If the `key` parameter is an `Object`, this parameter will be
			 * ignored.
			 *
			 * When `val` is of type function, it will be registered as event
			 * handler on the given `node` with the `key` parameter being the
			 * event name.
			 *
			 * When `val` is of type object, it will be serialized as JSON and
			 * added as attribute to the given `node`, using the given `key`
			 * as attribute name.
			 *
			 * When `val` is of any other type, it will be added as attribute
			 * to the given `node` as-is, with the underlying `setAttribute()`
			 * call implicitely turning it into a string.
			 */
			attr: function(node, key, val) {
				if (!this.elem(node))
					return null;

				var attr = null;

				if (typeof(key) === 'object' && key !== null)
					attr = key;
				else if (typeof(key) === 'string')
					attr = {}, attr[key] = val;

				for (key in attr) {
					if (!attr.hasOwnProperty(key) || attr[key] == null)
						continue;

					switch (typeof(attr[key])) {
					case 'function':
						node.addEventListener(key, attr[key]);
						break;

					case 'object':
						node.setAttribute(key, JSON.stringify(attr[key]));
						break;

					default:
						node.setAttribute(key, attr[key]);
					}
				}
			},

			/**
			 * Creates a new DOM `Node` from the given `html`, `attr` and
			 * `data` parameters.
			 *
			 * This function has multiple signatures, it can be either invoked
			 * in the form `create(html[, attr[, data]])` or in the form
			 * `create(html[, data])`. The used variant is determined from the
			 * type of the second argument.
			 *
			 * @instance
			 * @memberof LuCI.dom
			 * @param {*} html
			 * Describes the node to create.
			 *
			 * When the value of `html` is of type array, a `DocumentFragment`
			 * node is created and each item of the array is first converted
			 * to a DOM `Node` by passing it through `create()` and then added
			 * as child to the fragment.
			 *
			 * When the value of `html` is a DOM `Node` instance, no new
			 * element will be created but the node will be used as-is.
			 *
			 * When the value of `html` is a string starting with `<`, it will
			 * be passed to `dom.parse()` and the resulting value is used.
			 *
			 * When the value of `html` is any other string, it will be passed
			 * to `document.createElement()` for creating a new DOM `Node` of
			 * the given name.
			 *
			 * @param {Object<string, *>} [attr]
			 * Specifies an Object of key, value pairs to set as attributes
			 * or event handlers on the created node. Refer to
			 * {@link LuCI.dom#attr dom.attr()} for details.
			 *
			 * @param {*} [data]
			 * Specifies children to append to the newly created element.
			 * Refer to {@link LuCI.dom#append dom.append()} for details.
			 *
			 * @throws {InvalidCharacterError}
			 * Throws an `InvalidCharacterError` when the given `html`
			 * argument contained malformed markup (such as not escaped
			 * `&` characters in XHTML mode) or when the given node name
			 * in `html` contains characters which are not legal in DOM
			 * element names, such as spaces.
			 *
			 * @returns {Node}
			 * Returns the newly created `Node`.
			 */
			create: function() {
				var html = arguments[0],
				    attr = arguments[1],
				    data = arguments[2],
				    elem;

				if (!(attr instanceof Object) || Array.isArray(attr))
					data = attr, attr = null;

				if (Array.isArray(html)) {
					elem = document.createDocumentFragment();
					for (var i = 0; i < html.length; i++)
						elem.appendChild(this.create(html[i]));
				}
				else if (this.elem(html)) {
					elem = html;
				}
				else if (html.charCodeAt(0) === 60) {
					elem = this.parse(html);
				}
				else {
					elem = document.createElement(html);
				}

				if (!elem)
					return null;

				this.attr(elem, attr);
				this.append(elem, data);

				return elem;
			},

			registry: {},

			/**
			 * Attaches or detaches arbitrary data to and from a DOM `Node`.
			 *
			 * This function is useful to attach non-string values or runtime
			 * data that is not serializable to DOM nodes. To decouple data
			 * from the DOM, values are not added directly to nodes, but
			 * inserted into a registry instead which is then referenced by a
			 * string key stored as `data-idref` attribute in the node.
			 *
			 * This function has multiple signatures and is sensitive to the
			 * number of arguments passed to it.
			 *
			 *  - `dom.data(node)` -
			 *     Fetches all data associated with the given node.
			 *  - `dom.data(node, key)` -
			 *     Fetches a specific key associated with the given node.
			 *  - `dom.data(node, key, val)` -
			 *     Sets a specific key to the given value associated with the
			 *     given node.
			 *  - `dom.data(node, null)` -
			 *     Clears any data associated with the node.
			 *  - `dom.data(node, key, null)` -
			 *     Clears the given key associated with the node.
			 *
			 * @instance
			 * @memberof LuCI.dom
			 * @param {Node} node
			 * The DOM `Node` instance to set or retrieve the data for.
			 *
			 * @param {string|null} [key]
			 * This is either a string specifying the key to retrieve, or
			 * `null` to unset the entire node data.
			 *
			 * @param {*|null} [val]
			 * This is either a non-`null` value to set for a given key or
			 * `null` to remove the given `key` from the specified node.
			 *
			 * @returns {*}
			 * Returns the get or set value, or `null` when no value could
			 * be found.
			 */
			data: function(node, key, val) {
				if (!node || !node.getAttribute)
					return null;

				var id = node.getAttribute('data-idref');

				/* clear all data */
				if (arguments.length > 1 && key == null) {
					if (id != null) {
						node.removeAttribute('data-idref');
						val = this.registry[id]
						delete this.registry[id];
						return val;
					}

					return null;
				}

				/* clear a key */
				else if (arguments.length > 2 && key != null && val == null) {
					if (id != null) {
						val = this.registry[id][key];
						delete this.registry[id][key];
						return val;
					}

					return null;
				}

				/* set a key */
				else if (arguments.length > 2 && key != null && val != null) {
					if (id == null) {
						do { id = Math.floor(Math.random() * 0xffffffff).toString(16) }
						while (this.registry.hasOwnProperty(id));

						node.setAttribute('data-idref', id);
						this.registry[id] = {};
					}

					return (this.registry[id][key] = val);
				}

				/* get all data */
				else if (arguments.length == 1) {
					if (id != null)
						return this.registry[id];

					return null;
				}

				/* get a key */
				else if (arguments.length == 2) {
					if (id != null)
						return this.registry[id][key];
				}

				return null;
			},

			/**
			 * Binds the given class instance ot the specified DOM `Node`.
			 *
			 * This function uses the `dom.data()` facility to attach the
			 * passed instance of a Class to a node. This is needed for
			 * complex widget elements or similar where the corresponding
			 * class instance responsible for the element must be retrieved
			 * from DOM nodes obtained by `querySelector()` or similar means.
			 *
			 * @instance
			 * @memberof LuCI.dom
			 * @param {Node} node
			 * The DOM `Node` instance to bind the class to.
			 *
			 * @param {Class} inst
			 * The Class instance to bind to the node.
			 *
			 * @throws {TypeError}
			 * Throws a `TypeError` when the given instance argument isn't
			 * a valid Class instance.
			 *
			 * @returns {Class}
			 * Returns the bound class instance.
			 */
			bindClassInstance: function(node, inst) {
				if (!(inst instanceof Class))
					L.error('TypeError', 'Argument must be a class instance');

				return this.data(node, '_class', inst);
			},

			/**
			 * Finds a bound class instance on the given node itself or the
			 * first bound instance on its closest parent node.
			 *
			 * @instance
			 * @memberof LuCI.dom
			 * @param {Node} node
			 * The DOM `Node` instance to start from.
			 *
			 * @returns {Class|null}
			 * Returns the founds class instance if any or `null` if no bound
			 * class could be found on the node itself or any of its parents.
			 */
			findClassInstance: function(node) {
				var inst = null;

				do {
					inst = this.data(node, '_class');
					node = node.parentNode;
				}
				while (!(inst instanceof Class) && node != null);

				return inst;
			},

			/**
			 * Finds a bound class instance on the given node itself or the
			 * first bound instance on its closest parent node and invokes
			 * the specified method name on the found class instance.
			 *
			 * @instance
			 * @memberof LuCI.dom
			 * @param {Node} node
			 * The DOM `Node` instance to start from.
			 *
			 * @param {string} method
			 * The name of the method to invoke on the found class instance.
			 *
			 * @param {...*} params
			 * Additional arguments to pass to the invoked method as-is.
			 *
			 * @returns {*|null}
			 * Returns the return value of the invoked method if a class
			 * instance and method has been found. Returns `null` if either
			 * no bound class instance could be found, or if the found
			 * instance didn't have the requested `method`.
			 */
			callClassMethod: function(node, method /*, ... */) {
				var inst = this.findClassInstance(node);

				if (inst == null || typeof(inst[method]) != 'function')
					return null;

				return inst[method].apply(inst, inst.varargs(arguments, 2));
			},

			/**
			 * The ignore callback function is invoked by `isEmpty()` for each
			 * child node to decide whether to ignore a child node or not.
			 *
			 * When this function returns `false`, the node passed to it is
			 * ignored, else not.
			 *
			 * @callback LuCI.dom~ignoreCallbackFn
			 * @param {Node} node
			 * The child node to test.
			 *
			 * @returns {boolean}
			 * Boolean indicating whether to ignore the node or not.
			 */

			/**
			 * Tests whether a given DOM `Node` instance is empty or appears
			 * empty.
			 *
			 * Any element child nodes which have the CSS class `hidden` set
			 * or for which the optionally passed `ignoreFn` callback function
			 * returns `false` are ignored.
			 *
			 * @instance
			 * @memberof LuCI.dom
			 * @param {Node} node
			 * The DOM `Node` instance to test.
			 *
			 * @param {LuCI.dom~ignoreCallbackFn} [ignoreFn]
			 * Specifies an optional function which is invoked for each child
			 * node to decide whether the child node should be ignored or not.
			 *
			 * @returns {boolean}
			 * Returns `true` if the node does not have any children or if
			 * any children node either has a `hidden` CSS class or a `false`
			 * result when testing it using the given `ignoreFn`.
			 */
			isEmpty: function(node, ignoreFn) {
				for (var child = node.firstElementChild; child != null; child = child.nextElementSibling)
					if (!child.classList.contains('hidden') && (!ignoreFn || !ignoreFn(child)))
						return false;

				return true;
			}
		}),

		Poll: Poll,
		Class: Class,
		Request: Request,

		/**
		 * @class
		 * @memberof LuCI
		 * @hideconstructor
		 * @classdesc
		 *
		 * The `view` class forms the basis of views and provides a standard
		 * set of methods to inherit from.
		 */
		view: Class.extend(/* @lends LuCI.view.prototype */ {
			__name__: 'LuCI.View',

			__init__: function() {
				var vp = document.getElementById('view');

				L.dom.content(vp, E('div', { 'class': 'spinning' }, _('Loading view…')));

				return Promise.resolve(this.load())
					.then(L.bind(this.render, this))
					.then(L.bind(function(nodes) {
						var vp = document.getElementById('view');

						L.dom.content(vp, nodes);
						L.dom.append(vp, this.addFooter());
					}, this)).catch(L.error);
			},

			/**
			 * The load function is invoked before the view is rendered.
			 *
			 * The invocation of this function is wrapped by
			 * `Promise.resolve()` so it may return Promises if needed.
			 *
			 * The return value of the function (or the resolved values
			 * of the promise returned by it) will be passed as first
			 * argument to `render()`.
			 *
			 * This function is supposed to be overwritten by subclasses,
			 * the default implementation does nothing.
			 *
			 * @instance
			 * @abstract
			 * @memberof LuCI.view
			 *
			 * @returns {*|Promise<*>}
			 * May return any value or a Promise resolving to any value.
			 */
			load: function() {},

			/**
			 * The render function is invoked after the
			 * {@link LuCI.view#load load()} function and responsible
			 * for setting up the view contents. It must return a DOM
			 * `Node` or `DocumentFragment` holding the contents to
			 * insert into the view area.
			 *
			 * The invocation of this function is wrapped by
			 * `Promise.resolve()` so it may return Promises if needed.
			 *
			 * The return value of the function (or the resolved values
			 * of the promise returned by it) will be inserted into the
			 * main content area using
			 * {@link LuCI.dom#append dom.append()}.
			 *
			 * This function is supposed to be overwritten by subclasses,
			 * the default implementation does nothing.
			 *
			 * @instance
			 * @abstract
			 * @memberof LuCI.view
			 * @param {*|null} load_results
			 * This function will receive the return value of the
			 * {@link LuCI.view#load view.load()} function as first
			 * argument.
			 *
			 * @returns {Node|Promise<Node>}
			 * Should return a DOM `Node` value or a `Promise` resolving
			 * to a `Node` value.
			 */
			render: function() {},

			/**
			 * The handleSave function is invoked when the user clicks
			 * the `Save` button in the page action footer.
			 *
			 * The default implementation should be sufficient for most
			 * views using {@link form#Map form.Map()} based forms - it
			 * will iterate all forms present in the view and invoke
			 * the {@link form#Map#save Map.save()} method on each form.
			 *
			 * Views not using `Map` instances or requiring other special
			 * logic should overwrite `handleSave()` with a custom
			 * implementation.
			 *
			 * To disable the `Save` page footer button, views extending
			 * this base class should overwrite the `handleSave` function
			 * with `null`.
			 *
			 * The invocation of this function is wrapped by
			 * `Promise.resolve()` so it may return Promises if needed.
			 *
			 * @instance
			 * @memberof LuCI.view
			 * @param {Event} ev
			 * The DOM event that triggered the function.
			 *
			 * @returns {*|Promise<*>}
			 * Any return values of this function are discarded, but
			 * passed through `Promise.resolve()` to ensure that any
			 * returned promise runs to completion before the button
			 * is reenabled.
			 */
			handleSave: function(ev) {
				var tasks = [];

				document.getElementById('maincontent')
					.querySelectorAll('.cbi-map').forEach(function(map) {
						tasks.push(L.dom.callClassMethod(map, 'save'));
					});

				return Promise.all(tasks);
			},

			/**
			 * The handleSaveApply function is invoked when the user clicks
			 * the `Save & Apply` button in the page action footer.
			 *
			 * The default implementation should be sufficient for most
			 * views using {@link form#Map form.Map()} based forms - it
			 * will first invoke
			 * {@link LuCI.view.handleSave view.handleSave()} and then
			 * call {@link ui#changes#apply ui.changes.apply()} to start the
			 * modal config apply and page reload flow.
			 *
			 * Views not using `Map` instances or requiring other special
			 * logic should overwrite `handleSaveApply()` with a custom
			 * implementation.
			 *
			 * To disable the `Save & Apply` page footer button, views
			 * extending this base class should overwrite the
			 * `handleSaveApply` function with `null`.
			 *
			 * The invocation of this function is wrapped by
			 * `Promise.resolve()` so it may return Promises if needed.
			 *
			 * @instance
			 * @memberof LuCI.view
			 * @param {Event} ev
			 * The DOM event that triggered the function.
			 *
			 * @returns {*|Promise<*>}
			 * Any return values of this function are discarded, but
			 * passed through `Promise.resolve()` to ensure that any
			 * returned promise runs to completion before the button
			 * is reenabled.
			 */
			handleSaveApply: function(ev, mode) {
				return this.handleSave(ev).then(function() {
					L.ui.changes.apply(mode == '0');
				});
			},

			/**
			 * The handleReset function is invoked when the user clicks
			 * the `Reset` button in the page action footer.
			 *
			 * The default implementation should be sufficient for most
			 * views using {@link form#Map form.Map()} based forms - it
			 * will iterate all forms present in the view and invoke
			 * the {@link form#Map#save Map.reset()} method on each form.
			 *
			 * Views not using `Map` instances or requiring other special
			 * logic should overwrite `handleReset()` with a custom
			 * implementation.
			 *
			 * To disable the `Reset` page footer button, views extending
			 * this base class should overwrite the `handleReset` function
			 * with `null`.
			 *
			 * The invocation of this function is wrapped by
			 * `Promise.resolve()` so it may return Promises if needed.
			 *
			 * @instance
			 * @memberof LuCI.view
			 * @param {Event} ev
			 * The DOM event that triggered the function.
			 *
			 * @returns {*|Promise<*>}
			 * Any return values of this function are discarded, but
			 * passed through `Promise.resolve()` to ensure that any
			 * returned promise runs to completion before the button
			 * is reenabled.
			 */
			handleReset: function(ev) {
				var tasks = [];

				document.getElementById('maincontent')
					.querySelectorAll('.cbi-map').forEach(function(map) {
						tasks.push(L.dom.callClassMethod(map, 'reset'));
					});

				return Promise.all(tasks);
			},

			/**
			 * Renders a standard page action footer if any of the
			 * `handleSave()`, `handleSaveApply()` or `handleReset()`
			 * functions are defined.
			 *
			 * The default implementation should be sufficient for most
			 * views - it will render a standard page footer with action
			 * buttons labeled `Save`, `Save & Apply` and `Reset`
			 * triggering the `handleSave()`, `handleSaveApply()` and
			 * `handleReset()` functions respectively.
			 *
			 * When any of these `handle*()` functions is overwritten
			 * with `null` by a view extending this class, the
			 * corresponding button will not be rendered.
			 *
			 * @instance
			 * @memberof LuCI.view
			 * @returns {DocumentFragment}
			 * Returns a `DocumentFragment` containing the footer bar
			 * with buttons for each corresponding `handle*()` action
			 * or an empty `DocumentFragment` if all three `handle*()`
			 * methods are overwritten with `null`.
			 */
			addFooter: function() {
				var footer = E([]);

				var saveApplyBtn = this.handleSaveApply ? new L.ui.ComboButton('0', {
					0: [ _('Save & Apply') ],
					1: [ _('Apply unchecked') ]
				}, {
					classes: {
						0: 'cbi-button cbi-button-apply important',
						1: 'cbi-button cbi-button-negative important'
					},
					click: L.ui.createHandlerFn(this, 'handleSaveApply')
				}).render() : E([]);

				if (this.handleSaveApply || this.handleSave || this.handleReset) {
					footer.appendChild(E('div', { 'class': 'cbi-page-actions' }, [
						saveApplyBtn, ' ',
						this.handleSave ? E('button', {
							'class': 'cbi-button cbi-button-save',
							'click': L.ui.createHandlerFn(this, 'handleSave')
						}, [ _('Save') ]) : '', ' ',
						this.handleReset ? E('button', {
							'class': 'cbi-button cbi-button-reset',
							'click': L.ui.createHandlerFn(this, 'handleReset')
						}, [ _('Reset') ]) : ''
					]));
				}

				return footer;
			}
		})
	});

	/**
	 * @class
	 * @memberof LuCI
	 * @deprecated
	 * @classdesc
	 *
	 * The `LuCI.XHR` class is a legacy compatibility shim for the
	 * functionality formerly provided by `xhr.js`. It is registered as global
	 * `window.XHR` symbol for compatibility with legacy code.
	 *
	 * New code should use {@link LuCI.Request} instead to implement HTTP
	 * request handling.
	 */
	var XHR = Class.extend(/** @lends LuCI.XHR.prototype */ {
		__name__: 'LuCI.XHR',
		__init__: function() {
			if (window.console && console.debug)
				console.debug('Direct use XHR() is deprecated, please use L.Request instead');
		},

		_response: function(cb, res, json, duration) {
			if (this.active)
				cb(res, json, duration);
			delete this.active;
		},

		/**
		 * This function is a legacy wrapper around
		 * {@link LuCI#get LuCI.get()}.
		 *
		 * @instance
		 * @deprecated
		 * @memberof LuCI.XHR
		 *
		 * @param {string} url
		 * The URL to request
		 *
		 * @param {Object} [data]
		 * Additional query string data
		 *
		 * @param {LuCI.requestCallbackFn} [callback]
		 * Callback function to invoke on completion
		 *
		 * @param {number} [timeout]
		 * Request timeout to use
		 *
		 * @return {Promise<null>}
		 */
		get: function(url, data, callback, timeout) {
			this.active = true;
			L.get(url, data, this._response.bind(this, callback), timeout);
		},

		/**
		 * This function is a legacy wrapper around
		 * {@link LuCI#post LuCI.post()}.
		 *
		 * @instance
		 * @deprecated
		 * @memberof LuCI.XHR
		 *
		 * @param {string} url
		 * The URL to request
		 *
		 * @param {Object} [data]
		 * Additional data to append to the request body.
		 *
		 * @param {LuCI.requestCallbackFn} [callback]
		 * Callback function to invoke on completion
		 *
		 * @param {number} [timeout]
		 * Request timeout to use
		 *
		 * @return {Promise<null>}
		 */
		post: function(url, data, callback, timeout) {
			this.active = true;
			L.post(url, data, this._response.bind(this, callback), timeout);
		},

		/**
		 * Cancels a running request.
		 *
		 * This function does not actually cancel the underlying
		 * `XMLHTTPRequest` request but it sets a flag which prevents the
		 * invocation of the callback function when the request eventually
		 * finishes or timed out.
		 *
		 * @instance
		 * @deprecated
		 * @memberof LuCI.XHR
		 */
		cancel: function() { delete this.active },

		/**
		 * Checks the running state of the request.
		 *
		 * @instance
		 * @deprecated
		 * @memberof LuCI.XHR
		 *
		 * @returns {boolean}
		 * Returns `true` if the request is still running or `false` if it
		 * already completed.
		 */
		busy: function() { return (this.active === true) },

		/**
		 * Ignored for backwards compatibility.
		 *
		 * This function does nothing.
		 *
		 * @instance
		 * @deprecated
		 * @memberof LuCI.XHR
		 */
		abort: function() {},

		/**
		 * Existing for backwards compatibility.
		 *
		 * This function simply throws an `InternalError` when invoked.
		 *
		 * @instance
		 * @deprecated
		 * @memberof LuCI.XHR
		 *
		 * @throws {InternalError}
		 * Throws an `InternalError` with the message `Not implemented`
		 * when invoked.
		 */
		send_form: function() { L.error('InternalError', 'Not implemented') },
	});

	XHR.get = function() { return window.L.get.apply(window.L, arguments) };
	XHR.post = function() { return window.L.post.apply(window.L, arguments) };
	XHR.poll = function() { return window.L.poll.apply(window.L, arguments) };
	XHR.stop = Request.poll.remove.bind(Request.poll);
	XHR.halt = Request.poll.stop.bind(Request.poll);
	XHR.run = Request.poll.start.bind(Request.poll);
	XHR.running = Request.poll.active.bind(Request.poll);

	window.XHR = XHR;
	window.LuCI = LuCI;
})(window, document);
