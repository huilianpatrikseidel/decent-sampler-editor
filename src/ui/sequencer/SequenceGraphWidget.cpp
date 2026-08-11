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

    const int H = height();
    float stepWidth = width() / (float)m_steps.size();
    QColor barBase = ThemePalette::color("accent_brand", QColor(255, 102, 0));

    for (int i = 0; i < m_steps.size(); ++i) {
        QRectF stepRect(i * stepWidth, 0, stepWidth - 2, H);

        // Background track: alternate shade every 4 steps (beat grouping); brighten the playing step.
        QColor trackCol = ((i / 4) % 2 == 0) ? ThemePalette::color("qss_color_25", QColor(30, 30, 30))
                                             : ThemePalette::color("qss_color_18", QColor(45, 45, 45));
        if (i == m_activeStep) trackCol = ThemePalette::color("qss_color_11", QColor(70, 70, 70));
        p.fillRect(stepRect, trackCol);

        // Velocity bar: height AND opacity both encode velocity so it reads at a glance.
        float vel = m_steps[i].velocity;
        float velHeight = vel * H;
        QRectF barRect(stepRect.x(), H - velHeight, stepRect.width(), velHeight);

        QColor barCol = barBase;
        barCol.setAlpha(i == m_activeStep ? 255 : 90 + int(vel * 165.0f));
        QLinearGradient grad(barRect.topLeft(), barRect.bottomLeft());
        grad.setColorAt(0, barCol.lighter(120));
        grad.setColorAt(1, barCol);
        p.fillRect(barRect, grad);

        // Note name (pitch)
        p.setPen(ThemePalette::color("qss_color_8"));
        QFont f = p.font();
        f.setPointSize(8);
        f.setBold(true);
        p.setFont(f);
        p.drawText(stepRect.adjusted(0, 4, 0, 0), Qt::AlignTop | Qt::AlignHCenter, midiNoteToName(m_steps[i].note));

        // Length label only when it differs from the default (1.00x) — cuts clutter.
        if (std::abs(m_steps[i].length - 1.0f) > 0.001f) {
            p.setPen(ThemePalette::color("qss_color_17"));
            f.setBold(false);
            f.setPointSize(7);
            p.setFont(f);
            p.drawText(stepRect.adjusted(0, 18, 0, 0), Qt::AlignTop | Qt::AlignHCenter, QString::number(m_steps[i].length, 'f', 2) + "x");
        }

        // Step number
        p.setPen(ThemePalette::color("qss_color_9"));
        f.setBold(false);
        f.setPointSize(7);
        p.setFont(f);
        p.drawText(stepRect.adjusted(0, 0, 0, -4), Qt::AlignBottom | Qt::AlignHCenter, QString::number(i + 1));
    }

    // Beat divider lines every 4 steps for positional readability.
    p.setPen(QPen(ThemePalette::color("qss_color_14", QColor(62, 62, 66)), 1));
    for (int b = 4; b < m_steps.size(); b += 4) {
        float x = b * stepWidth;
        p.drawLine(QPointF(x, 0), QPointF(x, H));
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
