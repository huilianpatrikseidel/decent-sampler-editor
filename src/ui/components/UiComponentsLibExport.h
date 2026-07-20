#pragma once

#include <QtCore/qglobal.h>

#if defined(UICOMPONENTSLIB_BUILD)
#  define UICOMPONENTSLIB_EXPORT Q_DECL_EXPORT
#else
#  define UICOMPONENTSLIB_EXPORT Q_DECL_IMPORT
#endif
