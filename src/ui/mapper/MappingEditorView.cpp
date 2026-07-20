#include "MappingEditorView.h"
#include <QPainter>
#include <QMenu>
#include <QContextMenuEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include "MappingGridPainter.h"
#include "ZoneImportHandler.h"
#include "../../commands/ModifyPropertyCommand.h"
#include "../../commands/AddNodeCommand.h"
#include "../../commands/ZoneCommands.h"
#include <QMessageBox>
#include <QScrollBar>
#include <QTimer>
#include "../../audio/AudioAnalyzer.h"

MappingEditorView::MappingEditorView(ProjectManager* pm, QWidget* parent) 
    : QGraphicsView(parent), m_pm(pm) {
    m_scene = new QGraphicsScene(this);
    setScene(m_scene);
    
    // 128 notes * 20px, 128 velocities * 4px
    m_scene->setSceneRect(0, 0, 128 * m_noteWidth, 128 * m_velocityHeight);
    
    setAcceptDrops(true);
    setDragMode(QGraphicsView::RubberBandDrag);

    m_analyzer = new AudioAnalyzer(this);
    connect(m_analyzer, &AudioAnalyzer::pitchDetected, this, &MappingEditorView::onPitchDetected);
    connect(m_analyzer, &AudioAnalyzer::pitchDetectionFailed, this, &MappingEditorView::onPitchDetectionFailed);
    
    m_keyboardItem = new CanvasKeyboardItem(m_pm);
    m_keyboardItem->setLinearMode(true, m_noteWidth);
    m_scene->addItem(m_keyboardItem);
    m_keyboardItem->setZValue(100);
    
    connect(m_keyboardItem, &CanvasKeyboardItem::notePressed, this, &MappingEditorView::notePressed);
    connect(m_keyboardItem, &CanvasKeyboardItem::noteReleased, this, &MappingEditorView::noteReleased);
    
    // Disable automatic centering, align to top-left so we can stretch the scene correctly
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    
    connect(pm, &ProjectManager::nodeModified, this, [this](const QUuid& id, const QString&){
        if (id == m_currentSgId) rebuildZones();
    });
}

void MappingEditorView::setSampleGroup(const QUuid& sgId) {
    m_currentSgId = sgId;
    rebuildZones();
}

void MappingEditorView::rebuildZones() {
    for (ZoneItem* item : m_zoneItems) {
        m_scene->removeItem(item);
        delete item;
    }
    m_zoneItems.clear();
    
    if (m_currentSgId.isNull()) return;
    
    Node* node = m_pm->getNode(m_currentSgId);
    if (!node || node->type != "SampleGroup") return;
    
    SampleGroup* sg = static_cast<SampleGroup*>(node);
    for (int i = 0; i < sg->zones.size(); ++i) {
        Zone* z = &sg->zones[i];
        
        ZoneItem* item = new ZoneItem(z, i);
        item->updateFromZone(z, m_noteWidth, m_velocityHeight);
        connect(item, &ZoneItem::zoneSelected, this, [this](int idx){
            emit zoneSelected(m_currentSgId, idx);
        });
        m_scene->addItem(item);
        m_zoneItems.append(item);
    }
}

void MappingEditorView::resizeEvent(QResizeEvent* event) {
    QGraphicsView::resizeEvent(event);
    
    // Keyboard takes fixed 60px height at the bottom
    double availableHeight = viewport()->height() - 60.0;
    if (availableHeight < 128.0) availableHeight = 128.0; // Minimum 1px per velocity
    
    m_velocityHeight = availableHeight / 128.0;
    
    // Update scene rect to match perfectly
    m_scene->setSceneRect(0, 0, 128 * m_noteWidth, viewport()->height());
    
    // Position Keyboard at the exact bottom
    if (m_keyboardItem) {
        m_keyboardItem->setGeometry(QRectF(0, 0, 128 * m_noteWidth, 60));
        m_keyboardItem->setPos(0, availableHeight);
    }
    
    // Update all zone items scaling
    for (ZoneItem* item : m_zoneItems) {
        item->updateFromZone(item->getZone(), m_noteWidth, m_velocityHeight);
    }
}

void MappingEditorView::drawBackground(QPainter* painter, const QRectF& rect) {
    MappingGridPainter::paintBackground(painter, rect, m_noteWidth, m_velocityHeight);
}

void MappingEditorView::drawForeground(QPainter* painter, const QRectF& rect) {
    Q_UNUSED(rect);
    QRectF viewRect = mapToScene(viewport()->rect()).boundingRect();
    MappingGridPainter::paintForeground(painter, viewRect, m_velocityHeight);
}

void MappingEditorView::mousePressEvent(QMouseEvent* event) {
    m_dragInitialZones.clear();
    QGraphicsItem* clickedItem = itemAt(event->pos());
    
    QGraphicsView::mousePressEvent(event);
    
    if (clickedItem && clickedItem->isSelected() && dynamic_cast<ZoneItem*>(clickedItem)) {
        for (auto item : m_scene->selectedItems()) {
            if (ZoneItem* zi = dynamic_cast<ZoneItem*>(item)) {
                m_dragInitialZones[zi->getIndex()] = *zi->getZone();
                
                // Play a quick preview of the root note!
                emit notePressed(zi->getZone()->rootNote, 100);
                QTimer::singleShot(250, this, [this, zi](){
                    emit noteReleased(zi->getZone()->rootNote);
                });
            }
        }
    }
}

void MappingEditorView::mouseReleaseEvent(QMouseEvent* event) {
    QGraphicsView::mouseReleaseEvent(event);
    
    if (!m_dragInitialZones.isEmpty()) {
        QVector<int> indices;
        QVector<Zone> oldZones;
        QVector<Zone> newZones;
        
        for (auto item : m_scene->selectedItems()) {
            if (ZoneItem* zi = dynamic_cast<ZoneItem*>(item)) {
                if (!m_dragInitialZones.contains(zi->getIndex())) continue;
                
                int loN, hiN, loV, hiV;
                zi->getVisualBounds(loN, hiN, loV, hiV);
                
                Zone oldZ = m_dragInitialZones[zi->getIndex()];
                if (oldZ.loNote != loN || oldZ.hiNote != hiN || oldZ.loVel != loV || oldZ.hiVel != hiV) {
                    Zone newZ = oldZ;
                    newZ.loNote = loN;
                    newZ.hiNote = hiN;
                    newZ.loVel = loV;
                    newZ.hiVel = hiV;
                    
                    indices.append(zi->getIndex());
                    oldZones.append(oldZ);
                    newZones.append(newZ);
                }
            }
        }
        
        if (!indices.isEmpty()) {
            m_pm->getUndoStack()->push(new ModifyMultipleZonesCommand(m_pm, m_currentSgId, indices, oldZones, newZones));
        }
        
        m_dragInitialZones.clear();
    }
}

void MappingEditorView::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
        auto items = m_scene->selectedItems();
        if (!items.isEmpty()) {
            QVector<int> indices;
            for (auto item : items) {
                if (ZoneItem* zi = dynamic_cast<ZoneItem*>(item)) {
                    indices.append(zi->getIndex());
                }
            }
            if (!indices.isEmpty()) {
                m_pm->getUndoStack()->push(new RemoveMultipleZonesCommand(m_pm, m_currentSgId, indices));
            }
        }
    }
    QGraphicsView::keyPressEvent(event);
}

void MappingEditorView::wheelEvent(QWheelEvent *event) {
    // Zoom if Ctrl is pressed
    if (event->modifiers() & Qt::ControlModifier) {
        const double scaleFactor = 1.15;
        if (event->angleDelta().y() > 0) {
            scale(scaleFactor, scaleFactor);
        } else {
            scale(1.0 / scaleFactor, 1.0 / scaleFactor);
        }
        event->accept();
    } else {
        // Normal scroll
        QGraphicsView::wheelEvent(event);
    }
}

void MappingEditorView::contextMenuEvent(QContextMenuEvent* event) {
    auto items = m_scene->selectedItems();
    if (items.isEmpty()) return;
    
    QMenu menu(this);
    QAction* pitchAction = nullptr;
    QAction* removeZoneAction = menu.addAction("Remove Zone");
    
    if (items.size() == 1) {
        pitchAction = menu.addAction("Auto-Detect Pitch (Root Note)");
    }
    
    QAction* selected = menu.exec(event->globalPos());
    if (selected && selected == pitchAction) {
        auto item = dynamic_cast<ZoneItem*>(items.first());
        if (item) m_analyzer->detectPitchAsync(item->getZone()->samplePath);
    } else if (selected && selected == removeZoneAction) {
        auto item = dynamic_cast<ZoneItem*>(items.first());
        if (item) {
            m_pm->getUndoStack()->push(new RemoveZoneCommand(m_pm, m_currentSgId, item->getIndex()));
        }
    }
}

void MappingEditorView::onPitchDetected(const QString& filepath, int midiNote, double frequency) {
    if (m_currentSgId.isNull()) return;
    Node* node = m_pm->getNode(m_currentSgId);
    if (!node || node->type != "SampleGroup") return;
    
    SampleGroup* sg = static_cast<SampleGroup*>(node);
    for (int i = 0; i < sg->zones.size(); ++i) {
        if (sg->zones[i].samplePath == filepath) {
            Zone oldZone = sg->zones[i];
            Zone newZone = oldZone;
            newZone.rootNote = midiNote;
            newZone.loNote = qMax(0, midiNote - 2);
            newZone.hiNote = qMin(127, midiNote + 2);
            
            m_pm->getUndoStack()->push(new ModifyZoneCommand(m_pm, m_currentSgId, i, oldZone, newZone));
            rebuildZones();
            break;
        }
    }
}

void MappingEditorView::onPitchDetectionFailed(const QString& filepath) {
    qDebug() << "Failed to detect stable pitch in" << filepath;
}

// Removed old single onZoneModified


void MappingEditorView::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MappingEditorView::dragMoveEvent(QDragMoveEvent *event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MappingEditorView::dropEvent(QDropEvent *event) {
    ZoneImportHandler::handleDrop(this, m_pm, m_analyzer, m_currentSgId, event, m_noteWidth, m_velocityHeight);
}
