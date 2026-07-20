#pragma once
#include <QtCore/qglobal.h>

#if defined(CORELIB_BUILD)
#  define CORELIB_EXPORT Q_DECL_EXPORT
#else
#  define CORELIB_EXPORT Q_DECL_IMPORT
#endif
