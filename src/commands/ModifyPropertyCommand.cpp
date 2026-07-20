#include "ModifyPropertyCommand.h"
#include "core/models/AudioNodes.h"
#include "core/models/UiComponents.h"
#include "core/models/GraphStructures.h"

ModifyPropertyCommand::ModifyPropertyCommand(ProjectManager* pm, const QUuid& nodeId, const QString& propertyPath, const QVariant& oldVal, const QVariant& newVal)
    : m_pm(pm), m_nodeId(nodeId), m_propertyPath(propertyPath), m_oldVal(oldVal), m_newVal(newVal) {
    setText(QString("Change %1").arg(propertyPath));
}

void ModifyPropertyCommand::undo() { applyValue(m_oldVal); }
void ModifyPropertyCommand::redo() { applyValue(m_newVal); }

void ModifyPropertyCommand::applyValue(const QVariant& val) {
    Node* node = m_pm->getNode(m_nodeId);
    if (!node) return;
    
    if (m_propertyPath.isEmpty()) {
        node->loadFromJson(val.toJsonObject());
    } else if (m_propertyPath == "name") {
        node->name = val.toString();
    } else if (node->type == "SampleGroup") {
        SampleGroup* sg = static_cast<SampleGroup*>(node);
        if (m_propertyPath == "volume") sg->volume = val.toDouble();
        else if (m_propertyPath == "pan") sg->pan = val.toDouble();
        else if (m_propertyPath == "ampEnv") {
            QJsonObject o = val.toJsonObject();
            sg->ampEnv.attack = o["attack"].toDouble();
            sg->ampEnv.decay = o["decay"].toDouble();
            sg->ampEnv.sustain = o["sustain"].toDouble();
            sg->ampEnv.release = o["release"].toDouble();
        }
        else if (m_propertyPath == "trigger") sg->trigger = val.toString();
        else if (m_propertyPath == "seqMode") sg->seqMode = val.toString();
        else if (m_propertyPath == "loCC64") sg->loCC64 = val.toInt();
        else if (m_propertyPath == "hiCC64") sg->hiCC64 = val.toInt();
        else if (m_propertyPath == "customTags") sg->customTags = val.toString();
        else if (m_propertyPath == "silencedByTags") sg->silencedByTags = val.toString();
        else if (m_propertyPath == "silencingMode") sg->silencingMode = val.toString();
        else if (m_propertyPath == "isOscillator") sg->isOscillator = val.toBool();
        else if (m_propertyPath == "oscParams") {
            QJsonObject o = val.toJsonObject();
            sg->oscParams.waveform = o["waveform"].toString("sine");
            sg->oscParams.wavetableFile = o["wavetableFile"].toString();
            sg->oscParams.wavetablePosition = o["wavetablePosition"].toDouble(0.0);
            sg->oscParams.wavetableFrameInterpolation = o["wavetableFrameInterpolation"].toBool(true);
            sg->oscParams.pluckType = o["pluckType"].toString("karplus_strong");
            sg->oscParams.damping = o["damping"].toDouble(0.5);
        }
        else if (m_propertyPath == "routings") {
            QJsonArray arr = val.toJsonArray();
            sg->routings.clear();
            for (int i = 0; i < arr.size(); ++i) {
                QJsonObject obj = arr[i].toObject();
                ModRouting r;
                r.source = obj["source"].toString();
                r.destination = obj["destination"].toString();
                r.amount = obj["amount"].toDouble();
                sg->routings.append(r);
            }
        }
    } else if (node->type == "Bus") {
        BusNode* bus = static_cast<BusNode*>(node);
        if (m_propertyPath == "volume") bus->volume = val.toDouble();
    } else if (node->type == "Delay") {
        DelayNode* d = static_cast<DelayNode*>(node);
        if (m_propertyPath == "time") d->time = val.toDouble();
        else if (m_propertyPath == "feedback") d->feedback = val.toDouble();
    } else if (node->type == "Reverb") {
        ReverbNode* r = static_cast<ReverbNode*>(node);
        if (m_propertyPath == "size") r->size = val.toDouble();
        else if (m_propertyPath == "damp") r->damp = val.toDouble();
    } else if (node->type == "Filter") {
        FilterNode* f = static_cast<FilterNode*>(node);
        if (m_propertyPath == "cutoff") f->cutoff = val.toDouble();
        else if (m_propertyPath == "resonance") f->resonance = val.toDouble();
    }
    m_pm->setNodeModified(m_nodeId, m_propertyPath);
}

bool ModifyPropertyCommand::mergeWith(const QUndoCommand *command) {
    if (command->id() != id()) return false;
    const ModifyPropertyCommand* cmd = static_cast<const ModifyPropertyCommand*>(command);
    if (cmd->m_nodeId != m_nodeId || cmd->m_propertyPath != m_propertyPath) return false;
    
    m_newVal = cmd->m_newVal;
    return true;
}

AddLfoCommand::AddLfoCommand(ProjectManager* pm, const LFO& lfo) : m_pm(pm), m_lfo(lfo) {
    setText(QString("Add %1").arg(lfo.name));
}
void AddLfoCommand::undo() { m_pm->getAudioState()->removeGlobalLfo(m_lfo.id); }
void AddLfoCommand::redo() { m_pm->getAudioState()->addGlobalLfo(m_lfo); }

RemoveLfoCommand::RemoveLfoCommand(ProjectManager* pm, const LFO& lfo) : m_pm(pm), m_lfo(lfo) {
    setText(QString("Remove %1").arg(lfo.name));
}
void RemoveLfoCommand::undo() { m_pm->getAudioState()->addGlobalLfo(m_lfo); }
void RemoveLfoCommand::redo() { m_pm->getAudioState()->removeGlobalLfo(m_lfo.id); }

ModifyLfoCommand::ModifyLfoCommand(ProjectManager* pm, const QUuid& lfoId, const LFO& oldLfo, const LFO& newLfo)
    : m_pm(pm), m_lfoId(lfoId), m_oldLfo(oldLfo), m_newLfo(newLfo) {
    setText(QString("Modify %1").arg(newLfo.name));
}
void ModifyLfoCommand::undo() { m_pm->getAudioState()->updateGlobalLfo(m_lfoId, m_oldLfo); }
void ModifyLfoCommand::redo() { m_pm->getAudioState()->updateGlobalLfo(m_lfoId, m_newLfo); }
bool ModifyLfoCommand::mergeWith(const QUndoCommand *command) {
    if (command->id() != id()) return false;
    const ModifyLfoCommand* cmd = static_cast<const ModifyLfoCommand*>(command);
    if (cmd->m_lfoId != m_lfoId) return false;
    m_newLfo = cmd->m_newLfo;
    return true;
}
