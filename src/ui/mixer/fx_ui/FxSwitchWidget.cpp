#include "FxSwitchWidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QSvgRenderer>
#include <QFont>
#include "../../core/ThemePalette.h"

FxSwitchWidget::FxSwitchWidget(const QString& prefix, const QString& label, bool defaultValue, QWidget* parent)
    : QWidget(parent), m_prefix(prefix), m_label(label), m_checked(defaultValue)
{
    setFixedSize(60, 50);
    setCursor(Qt::PointingHandCursor);
    
    m_onRenderer = new QSvgRenderer(QString(":/fx/%1_switch_on.svg").arg(m_prefix), this);
    m_offRenderer = new QSvgRenderer(QString(":/fx/%1_switch_off.svg").arg(m_prefix), this);
}

void FxSwitchWidget::setChecked(bool checked) {
    if (m_checked != checked) {
        m_checked = checked;
        update();
        emit toggled(m_checked);
    }
}

void FxSwitchWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        setChecked(!m_checked);
        event->accept();
    }
}

void FxSwitchWidget::paintEvent(QPaintEvent* event) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::TextAntialiasing);
    
    QRect switchRect(20, 15, 20, 15);
    
    if (m_checked && m_onRenderer->isValid()) {
        m_onRenderer->render(&p, switchRect);
    } else if (!m_checked && m_offRenderer->isValid()) {
        m_offRenderer->render(&p, switchRect);
    }
    
    // Draw Label at top
    p.setFont(QFont("Inter", 7, QFont::Bold));
    p.setPen(ThemePalette::color("qss_color_8"));
    p.drawText(QRect(0, 0, 60, 15), Qt::AlignCenter, m_label.toUpper());
}
