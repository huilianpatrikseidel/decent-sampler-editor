#include "ProjectTreeWidget.h"
#include <QVBoxLayout>
#include <QTreeWidget>
#include <QFileInfo>
#include "../../core/ApplicationController.h"
#include "../../core/ProjectManager.h"
#include "../../core/models/AudioNodes.h"
#include "../../commands/ModifyPropertyCommand.h"
#include "../inspector/PropertiesInspector.h"
#include "../mapper/MappingEditorContainer.h"

ProjectTreeWidget::ProjectTreeWidget(ApplicationController* controller, PropertiesInspector* inspector, MappingEditorContainer* mappingContainer, QWidget* parent)
    : QWidget(parent), m_controller(controller), m_inspector(inspector), m_mappingContainer(mappingContainer) {
    
    setMinimumWidth(250);
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    
    m_projectTree = new QTreeWidget();
    m_projectTree->setHeaderHidden(true);
    m_projectTree->setIndentation(15);
    m_projectTree->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    layout->addWidget(m_projectTree);
    
    connect(m_projectTree, &QTreeWidget::itemChanged, [this](QTreeWidgetItem* item, int column) {
        if (!m_projectTree->signalsBlocked()) {
            QString type = item->data(0, Qt::UserRole).toString();
            if (type == "Group") {
                QUuid id = QUuid(item->data(0, Qt::UserRole + 1).toString());
                QString text = item->text(0);
                text.remove("📁 "); 
                
                Node* node = m_controller->getProjectManager()->getNode(id);
                if (node && node->name != text) {
                    m_controller->getProjectManager()->getUndoStack()->push(
                        new ModifyPropertyCommand(m_controller->getProjectManager(), id, "name", node->name, text)
                    );
                }
            }
        }
    });
    
    connect(m_projectTree, &QTreeWidget::itemSelectionChanged, [this]() {
        auto items = m_projectTree->selectedItems();
        if (items.isEmpty()) return;
        auto item = items.first();
        
        QString type = item->data(0, Qt::UserRole).toString();
        QUuid id = QUuid(item->data(0, Qt::UserRole + 1).toString());
        
        if (type == "Group") {
            m_mappingContainer->setSampleGroup(id);
            m_inspector->setNode(id);
        } else if (type == "Zone") {
            int zoneIdx = item->data(0, Qt::UserRole + 2).toInt();
            QUuid parentId = QUuid(item->parent()->data(0, Qt::UserRole + 1).toString());
            m_mappingContainer->setSampleGroup(parentId);
            m_inspector->setZone(parentId, zoneIdx);
        } else {
            m_inspector->setNode(QUuid());
        }
    });
    
    connect(m_controller->getProjectManager(), &ProjectManager::projectLoaded, this, &ProjectTreeWidget::updateTree);
    connect(m_controller->getProjectManager(), &ProjectManager::nodeAdded, this, &ProjectTreeWidget::updateTree);
    connect(m_controller->getProjectManager(), &ProjectManager::nodeRemoved, this, &ProjectTreeWidget::updateTree);
    connect(m_controller->getProjectManager(), &ProjectManager::nodeModified, this, [this](const QUuid& id, const QString& prop) {
        if (prop == "name" || prop == "zones" || prop == "isOscillator" || prop.isEmpty()) {
            updateTree();
        }
    });

    updateTree();
}

void ProjectTreeWidget::updateTree() {
    if (!m_projectTree) return;
    
    QString selType;
    QUuid selId;
    int selZone = -1;
    if (!m_projectTree->selectedItems().isEmpty()) {
        auto item = m_projectTree->selectedItems().first();
        selType = item->data(0, Qt::UserRole).toString();
        selId = QUuid(item->data(0, Qt::UserRole + 1).toString());
        if (selType == "Zone") {
            selZone = item->data(0, Qt::UserRole + 2).toInt();
            selId = QUuid(item->parent()->data(0, Qt::UserRole + 1).toString());
        }
    }
    
    m_projectTree->blockSignals(true);
    m_projectTree->clear();
    
    QTreeWidgetItem* rootItem = new QTreeWidgetItem(m_projectTree);
    rootItem->setText(0, "🎹 Instrument");
    rootItem->setData(0, Qt::UserRole, "Instrument");
    
    ProjectManager* pm = m_controller->getProjectManager();
    const auto& nodes = pm->getAllNodes();
    
    QTreeWidgetItem* itemToSelect = nullptr;
    
    for (const auto& pair : nodes) {
        if (pair.second->type == "SampleGroup") {
            SampleGroup* sg = static_cast<SampleGroup*>(pair.second.get());
            QTreeWidgetItem* groupItem = new QTreeWidgetItem(rootItem);
            groupItem->setText(0, "📁 " + (sg->name.isEmpty() ? "Group" : sg->name));
            groupItem->setData(0, Qt::UserRole, "Group");
            groupItem->setData(0, Qt::UserRole + 1, sg->id.toString());
            groupItem->setFlags(groupItem->flags() | Qt::ItemIsEditable);
            
            if (selType == "Group" && selId == sg->id) itemToSelect = groupItem;
            
            for (int i = 0; i < sg->zones.size(); ++i) {
                QTreeWidgetItem* zoneItem = new QTreeWidgetItem(groupItem);
                QString fileName = QFileInfo(sg->zones[i].samplePath).fileName();
                zoneItem->setText(0, "🎵 " + fileName);
                zoneItem->setData(0, Qt::UserRole, "Zone");
                zoneItem->setData(0, Qt::UserRole + 1, sg->id.toString());
                zoneItem->setData(0, Qt::UserRole + 2, i);
                
                if (selType == "Zone" && selId == sg->id && selZone == i) itemToSelect = zoneItem;
            }
        } else {
            Node* n = pair.second.get();
            QTreeWidgetItem* nodeItem = new QTreeWidgetItem(rootItem);
            nodeItem->setText(0, "🔌 " + (n->name.isEmpty() ? n->type : n->name));
            nodeItem->setData(0, Qt::UserRole, "Group");
            nodeItem->setData(0, Qt::UserRole + 1, n->id.toString());
            nodeItem->setFlags(nodeItem->flags() | Qt::ItemIsEditable);
            
            if (selType == "Group" && selId == n->id) itemToSelect = nodeItem;
        }
    }
    
    rootItem->setExpanded(true);
    for (int i = 0; i < rootItem->childCount(); ++i) {
        rootItem->child(i)->setExpanded(true);
    }
    
    if (itemToSelect) {
        itemToSelect->setSelected(true);
    } else if (selType == "Instrument") {
        rootItem->setSelected(true);
    }
    
    m_projectTree->blockSignals(false);
}
