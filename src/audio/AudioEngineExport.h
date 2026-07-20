#pragma once
#include <QtCore/qglobal.h>

#if defined(AUDIOENGINE_BUILD)
#  define AUDIOENGINE_EXPORT Q_DECL_EXPORT
#else
#  define AUDIOENGINE_EXPORT Q_DECL_IMPORT
#endif
