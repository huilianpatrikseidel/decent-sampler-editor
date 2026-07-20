#include "FxSlotWidget.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QApplication>
#include <QPainter>
#include "../../core/ThemePalette.h"

FxSlotWidget::FxSlotWidget(const QUuid& fxId, const QUuid& channelId, const QString& typeName, bool bypassed, QWidget* parent)
    : QWidget(parent), m_fxId(fxId), m_channelId(channelId), m_typeName(typeName), m_bypassed(bypassed)
{
    setFixedHeight(28);
    setAcceptDrops(true);
    
    m_bypassBtn = new QPushButton();
    m_bypassBtn->setFixedSize(14, 14);
    m_bypassBtn->setCheckable(true);
    m_bypassBtn->setChecked(!bypassed); // Checked means active
    m_bypassBtn->setStyleSheet(QString(
        "QPushButton { background-color: %1; border: 1px solid %2; border-radius: 7px; }"
        "QPushButton:checked { background-color: %3; border: 1px solid %4; }"
    ).arg(ThemePalette::colorHex("qss_color_25"))
     .arg(ThemePalette::colorHex("qss_color_9"))
     .arg(ThemePalette::colorHex("qss_color_6"))
     .arg(ThemePalette::colorHex("qss_color_3")));
    
    connect(m_bypassBtn, &QPushButton::clicked, this, [this](bool checked) {
        emit requestBypassToggle(m_fxId, !checked);
    });
    
    m_nameLabel = new QLabel(typeName);
    m_nameLabel->setStyleSheet(QString("color: %1; font-size: 11px; font-weight: bold;")
        .arg(bypassed ? ThemePalette::colorHex("qss_color_17") : ThemePalette::colorHex("qss_color_8")));
    m_nameLabel->setAlignment(Qt::AlignCenter);
    
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(6, 0, 6, 0);
    layout->addWidget(m_bypassBtn);
    layout->addWidget(m_nameLabel, 1);
}

void FxSlotWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_dragStartPos = event->pos();
    }
    QWidget::mousePressEvent(event);
}

void FxSlotWidget::mouseDoubleClickEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit requestEdit(m_fxId);
    }
    QWidget::mouseDoubleClickEvent(event);
}

void FxSlotWidget::mouseMoveEvent(QMouseEvent* event) {
    if (!(event->buttons() & Qt::LeftButton)) return;
    if ((event->pos() - m_dragStartPos).manhattanLength() < QApplication::startDragDistance()) return;
    
    QDrag* drag = new QDrag(this);
    QMimeData* mimeData = new QMimeData;
    
    // Encode both FxId and ChannelId
    QByteArray encodedData;
    encodedData.append(m_fxId.toString().toUtf8());
    encodedData.append("|");
    encodedData.append(m_channelId.toString().toUtf8());
    
    mimeData->setData("application/x-sampler-fx", encodedData);
    drag->setMimeData(mimeData);
    
    // Draw pixmap for drag
    QPixmap pixmap(size());
    render(&pixmap);
    drag->setPixmap(pixmap);
    drag->setHotSpot(event->pos());
    
    drag->exec(Qt::MoveAction);
}

void FxSlotWidget::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasFormat("application/x-sampler-fx")) {
        event->acceptProposedAction();
        m_isDragTarget = true;
        update();
    }
}

void FxSlotWidget::dragMoveEvent(QDragMoveEvent* event) {
    if (event->mimeData()->hasFormat("application/x-sampler-fx")) {
        event->acceptProposedAction();
    }
}

void FxSlotWidget::dragLeaveEvent(QDragLeaveEvent* event) {
    m_isDragTarget = false;
    update();
}

void FxSlotWidget::dropEvent(QDropEvent* event) {
    m_isDragTarget = false;
    update();
    
    if (event->mimeData()->hasFormat("application/x-sampler-fx")) {
        QByteArray encodedData = event->mimeData()->data("application/x-sampler-fx");
        QList<QByteArray> parts = encodedData.split('|');
        if (parts.size() == 2) {
            QUuid sourceFxId(QString::fromUtf8(parts[0]));
            QUuid sourceChannelId(QString::fromUtf8(parts[1]));
            
            if (sourceFxId != m_fxId) {
                emit requestReorder(sourceFxId, sourceChannelId, m_fxId);
            }
        }
        event->acceptProposedAction();
    }
}

void FxSlotWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    QColor bgColor = m_bypassed ? ThemePalette::color("qss_color_22") : ThemePalette::color("qss_color_20");
    if (underMouse()) bgColor = bgColor.lighter(120);
    
    painter.setPen(Qt::NoPen);
    painter.setBrush(bgColor);
    painter.drawRoundedRect(rect().adjusted(1, 1, -1, -1), 3, 3);
    
    painter.setPen(QPen(ThemePalette::color("qss_color_9"), 1));
    painter.drawRoundedRect(rect().adjusted(1, 1, -1, -1), 3, 3);
    
    // Draw drag highlight
    if (m_isDragTarget) {
        painter.setPen(QPen(ThemePalette::color("qss_color_6"), 2));
        painter.setBrush(Qt::NoBrush);
        painter.drawRoundedRect(rect().adjusted(1, 1, -1, -1), 3, 3);
    }
}
