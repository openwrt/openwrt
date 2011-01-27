dnl Copyright 2005 Red Hat, Inc
dnl 
dnl Permission to use, copy, modify, distribute, and sell this software and its
dnl documentation for any purpose is hereby granted without fee, provided that
dnl the above copyright notice appear in all copies and that both that
dnl copyright notice and this permission notice appear in supporting
dnl documentation.
dnl 
dnl The above copyright notice and this permission notice shall be included
dnl in all copies or substantial portions of the Software.
dnl 
dnl THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
dnl OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
dnl MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
dnl IN NO EVENT SHALL THE OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR
dnl OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
dnl ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
dnl OTHER DEALINGS IN THE SOFTWARE.
dnl 
dnl Except as contained in this notice, the name of the copyright holders shall
dnl not be used in advertising or otherwise to promote the sale, use or
dnl other dealings in this Software without prior written authorization
dnl from the copyright holders.
dnl 

# XAW_CHECK_XPRINT_SUPPORT()
# --------------------------
# Adds --enable/disable-xprint and selects the appropriate version of the Xaw 
# library. If neither --enable-xprint nor --disable-xprint are given, 
# the presence of an Xaw with Xprint support will be auto detected

AC_DEFUN([XAW_CHECK_XPRINT_SUPPORT],[
	AC_ARG_ENABLE(xprint, AC_HELP_STRING([--enable-xprint], [Enable XPrint support]),
			[use_xprint=$enableval],[use_xprint=auto])
	if test "x$use_xprint" = "xyes"; then
		TMP_CHECK1=xaw8
		TMP_CHECK2=
		xaw_use_xprint=yes
	elif test "x$use_xprint" = "xno"; then
		TMP_CHECK1=xaw7
		TMP_CHECK2=
		xaw_use_xprint=no
	else
		TMP_CHECK1=xaw8
		TMP_CHECK2=xaw7
		xaw_use_xprint=yes
	fi

	PKG_CHECK_MODULES(TMP_XAW, $TMP_CHECK1, success=yes, success=no)
	if [[ ! -z $TMP_CHECK2 ]] ; then
		if test $success = no ; then
			PKG_CHECK_MODULES(TMP_XAW, $TMP_CHECK2, success=yes, success=no)
			xaw_use_xprint=no
		fi
	fi

	if test "x$success" = "xyes"; then
		$1_CFLAGS=$TMP_XAW_CFLAGS
		$1_LIBS=$TMP_XAW_LIBS

		AM_CONDITIONAL([XAW_USE_XPRINT], [test "x$xaw_use_xprint" = "xyes"])
	else
		AC_MSG_ERROR([No suitable version of Xaw found])
	fi
])
