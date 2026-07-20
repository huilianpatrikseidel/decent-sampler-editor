#include "EnvelopeGraphWidget.h"
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <QBrush>
#include <QPen>
#include <QMouseEvent>
#include "../../core/ThemePalette.h"

EnvelopeGraphWidget::EnvelopeGraphWidget(QWidget* parent) : QWidget(parent) {
    setObjectName("NeumorphicInner");
    setAttribute(Qt::WA_StyledBackground, true);
    setFixedHeight(100);
    setMinimumWidth(200);
    setMouseTracking(true);
}

void EnvelopeGraphWidget::setParameters(double attack, double decay, double sustain, double release) {
    if (m_dragState == None) {
        m_attack = qBound(0.0, attack / 10.0, 1.0);
        m_decay = qBound(0.0, decay / 10.0, 1.0);
        m_sustain = qBound(0.0, sustain, 1.0);
        m_release = qBound(0.0, release / 10.0, 1.0);
        update();
    }
}

QPointF EnvelopeGraphWidget::getAttackNode() const {
    QRect rect = this->rect().adjusted(0, 5, 0, 0);
    double maxA_x = rect.width() * 0.3;
    return QPointF(rect.left() + (m_attack * maxA_x), rect.top());
}

QPointF EnvelopeGraphWidget::getDecayNode() const {
    QRect rect = this->rect().adjusted(0, 5, 0, 0);
    double ax = getAttackNode().x();
    double maxD_x = rect.width() * 0.3;
    double sy = rect.bottom() - (m_sustain * rect.height());
    return QPointF(ax + (m_decay * maxD_x), sy);
}

QPointF EnvelopeGraphWidget::getReleaseNode() const {
    QRect rect = this->rect().adjusted(0, 5, 0, 0);
    double dx = getDecayNode().x();
    double constS_x = rect.width() * 0.2;
    double sx = dx + constS_x;
    return QPointF(sx, rect.bottom() - (m_sustain * rect.height()));
}

void EnvelopeGraphWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect rect = this->rect().adjusted(0, 5, 0, 0);

    QPainterPath clipPath;
    clipPath.addRoundedRect(this->rect(), 8, 8);
    painter.setClipPath(clipPath);
    
    QPointF pA = getAttackNode();
    QPointF pD = getDecayNode();
    QPointF pS = getReleaseNode(); // start of release
    
    double maxR_x = rect.width() * 0.2;
    double rx = pS.x() + (m_release * maxR_x);
    if (rx > rect.right()) rx = rect.right();
    
    double bottomY = rect.bottom();
    double topY = rect.top();
    
    QPainterPath path;
    path.moveTo(rect.left(), bottomY);
    
    // Attack curve (easing out)
    path.cubicTo(rect.left() + (pA.x() - rect.left())/2.0, bottomY, pA.x(), pA.y(), pA.x(), pA.y());
    
    // Decay curve (easing in)
    path.cubicTo(pA.x() + (pD.x() - pA.x())/2.0, pD.y(), pD.x(), pD.y(), pD.x(), pD.y());
    
    // Sustain line
    path.lineTo(pS.x(), pS.y());
    
    // Release curve
    path.cubicTo(pS.x() + (rx - pS.x())/2.0, bottomY, rx, bottomY, rx, bottomY);
    
    // Fill path
    QPainterPath fillPath = path;
    fillPath.lineTo(rx, bottomY);
    fillPath.lineTo(rect.left(), bottomY);
    fillPath.closeSubpath();
    
    QLinearGradient gradient(0, topY, 0, bottomY);
    gradient.setColorAt(0.0, isEnabled() ? ThemePalette::color("env_graph_bg", QColor(40, 200, 100, 160)) : ThemePalette::color("env_graph_bg", QColor(40, 200, 100, 40)));
    gradient.setColorAt(1.0, ThemePalette::color("env_graph_bg", QColor(40, 200, 100, 0)));
    
    painter.fillPath(fillPath, gradient);
    

    // Draw outline
    QPen outlinePen(isEnabled() ? ThemePalette::color("env_graph_line", QColor(80, 220, 120)) : ThemePalette::color("env_graph_line", QColor(80, 220, 120, 80)), 2);
    painter.setPen(outlinePen);
    painter.drawPath(path);
    
    // Draw points
    if (isEnabled()) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(m_dragState == DraggingAttack ? ThemePalette::color("qss_color_8", Qt::white) : ThemePalette::color("env_graph_point", QColor(80, 220, 120)));
        painter.drawEllipse(pA, 5, 5);
        painter.setBrush(m_dragState == DraggingDecay || m_dragState == DraggingSustain ? ThemePalette::color("qss_color_8", Qt::white) : ThemePalette::color("env_graph_point", QColor(80, 220, 120)));
        painter.drawEllipse(pD, 5, 5);
        painter.setBrush(m_dragState == DraggingRelease || m_dragState == DraggingSustain ? ThemePalette::color("qss_color_8", Qt::white) : ThemePalette::color("env_graph_point", QColor(80, 220, 120)));
        painter.drawEllipse(pS, 5, 5);
    } else {
        painter.setPen(Qt::NoPen);
        painter.setBrush(ThemePalette::color("env_graph_point", QColor(80, 220, 120, 60)));
        painter.drawEllipse(pA, 5, 5);
        painter.drawEllipse(pD, 5, 5);
        painter.drawEllipse(pS, 5, 5);
    }
}

void EnvelopeGraphWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        QPointF pos = event->pos();
        if (QLineF(pos, getAttackNode()).length() < 10) m_dragState = DraggingAttack;
        else if (QLineF(pos, getDecayNode()).length() < 10) m_dragState = DraggingDecay;
        else if (QLineF(pos, getReleaseNode()).length() < 10) m_dragState = DraggingRelease;
        else m_dragState = None;
    }
}

void EnvelopeGraphWidget::mouseMoveEvent(QMouseEvent* event) {
    if (m_dragState != None) {
        QRect rect = this->rect().adjusted(5, 5, -5, -5);
        QPointF pos = event->pos();
        
        if (m_dragState == DraggingAttack) {
            double maxA_x = rect.width() * 0.3;
            m_attack = qBound(0.0, (pos.x() - rect.left()) / maxA_x, 1.0);
        } else if (m_dragState == DraggingDecay || m_dragState == DraggingSustain) {
            double maxD_x = rect.width() * 0.3;
            double ax = getAttackNode().x();
            m_decay = qBound(0.0, (pos.x() - ax) / maxD_x, 1.0);
            m_sustain = qBound(0.0, 1.0 - (pos.y() - rect.top()) / rect.height(), 1.0);
        } else if (m_dragState == DraggingRelease) {
            double maxR_x = rect.width() * 0.2;
            double sx = getReleaseNode().x();
            m_release = qBound(0.0, (pos.x() - sx) / maxR_x, 1.0);
        }
        
        update();
        emit parametersChanged(m_attack * 10.0, m_decay * 10.0, m_sustain, m_release * 10.0);
    }
}

void EnvelopeGraphWidget::mouseReleaseEvent(QMouseEvent* event) {
    m_dragState = None;
    update();
}
