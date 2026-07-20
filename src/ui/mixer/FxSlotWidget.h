#pragma once
#include <QWidget>
#include <QUuid>
#include <QString>


#include "ui/mixer/MixerLibExport.h"
class QLabel;
class QPushButton;

class MIXERLIB_EXPORT FxSlotWidget : public QWidget {
    Q_OBJECT
public:
    explicit FxSlotWidget(const QUuid& fxId, const QUuid& channelId, const QString& typeName, bool bypassed, QWidget* parent = nullptr);

signals:
    void requestEdit(QUuid fxId);
    void requestBypassToggle(QUuid fxId, bool bypassed);
    
    // For drag and drop reordering
    void requestReorder(QUuid sourceFxId, QUuid sourceChannelId, QUuid targetFxId);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void dragLeaveEvent(QDragLeaveEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    QUuid m_fxId;
    QUuid m_channelId;
    QString m_typeName;
    bool m_bypassed;
    
    QLabel* m_nameLabel;
    QPushButton* m_bypassBtn;
    
    QPoint m_dragStartPos;
    bool m_isDragTarget = false;
};
