#include "ZoneCommands.h"

AddZoneCommand::AddZoneCommand(ProjectManager* pm, const QUuid& sgId, const Zone& zone) 
    : m_pm(pm), m_sgId(sgId), m_zone(zone) { setText("Add Zone"); }
void AddZoneCommand::undo() {
    if (auto sg = static_cast<SampleGroup*>(m_pm->getNode(m_sgId))) {
        sg->zones.pop_back();
        m_pm->setNodeModified(m_sgId, "zones");
    }
}
void AddZoneCommand::redo() {
    if (auto sg = static_cast<SampleGroup*>(m_pm->getNode(m_sgId))) {
        sg->zones.push_back(m_zone);
        m_pm->setNodeModified(m_sgId, "zones");
    }
}

AddMultipleZonesCommand::AddMultipleZonesCommand(ProjectManager* pm, const QUuid& sgId, const QVector<Zone>& zones)
    : m_pm(pm), m_sgId(sgId), m_zones(zones), m_startIndex(-1) {
    setText(QObject::tr("Auto-Map %1 Zones").arg(zones.size()));
}

void AddMultipleZonesCommand::redo() {
    Node* n = m_pm->getNode(m_sgId);
    if (!n || n->type != "SampleGroup") return;
    SampleGroup* sg = static_cast<SampleGroup*>(n);
    m_startIndex = sg->zones.size();
    sg->zones.append(m_zones);
    m_pm->setNodeModified(m_sgId, "zones");
}

void AddMultipleZonesCommand::undo() {
    if (m_startIndex < 0) return;
    Node* n = m_pm->getNode(m_sgId);
    if (!n || n->type != "SampleGroup") return;
    SampleGroup* sg = static_cast<SampleGroup*>(n);
    sg->zones.remove(m_startIndex, m_zones.size());
    m_pm->setNodeModified(m_sgId, "zones");
}

RemoveZoneCommand::RemoveZoneCommand(ProjectManager* pm, const QUuid& sgId, int zoneIndex) 
    : m_pm(pm), m_sgId(sgId), m_zoneIndex(zoneIndex) { setText("Remove Zone"); }
void RemoveZoneCommand::undo() {
    if (auto sg = static_cast<SampleGroup*>(m_pm->getNode(m_sgId))) {
        sg->zones.insert(m_zoneIndex, m_savedZone);
        m_pm->setNodeModified(m_sgId, "zones");
    }
}
void RemoveZoneCommand::redo() {
    if (auto sg = static_cast<SampleGroup*>(m_pm->getNode(m_sgId))) {
        m_savedZone = sg->zones[m_zoneIndex];
        sg->zones.removeAt(m_zoneIndex);
        m_pm->setNodeModified(m_sgId, "zones");
    }
}

ModifyZoneCommand::ModifyZoneCommand(ProjectManager* pm, const QUuid& sgId, int zoneIndex, const Zone& oldZone, const Zone& newZone) 
    : m_pm(pm), m_sgId(sgId), m_zoneIndex(zoneIndex), m_oldZone(oldZone), m_newZone(newZone) { setText("Modify Zone"); }
void ModifyZoneCommand::undo() {
    if (auto sg = static_cast<SampleGroup*>(m_pm->getNode(m_sgId))) {
        sg->zones[m_zoneIndex] = m_oldZone;
        m_pm->setNodeModified(m_sgId, "zones");
    }
}
void ModifyZoneCommand::redo() {
    if (auto sg = static_cast<SampleGroup*>(m_pm->getNode(m_sgId))) {
        sg->zones[m_zoneIndex] = m_newZone;
        m_pm->setNodeModified(m_sgId, "zones");
    }
}
bool ModifyZoneCommand::mergeWith(const QUndoCommand *command) {
    if (command->id() != id()) return false;
    const ModifyZoneCommand* cmd = static_cast<const ModifyZoneCommand*>(command);
    if (cmd->m_sgId != m_sgId || cmd->m_zoneIndex != m_zoneIndex) return false;
    m_newZone = cmd->m_newZone; return true;
}

ModifyMultipleZonesCommand::ModifyMultipleZonesCommand(ProjectManager* pm, const QUuid& sgId, const QVector<int>& indices, const QVector<Zone>& oldZones, const QVector<Zone>& newZones)
    : m_pm(pm), m_sgId(sgId), m_indices(indices), m_oldZones(oldZones), m_newZones(newZones) {
    setText(QObject::tr("Modify %1 Zones").arg(indices.size()));
}
void ModifyMultipleZonesCommand::undo() {
    if (auto sg = static_cast<SampleGroup*>(m_pm->getNode(m_sgId))) {
        for (int i = 0; i < m_indices.size(); ++i) {
            if (m_indices[i] >= 0 && m_indices[i] < sg->zones.size()) {
                sg->zones[m_indices[i]] = m_oldZones[i];
            }
        }
        m_pm->setNodeModified(m_sgId, "zones");
    }
}
void ModifyMultipleZonesCommand::redo() {
    if (auto sg = static_cast<SampleGroup*>(m_pm->getNode(m_sgId))) {
        for (int i = 0; i < m_indices.size(); ++i) {
            if (m_indices[i] >= 0 && m_indices[i] < sg->zones.size()) {
                sg->zones[m_indices[i]] = m_newZones[i];
            }
        }
        m_pm->setNodeModified(m_sgId, "zones");
    }
}

RemoveMultipleZonesCommand::RemoveMultipleZonesCommand(ProjectManager* pm, const QUuid& sgId, const QVector<int>& indices)
    : m_pm(pm), m_sgId(sgId), m_indices(indices) {
    setText(QObject::tr("Remove %1 Zones").arg(indices.size()));
    
    // Sort descending to avoid shifting issues when deleting
    std::sort(m_indices.begin(), m_indices.end(), std::greater<int>());
}
void RemoveMultipleZonesCommand::undo() {
    if (auto sg = static_cast<SampleGroup*>(m_pm->getNode(m_sgId))) {
        // Insert back in the exact indices (since we iterate in descending order of original index)
        for (int i = 0; i < m_indices.size(); ++i) {
            sg->zones.insert(m_indices[i], m_savedZones[i]);
        }
        m_pm->setNodeModified(m_sgId, "zones");
    }
}
void RemoveMultipleZonesCommand::redo() {
    if (auto sg = static_cast<SampleGroup*>(m_pm->getNode(m_sgId))) {
        m_savedZones.clear();
        for (int i = 0; i < m_indices.size(); ++i) {
            int idx = m_indices[i];
            if (idx >= 0 && idx < sg->zones.size()) {
                m_savedZones.append(sg->zones[idx]);
                sg->zones.removeAt(idx);
            }
        }
        m_pm->setNodeModified(m_sgId, "zones");
    }
}
