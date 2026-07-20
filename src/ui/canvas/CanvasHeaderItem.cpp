#include "CanvasHeaderItem.h"
#include <QFont>
#include <QFontMetrics>
#include "../../core/ThemePalette.h"

CanvasHeaderItem::CanvasHeaderItem(ProjectManager* pm, QGraphicsItem* parent) 
    : QGraphicsItem(parent), m_pm(pm), m_headerHeight(45) {
    // We are going to draw on top of everything except maybe floating dialogs
    setZValue(100);
    int w = m_pm ? m_pm->getUiState()->getWidth() : 812;
    m_rect = QRectF(0, 0, w, m_headerHeight);
}

void CanvasHeaderItem::setGeometry(const QRectF& rect) {
    prepareGeometryChange();
    m_rect = rect;
}

QRectF CanvasHeaderItem::boundingRect() const {
    return m_rect;
}

void CanvasHeaderItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);
    
    if (!m_pm) return;
    int w = m_pm->getUiState()->getWidth();
    
    painter->setRenderHint(QPainter::Antialiasing, false);

    // ==========================================
    // 1. DRAW HEADER (1:1 Replica)
    // ==========================================
    // Header background (60% transparent black)
    QRectF header(0, 0, w, m_headerHeight);
    painter->fillRect(header, ThemePalette::color("qss_color_30", QColor(0, 0, 0, 150)));
    
    // Header text setup
    painter->setPen(ThemePalette::color("qss_color_8", Qt::white));
    QFont f = painter->font();
    f.setPixelSize(12);
    
    // Calculate widths for the 7 sections
    double w1 = 58;  // ds logo is narrower
    double w2 = 110; // BROWSE
    double w6 = 85;  // FILE...
    double w4 = 85;  // VOLUME
    double wMeter = 20; // Level Meter is narrower
    double w5 = 85;  // TUNE
    double w3 = w - (w1 + w2 + w4 + wMeter + w5 + w6); // Title (dynamic)
    
    // Draw Separators (Full height, no padding)
    painter->setPen(QPen(ThemePalette::color("qss_color_9", QColor(255, 255, 255, 45)), 1));
    double curX = w1; painter->drawLine(curX, 0, curX, m_headerHeight);
    curX += w2; painter->drawLine(curX, 0, curX, m_headerHeight);
    curX += w3; painter->drawLine(curX, 0, curX, m_headerHeight);
    curX += w4; painter->drawLine(curX, 0, curX, m_headerHeight);
    curX += wMeter; painter->drawLine(curX, 0, curX, m_headerHeight);
    curX += w5; painter->drawLine(curX, 0, curX, m_headerHeight);
    
    // Draw Text / Content for each section
    painter->setPen(ThemePalette::color("qss_color_8", Qt::white));
    
    // Sec 1: ds logo
    f.setFamily("Segoe UI");
    f.setPixelSize(26);
    f.setBold(true);
    f.setWeight(QFont::Bold);
    painter->setFont(f);
    QFontMetrics fmD(f);
    int wD = fmD.horizontalAdvance("d");
    
    f.setPixelSize(18);
    f.setBold(false);
    f.setWeight(QFont::Thin);
    painter->setFont(f);
    QFontMetrics fmS(f);
    int wS = fmS.horizontalAdvance("S");
    
    int spacing = 1;
    int totalW = wD + spacing + wS;
    int startX = (w1 - totalW) / 2;
    int baselineY = m_headerHeight / 2 + 8;
    
    // Draw d
    f.setPixelSize(26);
    f.setBold(true);
    f.setWeight(QFont::Bold);
    painter->setFont(f);
    painter->drawText(startX, baselineY, "d");
    
    // Draw S
    f.setPixelSize(18);
    f.setBold(false);
    f.setWeight(QFont::Thin);
    painter->setFont(f);
    painter->drawText(startX + wD + spacing, baselineY, "S");
    
    // Sec 2: BROWSE
    f.setPixelSize(12); f.setBold(false); f.setWeight(QFont::Normal); painter->setFont(f);
    painter->drawText(QRectF(w1, 0, w2, m_headerHeight), Qt::AlignCenter, "BROWSE");
    
    // Sec 3: Title
    f.setPixelSize(14); f.setBold(false); f.setWeight(QFont::Normal); painter->setFont(f);
    painter->drawText(QRectF(w1 + w2, 0, w3, m_headerHeight), Qt::AlignCenter, "Preset Browser");
    
    // Draw Title Chevrons
    double titleStartX = w1 + w2;
    double chevY = m_headerHeight / 2.0;
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(QPen(ThemePalette::color("qss_color_17", QColor(255, 255, 255, 180)), 1.5));
    // Left Chevron
    double chevLX = titleStartX + 20;
    painter->drawLine(QPointF(chevLX + 3, chevY - 5), QPointF(chevLX - 2, chevY));
    painter->drawLine(QPointF(chevLX - 2, chevY), QPointF(chevLX + 3, chevY + 5));
    // Right Chevron
    double chevRX = titleStartX + w3 - 20;
    painter->drawLine(QPointF(chevRX - 3, chevY - 5), QPointF(chevRX + 2, chevY));
    painter->drawLine(QPointF(chevRX + 2, chevY), QPointF(chevRX - 3, chevY + 5));
    painter->setRenderHint(QPainter::Antialiasing, false);
    
    // Sec 4: VOLUME
    painter->setPen(ThemePalette::color("qss_color_17", QColor(220, 220, 220)));
    f.setPixelSize(9); f.setBold(false); painter->setFont(f);
    painter->drawText(QRectF(w1 + w2 + w3, 6, w4, m_headerHeight/2), Qt::AlignBottom | Qt::AlignHCenter, "VOLUME");
    painter->setPen(ThemePalette::color("qss_color_8", Qt::white));
    f.setPixelSize(11); f.setBold(true); painter->setFont(f);
    painter->drawText(QRectF(w1 + w2 + w3, m_headerHeight/2, w4, m_headerHeight/2), Qt::AlignTop | Qt::AlignHCenter, "0.00 dB");
    
    // Sec 5: Level Meter
    double meterX = w1 + w2 + w3 + w4 + (wMeter - 8) / 2.0;
    double meterY = 10;
    double meterH = m_headerHeight - 20;
    painter->fillRect(QRectF(meterX, meterY, 2.5, meterH), ThemePalette::color("qss_color_9", QColor(255, 255, 255, 45)));
    painter->fillRect(QRectF(meterX + 5, meterY, 2.5, meterH), ThemePalette::color("qss_color_9", QColor(255, 255, 255, 45)));
    
    // Sec 6: TUNE
    painter->setPen(ThemePalette::color("qss_color_17", QColor(220, 220, 220)));
    f.setPixelSize(9); f.setBold(false); painter->setFont(f);
    painter->drawText(QRectF(w1 + w2 + w3 + w4 + wMeter, 6, w5, m_headerHeight/2), Qt::AlignBottom | Qt::AlignHCenter, "TUNE");
    painter->setPen(ThemePalette::color("qss_color_8", Qt::white));
    f.setPixelSize(11); f.setBold(true); painter->setFont(f);
    painter->drawText(QRectF(w1 + w2 + w3 + w4 + wMeter, m_headerHeight/2, w5, m_headerHeight/2), Qt::AlignTop | Qt::AlignHCenter, "0.00 st");
    
    // Sec 7: FILE...
    f.setPixelSize(12); f.setBold(false); f.setWeight(QFont::Normal); painter->setFont(f);
    painter->drawText(QRectF(w1 + w2 + w3 + w4 + wMeter + w5, 0, w6, m_headerHeight), Qt::AlignCenter, "FILE...");
}
