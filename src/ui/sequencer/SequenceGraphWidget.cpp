#include "SequenceGraphWidget.h"
#include "../../core/ThemePalette.h"
#include <QPainter>
#include <cmath>
#include <algorithm>
#include <QMenu>

static QString midiNoteToName(int note) {
    const char* noteNames[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
    int octave = (note / 12) - 1;
    return QString("%1%2").arg(noteNames[note % 12]).arg(octave);
}

SequenceGraphWidget::SequenceGraphWidget(QWidget* parent) : QWidget(parent) {
    setMouseTracking(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumHeight(150);
}

void SequenceGraphWidget::setSteps(const QVector<SequenceStep>& steps) {
    m_steps = steps;
    update();
}

void SequenceGraphWidget::setActiveStep(int stepIndex) {
    m_activeStep = stepIndex;
    update();
}

QVector<SequenceStep> SequenceGraphWidget::getSteps() const { return m_steps; }

void SequenceGraphWidget::paintEvent(QPaintEvent* event) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.fillRect(rect(), ThemePalette::color("qss_color_22"));
    
    if (m_steps.isEmpty()) return;
    
    float stepWidth = width() / (float)m_steps.size();
    for (int i = 0; i < m_steps.size(); ++i) {
        QRectF stepRect(i * stepWidth, 0, stepWidth - 2, height());
        
        // Draw background track
        if (i == m_activeStep) {
            p.fillRect(stepRect, ThemePalette::color("qss_color_18")); // Highlight active step
        } else {
            p.fillRect(stepRect, ThemePalette::color("qss_color_25"));
        }
        
        // Draw velocity bar
        float velHeight = m_steps[i].velocity * height();
        QRectF barRect(stepRect.x(), height() - velHeight, stepRect.width(), velHeight);
        
        QLinearGradient grad(barRect.topLeft(), barRect.bottomLeft());
        grad.setColorAt(0, ThemePalette::color("qss_color_6", QColor(255, 120, 30)));
        grad.setColorAt(1, ThemePalette::color("qss_color_6", QColor(200, 80, 0)));
        p.fillRect(barRect, grad);
        
        // Draw text: Note and Length
        p.setPen(ThemePalette::color("qss_color_8"));
        QFont f = p.font();
        f.setPointSize(8);
        f.setBold(true);
        p.setFont(f);
        p.drawText(stepRect.adjusted(0, 4, 0, 0), Qt::AlignTop | Qt::AlignHCenter, midiNoteToName(m_steps[i].note));
        
        p.setPen(ThemePalette::color("qss_color_17"));
        f.setBold(false);
        f.setPointSize(7);
        p.setFont(f);
        p.drawText(stepRect.adjusted(0, 18, 0, 0), Qt::AlignTop | Qt::AlignHCenter, QString::number(m_steps[i].length, 'f', 2) + "x");
        
        // Draw step number at bottom
        p.setPen(ThemePalette::color("qss_color_9"));
        p.drawText(stepRect.adjusted(0, 0, 0, -4), Qt::AlignBottom | Qt::AlignHCenter, QString::number(i + 1));
    }
}

void SequenceGraphWidget::mousePressEvent(QMouseEvent* event) {
    updateStepFromMouse(event->pos(), event->modifiers());
}

void SequenceGraphWidget::mouseMoveEvent(QMouseEvent* event) {
    if (event->buttons() & Qt::LeftButton) {
        updateStepFromMouse(event->pos(), event->modifiers());
    }
}

void SequenceGraphWidget::wheelEvent(QWheelEvent* event) {
    if (m_steps.isEmpty()) return;
    
    float stepWidth = width() / (float)m_steps.size();
    int index = event->position().x() / stepWidth;
    if (index >= 0 && index < m_steps.size()) {
        int delta = event->angleDelta().y() > 0 ? 1 : -1;
        m_steps[index].note = std::clamp(m_steps[index].note + delta, 0, 127);
        update();
        emit stepsModified();
    }
}

void SequenceGraphWidget::updateStepFromMouse(const QPoint& pos, Qt::KeyboardModifiers modifiers) {
    if (m_steps.isEmpty()) return;
    
    float stepWidth = width() / (float)m_steps.size();
    int index = pos.x() / stepWidth;
    if (index < 0) index = 0;
    if (index >= m_steps.size()) index = m_steps.size() - 1;
    
    float pct = 1.0f - (float)pos.y() / height();
    pct = std::clamp(pct, 0.0f, 1.0f);
    
    if (modifiers & Qt::ShiftModifier) {
        // Edit Pitch (0-127)
        m_steps[index].note = std::clamp((int)(pct * 127.0f), 0, 127);
    } else if (modifiers & Qt::ControlModifier) {
        // Edit Length (0.1 to 4.0)
        m_steps[index].length = std::clamp(pct * 4.0f, 0.1f, 4.0f);
    } else {
        // Edit Velocity
        m_steps[index].velocity = pct;
    }
    
    update();
    emit stepsModified();
}
