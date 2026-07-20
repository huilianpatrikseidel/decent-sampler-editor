#include "SvgToggleWidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QDir>
#include <QCoreApplication>

SvgToggleWidget::SvgToggleWidget(const QString& svgOff, const QString& svgOn, QWidget* parent)
    : QWidget(parent), m_checked(false)
{
    m_rendererOff.load(":/mixer/" + svgOff);
    m_rendererOn.load(":/mixer/" + svgOn);
    
    setFixedSize(28, 28);
    setCursor(Qt::PointingHandCursor);
}

void SvgToggleWidget::setChecked(bool checked) {
    if (m_checked != checked) {
        m_checked = checked;
        update();
    }
}

void SvgToggleWidget::paintEvent(QPaintEvent* /*event*/) {
    QPainter painter(this);
    if (m_checked) {
        m_rendererOn.render(&painter, rect());
    } else {
        m_rendererOff.render(&painter, rect());
    }
}

void SvgToggleWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        setChecked(!m_checked);
        emit toggled(m_checked);
        event->accept();
    }
}
