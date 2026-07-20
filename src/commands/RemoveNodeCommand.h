#pragma once
#include <QUndoCommand>
#include <QUuid>
#include <memory>
#include <vector>
#include "core/ProjectManager.h"
#include "core/models/AudioNodes.h"
#include "core/models/UiComponents.h"
#include "core/models/GraphStructures.h"

#include "core/CoreLibExport.h"

class CORELIB_EXPORT RemoveNodeCommand : public QUndoCommand {
public:
    RemoveNodeCommand(ProjectManager* pm, const QUuid& nodeId);
    void undo() override;
    void redo() override;
private:
    ProjectManager* m_pm;
    QUuid m_nodeId;
    std::unique_ptr<Node> m_savedNode;
    std::vector<Connection> m_orphanedConnections;
};
