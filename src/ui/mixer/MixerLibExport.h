#pragma once

#include <QtCore/qglobal.h>

#if defined(MIXERLIB_BUILD)
#  define MIXERLIB_EXPORT Q_DECL_EXPORT
#else
#  define MIXERLIB_EXPORT Q_DECL_IMPORT
#endif
