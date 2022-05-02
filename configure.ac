AC_INIT(
    [xfce4-brightness-plugin],
    [0.0],
    [https://github.com/Solitai7e/xfce4-brightness-plugin/issues],
    [xfce4-brightness-plugin],
    [https://github.com/Solitai7e/xfce4-brightness-plugin],
)


AC_PROG_CC_C99

AC_PATH_PROG([VALAC],        [valac])
AC_PATH_PROG([COMPILE_GRES], [glib-compile-resources])


PKG_CHECK_MODULES([libbrightness], m4_normalize([
    gtk+-3.0
    libxfce4panel-2.0
    libxfce4util-1.0
    libxfconf-0
    x11
    xrandr
]))


AX_APPEND_COMPILE_FLAGS(m4_normalize([
    -shared
    -fPIC
    -Wfatal-errors
    -Wno-deprecated-declarations
    -Wno-unused-variable
    -Wno-unused-but-set-variable
]))

AX_APPEND_FLAG([--pkg=gtk+-3.0],    [VALAFLAGS])
AX_APPEND_FLAG([--pkg=gdk-x11-3.0], [VALAFLAGS])
AX_APPEND_FLAG([--pkg=x11],         [VALAFLAGS])

AC_SUBST([VALAFLAGS])

AC_SUBST([LD],      ["${LD:-$CC}"])
AC_SUBST([LDFLAGS], ["${LDFLAGS:-$CFLAGS}"])


AC_CONFIG_FILES([GNUmakefile])
AC_OUTPUT
