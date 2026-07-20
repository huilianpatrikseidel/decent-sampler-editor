#pragma once
#include <QtCore/qglobal.h>

#if defined(UIDESIGNER_BUILD)
#  define UIDESIGNER_EXPORT Q_DECL_EXPORT
#else
#  define UIDESIGNER_EXPORT Q_DECL_IMPORT
#endif
