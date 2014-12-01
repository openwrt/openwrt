--- a/src/openvpn/ssl_polarssl.h
+++ b/src/openvpn/ssl_polarssl.h
@@ -38,6 +38,8 @@
 #include <polarssl/pkcs11.h>
 #endif
 
+#include <polarssl/compat-1.2.h>
+
 typedef struct _buffer_entry buffer_entry;
 
 struct _buffer_entry {
--- a/src/openvpn/ssl_polarssl.c
+++ b/src/openvpn/ssl_polarssl.c
@@ -46,7 +46,7 @@
 #include "manage.h"
 #include "ssl_common.h"
 
-#include <polarssl/sha2.h>
+#include <polarssl/sha256.h>
 #include <polarssl/havege.h>
 
 #include "ssl_verify_polarssl.h"
@@ -212,13 +212,13 @@ tls_ctx_load_dh_params (struct tls_root_
 {
   if (!strcmp (dh_file, INLINE_FILE_TAG) && dh_inline)
     {
-      if (0 != x509parse_dhm(ctx->dhm_ctx, (const unsigned char *) dh_inline,
+      if (0 != dhm_parse_dhm(ctx->dhm_ctx, (const unsigned char *) dh_inline,
 	  strlen(dh_inline)))
 	msg (M_FATAL, "Cannot read inline DH parameters");
   }
 else
   {
-    if (0 != x509parse_dhmfile(ctx->dhm_ctx, dh_file))
+    if (0 != dhm_parse_dhmfile(ctx->dhm_ctx, dh_file))
       msg (M_FATAL, "Cannot read DH parameters from file %s", dh_file);
   }
 
@@ -253,13 +253,13 @@ tls_ctx_load_cert_file (struct tls_root_
 
   if (!strcmp (cert_file, INLINE_FILE_TAG) && cert_inline)
     {
-      if (0 != x509parse_crt(ctx->crt_chain,
+      if (0 != x509_crt_parse(ctx->crt_chain,
 	  (const unsigned char *) cert_inline, strlen(cert_inline)))
         msg (M_FATAL, "Cannot load inline certificate file");
     }
   else
     {
-      if (0 != x509parse_crtfile(ctx->crt_chain, cert_file))
+      if (0 != x509_crt_parse_file(ctx->crt_chain, cert_file))
 	msg (M_FATAL, "Cannot load certificate file %s", cert_file);
     }
 }
@@ -277,7 +277,7 @@ tls_ctx_load_priv_file (struct tls_root_
       status = x509parse_key(ctx->priv_key,
 	  (const unsigned char *) priv_key_inline, strlen(priv_key_inline),
 	  NULL, 0);
-      if (POLARSSL_ERR_X509_PASSWORD_REQUIRED == status)
+      if (POLARSSL_ERR_PK_PASSWORD_REQUIRED == status)
 	{
 	  char passbuf[512] = {0};
 	  pem_password_callback(passbuf, 512, 0, NULL);
@@ -289,7 +289,7 @@ tls_ctx_load_priv_file (struct tls_root_
   else
     {
       status = x509parse_keyfile(ctx->priv_key, priv_key_file, NULL);
-      if (POLARSSL_ERR_X509_PASSWORD_REQUIRED == status)
+      if (POLARSSL_ERR_PK_PASSWORD_REQUIRED == status)
 	{
 	  char passbuf[512] = {0};
 	  pem_password_callback(passbuf, 512, 0, NULL);
@@ -480,14 +480,14 @@ void tls_ctx_load_ca (struct tls_root_ct
 
   if (ca_file && !strcmp (ca_file, INLINE_FILE_TAG) && ca_inline)
     {
-      if (0 != x509parse_crt(ctx->ca_chain, (const unsigned char *) ca_inline,
+      if (0 != x509_crt_parse(ctx->ca_chain, (const unsigned char *) ca_inline,
 	  strlen(ca_inline)))
 	msg (M_FATAL, "Cannot load inline CA certificates");
     }
   else
     {
       /* Load CA file for verifying peer supplied certificate */
-      if (0 != x509parse_crtfile(ctx->ca_chain, ca_file))
+      if (0 != x509_crt_parse_file(ctx->ca_chain, ca_file))
 	msg (M_FATAL, "Cannot load CA certificate file %s", ca_file);
     }
 }
@@ -501,14 +501,14 @@ tls_ctx_load_extra_certs (struct tls_roo
 
   if (!strcmp (extra_certs_file, INLINE_FILE_TAG) && extra_certs_inline)
     {
-      if (0 != x509parse_crt(ctx->crt_chain,
+      if (0 != x509_crt_parse(ctx->crt_chain,
 	  (const unsigned char *) extra_certs_inline,
 	  strlen(extra_certs_inline)))
         msg (M_FATAL, "Cannot load inline extra-certs file");
     }
   else
     {
-      if (0 != x509parse_crtfile(ctx->crt_chain, extra_certs_file))
+      if (0 != x509_crt_parse_file(ctx->crt_chain, extra_certs_file))
 	msg (M_FATAL, "Cannot load extra-certs file: %s", extra_certs_file);
     }
 }
@@ -724,7 +724,7 @@ void key_state_ssl_init(struct key_state
 	   external_key_len );
       else
 #endif
-	ssl_set_own_cert( ks_ssl->ctx, ssl_ctx->crt_chain, ssl_ctx->priv_key );
+	ssl_set_own_cert_rsa( ks_ssl->ctx, ssl_ctx->crt_chain, ssl_ctx->priv_key );
 
       /* Initialise SSL verification */
 #if P2MP_SERVER
@@ -1068,7 +1068,7 @@ print_details (struct key_state_ssl * ks
   cert = ssl_get_peer_cert(ks_ssl->ctx);
   if (cert != NULL)
     {
-      openvpn_snprintf (s2, sizeof (s2), ", " counter_format " bit RSA", (counter_type) cert->rsa.len * 8);
+      openvpn_snprintf (s2, sizeof (s2), ", " counter_format " bit RSA", (counter_type) pk_rsa(cert->pk)->len * 8);
     }
 
   msg (D_HANDSHAKE, "%s%s", s1, s2);
--- a/src/openvpn/crypto_polarssl.c
+++ b/src/openvpn/crypto_polarssl.c
@@ -487,7 +487,12 @@ cipher_ctx_get_cipher_kt (const cipher_c
 
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
--- a/src/openvpn/ssl_verify_polarssl.c
+++ b/src/openvpn/ssl_verify_polarssl.c
@@ -40,6 +40,7 @@
 #include "ssl_verify.h"
 #include <polarssl/error.h>
 #include <polarssl/bignum.h>
+#include <polarssl/oid.h>
 #include <polarssl/sha1.h>
 
 #define MAX_SUBJECT_LENGTH 256
@@ -102,7 +103,7 @@ x509_get_username (char *cn, int cn_len,
   /* Find common name */
   while( name != NULL )
   {
-      if( memcmp( name->oid.p, OID_CN, OID_SIZE(OID_CN) ) == 0)
+      if( memcmp( name->oid.p, OID_AT_CN, OID_SIZE(OID_AT_CN) ) == 0)
 	break;
 
       name = name->next;
@@ -224,60 +225,18 @@ x509_setenv (struct env_set *es, int cer
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
--- a/configure.ac
+++ b/configure.ac
@@ -819,13 +819,13 @@ if test "${with_crypto_library}" = "pola
 #include <polarssl/version.h>
 			]],
 			[[
-#if POLARSSL_VERSION_NUMBER < 0x01020A00 || POLARSSL_VERSION_NUMBER >= 0x01030000
+#if POLARSSL_VERSION_NUMBER < 0x01030000
 #error invalid version
 #endif
 			]]
 		)],
 		[AC_MSG_RESULT([ok])],
-		[AC_MSG_ERROR([PolarSSL 1.2.x required and must be 1.2.10 or later])]
+		[AC_MSG_ERROR([PolarSSL 1.3.x required])]
 	)
 
 	polarssl_with_pkcs11="no"
