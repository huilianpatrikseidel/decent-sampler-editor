#pragma once
#include <QUndoCommand>
#include "core/ProjectManager.h"
#include "core/models/AudioNodes.h"
#include "core/models/UiComponents.h"
#include "core/models/GraphStructures.h"

#include "core/CoreLibExport.h"

class CORELIB_EXPORT AddConnectionCommand : public QUndoCommand {
public:
    AddConnectionCommand(ProjectManager* pm, const Connection& conn);
    void undo() override;
    void redo() override;
private:
    ProjectManager* m_pm;
    Connection m_conn;
};

class CORELIB_EXPORT RemoveConnectionCommand : public QUndoCommand {
public:
    RemoveConnectionCommand(ProjectManager* pm, const Connection& conn);
    void undo() override;
    void redo() override;
private:
    ProjectManager* m_pm;
    Connection m_conn;
};
