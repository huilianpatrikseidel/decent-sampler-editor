#pragma once
#include <QtCore/qglobal.h>

#if defined(NODEMAP_BUILD)
#  define NODEMAP_EXPORT Q_DECL_EXPORT
#else
#  define NODEMAP_EXPORT Q_DECL_IMPORT
#endif
