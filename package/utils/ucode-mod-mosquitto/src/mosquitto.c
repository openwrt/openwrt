/*
 * ucode mosquitto module — wraps libmosquitto with uloop integration
 *
 * Exposes an MQTT client API to ucode scripts backed by libmosquitto,
 * giving full QoS, will and auth support, plus TLS when the system has
 * the libmosquitto-ssl variant installed. The libmosquitto socket
 * is registered with uloop so MQTT I/O runs in the same event loop as
 * the rest of the application; the libmosquitto helper thread is not
 * used. To reconnect after a broker drop, call client.connect() again
 * from the on_disconnect callback.
 *
 * Usage from ucode (TLS example):
 *
 *   import * as mqtt  from 'mosquitto';
 *   import * as uloop from 'uloop';
 *
 *   uloop.init();
 *
 *   let client = mqtt.new("my-agent", true);
 *
 *   client.tls_set({
 *       cafile:   "/etc/cert/ca.crt",
 *       certfile: "/etc/cert/client.crt",
 *       keyfile:  "/etc/cert/client.key",
 *   });
 *
 *   // Last-will so subscribers know when agent dies unexpectedly
 *   client.will_set("my-agent/status",
 *       '{"online":false}', 0, true);
 *
 *   client.on_connect(function(rc) {
 *       if (rc != 0) return;
 *       client.subscribe("my-agent/cmd", 0);
 *       client.publish("my-agent/status", '{"online":true}', 0, true);
 *   });
 *
 *   client.on_message(function(topic, payload, qos, retain) {
 *       print(topic + ": " + payload + "\n");
 *   });
 *
 *   client.on_disconnect(function(rc) {
 *       warn("disconnected (rc=" + rc + ")\n");
 *   });
 *
 *   client.on_subscribe(function(mid, granted_qos) {
 *       warn("subscribed, granted qos: " + granted_qos[0] + "\n");
 *   });
 *
 *   client.connect("broker.example.com", 8883, 60);
 *   client.loop_start();   // register with uloop; call before uloop.run()
 *
 *   uloop.signal("SIGTERM", function() {
 *       client.publish("my-agent/status", '{"online":false}', 0, true);
 *       client.loop_stop();
 *       client.disconnect();
 *       uloop.end();
 *   });
 *
 *   uloop.run();
 *
 * Optional plain-text auth:
 *   client.username_pw_set("user", "secret");
 *
 * Optional: skip hostname verification (dev only):
 *   client.tls_insecure_set(true);
 *
 * Call client.loop_stop() to deregister from uloop before destroying.
 *
 * Lifecycle: client.destroy() must be called when the client is no longer
 * needed (typically from a SIGTERM/SIGINT handler as in the example above).
 * The resource holds a self-reference so that uloop callbacks can keep
 * firing after the script drops its handle; without an explicit destroy()
 * the client, its mosquitto context, and any uloop registration leak for
 * the lifetime of the ucode VM. This mirrors the cancel() requirement on
 * ucode's own uloop.timer / uloop.handle resources.
 */

#include <errno.h>
#include <limits.h>
#include <string.h>
#include <stdint.h>

#include <libubox/uloop.h>
#include <mosquitto.h>

#include "ucode/module.h"

/* -------------------------------------------------------------------------
 * Resource value slot indices
 * ---------------------------------------------------------------------- */
enum {
	CB_ON_CONNECT = 0,
	CB_ON_DISCONNECT,
	CB_ON_MESSAGE,
	CB_ON_PUBLISH,
	CB_ON_SUBSCRIBE,
	_CB_MAX
};

/* -------------------------------------------------------------------------
 * Client context
 * ---------------------------------------------------------------------- */
typedef struct {
	struct mosquitto   *mosq;
	uc_vm_t            *vm;
	uc_value_t         *res;        /* persistent self-reference */
	struct uloop_fd     ufd;        /* registered socket watcher */
	struct uloop_timeout misc_tmr;  /* periodic mosquitto_loop_misc() */
	bool                loop_active;
} uc_mosq_t;

/* -------------------------------------------------------------------------
 * Error helpers
 * ---------------------------------------------------------------------- */
#define ok_return(expr)   do { return (expr); } while (0)
#define err_return(rc, msg) \
	do { \
		uc_vm_raise_exception(vm, EXCEPTION_RUNTIME, \
		    "mosquitto: %s: %s", (msg), mosquitto_strerror(rc)); \
		return NULL; \
	} while (0)

/* -------------------------------------------------------------------------
 * uloop integration
 * ---------------------------------------------------------------------- */

/* Forward declaration */
static void mosq_uloop_update(uc_mosq_t *m);

static void
mosq_uloop_fd_cb(struct uloop_fd *ufd, unsigned int events)
{
	uc_mosq_t *m = container_of(ufd, uc_mosq_t, ufd);

	if (events & ULOOP_READ)
		mosquitto_loop_read(m->mosq, 1);

	if ((events & ULOOP_WRITE) && mosquitto_want_write(m->mosq))
		mosquitto_loop_write(m->mosq, 1);

	/* Re-evaluate write interest after each I/O cycle */
	mosq_uloop_update(m);
}

static void
mosq_misc_timer_cb(struct uloop_timeout *t)
{
	uc_mosq_t *m = container_of(t, uc_mosq_t, misc_tmr);
	int fd;

	mosquitto_loop_misc(m->mosq);

	fd = mosquitto_socket(m->mosq);

	if (fd < 0) {
		/* Connection gone — drop stale registration so a reused fd
		 * number cannot trigger callbacks against a different owner */
		if (m->ufd.fd >= 0) {
			uloop_fd_delete(&m->ufd);
			m->ufd.fd = -1;
		}
	} else if (fd != m->ufd.fd) {
		uloop_fd_delete(&m->ufd);
		m->ufd.fd = fd;
	}

	mosq_uloop_update(m);
	uloop_timeout_set(&m->misc_tmr, 1000);
}

static void
mosq_uloop_update(uc_mosq_t *m)
{
	unsigned int flags = ULOOP_READ;

	if (!m->loop_active || m->ufd.fd < 0)
		return;

	if (mosquitto_want_write(m->mosq))
		flags |= ULOOP_WRITE;

	uloop_fd_add(&m->ufd, flags);
}

static void
mosq_uloop_register(uc_mosq_t *m)
{
	int fd = mosquitto_socket(m->mosq);

	if (fd < 0 || m->loop_active)
		return;

	m->ufd.fd = fd;
	m->ufd.cb = mosq_uloop_fd_cb;
	m->misc_tmr.cb = mosq_misc_timer_cb;

	m->loop_active = true;

	/* mosq_uloop_update() requires loop_active=true; sets READ + WRITE
	 * flags as needed so any pending CONNECT packet is flushed promptly */
	mosq_uloop_update(m);
	uloop_timeout_set(&m->misc_tmr, 1000);
}

static void
mosq_uloop_unregister(uc_mosq_t *m)
{
	if (!m->loop_active)
		return;

	uloop_fd_delete(&m->ufd);
	uloop_timeout_cancel(&m->misc_tmr);
	m->ufd.fd = -1;
	m->loop_active = false;
}

/* -------------------------------------------------------------------------
 * Libmosquitto → ucode callbacks
 * ---------------------------------------------------------------------- */

static void
mosq_invoke_cb(uc_mosq_t *m, int slot, uc_value_t **args, size_t nargs)
{
	uc_value_t *cb = ucv_resource_value_get(m->res, slot);

	if (!ucv_is_callable(cb))
		return;

	uc_vm_stack_push(m->vm, ucv_get(m->res));
	uc_vm_stack_push(m->vm, ucv_get(cb));
	for (size_t i = 0; i < nargs; i++)
		uc_vm_stack_push(m->vm, ucv_get(args[i]));

	if (uc_vm_call(m->vm, true, nargs) == EXCEPTION_NONE)
		ucv_put(uc_vm_stack_pop(m->vm));
	else
		ucv_put(uc_vm_exception_object(m->vm));
}

static void
cb_on_connect(struct mosquitto *mosq, void *obj, int rc)
{
	uc_mosq_t *m = obj;
	uc_value_t *args[] = { ucv_int64_new(rc) };

	/* On successful (re)connect the fd may be new — refresh uloop */
	if (rc == 0 && m->loop_active) {
		int fd = mosquitto_socket(m->mosq);
		if (fd >= 0 && fd != m->ufd.fd) {
			uloop_fd_delete(&m->ufd);
			m->ufd.fd = fd;
		}
		mosq_uloop_update(m);
	}

	mosq_invoke_cb(m, CB_ON_CONNECT, args, 1);
	ucv_put(args[0]);
}

static void
cb_on_disconnect(struct mosquitto *mosq, void *obj, int rc)
{
	uc_mosq_t *m = obj;
	uc_value_t *args[] = { ucv_int64_new(rc) };

	/* libmosquitto closes the socket before calling us; drop the now-stale
	 * uloop watcher so we don't spin on a closed fd, and so a connect()
	 * issued from the user callback starts from a clean slate */
	if (m->loop_active && m->ufd.fd >= 0 && mosquitto_socket(m->mosq) < 0) {
		uloop_fd_delete(&m->ufd);
		m->ufd.fd = -1;
	}

	mosq_invoke_cb(m, CB_ON_DISCONNECT, args, 1);
	ucv_put(args[0]);
}

static void
cb_on_message(struct mosquitto *mosq, void *obj,
              const struct mosquitto_message *msg)
{
	uc_mosq_t *m = obj;

	/* payload may be NULL for zero-length messages; payloadlen is int in
	 * libmosquitto's struct so guard against a malformed negative value */
	const char *pl  = msg->payload ? msg->payload : "";
	size_t      pll = (msg->payload && msg->payloadlen > 0)
	                    ? (size_t)msg->payloadlen : 0;

	uc_value_t *args[] = {
		ucv_string_new(msg->topic),
		ucv_string_new_length(pl, pll),
		ucv_int64_new(msg->qos),
		ucv_boolean_new(msg->retain),
	};

	mosq_invoke_cb(m, CB_ON_MESSAGE, args, 4);

	for (size_t i = 0; i < 4; i++)
		ucv_put(args[i]);
}

static void
cb_on_publish(struct mosquitto *mosq, void *obj, int mid)
{
	uc_mosq_t *m = obj;
	uc_value_t *args[] = { ucv_int64_new(mid) };

	mosq_invoke_cb(m, CB_ON_PUBLISH, args, 1);
	ucv_put(args[0]);
}

static void
cb_on_subscribe(struct mosquitto *mosq, void *obj, int mid,
                int qos_count, const int *granted_qos)
{
	uc_mosq_t *m = obj;
	uc_value_t *arr = ucv_array_new(m->vm);

	for (int i = 0; i < qos_count; i++)
		ucv_array_push(arr, ucv_int64_new(granted_qos[i]));

	uc_value_t *args[] = { ucv_int64_new(mid), arr };
	mosq_invoke_cb(m, CB_ON_SUBSCRIBE, args, 2);
	ucv_put(args[0]);
	ucv_put(arr);
}

/* -------------------------------------------------------------------------
 * Resource allocation / destruction
 * ---------------------------------------------------------------------- */

static uc_mosq_t *
mosq_get(uc_vm_t *vm)
{
	uc_mosq_t *m = uc_fn_thisval("mosquitto.client");

	if (!m || !m->mosq) {
		uc_vm_raise_exception(vm, EXCEPTION_RUNTIME,
		    "mosquitto: invalid or destroyed client");
		return NULL;
	}
	return m;
}

static void
free_client(void *ud)
{
	uc_mosq_t *m = ud;

	if (!m)
		return;

	mosq_uloop_unregister(m);

	if (m->mosq) {
		mosquitto_disconnect(m->mosq);
		mosquitto_destroy(m->mosq);
		m->mosq = NULL;
	}
}

/* -------------------------------------------------------------------------
 * module:mosquitto#new
 *
 * Create a new MQTT client.
 *
 * @param {string}  [id]            Client identifier (auto-generated if null)
 * @param {boolean} [clean_session] Clean session flag (default: true)
 * @returns {?module:mosquitto.client}
 * ---------------------------------------------------------------------- */
static uc_value_t *
uc_mosq_new(uc_vm_t *vm, size_t nargs)
{
	uc_value_t *id_v     = uc_fn_arg(0);
	uc_value_t *clean_v  = uc_fn_arg(1);
	const char *id       = (id_v && ucv_type(id_v) == UC_STRING)
	                         ? ucv_string_get(id_v) : NULL;
	bool        clean    = clean_v ? ucv_is_truish(clean_v) : true;
	uc_mosq_t  *m        = NULL;
	uc_value_t *res;

	res = ucv_resource_create_ex(vm, "mosquitto.client",
	                             (void **)&m, _CB_MAX, sizeof(*m));
	if (!m) {
		uc_vm_raise_exception(vm, EXCEPTION_RUNTIME,
		    "mosquitto: out of memory");
		return NULL;
	}

	m->mosq = mosquitto_new(id, clean, m);
	if (!m->mosq) {
		ucv_put(res);
		uc_vm_raise_exception(vm, EXCEPTION_RUNTIME,
		    "mosquitto: mosquitto_new() failed: %s", strerror(errno));
		return NULL;
	}

	m->vm  = vm;
	m->res = ucv_get(res);
	m->ufd.fd = -1;

	mosquitto_connect_callback_set(m->mosq,    cb_on_connect);
	mosquitto_disconnect_callback_set(m->mosq, cb_on_disconnect);
	mosquitto_message_callback_set(m->mosq,    cb_on_message);
	mosquitto_publish_callback_set(m->mosq,    cb_on_publish);
	mosquitto_subscribe_callback_set(m->mosq,  cb_on_subscribe);

	ucv_resource_persistent_set(res, true);
	ok_return(res);
}

/* -------------------------------------------------------------------------
 * client.connect(host, port, keepalive)
 * ---------------------------------------------------------------------- */
static uc_value_t *
uc_mosq_connect(uc_vm_t *vm, size_t nargs)
{
	uc_mosq_t  *m        = mosq_get(vm);
	uc_value_t *host_v   = uc_fn_arg(0);
	uc_value_t *port_v   = uc_fn_arg(1);
	uc_value_t *ka_v     = uc_fn_arg(2);

	if (!m) return NULL;

	const char *host = (host_v && ucv_type(host_v) == UC_STRING)
	                     ? ucv_string_get(host_v) : "localhost";
	int port      = (port_v) ? (int)ucv_int64_get(port_v) : 1883;
	int keepalive = (ka_v)   ? (int)ucv_int64_get(ka_v)   : 60;

	int rc = mosquitto_connect_async(m->mosq, host, port, keepalive);
	if (rc != MOSQ_ERR_SUCCESS)
		err_return(rc, "connect");

	ok_return(ucv_boolean_new(true));
}

/* -------------------------------------------------------------------------
 * client.disconnect()
 * ---------------------------------------------------------------------- */
static uc_value_t *
uc_mosq_disconnect(uc_vm_t *vm, size_t nargs)
{
	uc_mosq_t *m = mosq_get(vm);
	if (!m) return NULL;

	int rc = mosquitto_disconnect(m->mosq);
	if (rc != MOSQ_ERR_SUCCESS && rc != MOSQ_ERR_NO_CONN)
		err_return(rc, "disconnect");

	ok_return(ucv_boolean_new(true));
}

/* -------------------------------------------------------------------------
 * client.publish(topic, payload, qos, retain) → mid
 * ---------------------------------------------------------------------- */
static uc_value_t *
uc_mosq_publish(uc_vm_t *vm, size_t nargs)
{
	uc_mosq_t  *m       = mosq_get(vm);
	uc_value_t *topic_v = uc_fn_arg(0);
	uc_value_t *pay_v   = uc_fn_arg(1);
	uc_value_t *qos_v   = uc_fn_arg(2);
	uc_value_t *ret_v   = uc_fn_arg(3);

	if (!m) return NULL;

	if (!topic_v || ucv_type(topic_v) != UC_STRING) {
		uc_vm_raise_exception(vm, EXCEPTION_RUNTIME,
		    "mosquitto: publish: topic must be a string");
		return NULL;
	}

	const char *topic   = ucv_string_get(topic_v);
	const char *payload = NULL;
	size_t      paylen  = 0;
	int         qos     = qos_v  ? (int)ucv_int64_get(qos_v)  : 0;
	bool        retain  = ret_v  ? ucv_is_truish(ret_v)        : false;

	if (pay_v && ucv_type(pay_v) == UC_STRING) {
		payload = ucv_string_get(pay_v);
		paylen  = ucv_string_length(pay_v);
	}

	if (paylen > INT_MAX) {
		uc_vm_raise_exception(vm, EXCEPTION_RUNTIME,
		    "mosquitto: publish: payload too large");
		return NULL;
	}

	int mid = 0;
	int rc  = mosquitto_publish(m->mosq, &mid, topic,
	                            (int)paylen, payload, qos, retain);
	if (rc != MOSQ_ERR_SUCCESS)
		err_return(rc, "publish");

	mosq_uloop_update(m);   /* trigger write if needed */
	ok_return(ucv_int64_new(mid));
}

/* -------------------------------------------------------------------------
 * client.subscribe(topic, qos) → mid
 * ---------------------------------------------------------------------- */
static uc_value_t *
uc_mosq_subscribe(uc_vm_t *vm, size_t nargs)
{
	uc_mosq_t  *m       = mosq_get(vm);
	uc_value_t *topic_v = uc_fn_arg(0);
	uc_value_t *qos_v   = uc_fn_arg(1);

	if (!m) return NULL;

	if (!topic_v || ucv_type(topic_v) != UC_STRING) {
		uc_vm_raise_exception(vm, EXCEPTION_RUNTIME,
		    "mosquitto: subscribe: topic must be a string");
		return NULL;
	}

	int mid = 0;
	int qos = qos_v ? (int)ucv_int64_get(qos_v) : 0;
	int rc  = mosquitto_subscribe(m->mosq, &mid,
	                              ucv_string_get(topic_v), qos);
	if (rc != MOSQ_ERR_SUCCESS)
		err_return(rc, "subscribe");

	mosq_uloop_update(m);
	ok_return(ucv_int64_new(mid));
}

/* -------------------------------------------------------------------------
 * client.unsubscribe(topic) → mid
 * ---------------------------------------------------------------------- */
static uc_value_t *
uc_mosq_unsubscribe(uc_vm_t *vm, size_t nargs)
{
	uc_mosq_t  *m       = mosq_get(vm);
	uc_value_t *topic_v = uc_fn_arg(0);

	if (!m) return NULL;

	if (!topic_v || ucv_type(topic_v) != UC_STRING) {
		uc_vm_raise_exception(vm, EXCEPTION_RUNTIME,
		    "mosquitto: unsubscribe: topic must be a string");
		return NULL;
	}

	int mid = 0;
	int rc  = mosquitto_unsubscribe(m->mosq, &mid, ucv_string_get(topic_v));
	if (rc != MOSQ_ERR_SUCCESS)
		err_return(rc, "unsubscribe");

	ok_return(ucv_int64_new(mid));
}

/* -------------------------------------------------------------------------
 * client.username_pw_set(username, password)
 * ---------------------------------------------------------------------- */
static uc_value_t *
uc_mosq_username_pw_set(uc_vm_t *vm, size_t nargs)
{
	uc_mosq_t  *m    = mosq_get(vm);
	uc_value_t *u_v  = uc_fn_arg(0);
	uc_value_t *pw_v = uc_fn_arg(1);

	if (!m) return NULL;

	const char *user = (u_v  && ucv_type(u_v)  == UC_STRING)
	                     ? ucv_string_get(u_v)  : NULL;
	const char *pw   = (pw_v && ucv_type(pw_v) == UC_STRING)
	                     ? ucv_string_get(pw_v) : NULL;

	int rc = mosquitto_username_pw_set(m->mosq, user, pw);
	if (rc != MOSQ_ERR_SUCCESS)
		err_return(rc, "username_pw_set");

	ok_return(ucv_boolean_new(true));
}

/* -------------------------------------------------------------------------
 * client.will_set(topic, payload, qos, retain)
 * ---------------------------------------------------------------------- */
static uc_value_t *
uc_mosq_will_set(uc_vm_t *vm, size_t nargs)
{
	uc_mosq_t  *m       = mosq_get(vm);
	uc_value_t *topic_v = uc_fn_arg(0);
	uc_value_t *pay_v   = uc_fn_arg(1);
	uc_value_t *qos_v   = uc_fn_arg(2);
	uc_value_t *ret_v   = uc_fn_arg(3);

	if (!m) return NULL;

	if (!topic_v || ucv_type(topic_v) != UC_STRING) {
		uc_vm_raise_exception(vm, EXCEPTION_RUNTIME,
		    "mosquitto: will_set: topic must be a string");
		return NULL;
	}

	const char *payload = NULL;
	size_t      paylen  = 0;

	if (pay_v && ucv_type(pay_v) == UC_STRING) {
		payload = ucv_string_get(pay_v);
		paylen  = ucv_string_length(pay_v);
	}

	if (paylen > INT_MAX) {
		uc_vm_raise_exception(vm, EXCEPTION_RUNTIME,
		    "mosquitto: will_set: payload too large");
		return NULL;
	}

	int rc = mosquitto_will_set(m->mosq,
	                            ucv_string_get(topic_v),
	                            (int)paylen, payload,
	                            qos_v  ? (int)ucv_int64_get(qos_v)  : 0,
	                            ret_v  ? ucv_is_truish(ret_v)        : false);
	if (rc != MOSQ_ERR_SUCCESS)
		err_return(rc, "will_set");

	ok_return(ucv_boolean_new(true));
}

/* -------------------------------------------------------------------------
 * client.will_clear()
 * ---------------------------------------------------------------------- */
static uc_value_t *
uc_mosq_will_clear(uc_vm_t *vm, size_t nargs)
{
	uc_mosq_t *m = mosq_get(vm);
	if (!m) return NULL;

	int rc = mosquitto_will_clear(m->mosq);
	if (rc != MOSQ_ERR_SUCCESS)
		err_return(rc, "will_clear");

	ok_return(ucv_boolean_new(true));
}

/* -------------------------------------------------------------------------
 * client.tls_set({ cafile, capath, certfile, keyfile })
 * ---------------------------------------------------------------------- */
static uc_value_t *
uc_mosq_tls_set(uc_vm_t *vm, size_t nargs)
{
	uc_mosq_t  *m    = mosq_get(vm);
	uc_value_t *opts = uc_fn_arg(0);

	if (!m) return NULL;

	if (!opts || ucv_type(opts) != UC_OBJECT) {
		uc_vm_raise_exception(vm, EXCEPTION_RUNTIME,
		    "mosquitto: tls_set: argument must be an object");
		return NULL;
	}

	const char *cafile   = NULL, *capath  = NULL;
	const char *certfile = NULL, *keyfile = NULL;

	uc_value_t *v;

	v = ucv_object_get(opts, "cafile",   NULL);
	if (v && ucv_type(v) == UC_STRING) cafile   = ucv_string_get(v);
	v = ucv_object_get(opts, "capath",   NULL);
	if (v && ucv_type(v) == UC_STRING) capath   = ucv_string_get(v);
	v = ucv_object_get(opts, "certfile", NULL);
	if (v && ucv_type(v) == UC_STRING) certfile = ucv_string_get(v);
	v = ucv_object_get(opts, "keyfile",  NULL);
	if (v && ucv_type(v) == UC_STRING) keyfile  = ucv_string_get(v);

	int rc = mosquitto_tls_set(m->mosq, cafile, capath,
	                           certfile, keyfile, NULL);
	if (rc != MOSQ_ERR_SUCCESS)
		err_return(rc, "tls_set");

	ok_return(ucv_boolean_new(true));
}

/* -------------------------------------------------------------------------
 * client.tls_insecure_set(bool)  — skip hostname verification (dev only)
 * ---------------------------------------------------------------------- */
static uc_value_t *
uc_mosq_tls_insecure_set(uc_vm_t *vm, size_t nargs)
{
	uc_mosq_t  *m   = mosq_get(vm);
	uc_value_t *v   = uc_fn_arg(0);

	if (!m) return NULL;

	int rc = mosquitto_tls_insecure_set(m->mosq, v ? ucv_is_truish(v) : false);
	if (rc != MOSQ_ERR_SUCCESS)
		err_return(rc, "tls_insecure_set");

	ok_return(ucv_boolean_new(true));
}

/* -------------------------------------------------------------------------
 * Callback setters:  client.on_connect(fn) etc.
 * ---------------------------------------------------------------------- */
#define DEFINE_CB_SETTER(name, slot) \
static uc_value_t * \
uc_mosq_##name(uc_vm_t *vm, size_t nargs) \
{ \
	uc_mosq_t  *m  = mosq_get(vm); \
	uc_value_t *fn = uc_fn_arg(0); \
	if (!m) return NULL; \
	if (fn && !ucv_is_callable(fn)) { \
		uc_vm_raise_exception(vm, EXCEPTION_RUNTIME, \
		    "mosquitto: " #name ": argument must be a function"); \
		return NULL; \
	} \
	ucv_resource_value_set(m->res, slot, fn ? ucv_get(fn) : NULL); \
	ok_return(ucv_boolean_new(true)); \
}

DEFINE_CB_SETTER(on_connect,    CB_ON_CONNECT)
DEFINE_CB_SETTER(on_disconnect, CB_ON_DISCONNECT)
DEFINE_CB_SETTER(on_message,    CB_ON_MESSAGE)
DEFINE_CB_SETTER(on_publish,    CB_ON_PUBLISH)
DEFINE_CB_SETTER(on_subscribe,  CB_ON_SUBSCRIBE)

/* -------------------------------------------------------------------------
 * client.loop_start()  — register the mosquitto fd with uloop
 * ---------------------------------------------------------------------- */
static uc_value_t *
uc_mosq_loop_start(uc_vm_t *vm, size_t nargs)
{
	uc_mosq_t *m = mosq_get(vm);
	if (!m) return NULL;

	if (mosquitto_socket(m->mosq) < 0) {
		uc_vm_raise_exception(vm, EXCEPTION_RUNTIME,
		    "mosquitto: loop_start: no socket — call connect() first");
		return NULL;
	}

	mosq_uloop_register(m);
	ok_return(ucv_boolean_new(true));
}

/* -------------------------------------------------------------------------
 * client.loop_stop()  — deregister from uloop
 * ---------------------------------------------------------------------- */
static uc_value_t *
uc_mosq_loop_stop(uc_vm_t *vm, size_t nargs)
{
	uc_mosq_t *m = mosq_get(vm);
	if (!m) return NULL;

	mosq_uloop_unregister(m);
	ok_return(ucv_boolean_new(true));
}

/* -------------------------------------------------------------------------
 * client.destroy()
 * ---------------------------------------------------------------------- */
static uc_value_t *
uc_mosq_destroy(uc_vm_t *vm, size_t nargs)
{
	uc_mosq_t *m = uc_fn_thisval("mosquitto.client");
	uc_value_t *res;

	if (!m) return NULL;

	mosq_uloop_unregister(m);

	if (m->mosq) {
		mosquitto_disconnect(m->mosq);
		mosquitto_destroy(m->mosq);
		m->mosq = NULL;
	}

	/* Release the self-reference taken in uc_mosq_new(); when the script
	 * also drops its handle the resource refcount falls to zero and
	 * free_client() runs via the type destructor. Null first so a
	 * re-entrant free_client() call from ucv_put() is a no-op. */
	res = m->res;
	m->res = NULL;
	if (res) {
		ucv_resource_persistent_set(res, false);
		ucv_put(res);
	}

	ok_return(ucv_boolean_new(true));
}

/* -------------------------------------------------------------------------
 * module:mosquitto#version()  → { major, minor, revision }
 * ---------------------------------------------------------------------- */
static uc_value_t *
uc_mosq_version(uc_vm_t *vm, size_t nargs)
{
	int maj, min, rev;
	mosquitto_lib_version(&maj, &min, &rev);

	uc_value_t *obj = ucv_object_new(vm);
	ucv_object_add(obj, "major",    ucv_int64_new(maj));
	ucv_object_add(obj, "minor",    ucv_int64_new(min));
	ucv_object_add(obj, "revision", ucv_int64_new(rev));
	return obj;
}

/* -------------------------------------------------------------------------
 * Function tables
 * ---------------------------------------------------------------------- */

static const uc_function_list_t client_fns[] = {
	{ "connect",             uc_mosq_connect             },
	{ "disconnect",          uc_mosq_disconnect          },
	{ "publish",             uc_mosq_publish             },
	{ "subscribe",           uc_mosq_subscribe           },
	{ "unsubscribe",         uc_mosq_unsubscribe         },
	{ "username_pw_set",     uc_mosq_username_pw_set     },
	{ "will_set",            uc_mosq_will_set            },
	{ "will_clear",          uc_mosq_will_clear          },
	{ "tls_set",             uc_mosq_tls_set             },
	{ "tls_insecure_set",    uc_mosq_tls_insecure_set    },
	{ "on_connect",          uc_mosq_on_connect          },
	{ "on_disconnect",       uc_mosq_on_disconnect       },
	{ "on_message",          uc_mosq_on_message          },
	{ "on_publish",          uc_mosq_on_publish          },
	{ "on_subscribe",        uc_mosq_on_subscribe        },
	{ "loop_start",          uc_mosq_loop_start          },
	{ "loop_stop",           uc_mosq_loop_stop           },
	{ "destroy",             uc_mosq_destroy             },
};

static const uc_function_list_t global_fns[] = {
	{ "new",     uc_mosq_new     },
	{ "version", uc_mosq_version },
};

/* -------------------------------------------------------------------------
 * Module init
 * ---------------------------------------------------------------------- */
void uc_module_init(uc_vm_t *vm, uc_value_t *scope)
{
	mosquitto_lib_init();

	uc_function_list_register(scope, global_fns);
	uc_type_declare(vm, "mosquitto.client", client_fns, free_client);
}
