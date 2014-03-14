--- a/src/openvpn/ssl_polarssl.h
+++ b/src/openvpn/ssl_polarssl.h
@@ -36,6 +36,8 @@
 #include <polarssl/pkcs11.h>
 #endif
 
+#include <polarssl/compat-1.2.h>
+
 typedef struct _buffer_entry buffer_entry;
 
 struct _buffer_entry {
--- a/src/openvpn/ssl_polarssl.c
+++ b/src/openvpn/ssl_polarssl.c
@@ -44,7 +44,7 @@
 #include "manage.h"
 #include "ssl_common.h"
 
-#include <polarssl/sha2.h>
+#include <polarssl/sha256.h>
 #include <polarssl/havege.h>
 
 #include "ssl_verify_polarssl.h"
@@ -204,12 +204,12 @@ tls_ctx_load_dh_params (struct tls_root_
 {
   if (!strcmp (dh_file, INLINE_FILE_TAG) && dh_file_inline)
     {
-      if (0 != x509parse_dhm(ctx->dhm_ctx, dh_file_inline, strlen(dh_file_inline)))
+      if (0 != dhm_parse_dhm(ctx->dhm_ctx, dh_file_inline, strlen(dh_file_inline)))
 	msg (M_FATAL, "Cannot read inline DH parameters");
   }
 else
   {
-    if (0 != x509parse_dhmfile(ctx->dhm_ctx, dh_file))
+    if (0 != dhm_parse_dhmfile(ctx->dhm_ctx, dh_file))
       msg (M_FATAL, "Cannot read DH parameters from file %s", dh_file);
   }
 
@@ -530,7 +530,7 @@ void key_state_ssl_init(struct key_state
 	    ssl_pkcs11_key_len );
       else
 #endif
-	ssl_set_own_cert( ks_ssl->ctx, ssl_ctx->crt_chain, ssl_ctx->priv_key );
+	ssl_set_own_cert_rsa( ks_ssl->ctx, ssl_ctx->crt_chain, ssl_ctx->priv_key );
 
       /* Initialise SSL verification */
       ssl_set_authmode (ks_ssl->ctx, SSL_VERIFY_REQUIRED);
@@ -832,7 +832,7 @@ print_details (struct key_state_ssl * ks
   cert = ssl_get_peer_cert(ks_ssl->ctx);
   if (cert != NULL)
     {
-      openvpn_snprintf (s2, sizeof (s2), ", " counter_format " bit RSA", (counter_type) cert->rsa.len * 8);
+      openvpn_snprintf (s2, sizeof (s2), ", " counter_format " bit RSA", (counter_type) pk_rsa(cert->pk)->len * 8);
     }
 
   msg (D_HANDSHAKE, "%s%s", s1, s2);
--- a/src/openvpn/crypto_polarssl.c
+++ b/src/openvpn/crypto_polarssl.c
@@ -466,7 +466,12 @@ int cipher_ctx_mode (const cipher_contex
 
 int cipher_ctx_reset (cipher_context_t *ctx, uint8_t *iv_buf)
 {
-  return 0 == cipher_reset(ctx, iv_buf);
+  int retval = cipher_reset(ctx);
+
+  if (0 == retval)
+    cipher_set_iv(ctx, iv_buf, ctx->cipher_info->iv_size);
+
+  return 0 == retval;
 }
 
 int cipher_ctx_update (cipher_context_t *ctx, uint8_t *dst, int *dst_len,
--- a/src/openvpn/ssl_verify_polarssl.h
+++ b/src/openvpn/ssl_verify_polarssl.h
@@ -34,6 +34,7 @@
 #include "misc.h"
 #include "manage.h"
 #include <polarssl/x509.h>
+#include <polarssl/compat-1.2.h>
 
 #ifndef __OPENVPN_X509_CERT_T_DECLARED
 #define __OPENVPN_X509_CERT_T_DECLARED
--- a/src/openvpn/ssl_verify.c
+++ b/src/openvpn/ssl_verify.c
@@ -437,7 +437,7 @@ verify_cert_set_env(struct env_set *es, 
 #endif
 
   /* export serial number as environmental variable */
-  serial = x509_get_serial(peer_cert, &gc);
+  serial = backend_x509_get_serial(peer_cert, &gc);
   openvpn_snprintf (envname, sizeof(envname), "tls_serial_%d", cert_depth);
   setenv_str (es, envname, serial);
 
@@ -564,7 +564,7 @@ verify_check_crl_dir(const char *crl_dir
   int fd = -1;
   struct gc_arena gc = gc_new();
 
-  char *serial = x509_get_serial(cert, &gc);
+  char *serial = backend_x509_get_serial(cert, &gc);
 
   if (!openvpn_snprintf(fn, sizeof(fn), "%s%c%s", crl_dir, OS_SPECIFIC_DIRSEP, serial))
     {
--- a/src/openvpn/ssl_verify_backend.h
+++ b/src/openvpn/ssl_verify_backend.h
@@ -122,7 +122,7 @@ result_t x509_get_username (char *common
  *
  * @return 		The certificate's serial number.
  */
-char *x509_get_serial (openvpn_x509_cert_t *cert, struct gc_arena *gc);
+char *backend_x509_get_serial (openvpn_x509_cert_t *cert, struct gc_arena *gc);
 
 /*
  * Save X509 fields to environment, using the naming convention:
--- a/src/openvpn/ssl_verify_openssl.c
+++ b/src/openvpn/ssl_verify_openssl.c
@@ -220,7 +220,7 @@ x509_get_username (char *common_name, in
 }
 
 char *
-x509_get_serial (openvpn_x509_cert_t *cert, struct gc_arena *gc)
+backend_x509_get_serial (openvpn_x509_cert_t *cert, struct gc_arena *gc)
 {
   ASN1_INTEGER *asn1_i;
   BIGNUM *bignum;
--- a/src/openvpn/ssl_verify_polarssl.c
+++ b/src/openvpn/ssl_verify_polarssl.c
@@ -38,6 +38,7 @@
 #if defined(ENABLE_SSL) && defined(ENABLE_CRYPTO_POLARSSL)
 
 #include "ssl_verify.h"
+#include <polarssl/oid.h>
 #include <polarssl/sha1.h>
 
 #define MAX_SUBJECT_LENGTH 256
@@ -100,7 +101,7 @@ x509_get_username (char *cn, int cn_len,
   /* Find common name */
   while( name != NULL )
   {
-      if( memcmp( name->oid.p, OID_CN, OID_SIZE(OID_CN) ) == 0)
+      if( memcmp( name->oid.p, OID_AT_CN, OID_SIZE(OID_AT_CN) ) == 0)
 	break;
 
       name = name->next;
@@ -123,7 +124,7 @@ x509_get_username (char *cn, int cn_len,
 }
 
 char *
-x509_get_serial (x509_cert *cert, struct gc_arena *gc)
+backend_x509_get_serial (x509_cert *cert, struct gc_arena *gc)
 {
   int ret = 0;
   int i = 0;
@@ -184,60 +185,18 @@ x509_setenv (struct env_set *es, int cer
   while( name != NULL )
     {
       char name_expand[64+8];
+      const char *shortname;
 
-      if( name->oid.len == 2 && memcmp( name->oid.p, OID_X520, 2 ) == 0 )
+      if( 0 == oid_get_attr_short_name(&name->oid, &shortname) )
 	{
-	  switch( name->oid.p[2] )
-	    {
-	    case X520_COMMON_NAME:
-		openvpn_snprintf (name_expand, sizeof(name_expand), "X509_%d_CN",
-		    cert_depth); break;
-
-	    case X520_COUNTRY:
-		openvpn_snprintf (name_expand, sizeof(name_expand), "X509_%d_C",
-		    cert_depth); break;
-
-	    case X520_LOCALITY:
-		openvpn_snprintf (name_expand, sizeof(name_expand), "X509_%d_L",
-		    cert_depth); break;
-
-	    case X520_STATE:
-		openvpn_snprintf (name_expand, sizeof(name_expand), "X509_%d_ST",
-		    cert_depth); break;
-
-	    case X520_ORGANIZATION:
-		openvpn_snprintf (name_expand, sizeof(name_expand), "X509_%d_O",
-		    cert_depth); break;
-
-	    case X520_ORG_UNIT:
-		openvpn_snprintf (name_expand, sizeof(name_expand), "X509_%d_OU",
-		    cert_depth); break;
-
-	    default:
-		openvpn_snprintf (name_expand, sizeof(name_expand),
-		    "X509_%d_0x%02X", cert_depth, name->oid.p[2]);
-		break;
-	    }
+	  openvpn_snprintf (name_expand, sizeof(name_expand), "X509_%d_%s",
+	      cert_depth, shortname);
+	}
+      else
+	{
+	  openvpn_snprintf (name_expand, sizeof(name_expand), "X509_%d_\?\?",
+	      cert_depth);
 	}
-	else if( name->oid.len == 8 && memcmp( name->oid.p, OID_PKCS9, 8 ) == 0 )
-	  {
-	    switch( name->oid.p[8] )
-	      {
-		case PKCS9_EMAIL:
-		  openvpn_snprintf (name_expand, sizeof(name_expand),
-		      "X509_%d_emailAddress", cert_depth); break;
-
-		default:
-		  openvpn_snprintf (name_expand, sizeof(name_expand),
-		      "X509_%d_0x%02X", cert_depth, name->oid.p[8]);
-		  break;
-	      }
-	  }
-	else
-	  {
-	    openvpn_snprintf (name_expand, sizeof(name_expand), "X509_%d_\?\?",
-		cert_depth);
-	  }
 
 	for( i = 0; i < name->val.len; i++ )
 	{
