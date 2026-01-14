
/* @(#)s_lib_ver.c 5.1 93/09/24 */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 */

/*
 * MACRO for standards
 */

#include "fdlibm.h"

/*
 * define and initialize _LIB_VERSION
 */
#if 0
// Do not use the global data thing, to prevent jumps to __bss_start. - BL
_LIB_VERSION_TYPE _LIB_VERSION() {
#ifdef _POSIX_MODE
	return _POSIX_;
#else
#ifdef _XOPEN_MODE
	return _XOPEN_;
#else
#ifdef _SVID3_MODE
	return _SVID_;
#else					/* default _IEEE_MODE */
	return _IEEE_;
#endif
#endif
#endif

}
#endif

#ifdef _POSIX_MODE
_LIB_VERSION_TYPE _LIB_VERSION = _POSIX_;
#else
#ifdef _XOPEN_MODE
_LIB_VERSION_TYPE _LIB_VERSION = _XOPEN_;
#else
#ifdef _SVID3_MODE
_LIB_VERSION_TYPE _LIB_VERSION = _SVID_;
#else					/* default _IEEE_MODE */
_LIB_VERSION_TYPE _LIB_VERSION = _IEEE_;
#endif
#endif
#endif
