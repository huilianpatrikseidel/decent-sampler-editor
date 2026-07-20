#pragma once
#include <QMainWindow>

#include <QString>
#include <QColor>

class ThemeManager {
public:
    static void applyTheme(QMainWindow* mainWindow);

#ifdef Q_OS_WIN
    static bool handleNativeEvent(QMainWindow* mainWindow, const QByteArray &eventType, void *message, qintptr *result);
#endif
};
