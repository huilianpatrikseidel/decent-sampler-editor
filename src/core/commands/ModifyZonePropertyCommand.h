#pragma once
#include <QUndoCommand>
#include <QVariant>
#include "../ProjectManager.h"

class ModifyZonePropertyCommand : public QUndoCommand {
public:
    ModifyZonePropertyCommand(ProjectManager* pm, const QUuid& sgId, int zoneIndex, 
                              const QString& propertyName, const QVariant& oldValue, const QVariant& newValue)
        : m_pm(pm), m_sgId(sgId), m_zoneIndex(zoneIndex), m_propertyName(propertyName), 
          m_oldValue(oldValue), m_newValue(newValue) {
        setText(QString("Modify Zone %1 %2").arg(zoneIndex).arg(propertyName));
    }

    void undo() override {
        applyValue(m_oldValue);
    }

    void redo() override {
        applyValue(m_newValue);
    }

private:
    void applyValue(const QVariant& value) {
        Node* node = m_pm->getNode(m_sgId);
        if (node && node->type == "SampleGroup") {
            SampleGroup* sg = static_cast<SampleGroup*>(node);
            if (m_zoneIndex >= 0 && m_zoneIndex < sg->zones.size()) {
                Zone& z = sg->zones[m_zoneIndex];
                if (m_propertyName == "loNote") z.loNote = value.toInt();
                else if (m_propertyName == "hiNote") z.hiNote = value.toInt();
                else if (m_propertyName == "rootNote") z.rootNote = value.toInt();
                else if (m_propertyName == "loVel") z.loVel = value.toInt();
                else if (m_propertyName == "hiVel") z.hiVel = value.toInt();
                else if (m_propertyName == "samplePath") z.samplePath = value.toString();
                else if (m_propertyName == "loopEnabled") z.loopEnabled = value.toBool();
                else if (m_propertyName == "loopStart") z.loopStart = value.toInt();
                else if (m_propertyName == "loopEnd") z.loopEnd = value.toInt();
                else if (m_propertyName == "loopCrossfade") z.loopCrossfade = value.toInt();
                else if (m_propertyName == "useLocalAmpEnv") z.useLocalAmpEnv = value.toBool();
                else if (m_propertyName == "ampEnv") {
                    QJsonObject o = value.toJsonObject();
                    z.ampEnv.attack = o["attack"].toDouble();
                    z.ampEnv.decay = o["decay"].toDouble();
                    z.ampEnv.sustain = o["sustain"].toDouble();
                    z.ampEnv.release = o["release"].toDouble();
                }
                
                m_pm->setNodeModified(m_sgId, "zones");
            }
        }
    }

    ProjectManager* m_pm;
    QUuid m_sgId;
    int m_zoneIndex;
    QString m_propertyName;
    QVariant m_oldValue;
    QVariant m_newValue;
};
