#include "MixerTopology.h"

#include "DecibelUtils.h"
#include "ProjectManager.h"

#include <algorithm>

namespace {

// Hops from a channel to master. Buses may feed buses, so this walks the chain. Cycles
// are refused when a route is assigned (ProjectManager::canRouteToBus), but a project
// edited by hand could still hold one, so the walk is bounded rather than trusting.
int depthToMaster(const ProjectManager* pm, const QUuid& channelId, int channelCount) {
    int depth = 0;
    QUuid cursor = pm->getOutputBus(channelId);
    while (!cursor.isNull() && depth <= channelCount) {
        ++depth;
        cursor = pm->getOutputBus(cursor);
    }
    return depth;
}

} // namespace

MixerTopology MixerTopology::build(const ProjectManager* pm, int maxChannels) {
    MixerTopology topology;
    if (!pm || maxChannels <= 0) return topology;

    // Collect every channel, with the depth that decides processing order.
    struct Entry { QUuid id; int depth; bool isBus; float gain; };
    std::vector<Entry> entries;

    const auto& nodes = pm->getAllNodes();
    const int nodeCount = static_cast<int>(nodes.size());
    for (const auto& pair : nodes) {
        const Node* n = pair.second.get();
        const bool isBus = (n->type == "Bus");
        if (!isBus && n->type != "SampleGroup") continue;

        // A group's own volume is already folded into its voices, so only a bus applies
        // gain at the channel stage; giving groups one too would apply it twice.
        const float gain = isBus
            ? DecibelUtils::dbToLinear(static_cast<const BusNode*>(n)->volume)
            : 1.0f;

        entries.push_back({ n->id, depthToMaster(pm, n->id, nodeCount), isBus, gain });
    }

    // Deepest first: a channel's destination is always exactly one hop shallower, so this
    // guarantees every channel is processed before whatever it feeds. Ties are ordered by
    // id so the layout is stable across rebuilds rather than following hash order.
    std::sort(entries.begin(), entries.end(), [](const Entry& a, const Entry& b) {
        if (a.depth != b.depth) return a.depth > b.depth;
        return a.id.toString() < b.id.toString();
    });

    if (static_cast<int>(entries.size()) > maxChannels) {
        entries.resize(maxChannels);
    }

    topology.order.reserve(entries.size());
    topology.gain.reserve(entries.size());
    for (const Entry& e : entries) {
        topology.indexOf.insert(e.id, static_cast<int>(topology.order.size()));
        topology.order.push_back(e.id);
        topology.gain.push_back(e.gain);
    }

    // Resolve destinations only after every index exists. A destination that was dropped
    // past maxChannels becomes -1, sending the channel to master instead of nowhere.
    topology.destination.reserve(topology.order.size());
    for (const QUuid& id : topology.order) {
        topology.destination.push_back(topology.indexFor(pm->getOutputBus(id)));
    }

    return topology;
}
