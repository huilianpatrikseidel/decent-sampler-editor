#pragma once
#include <QUndoCommand>
#include <QUuid>
#include <QVariant>
#include <QString>
#include "core/ProjectManager.h"

#include "core/CoreLibExport.h"

class CORELIB_EXPORT ModifyPropertyCommand : public QUndoCommand {
public:
    ModifyPropertyCommand(ProjectManager* pm, const QUuid& nodeId, const QString& propertyPath, const QVariant& oldVal, const QVariant& newVal);
    void undo() override;
    void redo() override;
    bool mergeWith(const QUndoCommand *command) override;
    int id() const override { return 2; }

private:
    ProjectManager* m_pm;
    QUuid m_nodeId;
    QString m_propertyPath;
    QVariant m_oldVal;
    QVariant m_newVal;
    
    void applyValue(const QVariant& val);
};

class CORELIB_EXPORT AddLfoCommand : public QUndoCommand {
public:
    AddLfoCommand(ProjectManager* pm, const LFO& lfo);
    void undo() override;
    void redo() override;
    int id() const override { return 3; }
private:
    ProjectManager* m_pm;
    LFO m_lfo;
};

class CORELIB_EXPORT RemoveLfoCommand : public QUndoCommand {
public:
    RemoveLfoCommand(ProjectManager* pm, const LFO& lfo);
    void undo() override;
    void redo() override;
    int id() const override { return 4; }
private:
    ProjectManager* m_pm;
    LFO m_lfo;
};

class CORELIB_EXPORT ModifyLfoCommand : public QUndoCommand {
public:
    ModifyLfoCommand(ProjectManager* pm, const QUuid& lfoId, const LFO& oldLfo, const LFO& newLfo);
    void undo() override;
    void redo() override;
    bool mergeWith(const QUndoCommand *command) override;
    int id() const override { return 5; }
private:
    ProjectManager* m_pm;
    QUuid m_lfoId;
    LFO m_oldLfo;
    LFO m_newLfo;
};
