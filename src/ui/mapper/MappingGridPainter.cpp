#include "MappingGridPainter.h"
#include <QPainter>
#include <QRectF>
#include "../../core/AppSettings.h"
#include "../../core/ThemePalette.h"

void MappingGridPainter::paintBackground(QPainter* painter, const QRectF& rect, double noteWidth, double velocityHeight) {
    painter->fillRect(rect, ThemePalette::color("grid_bg"));
    
    for (int i = 0; i <= 128; ++i) {
        int noteInOctave = i % 12;
        bool isBlack = (noteInOctave == 1 || noteInOctave == 3 || noteInOctave == 6 || 
                        noteInOctave == 8 || noteInOctave == 10);
        
        double x = i * noteWidth;
        if (x + noteWidth >= rect.left() && x <= rect.right()) {
            if (isBlack) {
                painter->fillRect(QRectF(x, rect.top(), noteWidth, rect.height()), ThemePalette::color("grid_black_keys")); 
            }
        }
    }
    
    for (int i = 0; i <= 128; ++i) {
        double x = i * noteWidth;
        if (x >= rect.left() && x <= rect.right()) {
            if (i % 12 == 0) {
                painter->setPen(QPen(ThemePalette::color("grid_octave_lines"), 1)); 
            } else {
                painter->setPen(QPen(ThemePalette::color("grid_other_lines"), 1));
            }
            painter->drawLine(x, rect.top(), x, rect.bottom());
        }
    }
    
    painter->setPen(QPen(ThemePalette::color("grid_other_lines"), 1));
    for (int i = 0; i <= 128; i += 16) { 
        double y = i * velocityHeight;
        if (y >= rect.top() && y <= rect.bottom()) {
            painter->drawLine(rect.left(), y, rect.right(), y);
        }
    }
}

void MappingGridPainter::paintForeground(QPainter* painter, const QRectF& viewRect, double velocityHeight) {
    double rulerWidth = 35.0;
    QRectF vrRect(viewRect.left(), viewRect.top(), rulerWidth, viewRect.height());
    
    QLinearGradient bgGrad(viewRect.left(), 0, viewRect.left() + rulerWidth, 0);
    bgGrad.setColorAt(0.0, ThemePalette::color("grid_ruler_bg_start"));
    bgGrad.setColorAt(1.0, ThemePalette::color("grid_ruler_bg_end"));
    painter->fillRect(vrRect, bgGrad);
    
    painter->setPen(QPen(ThemePalette::color("grid_ruler_border"), 1));
    painter->drawLine(viewRect.left() + rulerWidth, viewRect.top(), viewRect.left() + rulerWidth, viewRect.bottom());
    
    painter->setPen(ThemePalette::color("grid_ruler_text"));
    QFont font = painter->font();
    font.setPixelSize(10);
    painter->setFont(font);
    
    QList<int> velocities = {1, 32, 64, 96, 127};
    for (int v : velocities) {
        double y = (127 - v) * velocityHeight;
        if (y >= viewRect.top() && y <= viewRect.bottom()) {
            painter->drawLine(viewRect.left() + rulerWidth - 5, y, viewRect.left() + rulerWidth, y);
            painter->drawText(QRectF(viewRect.left(), y - 10, rulerWidth - 5, 20), Qt::AlignRight | Qt::AlignVCenter, QString::number(v));
        }
    }
}
