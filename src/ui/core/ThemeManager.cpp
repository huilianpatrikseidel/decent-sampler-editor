#include "ThemeManager.h"
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QLibrary>
#include "../components/CustomTitleBar.h"
#include "core/AppSettings.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QHash>
#include "../../core/ThemePalette.h"

#ifdef Q_OS_WIN
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <windowsx.h>
#endif

void ThemeManager::applyTheme(QMainWindow* mainWindow) {
    QString themeName = AppSettings::instance().theme();
    bool isLight = (themeName == "Light");
    QString jsonFile = isLight ? ":/themes/light.json" : ":/themes/dark.json";
    QHash<QString, QString> palette;
    QFile jFile(jsonFile);
    if (jFile.open(QFile::ReadOnly | QFile::Text)) {
        QJsonDocument doc = QJsonDocument::fromJson(jFile.readAll());
        if (doc.isObject()) {
            QJsonObject obj = doc.object();
            for (auto it = obj.begin(); it != obj.end(); ++it) {
                palette[it.key()] = it.value().toString();
            }
        }
        jFile.close();
    }
    ThemePalette::setPalette(palette);

    QFile styleFile(":/ui/style_template.qss");
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QString qssContent = QTextStream(&styleFile).readAll();
        for (auto it = palette.begin(); it != palette.end(); ++it) {
            qssContent.replace("@" + it.key(), it.value());
        }
        qApp->setStyleSheet(qssContent);
        styleFile.close();
    }

    QPalette appPalette;
    appPalette.setColor(QPalette::Window, QColor(palette.value("qss_color_1")));
    appPalette.setColor(QPalette::WindowText, QColor(palette.value("qss_color_8")));
    appPalette.setColor(QPalette::Base, QColor(palette.value("qss_color_22")));
    appPalette.setColor(QPalette::AlternateBase, QColor(palette.value("qss_color_20")));
    appPalette.setColor(QPalette::ToolTipBase, QColor(palette.value("qss_color_22")));
    appPalette.setColor(QPalette::ToolTipText, QColor(palette.value("qss_color_8")));
    appPalette.setColor(QPalette::Text, QColor(palette.value("qss_color_8")));
    appPalette.setColor(QPalette::Button, QColor(palette.value("qss_color_20")));
    appPalette.setColor(QPalette::ButtonText, QColor(palette.value("qss_color_8")));
    appPalette.setColor(QPalette::BrightText, Qt::red);
    appPalette.setColor(QPalette::Link, QColor(palette.value("qss_color_6")));
    appPalette.setColor(QPalette::Highlight, QColor(palette.value("qss_color_3")));
    appPalette.setColor(QPalette::HighlightedText, QColor(palette.value("qss_color_4")));
    
    appPalette.setColor(QPalette::Light, QColor(palette.value("qss_color_9")));
    appPalette.setColor(QPalette::Midlight, QColor(palette.value("qss_color_9")));
    appPalette.setColor(QPalette::Dark, QColor(palette.value("qss_color_1")));
    appPalette.setColor(QPalette::Mid, QColor(palette.value("qss_color_22")));
    appPalette.setColor(QPalette::Shadow, QColor(0, 0, 0));
    qApp->setPalette(appPalette);

#ifdef Q_OS_WIN
    int dark = isLight ? 0 : 1;
    using DwmSetWindowAttribute_t = long (*)(void* hwnd, unsigned int dwAttribute, const void* pvAttribute, unsigned int cbAttribute);
    QFunctionPointer func = QLibrary::resolve("dwmapi", "DwmSetWindowAttribute");
    if (func) {
        auto pDwmSetWindowAttribute = reinterpret_cast<DwmSetWindowAttribute_t>(func);
        pDwmSetWindowAttribute(reinterpret_cast<void*>(mainWindow->winId()), 20, &dark, sizeof(dark));
        pDwmSetWindowAttribute(reinterpret_cast<void*>(mainWindow->winId()), 19, &dark, sizeof(dark));
    }
    
    using DwmExtendFrameIntoClientArea_t = long (*)(void* hwnd, const void* pMarInset);
    QFunctionPointer func2 = QLibrary::resolve("dwmapi", "DwmExtendFrameIntoClientArea");
    if (func2) {
        auto pDwmExtendFrame = reinterpret_cast<DwmExtendFrameIntoClientArea_t>(func2);
        struct { int cxLeft; int cxRight; int cyTop; int cyBottom; } margins = {0, 0, 0, 1};
        pDwmExtendFrame(reinterpret_cast<void*>(mainWindow->winId()), &margins);
    }
#endif
}


#ifdef Q_OS_WIN
bool ThemeManager::handleNativeEvent(QMainWindow* mainWindow, const QByteArray &eventType, void *message, qintptr *result) {
    MSG *msg = static_cast<MSG *>(message);
    
    if (msg->message == WM_NCCALCSIZE) {
        if (msg->wParam == TRUE) {
            if (IsZoomed(msg->hwnd)) {
                NCCALCSIZE_PARAMS* params = reinterpret_cast<NCCALCSIZE_PARAMS*>(msg->lParam);
                int frameX = GetSystemMetrics(SM_CXFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER);
                int frameY = GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER);
                params->rgrc[0].left   += frameX;
                params->rgrc[0].top    += frameY;
                params->rgrc[0].right  -= frameX;
                params->rgrc[0].bottom -= frameY;
            }
            *result = 0;
            return true;
        }
    }
    
    if (msg->message == WM_NCLBUTTONDOWN) {
        if (msg->wParam == HTMAXBUTTON) {
            CustomTitleBar* titleBar = qobject_cast<CustomTitleBar*>(mainWindow->menuWidget());
            if (titleBar) titleBar->setMaximizeButtonPressed(true);
            *result = 0;
            return true;
        }
    }
    
    if (msg->message == WM_NCLBUTTONUP) {
        if (msg->wParam == HTMAXBUTTON) {
            CustomTitleBar* titleBar = qobject_cast<CustomTitleBar*>(mainWindow->menuWidget());
            if (titleBar) {
                titleBar->setMaximizeButtonPressed(false);
                titleBar->setMaximizeButtonHovered(false);
            }
            if (mainWindow->isMaximized()) {
                mainWindow->showNormal();
            } else {
                mainWindow->showMaximized();
            }
            if (titleBar) titleBar->updateMaximizeButton(mainWindow->isMaximized());
            *result = 0;
            return true;
        }
    }
    
    if (msg->message == WM_NCMOUSEMOVE) {
        CustomTitleBar* titleBar = qobject_cast<CustomTitleBar*>(mainWindow->menuWidget());
        if (msg->wParam == HTMAXBUTTON) {
            if (titleBar) titleBar->setMaximizeButtonHovered(true);
            TRACKMOUSEEVENT tme = {};
            tme.cbSize = sizeof(tme);
            tme.dwFlags = TME_LEAVE | TME_NONCLIENT;
            tme.hwndTrack = msg->hwnd;
            TrackMouseEvent(&tme);
            *result = 0;
            return true;
        } else {
            if (titleBar) titleBar->setMaximizeButtonHovered(false);
        }
    }
    
    if (msg->message == WM_NCMOUSELEAVE) {
        CustomTitleBar* titleBar = qobject_cast<CustomTitleBar*>(mainWindow->menuWidget());
        if (titleBar) {
            titleBar->setMaximizeButtonHovered(false);
            titleBar->setMaximizeButtonPressed(false);
        }
    }
    
    if (msg->message == WM_NCHITTEST) {
        int x = GET_X_LPARAM(msg->lParam);
        int y = GET_Y_LPARAM(msg->lParam);
        
        QPoint globalPos(x, y);
        QPoint localPos = mainWindow->mapFromGlobal(globalPos);
        
        int border = 8;
        bool left = localPos.x() < border;
        bool right = localPos.x() > mainWindow->width() - border;
        bool top = localPos.y() < border;
        bool bottom = localPos.y() > mainWindow->height() - border;
        
        if (left && top) { *result = HTTOPLEFT; return true; }
        if (right && top) { *result = HTTOPRIGHT; return true; }
        if (left && bottom) { *result = HTBOTTOMLEFT; return true; }
        if (right && bottom) { *result = HTBOTTOMRIGHT; return true; }
        if (left) { *result = HTLEFT; return true; }
        if (right) { *result = HTRIGHT; return true; }
        if (top) { *result = HTTOP; return true; }
        if (bottom) { *result = HTBOTTOM; return true; }
        
        if (mainWindow->menuWidget() && localPos.y() < mainWindow->menuWidget()->height()) {
            CustomTitleBar* titleBar = qobject_cast<CustomTitleBar*>(mainWindow->menuWidget());
            if (titleBar) {
                int hit = titleBar->nativeHitTest(localPos);
                if (hit != 0) {
                    *result = hit;
                    return true;
                }
            }
        }
        return false;
    }
    return false;
}
#endif
