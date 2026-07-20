#include "RoutingPopupWidget.h"
#include "SynthKnobWidget.h"
#include "ModRoutingWidget.h"
#include <QPainter>
#include <QPainterPath>
#include <QVBoxLayout>
#include "../../core/ThemePalette.h"

RoutingPopupWidget::RoutingPopupWidget(SynthKnobWidget* knob, const QStringList& sources, const ModRouting& routing)
    : QWidget(nullptr, Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint),
      m_knob(knob)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(16, 24, 16, 16); // 24 at top for arrow + shadow padding
    
    QWidget* contentWidget = new QWidget();
    contentWidget->setStyleSheet("background: transparent; color: #ccc; font-size: 11px;");
    QVBoxLayout* layout = new QVBoxLayout(contentWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    
    QStringList destinations = {"Pitch", "Volume", "Pan", "Cutoff", "Resonance", "AmpEnv_Attack", "AmpEnv_Decay", "AmpEnv_Sustain", "AmpEnv_Release"};
    ModRoutingWidget* rw = new ModRoutingWidget(routing, sources, destinations, true, contentWidget);
    rw->setParameterLimits(knob->minimum(), knob->maximum());
    rw->setBaseValue(knob->value());
    
    layout->addWidget(rw);
    mainLayout->addWidget(contentWidget);
    
    // Handle changes from the RangeSlider inside ModRoutingWidget
    connect(rw, &ModRoutingWidget::routingChanged, this, [this, knob](const ModRouting& newR) {
        knob->setModRouting(newR);
        emit knob->modulationChanged(newR);
    });
    
    connect(knob, &SynthKnobWidget::modRoutingUpdatedExternally, this, [rw](const ModRouting& newR) {
        rw->setRouting(newR);
    });
    
    connect(rw, &ModRoutingWidget::baseValueChanged, this, [this, knob](double newBase) {
        knob->setValue(newBase);
    });
}

void RoutingPopupWidget::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRectF rect(8, 16, width() - 16, height() - 24); // 8px shadow, extra 8 top for arrow
    
    QPainterPath path;
    path.moveTo(rect.left() + 6, rect.top());
    path.lineTo(width() / 2.0 - 8, rect.top());
    path.lineTo(width() / 2.0, rect.top() - 8);
    path.lineTo(width() / 2.0 + 8, rect.top());
    path.lineTo(rect.right() - 6, rect.top());
    path.arcTo(rect.right() - 12, rect.top(), 12, 12, 90, -90);
    path.lineTo(rect.right(), rect.bottom() - 6);
    path.arcTo(rect.right() - 12, rect.bottom() - 12, 12, 12, 0, -90);
    path.lineTo(rect.left() + 6, rect.bottom());
    path.arcTo(rect.left(), rect.bottom() - 12, 12, 12, 270, -90);
    path.lineTo(rect.left(), rect.top() + 6);
    path.arcTo(rect.left(), rect.top(), 12, 12, 180, -90);
    path.closeSubpath();
    
    painter.setPen(Qt::NoPen);
    painter.setBrush(ThemePalette::color("qss_color_30", QColor(0, 0, 0, 80)));
    painter.drawPath(path.translated(0, 2));

    painter.setBrush(ThemePalette::color("qss_color_25", QColor(42, 42, 44)));
    painter.setPen(QPen(ThemePalette::color("qss_color_17", QColor(68, 68, 68)), 1));
    painter.drawPath(path);
}
