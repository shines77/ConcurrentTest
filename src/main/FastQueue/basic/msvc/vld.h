
#ifndef FASTQUEUE_BASIC_MSVC_VLD_H
#define FASTQUEUE_BASIC_MSVC_VLD_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "FastQueue/config/config.h"

/**********************************************************
 *
 *  Use Visual Leak Detector(vld) for Visual C++,
 *  Homepage: http://vld.codeplex.com/
 *
 **********************************************************/
#ifdef _MSC_VER

#if defined(_DEBUG) || !defined(NDEBUG)

#if defined(TI_USE_VLD) && (TI_USE_VLD != 0)

// If you don't install vld(Visual Leak Detector), comment this line please.
#include <vld.h>

#endif /* TI_USE_VLD */

#endif /* _DEBUG */

#endif /* _MSC_VER */

#endif /* !FASTQUEUE_BASIC_MSVC_VLD_H */
