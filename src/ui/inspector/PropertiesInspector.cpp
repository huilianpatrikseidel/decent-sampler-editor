#include "PropertiesInspector.h"
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QSlider>
#include <QCheckBox>
#include <QPushButton>
#include "UiPropertyForms.h"
#include "core/models/AudioNodes.h"
#include "core/models/UiComponents.h"
#include "core/models/GraphStructures.h"
#include "strategies/NodeInspectorStrategy.h"
#include "strategies/UiComponentInspectorStrategy.h"
#include "strategies/ZoneInspectorStrategy.h"
#include "strategies/CanvasInspectorStrategy.h"

PropertiesInspector::PropertiesInspector(ProjectManager* pm, QWidget* parent)
    : QWidget(parent), m_pm(pm) {
    m_layout = new QFormLayout(this);
    setLayout(m_layout);
    clearForm();
}

void PropertiesInspector::clearForm() {
    QLayoutItem* item;
    while ((item = m_layout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    m_layout->addRow(new QLabel("No node selected"));
}

void PropertiesInspector::setNode(const QUuid& id) {
    m_currentUiCompId = QUuid();
    m_currentZoneGroupId = QUuid();
    m_currentNodeId = id;
    
    m_isUpdatingUI = true;
    clearForm();
    if (Node* node = m_pm->getNode(id)) {
        NodeInspectorStrategy strategy(node, id);
        strategy.buildForm(m_layout, this);
    }
    m_isUpdatingUI = false;
}

void PropertiesInspector::setUiComponent(const QUuid& id) {
    m_currentNodeId = QUuid();
    m_currentZoneGroupId = QUuid();
    m_currentUiCompId = id;
    
    m_isUpdatingUI = true;
    clearForm();
    if (!id.isNull()) {
        if (UiComponent* comp = m_pm->getUiComponent(id)) {
            UiComponentInspectorStrategy strategy(comp, id);
            strategy.buildForm(m_layout, this);
            m_isUpdatingUI = false;
            return;
        }
    }
    
    CanvasInspectorStrategy strategy(m_pm->getUiState());
    strategy.buildForm(m_layout, this);
    m_isUpdatingUI = false;
}

void PropertiesInspector::setZone(const QUuid& groupId, int zoneIndex) {
    m_currentNodeId = QUuid();
    m_currentUiCompId = QUuid();
    m_currentZoneGroupId = groupId;
    m_currentZoneIndex = zoneIndex;
    
    m_isUpdatingUI = true;
    clearForm();
    Node* node = m_pm->getNode(groupId);
    if (node && node->type == "SampleGroup") {
        SampleGroup* sg = static_cast<SampleGroup*>(node);
        if (zoneIndex >= 0 && zoneIndex < sg->zones.size()) {
            ZoneInspectorStrategy strategy(&sg->zones[zoneIndex], groupId, zoneIndex);
            strategy.buildForm(m_layout, this);
        }
    }
    m_isUpdatingUI = false;
}



void PropertiesInspector::onCanvasSizeChanged(int width, int height) {
    if (m_currentUiCompId.isNull() && m_currentNodeId.isNull() && m_currentZoneGroupId.isNull()) {
        // Find spinboxes and update
        for (int i = 0; i < m_layout->rowCount(); ++i) {
            auto labelItem = m_layout->itemAt(i, QFormLayout::LabelRole);
            auto fieldItem = m_layout->itemAt(i, QFormLayout::FieldRole);
            if (labelItem && fieldItem) {
                QLabel* label = qobject_cast<QLabel*>(labelItem->widget());
                QSpinBox* spin = qobject_cast<QSpinBox*>(fieldItem->widget());
                if (label && spin) {
                    m_isUpdatingUI = true;
                    if (label->text() == "Width") spin->setValue(width);
                    if (label->text() == "Height") spin->setValue(height);
                    m_isUpdatingUI = false;
                }
            }
        }
    }
}

void PropertiesInspector::onNodeModified(const QUuid& id, const QString& propertyName) {
    if (id == m_currentNodeId && !m_isUpdatingUI) {
        setNode(m_currentNodeId); // Rebuild to sync
    } else if (!m_currentUiCompId.isNull() && !m_isUpdatingUI) {
        if (propertyName == "name") {
            setUiComponent(m_currentUiCompId); // Rebuild to sync combobox names
        }
    }
}

void PropertiesInspector::onUiComponentModified(const QUuid& id, const QString& propertyName) {
    if (id == m_currentUiCompId && !m_isUpdatingUI) setUiComponent(m_currentUiCompId);
}


