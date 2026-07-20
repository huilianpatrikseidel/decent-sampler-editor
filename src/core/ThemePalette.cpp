#include "ThemePalette.h"

QHash<QString, QString> ThemePalette::s_palette;

QColor ThemePalette::color(const QString& key, const QColor& defaultColor) {
    if (s_palette.contains(key)) {
        return QColor(s_palette.value(key));
    }
    return defaultColor;
}

QString ThemePalette::colorHex(const QString& key, const QString& defaultHex) {
    if (s_palette.contains(key)) {
        return s_palette.value(key);
    }
    return defaultHex;
}

void ThemePalette::setPalette(const QHash<QString, QString>& palette) {
    s_palette = palette;
}
