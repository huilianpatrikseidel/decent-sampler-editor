#pragma once
#include <QUndoCommand>
#include <QUuid>
#include "core/ProjectManager.h"
#include "core/models/AudioNodes.h"
#include "core/models/UiComponents.h"
#include "core/models/GraphStructures.h"

#include "core/CoreLibExport.h"

class CORELIB_EXPORT AddZoneCommand : public QUndoCommand {
public:
    AddZoneCommand(ProjectManager* pm, const QUuid& sgId, const Zone& zone);
    void undo() override;
    void redo() override;
private:
    ProjectManager* m_pm;
    QUuid m_sgId;
    Zone m_zone;
};

class CORELIB_EXPORT AddMultipleZonesCommand : public QUndoCommand {
public:
    AddMultipleZonesCommand(ProjectManager* pm, const QUuid& sgId, const QVector<Zone>& zones);
    void undo() override;
    void redo() override;
private:
    ProjectManager* m_pm;
    QUuid m_sgId;
    QVector<Zone> m_zones;
    int m_startIndex; // Used to track where they were inserted if we need to undo
};

class CORELIB_EXPORT RemoveZoneCommand : public QUndoCommand {
public:
    RemoveZoneCommand(ProjectManager* pm, const QUuid& sgId, int zoneIndex);
    void undo() override;
    void redo() override;
private:
    ProjectManager* m_pm;
    QUuid m_sgId;
    int m_zoneIndex;
    Zone m_savedZone;
};

class CORELIB_EXPORT ModifyZoneCommand : public QUndoCommand {
public:
    ModifyZoneCommand(ProjectManager* pm, const QUuid& sgId, int zoneIndex, const Zone& oldZone, const Zone& newZone);
    void undo() override;
    void redo() override;
    bool mergeWith(const QUndoCommand *command) override;
    int id() const override { return 12; }
private:
    ProjectManager* m_pm;
    QUuid m_sgId;
    int m_zoneIndex;
    Zone m_oldZone;
    Zone m_newZone;
};

class CORELIB_EXPORT ModifyMultipleZonesCommand : public QUndoCommand {
public:
    ModifyMultipleZonesCommand(ProjectManager* pm, const QUuid& sgId, const QVector<int>& indices, const QVector<Zone>& oldZones, const QVector<Zone>& newZones);
    void undo() override;
    void redo() override;
private:
    ProjectManager* m_pm;
    QUuid m_sgId;
    QVector<int> m_indices;
    QVector<Zone> m_oldZones;
    QVector<Zone> m_newZones;
};

class CORELIB_EXPORT RemoveMultipleZonesCommand : public QUndoCommand {
public:
    RemoveMultipleZonesCommand(ProjectManager* pm, const QUuid& sgId, const QVector<int>& indices);
    void undo() override;
    void redo() override;
private:
    ProjectManager* m_pm;
    QUuid m_sgId;
    QVector<int> m_indices; // Indices sorted in descending order
    QVector<Zone> m_savedZones; // Corresponds to indices
};
