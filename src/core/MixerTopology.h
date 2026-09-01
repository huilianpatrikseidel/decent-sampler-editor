#pragma once

#include <QHash>
#include <QUuid>
#include <vector>

#include "CoreLibExport.h"

class ProjectManager;

// The mixer's channel layout, resolved once on the UI thread and consumed by two
// independent producers: AudioGraphBuilder stamps a channel index onto every render zone,
// and VstPluginManager builds the matching per-channel effect chains. They must agree on
// which index means which channel, so both derive it from here rather than numbering
// channels themselves -- disagreeing would route audio through the wrong effects.
//
// A channel is a SampleGroup or a Bus. Master is not a channel: it is the implicit sink,
// written as destination -1.
struct CORELIB_EXPORT MixerTopology {
    // Channel ids in processing order. A channel always appears before the channel it
    // feeds, so the audio thread can render the vector front to back in a single pass.
    std::vector<QUuid> order;

    // Index into `order` for each channel id.
    QHash<QUuid, int> indexOf;

    // Parallel to `order`: index of the channel this one feeds, or -1 for master.
    std::vector<int> destination;

    // Parallel to `order`: linear gain to apply when summing this channel into its
    // destination. Group gain is already applied per voice, so only buses carry one here.
    std::vector<float> gain;

    int channelCount() const { return static_cast<int>(order.size()); }

    // -1 when the channel is unknown or was dropped past maxChannels; callers treat that
    // as "straight to master, no inserts" rather than dropping its audio.
    int indexFor(const QUuid& channelId) const {
        const auto it = indexOf.find(channelId);
        return it == indexOf.end() ? -1 : it.value();
    }

    static MixerTopology build(const ProjectManager* pm, int maxChannels);
};
