#include "ModSourceHelper.h"
#include "../../core/ProjectManager.h"

QStringList ModSourceHelper::getAvailableSources(ProjectManager* pm) {
    QStringList list;
    for (const auto& lfo : pm->getAudioState()->getGlobalLfos()) {
        list << lfo.name;
    }
    list << "Env 1" << "Env 2" << "Velocity" << "ModWheel" << "PitchBend" << "Aftertouch";
    return list;
}

ModSourceHelper::ModSourceProvider ModSourceHelper::createProvider(ProjectManager* pm) {
    return [pm]() -> QStringList {
        return ModSourceHelper::getAvailableSources(pm);
    };
}
