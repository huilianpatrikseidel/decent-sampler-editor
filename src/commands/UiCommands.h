#pragma once
#include <QUndoCommand>
#include <QUuid>
#include <QPointF>
#include <QVariant>
#include "core/ProjectManager.h"
#include "core/models/AudioNodes.h"
#include "core/models/UiComponents.h"
#include "core/models/GraphStructures.h"

#include "core/CoreLibExport.h"

class CORELIB_EXPORT AddUiComponentCommand : public QUndoCommand {
public:
    AddUiComponentCommand(ProjectManager* pm, std::unique_ptr<UiComponent> comp);
    void undo() override;
    void redo() override;
private:
    ProjectManager* m_pm;
    std::unique_ptr<UiComponent> m_comp;
    QUuid m_id;
};

class CORELIB_EXPORT RemoveUiComponentCommand : public QUndoCommand {
public:
    RemoveUiComponentCommand(ProjectManager* pm, const QUuid& id);
    void undo() override;
    void redo() override;
private:
    ProjectManager* m_pm;
    QUuid m_id;
    std::unique_ptr<UiComponent> m_savedComp;
};

class CORELIB_EXPORT MoveUiComponentCommand : public QUndoCommand {
public:
    MoveUiComponentCommand(ProjectManager* pm, const QUuid& id, const QPointF& oldPos, const QPointF& newPos);
    void undo() override;
    void redo() override;
    bool mergeWith(const QUndoCommand *command) override;
    int id() const override { return 10; }
private:
    ProjectManager* m_pm;
    QUuid m_id;
    QPointF m_oldPos;
    QPointF m_newPos;
};

class CORELIB_EXPORT ModifyUiPropertyCommand : public QUndoCommand {
public:
    ModifyUiPropertyCommand(ProjectManager* pm, const QUuid& id, const QString& propertyPath, const QVariant& oldVal, const QVariant& newVal);
    void undo() override;
    void redo() override;
    bool mergeWith(const QUndoCommand *command) override;
    int id() const override { return 11; }
private:
    ProjectManager* m_pm;
    QUuid m_id;
    QString m_propertyPath;
    QVariant m_oldVal;
    QVariant m_newVal;
    void applyValue(const QVariant& val);
};

class CORELIB_EXPORT ModifyCanvasSizeCommand : public QUndoCommand {
public:
    ModifyCanvasSizeCommand(ProjectManager* pm, int oldW, int oldH, int newW, int newH);
    void undo() override;
    void redo() override;
    bool mergeWith(const QUndoCommand *command) override;
    int id() const override { return 12; }
private:
    ProjectManager* m_pm;
    int m_oldW, m_oldH;
    int m_newW, m_newH;
};

class CORELIB_EXPORT ModifyCanvasBgCommand : public QUndoCommand {
public:
    ModifyCanvasBgCommand(ProjectManager* pm, const QString& oldImg, const QString& newImg, BgMode oldMode, BgMode newMode);
    void undo() override;
    void redo() override;
    bool mergeWith(const QUndoCommand *command) override;
    int id() const override { return 13; }
private:
    ProjectManager* m_pm;
    QString m_oldImg;
    QString m_newImg;
    BgMode m_oldMode;
    BgMode m_newMode;
};
