#include "MoveNodeCommand.h"
#include "core/models/AudioNodes.h"
#include "core/models/UiComponents.h"
#include "core/models/GraphStructures.h"

MoveNodeCommand::MoveNodeCommand(ProjectManager* pm, const QUuid& nodeId, const QPointF& oldPos, const QPointF& newPos)
    : m_pm(pm), m_nodeId(nodeId), m_oldPos(oldPos), m_newPos(newPos) {
    setText(QString("Move Node"));
}

void MoveNodeCommand::undo() {
    m_pm->setNodePosition(m_nodeId, m_oldPos.x(), m_oldPos.y());
    m_pm->setNodeModified(m_nodeId, "position");
}

void MoveNodeCommand::redo() {
    m_pm->setNodePosition(m_nodeId, m_newPos.x(), m_newPos.y());
    m_pm->setNodeModified(m_nodeId, "position");
}

bool MoveNodeCommand::mergeWith(const QUndoCommand *command) {
    if (command->id() != id()) return false;
    const MoveNodeCommand* cmd = static_cast<const MoveNodeCommand*>(command);
    if (cmd->m_nodeId != m_nodeId) return false;
    
    m_newPos = cmd->m_newPos;
    return true;
}
