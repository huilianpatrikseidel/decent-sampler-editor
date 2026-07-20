#include "AdsrEditorView.h"
#include <QPainter>
#include <QMouseEvent>
#include <cmath>
#include <algorithm>
#include "../../core/AppSettings.h"
#include "../../core/ThemePalette.h"

AdsrEditorView::AdsrEditorView(QWidget* parent) : QWidget(parent) {
    setMinimumHeight(150);
}

void AdsrEditorView::setAdsr(const ADSR& adsr, const QUuid& sgId) {
    m_adsr = adsr;
    m_sgId = sgId;
    update();
}

QPointF AdsrEditorView::getPoint(DragNode node) const {
    double w = width() - 20;
    double h = height() - 20;
    double maxTime = 4.0; // Assume 4s max for visual
    
    auto timeToX = [&](double t) { return 10 + (t / maxTime) * w; };
    auto levelToY = [&](double l) { return 10 + (1.0 - l) * h; };
    
    switch (node) {
        case Attack: return QPointF(timeToX(m_adsr.attack), levelToY(1.0));
        case Decay: return QPointF(timeToX(m_adsr.attack + m_adsr.decay), levelToY(m_adsr.sustain));
        case Sustain: return QPointF(timeToX(m_adsr.attack + m_adsr.decay + 1.0), levelToY(m_adsr.sustain));
        case Release: return QPointF(timeToX(m_adsr.attack + m_adsr.decay + 1.0 + m_adsr.release), levelToY(0.0));
        default: return QPointF();
    }
}

void AdsrEditorView::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    painter.fillRect(rect(), ThemePalette::color("adsr_bg"));
    
    QPointF start(10, height() - 10);
    QPointF a = getPoint(Attack);
    QPointF d = getPoint(Decay);
    QPointF s = getPoint(Sustain);
    QPointF r = getPoint(Release);
    
    painter.setPen(QPen(ThemePalette::color("adsr_line"), 2));
    painter.drawLine(start, a);
    painter.drawLine(a, d);
    painter.drawLine(d, s);
    painter.drawLine(s, r);
    
    painter.setBrush(ThemePalette::color("adsr_point_border"));
    painter.drawEllipse(a, 4, 4);
    painter.drawEllipse(d, 4, 4);
    painter.drawEllipse(r, 4, 4);
}

void AdsrEditorView::mousePressEvent(QMouseEvent* event) {
    double margin = 10.0;
    if (QLineF(event->pos(), getPoint(Attack)).length() < margin) m_dragNode = Attack;
    else if (QLineF(event->pos(), getPoint(Decay)).length() < margin) m_dragNode = Decay;
    else if (QLineF(event->pos(), getPoint(Release)).length() < margin) m_dragNode = Release;
}

void AdsrEditorView::mouseMoveEvent(QMouseEvent* event) {
    if (m_dragNode == None) return;
    
    double w = width() - 20;
    double h = height() - 20;
    double maxTime = 4.0;
    
    double t = std::clamp((event->pos().x() - 10.0) / w * maxTime, 0.0, maxTime);
    double l = std::clamp(1.0 - (event->pos().y() - 10.0) / h, 0.0, 1.0);
    
    if (m_dragNode == Attack) {
        m_adsr.attack = t;
    } else if (m_dragNode == Decay) {
        m_adsr.decay = std::max(0.0, t - m_adsr.attack);
        m_adsr.sustain = l;
    } else if (m_dragNode == Release) {
        m_adsr.release = std::max(0.0, t - (m_adsr.attack + m_adsr.decay + 1.0));
    }
    update();
}

void AdsrEditorView::mouseReleaseEvent(QMouseEvent* event) {
    Q_UNUSED(event);
    if (m_dragNode != None) {
        emit adsrChanged(m_sgId, m_adsr);
        m_dragNode = None;
    }
}
