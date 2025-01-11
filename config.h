/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Define if building universal (internal helper macro) */
#undef AC_APPLE_UNIVERSAL_BUILD

/* Make use of ARM4 assembly optimizations */
#undef ARM4_ASM

/* Make use of ARM5E assembly optimizations */
#undef ARM5E_ASM

/* Make use of Blackfin assembly optimizations */
#undef BFIN_ASM

/* Disable all parts of the API that are using floats */
#undef DISABLE_FLOAT_API

/* Symbol visibility prefix */

#define EXPORT

/* Debug fixed-point implementation */
#undef FIXED_DEBUG

/* Compile as fixed-point */
#undef FIXED_POINT

/* Compile as floating-point */
#define FLOATING_POINT

/* Define to 1 if you have the <dlfcn.h> header file. */
#undef HAVE_DLFCN_H

/* Define to 1 if you have the <inttypes.h> header file. */
#undef HAVE_INTTYPES_H

/* Define to 1 if you have the <memory.h> header file. */
#undef HAVE_MEMORY_H

/* Define to 1 if you have the <stdint.h> header file. */
#undef HAVE_STDINT_H

/* Define to 1 if you have the <stdlib.h> header file. */
#undef HAVE_STDLIB_H

/* Define to 1 if you have the <strings.h> header file. */
#undef HAVE_STRINGS_H

/* Define to 1 if you have the <string.h> header file. */
#undef HAVE_STRING_H

/* Define to 1 if you have the <sys/audioio.h> header file. */
#undef HAVE_SYS_AUDIOIO_H

/* Define to 1 if you have the <sys/soundcard.h> header file. */
#undef HAVE_SYS_SOUNDCARD_H

/* Define to 1 if you have the <sys/stat.h> header file. */
#undef HAVE_SYS_STAT_H

/* Define to 1 if you have the <sys/types.h> header file. */
#undef HAVE_SYS_TYPES_H

/* Define to 1 if you have the <unistd.h> header file. */
#undef HAVE_UNISTD_H

/* Define to the sub-directory where libtool stores uninstalled libraries. */
#undef LT_OBJDIR

/* Define to the address where bug reports for this package should be sent. */
#undef PACKAGE_BUGREPORT

/* Define to the full name of this package. */
#undef PACKAGE_NAME

/* Define to the full name and version of this package. */
#undef PACKAGE_STRING

/* Define to the one symbol short name of this package. */
#undef PACKAGE_TARNAME

/* Define to the home page for this package. */
#undef PACKAGE_URL

/* Define to the version of this package. */
#undef PACKAGE_VERSION

/* Resample with full SINC table (no interpolation) */
#undef RESAMPLE_FULL_SINC_TABLE

/* The size of `int', as computed by sizeof. */
#undef SIZEOF_INT

/* The size of `int16_t', as computed by sizeof. */
#undef SIZEOF_INT16_T

/* The size of `int32_t', as computed by sizeof. */
#undef SIZEOF_INT32_T

/* The size of `long', as computed by sizeof. */
#undef SIZEOF_LONG

/* The size of `short', as computed by sizeof. */
#undef SIZEOF_SHORT

/* The size of `uint16_t', as computed by sizeof. */
#undef SIZEOF_UINT16_T

/* The size of `uint32_t', as computed by sizeof. */
#undef SIZEOF_UINT32_T

/* The size of `u_int16_t', as computed by sizeof. */
#undef SIZEOF_U_INT16_T

/* The size of `u_int32_t', as computed by sizeof. */
#undef SIZEOF_U_INT32_T

/* Define to 1 if you have the ANSI C header files. */
#undef STDC_HEADERS

/* Enable support for TI C55X DSP */
#undef TI_C55X

/* Use FFTW3 for FFT */
#undef USE_GPL_FFTW3

/* Use Intel Math Kernel Library for FFT */
#undef USE_INTEL_MKL

/* Use KISS Fast Fourier Transform */
#define USE_KISS_FFT

/* Enable NEON support */
#undef USE_NEON

/* Use FFT from OggVorbis */
#undef USE_SMALLFT

/* Enable SSE support */
#undef USE_SSE

/* Enable SSE2 support */
#undef USE_SSE2

/* Use C99 variable-size arrays */
#undef VAR_ARRAYS

/* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most
   significant byte first (like Motorola and SPARC, unlike Intel). */
#if defined AC_APPLE_UNIVERSAL_BUILD
# if defined __BIG_ENDIAN__
#  define WORDS_BIGENDIAN 1
# endif
#else
# ifndef WORDS_BIGENDIAN
#  undef WORDS_BIGENDIAN
# endif
#endif

/* Define to empty if `const' does not conform to ANSI C. */
#undef const

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
#undef inline
#endif

/* Define to the equivalent of the C99 'restrict' keyword, or to
   nothing if this is not supported.  Do not define if restrict is
   supported directly.  */
#undef restrict
/* Work around a bug in Sun C++: it does not support _Restrict or
   __restrict__, even though the corresponding Sun C compiler ends up with
   "#define restrict _Restrict" or "#define restrict __restrict__" in the
   previous line.  Perhaps some future version of Sun C++ will work with
   restrict; if so, hopefully it defines __RESTRICT like Sun C does.  */
#if defined __SUNPRO_CC && !defined __RESTRICT
# define _Restrict
# define __restrict__
#endif
