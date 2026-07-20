#include "RemoveNodeCommand.h"

RemoveNodeCommand::RemoveNodeCommand(ProjectManager* pm, const QUuid& nodeId)
    : m_pm(pm), m_nodeId(nodeId) { setText("Remove Node"); }

void RemoveNodeCommand::undo() {
    if (m_savedNode) {
        m_pm->addNode(std::move(m_savedNode));
        // Restaura em cascata os cabos conectados (Orphaned Connections)
        for (const auto& conn : m_orphanedConnections) {
            m_pm->addConnection(conn);
        }
        m_orphanedConnections.clear();
    }
}

void RemoveNodeCommand::redo() {
    // Guarda as conexões para restauração e remove do grafo
    m_orphanedConnections = m_pm->getConnectionsForNode(m_nodeId);
    for (const auto& conn : m_orphanedConnections) {
        m_pm->removeConnection(conn);
    }
    // Remove o Node
    m_savedNode = m_pm->removeNode(m_nodeId);
}
