#pragma once
#include <QtCore/qglobal.h>

#if defined(VST3HOST_BUILD)
#  define VST3HOST_EXPORT Q_DECL_EXPORT
#else
#  define VST3HOST_EXPORT Q_DECL_IMPORT
#endif
