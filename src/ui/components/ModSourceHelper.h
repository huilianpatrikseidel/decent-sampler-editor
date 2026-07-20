#pragma once
#include <QStringList>
#include <functional>


#include "ui/components/UiComponentsLibExport.h"
class ProjectManager;

namespace ModSourceHelper {
    struct ModSource {
        QString id;
        QString displayName;
    };
    
    // Returns all available modulation sources (LFOs + Envelopes + MIDI sources)
    UICOMPONENTSLIB_EXPORT QList<ModSource> getAvailableSources(ProjectManager* pm);
    
    // Creates a ModSourceProvider lambda suitable for SynthKnobWidget
    using ModSourceProvider = std::function<QList<ModSource>()>;
    UICOMPONENTSLIB_EXPORT ModSourceProvider createProvider(ProjectManager* pm);
}
