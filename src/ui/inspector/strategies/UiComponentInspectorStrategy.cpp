#include "UiComponentInspectorStrategy.h"
#include "../PropertiesInspector.h"
#include "../UiPropertyForms.h"
#include "../../../core/models/UiComponents.h"
#include "../../../core/models/AudioNodes.h"
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QLabel>
#include <QComboBox>

UiComponentInspectorStrategy::UiComponentInspectorStrategy(const UiComponent* comp, const QUuid& id)
    : m_comp(comp), m_id(id) {}

void UiComponentInspectorStrategy::buildForm(QFormLayout* layout, PropertiesInspector* inspector) {
    if (!m_comp) return;

    QLineEdit* nameEdit = new QLineEdit(m_comp->name.isEmpty() ? m_comp->type : m_comp->name);
    nameEdit->setPlaceholderText(m_comp->type + " Name");
    QObject::connect(nameEdit, &QLineEdit::editingFinished, [inspector, this, nameEdit]() {
        if (!inspector->isUpdatingUI() && nameEdit->text() != m_comp->name) {
            inspector->notifyUiPropertyChanged(m_id, "name", m_comp->name, nameEdit->text());
        }
    });
    layout->addRow("<b>Name</b>", nameEdit);
    
    QDoubleSpinBox* spinX = new QDoubleSpinBox(); spinX->setRange(-10000, 10000); spinX->setValue(m_comp->x);
    QObject::connect(spinX, &QDoubleSpinBox::editingFinished, [inspector, this, spinX]() { inspector->notifyUiPropertyChanged(m_id, "x", m_comp->x, spinX->value()); });
    layout->addRow("X", spinX);
    
    QDoubleSpinBox* spinY = new QDoubleSpinBox(); spinY->setRange(-10000, 10000); spinY->setValue(m_comp->y);
    QObject::connect(spinY, &QDoubleSpinBox::editingFinished, [inspector, this, spinY]() { inspector->notifyUiPropertyChanged(m_id, "y", m_comp->y, spinY->value()); });
    layout->addRow("Y", spinY);
    
    QDoubleSpinBox* spinW = new QDoubleSpinBox(); spinW->setRange(1, 10000); spinW->setValue(m_comp->width);
    QObject::connect(spinW, &QDoubleSpinBox::editingFinished, [inspector, this, spinW]() { inspector->notifyUiPropertyChanged(m_id, "width", m_comp->width, spinW->value()); });
    layout->addRow("Width", spinW);
    
    QDoubleSpinBox* spinH = new QDoubleSpinBox(); spinH->setRange(1, 10000); spinH->setValue(m_comp->height);
    QObject::connect(spinH, &QDoubleSpinBox::editingFinished, [inspector, this, spinH]() { inspector->notifyUiPropertyChanged(m_id, "height", m_comp->height, spinH->value()); });
    layout->addRow("Height", spinH);
    
    QSpinBox* spinZ = new QSpinBox(); spinZ->setRange(-100, 100); spinZ->setValue(m_comp->zIndex);
    QObject::connect(spinZ, &QSpinBox::editingFinished, [inspector, this, spinZ]() { inspector->notifyUiPropertyChanged(m_id, "zIndex", m_comp->zIndex, spinZ->value()); });
    layout->addRow("Z-Index", spinZ);
    
    layout->addRow(new QLabel("<b>Specific Properties</b>"));
    
    UiPropertyForms::buildForm(m_comp, inspector, layout);
    
    buildDataBindingForm(m_comp, layout, inspector);
}

void UiComponentInspectorStrategy::buildDataBindingForm(const UiComponent* comp, QFormLayout* layout, PropertiesInspector* inspector) {
    layout->addRow(new QLabel("<b>Data Binding</b>"));
    QComboBox* nodeCombo = new QComboBox();
    nodeCombo->addItem("None", QUuid());
    for (const auto& pair : inspector->getProjectManager()->getAllNodes()) {
        const auto& node = pair.second;
        nodeCombo->addItem(node->name.isEmpty() ? node->type : node->name, node->id);
    }
    int idx = nodeCombo->findData(comp->targetNodeId);
    nodeCombo->setCurrentIndex(idx >= 0 ? idx : 0);
    
    QObject::connect(nodeCombo, &QComboBox::currentIndexChanged, [inspector, this, comp, nodeCombo]() {
        inspector->notifyUiPropertyChanged(m_id, "targetNodeId", comp->targetNodeId, nodeCombo->currentData().toUuid());
    });
    layout->addRow("Target Node", nodeCombo);
    
    QLineEdit* propEdit = new QLineEdit(comp->targetProperty);
    QObject::connect(propEdit, &QLineEdit::editingFinished, [inspector, this, comp, propEdit]() {
        inspector->notifyUiPropertyChanged(m_id, "targetProperty", comp->targetProperty, propEdit->text());
    });
    layout->addRow("Target Prop", propEdit);
}
