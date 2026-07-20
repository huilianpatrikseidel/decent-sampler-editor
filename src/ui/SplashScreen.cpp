#include "SplashScreen.h"
#include <QPainter>
#include <QPainterPath>
#include <QScreen>
#include <QGuiApplication>
#include <QCloseEvent>
#include "../core/ThemePalette.h"

extern const char* APP_BUILD_NUMBER;
static constexpr int SHADOW_RADIUS = 0;

SplashScreen::SplashScreen(QWidget *parent) 
    : QWidget(parent, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint)
    , m_renderer(QString(":/ui/splashscreen.svg"))
{
    setAttribute(Qt::WA_TranslucentBackground);
    
    // Content size + shadow margin on all sides
    setFixedSize(720 + SHADOW_RADIUS * 2, 360 + SHADOW_RADIUS * 2);
    
    // Center on primary screen
    if (QScreen* screen = QGuiApplication::primaryScreen()) {
        QRect screenGeometry = screen->availableGeometry();
        move((screenGeometry.width() - width()) / 2,
             (screenGeometry.height() - height()) / 2);
    }
}

void SplashScreen::setProgress(int percent, const QString& message) {
    m_percent = qBound(0, percent, 100);
    m_message = message;
    update();
}

void SplashScreen::closeEvent(QCloseEvent *event) {
    emit cancelRequested();
    QWidget::closeEvent(event);
}

void SplashScreen::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::SmoothPixmapTransform);
    
    const int S = SHADOW_RADIUS;
    const int radius = 31; // Matches SVG corner radius: 11.45 * (720 / 264.58)
    QRect contentRect(S, S, width() - S * 2, height() - S * 2);
    
    // Drop shadow removed
    
    // --- Clipped content area ---
    QPainterPath clipPath;
    clipPath.addRoundedRect(contentRect, radius, radius);
    p.setClipPath(clipPath);
    
    // The background fill was removed to allow SVG transparencies to show through
    
    // Render the SVG as background
    m_renderer.render(&p, QRectF(contentRect));
    
    // --- Progress bar in the dark lower band ---
    const int barHeight = 4;
    const int barMarginH = 24;
    const int barY = contentRect.bottom() - 26;
    const int barWidth = contentRect.width() - barMarginH * 2;
    const int barX = contentRect.left() + barMarginH;
    
    // Background track
    p.setPen(Qt::NoPen);
    p.setBrush(ThemePalette::color("qss_color_30", QColor(0, 0, 0, 80)));
    p.drawRoundedRect(barX, barY, barWidth, barHeight, 2, 2);
    
    const int fillWidth = static_cast<int>(barWidth * m_percent / 100.0);
    if (fillWidth > 0) {
        QLinearGradient grad(barX, 0, barX + fillWidth, 0);
        grad.setColorAt(0.0, ThemePalette::color("qss_color_6", QColor(255, 102, 0)));    // #ff6600
        grad.setColorAt(1.0, ThemePalette::color("qss_color_6", QColor(255, 153, 51)));    // lighter orange
        p.setBrush(grad);
        p.drawRoundedRect(barX, barY, fillWidth, barHeight, 2, 2);
    }
    
    QFont font("Segoe UI", 9);
    font.setWeight(QFont::Normal);
    p.setFont(font);
    p.setPen(ThemePalette::color("qss_color_8", QColor(200, 200, 200, 200)));
    
    QRect textRect(barX, barY + barHeight + 2, barWidth, 20);
    p.drawText(textRect, Qt::AlignLeft | Qt::AlignTop, m_message);

    // --- Version text (right aligned) ---
    p.setPen(ThemePalette::color("qss_color_8", QColor(160, 160, 160, 150)));
    QString versionStr = QString("v1.0.%1").arg(APP_BUILD_NUMBER);
    p.drawText(textRect, Qt::AlignRight | Qt::AlignTop, versionStr);
}
