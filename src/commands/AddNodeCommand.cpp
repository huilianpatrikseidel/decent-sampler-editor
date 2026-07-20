#include "AddNodeCommand.h"

AddNodeCommand::AddNodeCommand(ProjectManager* manager, std::unique_ptr<Node> node, const QPointF& pos, QUndoCommand *parent)
    : QUndoCommand(parent), m_manager(manager), m_node(std::move(node)), m_pos(pos)
{
    m_nodeId = m_node->id;
    setText("Add Node: " + m_node->name);
}

void AddNodeCommand::undo() {
    // Remove do Manager e guarda a posse da memória de volta aqui no comando
    if (m_manager) {
        m_node = m_manager->removeNode(m_nodeId);
    }
}

void AddNodeCommand::redo() {
    // Transfere a posse de memória de volta para o Manager
    if (m_node && m_manager) {
        m_manager->addNode(std::move(m_node));
        m_manager->setNodePosition(m_nodeId, m_pos.x(), m_pos.y());
    }
}
