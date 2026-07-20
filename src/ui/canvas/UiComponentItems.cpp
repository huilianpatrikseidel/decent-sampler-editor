#include "UiComponentItems.h"
#include <QPainter>
#include <cmath>
#include "../../core/ThemePalette.h"

// ==========================================
// UiKnobItem
// ==========================================
UiKnobItem::UiKnobItem(const UiComponent* comp, QGraphicsItem* p) : UiComponentItem(comp, p) { updateComponentSpecifics(comp); }
void UiKnobItem::updateComponentSpecifics(const UiComponent* comp) {
    const UiKnob* knob = static_cast<const UiKnob*>(comp);
    m_currentValue = knob->currentValue;
    if (!knob->filmstripPath.isEmpty()) {
        m_renderer.load(knob->filmstripPath, knob->frameCount, knob->isVertical);
    }
}
void UiKnobItem::paintComponent(QPainter* painter) {
    if (m_renderer.isValid()) {
        QPixmap pixmap;
        QRect srcRect;
        if (m_renderer.getFrame(m_currentValue, pixmap, srcRect)) {
            painter->drawPixmap(boundingRect().toRect(), pixmap, srcRect);
        }
    } else {
        painter->setRenderHint(QPainter::Antialiasing);
        double radius = qMin(m_width, m_height) / 2.0;
        QPointF center(m_width / 2.0, m_height / 2.0);
        
        // DS Default Knob: dark grey circle
        painter->setBrush(QColor("#3A3A3A"));
        painter->setPen(QPen(QColor("#1A1A1A"), 1));
        painter->drawEllipse(center, radius - 2, radius - 2);
        
        // DS Default Knob Indicator: white line from center
        double angleDeg = 225.0 - m_currentValue * 270.0;
        double angleRad = angleDeg * M_PI / 180.0;
        double lineLength = radius - 4;
        QPointF endPt(center.x() + lineLength * std::cos(angleRad),
                      center.y() - lineLength * std::sin(angleRad)); // -y because y goes down
                      
        painter->setPen(QPen(ThemePalette::color("qss_color_8", Qt::white), 2, Qt::SolidLine, Qt::RoundCap));
        painter->drawLine(center, endPt);
    }
}

// ==========================================
// UiSliderItem
// ==========================================
UiSliderItem::UiSliderItem(const UiComponent* comp, QGraphicsItem* p) : UiComponentItem(comp, p) { updateComponentSpecifics(comp); }
void UiSliderItem::updateComponentSpecifics(const UiComponent* comp) {
    const UiSlider* slider = static_cast<const UiSlider*>(comp);
    m_currentValue = slider->currentValue;
}
void UiSliderItem::paintComponent(QPainter* painter) {
    painter->setRenderHint(QPainter::Antialiasing);
    
    // DS Default Slider: thin track
    double trackW = 4.0;
    QRectF trackRect((m_width - trackW) / 2.0, 0, trackW, m_height);
    painter->setBrush(QColor("#202020"));
    painter->setPen(Qt::NoPen);
    painter->drawRect(trackRect);
    
    // DS Default Slider: Handle
    double handleH = 12.0;
    double handleW = m_width;
    double handleY = (m_height - handleH) * (1.0 - m_currentValue);
    painter->setBrush(QColor("#707070"));
    painter->setPen(QPen(QColor("#1A1A1A"), 1));
    painter->drawRect(0, handleY, handleW, handleH);
}

// ==========================================
// UiButtonItem
// ==========================================
UiButtonItem::UiButtonItem(const UiComponent* comp, QGraphicsItem* p) : UiComponentItem(comp, p) { updateComponentSpecifics(comp); }
void UiButtonItem::updateComponentSpecifics(const UiComponent* comp) {
    const UiButton* btn = static_cast<const UiButton*>(comp);
    QString path = btn->isPressed ? btn->imagePathOn : btn->imagePathOff;
    if (m_imagePath != path) {
        m_imagePath = path;
        m_imageCache.load(m_imagePath);
    }
}
void UiButtonItem::paintComponent(QPainter* painter) {
    if (!m_imageCache.isNull()) {
        painter->drawImage(boundingRect(), m_imageCache);
    } else {
        painter->setRenderHint(QPainter::Antialiasing, false);
        // DS Default Button: Flat Grey Rectangle
        painter->setBrush(QColor("#555555"));
        painter->setPen(QPen(QColor("#1A1A1A"), 1));
        painter->drawRect(boundingRect().adjusted(0, 0, -1, -1));
        
        painter->setPen(ThemePalette::color("qss_color_8", Qt::white));
        QFont f = painter->font(); f.setPointSize(10); painter->setFont(f);
        painter->drawText(boundingRect(), Qt::AlignCenter, m_name.isEmpty() ? "Button" : m_name);
    }
}

// ==========================================
// UiMenuItem
// ==========================================
UiMenuItem::UiMenuItem(const UiComponent* comp, QGraphicsItem* p) : UiComponentItem(comp, p) {}
void UiMenuItem::paintComponent(QPainter* painter) {
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setBrush(QColor("#222222"));
    painter->setPen(QPen(QColor("#555555"), 1));
    painter->drawRoundedRect(boundingRect(), 3, 3);
    painter->setPen(ThemePalette::color("qss_color_8", Qt::white));
    painter->drawText(boundingRect().adjusted(5, 0, -20, 0), Qt::AlignVCenter | Qt::AlignLeft, m_name.isEmpty() ? "Menu" : m_name);
    painter->setBrush(ThemePalette::color("qss_color_8", Qt::white));
    painter->setPen(Qt::NoPen);
    QPolygonF triangle;
    triangle << QPointF(m_width - 15, m_height / 2 - 2)
             << QPointF(m_width - 5, m_height / 2 - 2)
             << QPointF(m_width - 10, m_height / 2 + 4);
    painter->drawPolygon(triangle);
}

// ==========================================
// UiXYPadItem
// ==========================================
UiXYPadItem::UiXYPadItem(const UiComponent* comp, QGraphicsItem* p) : UiComponentItem(comp, p) {}
void UiXYPadItem::paintComponent(QPainter* painter) {
    painter->setBrush(QColor("#000000"));
    painter->setPen(QPen(QColor("#333333"), 1));
    painter->drawRect(boundingRect());
    painter->setPen(QPen(QColor("#222222"), 1, Qt::DotLine));
    painter->drawLine(m_width/2, 0, m_width/2, m_height);
    painter->drawLine(0, m_height/2, m_width, m_height/2);
    painter->setBrush(QColor("#00FF00"));
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(QPointF(m_width * 0.5, m_height * 0.5), 4, 4);
}

// ==========================================
// UiOscilloscopeItem
// ==========================================
UiOscilloscopeItem::UiOscilloscopeItem(const UiComponent* comp, QGraphicsItem* p) : UiComponentItem(comp, p) {}
void UiOscilloscopeItem::paintComponent(QPainter* painter) {
    painter->setBrush(QColor("#000000"));
    painter->setPen(QPen(QColor("#333333"), 1));
    painter->drawRect(boundingRect());
    QPen wavePen(QColor("#00FF00"), 1);
    painter->setPen(wavePen);
    painter->setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
    path.moveTo(0, m_height/2);
    for(int i=1; i<m_width; ++i) {
        path.lineTo(i, m_height/2 + (m_height/3) * std::sin(i * 0.2));
    }
    painter->drawPath(path);
}

// ==========================================
// UiKeyboardItem
// ==========================================
UiKeyboardItem::UiKeyboardItem(const UiComponent* comp, QGraphicsItem* p) : UiComponentItem(comp, p) {}
void UiKeyboardItem::paintComponent(QPainter* painter) {
    int whiteKeyCount = 14; 
    double keyW = m_width / whiteKeyCount;
    painter->setPen(QPen(ThemePalette::color("qss_color_8", Qt::black), 1));
    painter->setBrush(ThemePalette::color("qss_color_15", Qt::white));
    for(int i=0; i<whiteKeyCount; ++i) {
        painter->drawRect(i * keyW, 0, keyW, m_height);
    }
    painter->setBrush(ThemePalette::color("qss_color_19", Qt::black));
    for(int i=0; i<whiteKeyCount; ++i) {
        if (i % 7 != 2 && i % 7 != 6) {
            painter->drawRect(i * keyW + keyW * 0.7, 0, keyW * 0.6, m_height * 0.6);
        }
    }
}

// ==========================================
// UiTabItem
// ==========================================
UiTabItem::UiTabItem(const UiComponent* comp, QGraphicsItem* p) : UiComponentItem(comp, p) {}
void UiTabItem::paintComponent(QPainter* painter) {
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setBrush(QColor("#111111"));
    painter->setPen(Qt::NoPen);
    painter->drawRect(boundingRect());
    painter->setBrush(QColor("#333333"));
    painter->drawRect(0, 0, m_width/2, m_height);
    painter->setPen(ThemePalette::color("qss_color_8", Qt::white));
    painter->drawText(QRectF(0, 0, m_width/2, m_height), Qt::AlignCenter, "Tab 1");
    painter->setPen(QColor("#777777"));
    painter->drawText(QRectF(m_width/2, 0, m_width/2, m_height), Qt::AlignCenter, "Tab 2");
}

// ==========================================
// UiLabelItem
// ==========================================
UiLabelItem::UiLabelItem(const UiComponent* comp, QGraphicsItem* p) : UiComponentItem(comp, p) { updateComponentSpecifics(comp); }
void UiLabelItem::updateComponentSpecifics(const UiComponent* comp) {
    const UiLabel* lbl = static_cast<const UiLabel*>(comp);
    m_text = lbl->text;
    m_font = lbl->font;
    m_fontSize = lbl->fontSize;
    m_colorHex = lbl->colorHex;
}
void UiLabelItem::paintComponent(QPainter* painter) {
    painter->setPen(QColor(m_colorHex.isEmpty() ? "#FFFFFF" : m_colorHex));
    QFont f = painter->font();
    if (!m_font.isEmpty()) f.setFamily(m_font);
    if (m_fontSize > 0) f.setPointSize(m_fontSize);
    painter->setFont(f);
    painter->drawText(boundingRect(), Qt::AlignCenter, m_text.isEmpty() ? "Label" : m_text);
}

// ==========================================
// UiShapeItem
// ==========================================
UiShapeItem::UiShapeItem(const UiComponent* comp, QGraphicsItem* p) : UiComponentItem(comp, p) { updateComponentSpecifics(comp); }
void UiShapeItem::updateComponentSpecifics(const UiComponent* comp) {
    const UiShape* s = static_cast<const UiShape*>(comp);
    m_shapeType = s->shapeType;
    m_fillColorHex = s->fillColorHex;
    m_strokeColorHex = s->strokeColorHex;
    m_strokeWidth = s->strokeWidth;
}
void UiShapeItem::paintComponent(QPainter* painter) {
    QColor fill(m_fillColorHex.isEmpty() ? "#404040" : m_fillColorHex);
    QColor stroke(m_strokeColorHex.isEmpty() ? "#808080" : m_strokeColorHex);
    painter->setBrush(fill);
    painter->setPen(QPen(stroke, m_strokeWidth));
    if (m_shapeType == "Ellipse") {
        painter->drawEllipse(boundingRect());
    } else {
        painter->drawRect(boundingRect());
    }
}

// ==========================================
// UiImageItem
// ==========================================
UiImageItem::UiImageItem(const UiComponent* comp, QGraphicsItem* p) : UiComponentItem(comp, p) { updateComponentSpecifics(comp); }
void UiImageItem::updateComponentSpecifics(const UiComponent* comp) {
    const UiImage* img = static_cast<const UiImage*>(comp);
    if (m_imagePath != img->imagePath) {
        m_imagePath = img->imagePath;
        m_imageCache.load(m_imagePath);
    }
}
void UiImageItem::paintComponent(QPainter* painter) {
    if (!m_imageCache.isNull()) {
        painter->drawImage(boundingRect(), m_imageCache);
    } else {
        painter->setBrush(ThemePalette::color("qss_color_35", Qt::darkGray));
        painter->drawRect(boundingRect());
        painter->setPen(ThemePalette::color("qss_color_8", Qt::white));
        painter->drawText(boundingRect(), Qt::AlignCenter, "No Image");
    }
}
