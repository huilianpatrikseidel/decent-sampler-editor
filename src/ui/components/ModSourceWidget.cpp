#include "ModSourceWidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QApplication>
#include <QPainterPath>

ModSourceWidget::ModSourceWidget(const QString& sourceId, const QString& displayName, const QColor& color, QWidget* parent)
    : QWidget(parent), m_sourceId(sourceId), m_displayName(displayName), m_color(color)
{
    setFixedSize(40, 40);
    setCursor(Qt::OpenHandCursor);
    setToolTip("Drag to assign " + displayName + " modulation");
}

void ModSourceWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRectF rect = this->rect().adjusted(4, 4, -4, -4);
    
    // Draw flat background
    painter.setBrush(QColor(25, 25, 28));
    
    // Draw glowing border
    QPen pen(m_color, 2);
    painter.setPen(pen);
    painter.drawEllipse(rect);
    
    // Draw text
    painter.setPen(Qt::white);
    QFont f = painter.font();
    f.setBold(true);
    f.setPointSize(10);
    painter.setFont(f);
    painter.drawText(this->rect(), Qt::AlignCenter, m_displayName);
}

void ModSourceWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_dragStartPosition = event->pos();
        setCursor(Qt::ClosedHandCursor);
    }
}

void ModSourceWidget::mouseMoveEvent(QMouseEvent* event) {
    if (!(event->buttons() & Qt::LeftButton))
        return;
    if ((event->pos() - m_dragStartPosition).manhattanLength() < QApplication::startDragDistance())
        return;

    QDrag* drag = new QDrag(this);
    QMimeData* mimeData = new QMimeData;

    mimeData->setData("application/x-modsource", m_sourceId.toUtf8());
    drag->setMimeData(mimeData);

    // Create a drag image
    QPixmap pixmap(size());
    pixmap.fill(Qt::transparent);
    render(&pixmap);
    drag->setPixmap(pixmap);
    drag->setHotSpot(event->pos());

    drag->exec(Qt::CopyAction | Qt::MoveAction);
    setCursor(Qt::OpenHandCursor);
}
