#pragma once
#include <QStringList>
#include <functional>


#include "ui/components/UiComponentsLibExport.h"
class ProjectManager;

namespace ModSourceHelper {
    // Returns all available modulation sources (LFOs + Envelopes + MIDI sources)
    UICOMPONENTSLIB_EXPORT QStringList getAvailableSources(ProjectManager* pm);
    
    // Creates a ModSourceProvider lambda suitable for SynthKnobWidget
    using ModSourceProvider = std::function<QStringList()>;
    UICOMPONENTSLIB_EXPORT ModSourceProvider createProvider(ProjectManager* pm);
}
