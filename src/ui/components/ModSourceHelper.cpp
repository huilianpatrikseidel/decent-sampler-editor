#include "ModSourceHelper.h"
#include "../../core/ProjectManager.h"

QList<ModSourceHelper::ModSource> ModSourceHelper::getAvailableSources(ProjectManager* pm) {
    QList<ModSource> list;
    for (const auto& lfo : pm->getAudioState()->getGlobalLfos()) {
        list.append({lfo.name, lfo.name});
    }
    for (const auto& macro : pm->getAudioState()->getGlobalMacros()) {
        list.append({"MACRO_" + macro.id.toString(QUuid::WithoutBraces), macro.name});
    }
    QStringList fixed = {"Env 1", "Env 2", "Velocity", "ModWheel", "PitchBend", "Aftertouch"};
    for (const auto& f : fixed) {
        list.append({f, f});
    }
    return list;
}

ModSourceHelper::ModSourceProvider ModSourceHelper::createProvider(ProjectManager* pm) {
    return [pm]() -> QList<ModSource> {
        return ModSourceHelper::getAvailableSources(pm);
    };
}
