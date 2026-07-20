#pragma once
#include <QGraphicsView>
#include <QGraphicsScene>
#include "core/ProjectManager.h"
#include "ZoneItem.h"
#include "../canvas/CanvasKeyboardItem.h"


#include "ui/mapper/MapperLibExport.h"
class MAPPERLIB_EXPORT MappingEditorView : public QGraphicsView {
    Q_OBJECT
public:
    explicit MappingEditorView(ProjectManager* pm, QWidget* parent = nullptr);
    void setSampleGroup(const QUuid& sgId);
    void notifyGroupAutoCreated(const QUuid& sgId) { emit groupAutoCreated(sgId); }

signals:
    void zoneSelected(const QUuid& sgId, int index);
    void groupAutoCreated(const QUuid& sgId);
    void notePressed(int note, int vel);
    void noteReleased(int note);

protected:
    void drawBackground(QPainter* painter, const QRectF& rect) override;
    void drawForeground(QPainter* painter, const QRectF& rect) override;
    void resizeEvent(QResizeEvent* event) override;
    
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;
    
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private slots:
    void onPitchDetected(const QString& filepath, int midiNote, double frequency);
    void onPitchDetectionFailed(const QString& filepath);
    
private:
    ProjectManager* m_pm;
    QGraphicsScene* m_scene;
    QUuid m_currentSgId;
    class AudioAnalyzer* m_analyzer = nullptr;
    
    double m_noteWidth = 20.0;
    double m_velocityHeight = 4.0;
    QMap<int, Zone> m_dragInitialZones;
    
    CanvasKeyboardItem* m_keyboardItem = nullptr;
    QList<ZoneItem*> m_zoneItems;
    
    void rebuildZones();
};
