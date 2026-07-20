#include "NodeInspectorStrategy.h"
#include "../PropertiesInspector.h"
#include "../NodePropertyForms.h"
#include "../../../core/models/AudioNodes.h"
#include <QLineEdit>

NodeInspectorStrategy::NodeInspectorStrategy(const Node* node, const QUuid& id)
    : m_node(node), m_id(id) {}

void NodeInspectorStrategy::buildForm(QFormLayout* layout, PropertiesInspector* inspector) {
    if (!m_node) return;
    
    QLineEdit* nameEdit = new QLineEdit(m_node->name.isEmpty() ? m_node->type : m_node->name);
    nameEdit->setPlaceholderText(m_node->type + " Name");
    
    QObject::connect(nameEdit, &QLineEdit::editingFinished, [inspector, this, nameEdit]() {
        if (!inspector->isUpdatingUI() && nameEdit->text() != m_node->name) {
            inspector->notifyPropertyChanged(m_id, "name", m_node->name, nameEdit->text());
        }
    });
    layout->addRow("<b>Name</b>", nameEdit);
    
    NodePropertyForms::buildForm(m_node, m_id, inspector, layout);
}
