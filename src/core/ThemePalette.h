#pragma once
#include <QString>
#include <QColor>
#include <QHash>

#ifdef CORELIB_BUILD
#define CORELIB_EXPORT __declspec(dllexport)
#else
#define CORELIB_EXPORT __declspec(dllimport)
#endif

class CORELIB_EXPORT ThemePalette {
public:
    static QColor color(const QString& key, const QColor& defaultColor = Qt::black);
    static QString colorHex(const QString& key, const QString& defaultHex = "#000000");
    static void setPalette(const QHash<QString, QString>& palette);
private:
    static QHash<QString, QString> s_palette;
};
