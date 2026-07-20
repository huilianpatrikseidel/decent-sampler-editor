#pragma once

#include <QtCore/qglobal.h>

#if defined(MAPPERLIB_BUILD)
#  define MAPPERLIB_EXPORT Q_DECL_EXPORT
#else
#  define MAPPERLIB_EXPORT Q_DECL_IMPORT
#endif
