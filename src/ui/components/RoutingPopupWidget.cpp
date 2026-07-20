#include "RoutingPopupWidget.h"
#include "SynthKnobWidget.h"
#include "ModRoutingWidget.h"
#include <QPainter>
#include <QPainterPath>
#include <QVBoxLayout>
#include "../../core/ThemePalette.h"

RoutingPopupWidget::RoutingPopupWidget(SynthKnobWidget* knob, const QList<ModSourceHelper::ModSource>& sources, const QVector<ModRouting>& routings)
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
    
    // Separate sources into standard and macro
    QList<ModSourceHelper::ModSource> stdSources;
    QList<ModSourceHelper::ModSource> macroSources;
    for (const auto& src : sources) {
        if (src.id.startsWith("MACRO_")) macroSources.append(src);
        else stdSources.append(src);
    }
    
    // Find matching routings
    ModRouting stdRouting, macroRouting;
    for (const auto& r : routings) {
        if (r.source.startsWith("MACRO_")) {
            macroRouting = r;
        } else if (!r.source.isEmpty()) {
            stdRouting = r;
        }
    }
    
    if (stdRouting.destination.isEmpty() && !routings.isEmpty()) stdRouting.destination = routings.first().destination;
    if (macroRouting.destination.isEmpty() && !routings.isEmpty()) macroRouting.destination = routings.first().destination;
    
    // 1. Standard Modulator Section
    ModRoutingWidget* stdRw = new ModRoutingWidget(stdRouting, stdSources, destinations, true, contentWidget);
    stdRw->setParameterLimits(knob->minimum(), knob->maximum());
    stdRw->setBaseValue(knob->value());
    
    // 2. Separator
    QFrame* sep = new QFrame();
    sep->setFrameShape(QFrame::HLine);
    sep->setStyleSheet("border: 1px solid rgba(255, 255, 255, 0.1); margin: 4px 0px;");
    
    // 3. Macro Section
    ModRoutingWidget* macroRw = new ModRoutingWidget(macroRouting, macroSources, destinations, true, contentWidget);
    macroRw->setParameterLimits(knob->minimum(), knob->maximum());
    macroRw->setBaseValue(knob->value());
    
    layout->addWidget(stdRw);
    layout->addWidget(sep);
    layout->addWidget(macroRw);
    
    mainLayout->addWidget(contentWidget);
    
    auto updateRoutings = [this, stdRw, macroRw, knob]() {
        QVector<ModRouting> newRoutings;
        if (!stdRw->getRouting().source.isEmpty()) newRoutings.append(stdRw->getRouting());
        if (!macroRw->getRouting().source.isEmpty()) newRoutings.append(macroRw->getRouting());
        knob->setModRoutings(newRoutings);
        emit knob->modRoutingsChanged(newRoutings);
    };
    
    connect(stdRw, &ModRoutingWidget::routingChanged, this, [updateRoutings](const ModRouting&) { updateRoutings(); });
    connect(macroRw, &ModRoutingWidget::routingChanged, this, [updateRoutings](const ModRouting&) { updateRoutings(); });
    
    connect(knob, &SynthKnobWidget::modRoutingsUpdatedExternally, this, [stdRw, macroRw](const QVector<ModRouting>& newRs) {
        ModRouting stdR, macR;
        for (const auto& r : newRs) {
            if (r.source.startsWith("MACRO_")) macR = r;
            else if (!r.source.isEmpty()) stdR = r;
        }
        stdRw->setRouting(stdR);
        macroRw->setRouting(macR);
    });
    
    connect(stdRw, &ModRoutingWidget::baseValueChanged, this, [this, knob, macroRw](double newBase) {
        knob->setValue(newBase);
        macroRw->setBaseValue(newBase);
    });
    
    connect(macroRw, &ModRoutingWidget::baseValueChanged, this, [this, knob, stdRw](double newBase) {
        knob->setValue(newBase);
        stdRw->setBaseValue(newBase);
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
