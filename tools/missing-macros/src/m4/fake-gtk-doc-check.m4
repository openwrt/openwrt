dnl fake-gtk-doc-check.m4 serial 1 (OpenWrt)
dnl Provide a fake GTK_DOC_CHECK macros which
dnl always defines false.

AC_DEFUN([GTK_DOC_CHECK],[AM_CONDITIONAL(ENABLE_GTK_DOC,false)])
