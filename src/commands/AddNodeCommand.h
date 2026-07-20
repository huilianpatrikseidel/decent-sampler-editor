#pragma once
#include <QUndoCommand>
#include <QString>
#include <memory>
#include "core/ProjectManager.h"
#include "core/models/AudioNodes.h"
#include "core/models/UiComponents.h"
#include "core/models/GraphStructures.h"

#include "core/CoreLibExport.h"

// Exemplo concreto de QUndoCommand (Pattern Command)
class CORELIB_EXPORT AddNodeCommand : public QUndoCommand {
public:
    AddNodeCommand(ProjectManager* manager, std::unique_ptr<Node> node, const QPointF& pos, QUndoCommand *parent = nullptr);
    
    void undo() override;
    void redo() override;

private:
    ProjectManager* m_manager;
    std::unique_ptr<Node> m_node; // Holds ownership when node is NOT in the manager
    QUuid m_nodeId;
    QPointF m_pos;
};
