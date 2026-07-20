#pragma once
#include <QUndoCommand>
#include <QUuid>
#include <QPointF>
#include "core/ProjectManager.h"

#include "core/CoreLibExport.h"

class CORELIB_EXPORT MoveNodeCommand : public QUndoCommand {
public:
    MoveNodeCommand(ProjectManager* pm, const QUuid& nodeId, const QPointF& oldPos, const QPointF& newPos);
    void undo() override;
    void redo() override;
    bool mergeWith(const QUndoCommand *command) override;
    int id() const override { return 1; }

private:
    ProjectManager* m_pm;
    QUuid m_nodeId;
    QPointF m_oldPos;
    QPointF m_newPos;
};
