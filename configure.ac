AC_PREREQ([2.69])
AC_INIT(fcrackzip, 1.0)
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([main.c])
AC_CONFIG_HEADERS([config.h])

AC_PROG_CC
AC_HEADER_STDC

AC_CHECK_SIZEOF(short, 2)
AC_CHECK_SIZEOF(int,   4)
AC_CHECK_SIZEOF(long,  4)

AC_C_CONST
AC_C_INLINE

AC_DEFINE([USE_UNIX_REDIRECTION], 1, [Define if you can use unix-style redirection.])

AC_CHECK_PROGS(PERL,perl5 perl)
AM_CONDITIONAL(HAVE_PERL,test -n "$PERL")

if test "$GCC" = yes; then
   AC_DEFINE([USE_GCC_ASM], 1, [Define if you can use gcc inline assembly.])
   CFLAGS="$CFLAGS -funroll-all-loops -O3"
   AC_MSG_CHECKING(for pgcc/egcs)
   case `$CC -v 2>&1` in
      *pgcc-* )
         AC_MSG_RESULT(pgcc)
         ;;
      *egcs-* )
         AC_MSG_RESULT(egcs)
         ;;
   esac
   AC_MSG_CHECKING(for x86 asm)
   case `uname -m` in
      i386 | i486 | i586 | ix86 | i86 )
         AC_DEFINE([COMPILE_CPU], 5, [Define this to the default x86 target cpu (5 pentium, 6 ppro).])
         AC_MSG_RESULT(pentium)
         ;;
      i686 )
         AC_DEFINE([COMPILE_CPU], 6, [Define this to the default x86 target cpu (5 pentium, 6 ppro).])
         AC_MSG_RESULT(pentiumpro)
         ;;
      *)
         AC_MSG_RESULT(not x86)
   esac
else
   AC_MSG_RESULT(no gcc)
fi

AC_SUBST(PERL)
AC_CHECK_HEADERS(stdbool.h getopt.h unistd.h sys/time.h)
AC_CHECK_FUNCS(gettimeofday)
AC_CHECK_FUNCS(getopt_long,,[
   AC_LIBOBJ(getopt)
   AC_LIBOBJ(getopt1)
])

AH_BOTTOM([
typedef unsigned char u8;

#if SIZEOF_SHORT == 2
typedef unsigned short u16;
#elif SIZEOF_INT == 2
typedef unsigned int u16;
#else
#warn unable to find type with 2 bytes size
#endif

#if SIZEOF_INT == 4
typedef unsigned int u32;
#elif SIZEOF_LONG == 4
typedef unsigned long u32;
#else
#warn unable to find type with 4 bytes size
#endif

#if defined(__GNUC__) && defined(__i386__)
#define REGPARAM __attribute__((regparm(1)))
#else
#define REGPARAM
#endif
])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
