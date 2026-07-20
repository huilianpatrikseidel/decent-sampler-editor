#include "ProjectManager.h"
#include "ProjectSerializer.h"
#include <QFile>
#include <QJsonArray>
#include <QDebug>

ProjectManager::ProjectManager(QObject *parent) : QObject(parent) {
    m_presetManager = std::make_unique<PresetManager>(this);
    m_uiState = std::make_unique<UiStateModel>(this);
    m_audioState = std::make_unique<AudioStateModel>(this);
    
    m_undoStack.setUndoLimit(200); 
    
    m_autoSaveTimer = new QTimer(this);
    m_autoSaveTimer->setSingleShot(true);
    connect(m_autoSaveTimer, &QTimer::timeout, this, [this]() {
        if (m_currentProjectId != -1) {
            saveProject();
        }
    });
    
    // Connect state changes to dirty flag
    connect(m_uiState.get(), &UiStateModel::stateModified, this, [this]() { setDirty(true); });
    connect(m_audioState.get(), &AudioStateModel::audioStateModified, this, [this]() { setDirty(true); });
    
    createNewProject("Untitled");
}

void ProjectManager::clearProject() {
    clearProjectNodes();
    m_presetManager->clear();
}

void ProjectManager::clearProjectNodes() {
    m_nodes.clear();
    m_nodePositions.clear();
    m_uiComponents.clear();
    m_connections.clear();
    m_undoStack.clear();
    m_uiState->clear();
    m_audioState->clear();
}

bool ProjectManager::createNewProject(const QString &projectName) {
    m_currentProjectName = projectName;
    clearProject();
    
    auto sg = std::make_unique<SampleGroup>();
    sg->name = "Group 1";
    sg->id = QUuid::createUuid();
    m_nodes[sg->id] = std::move(sg);
    
    setDirty(false);
    emit projectLoaded();
    return true;
}

void ProjectManager::setDirty(bool dirty) {
    if (m_isDirty != dirty) {
        m_isDirty = dirty;
        emit projectDirtyChanged(m_isDirty);
    }
    if (dirty) {
        m_autoSaveTimer->start(2000); // Auto-save after 2 seconds
    } else {
        m_autoSaveTimer->stop();
    }
}

void ProjectManager::setNodeModified(const QUuid& id, const QString& propertyName) {
    setDirty(true);
    emit nodeModified(id, propertyName);
}

void ProjectManager::addNode(std::unique_ptr<Node> node) {
    if (node) {
        QUuid id = node->id;
        m_nodes[id] = std::move(node);
        setDirty(true);
        emit nodeAdded(id);
    }
}

Node* ProjectManager::getNode(const QUuid& id) const {
    auto it = m_nodes.find(id);
    if (it != m_nodes.end()) return it->second.get();
    return nullptr;
}

void ProjectManager::setNodePosition(const QUuid& id, double x, double y) {
    m_nodePositions[id] = QPointF(x, y);
    setDirty(true);
}

QPointF ProjectManager::getNodePosition(const QUuid& id) const {
    auto it = m_nodePositions.find(id);
    if (it != m_nodePositions.end()) return it->second;
    return QPointF(0.0, 0.0);
}

std::unique_ptr<Node> ProjectManager::removeNode(const QUuid& id) {
    auto it = m_nodes.find(id);
    if (it != m_nodes.end()) {
        std::unique_ptr<Node> extractedNode = std::move(it->second);
        m_nodes.erase(it);
        m_nodePositions.erase(id);
        
        auto connIt = m_connections.begin();
        while (connIt != m_connections.end()) {
            if (connIt->sourceId == id || connIt->targetId == id) {
                emit connectionRemoved(*connIt);
                connIt = m_connections.erase(connIt);
            } else {
                ++connIt;
            }
        }
        
        setDirty(true);
        emit nodeRemoved(id);
        return extractedNode;
    }
    return nullptr;
}

void ProjectManager::setUiComponentModified(const QUuid& id, const QString& propertyName) {
    setDirty(true);
    emit uiComponentModified(id, propertyName);
}

void ProjectManager::addUiComponent(std::unique_ptr<UiComponent> comp) {
    if (comp) {
        QUuid id = comp->id;
        m_uiComponents[id] = std::move(comp);
        setDirty(true);
        emit uiComponentAdded(id);
    }
}

UiComponent* ProjectManager::getUiComponent(const QUuid& id) const {
    auto it = m_uiComponents.find(id);
    if (it != m_uiComponents.end()) return it->second.get();
    return nullptr;
}

std::unique_ptr<UiComponent> ProjectManager::removeUiComponent(const QUuid& id) {
    auto it = m_uiComponents.find(id);
    if (it != m_uiComponents.end()) {
        std::unique_ptr<UiComponent> extracted = std::move(it->second);
        m_uiComponents.erase(it);
        setDirty(true);
        emit uiComponentRemoved(id);
        return extracted;
    }
    return nullptr;
}

void ProjectManager::addConnection(const Connection& conn) {
    if (canConnect(conn)) {
        m_connections.push_back(conn);
        setDirty(true);
        emit connectionAdded(conn);
    }
}

void ProjectManager::removeConnection(const Connection& conn) {
    auto it = std::find(m_connections.begin(), m_connections.end(), conn);
    if (it != m_connections.end()) {
        m_connections.erase(it);
        setDirty(true);
        emit connectionRemoved(conn);
    }
}

std::vector<Connection> ProjectManager::getConnectionsForNode(const QUuid& id) const {
    std::vector<Connection> conns;
    for (const auto& c : m_connections) {
        if (c.sourceId == id || c.targetId == id) {
            conns.push_back(c);
        }
    }
    return conns;
}

bool ProjectManager::canConnect(const Connection& conn) const {
    if (conn.sourceId == conn.targetId) return false;

    Node* src = getNode(conn.sourceId);
    Node* tgt = getNode(conn.targetId);
    if (!src || !tgt) return false;

    PortDefinition srcDef, tgtDef;
    bool srcFound = false, tgtFound = false;
    for (const auto& p : src->getPorts()) {
        if (p.name == conn.sourcePort && p.direction == PortDefinition::Output) {
            srcDef = p; srcFound = true; break;
        }
    }
    for (const auto& p : tgt->getPorts()) {
        if (p.name == conn.targetPort && p.direction == PortDefinition::Input) {
            tgtDef = p; tgtFound = true; break;
        }
    }

    if (!srcFound || !tgtFound || srcDef.type != tgtDef.type) return false;

    if (std::find(m_connections.begin(), m_connections.end(), conn) != m_connections.end()) {
        return false;
    }

    // Anti-Cycle DFS
    std::function<bool(const QUuid&)> hasCycle = [&](const QUuid& currentId) {
        if (currentId == conn.sourceId) return true;
        for (const auto& c : m_connections) {
            if (c.sourceId == currentId) {
                if (hasCycle(c.targetId)) return true;
            }
        }
        return false;
    };
    if (hasCycle(conn.targetId)) return false;

    return true;
}

bool ProjectManager::loadProject(const QString &projectData, int id) {
    clearProject(); // Crucial: clear before loading!
    
    // Check if empty, which means a brand new empty project from DB
    if (projectData.isEmpty()) {
        auto sg = std::make_unique<SampleGroup>();
        sg->name = "Group 1";
        sg->id = QUuid::createUuid();
        m_nodes[sg->id] = std::move(sg);
        m_currentProjectId = id;
        emit projectLoaded();
        emit projectDirtyChanged(m_isDirty);
        return true;
    }
    
    if (ProjectSerializer::loadProject(this, projectData)) {
        m_currentProjectId = id;
        
        emit projectLoaded();
        emit projectDirtyChanged(m_isDirty);
        return true;
    }
    return false;
}

bool ProjectManager::saveProject() {
    if (m_currentProjectId == -1) return false;
    
    QString projectData = ProjectSerializer::saveProject(this);
    
    setDirty(false);
    emit projectSaved();
    emit projectSavedWithData(m_currentProjectId, projectData);
    
    return true;
}

void ProjectManager::reorderEffect(const QUuid& srcFxId, const QUuid& srcChId, const QUuid& tgtFxId, const QUuid& tgtChId) {
    if (srcFxId.isNull()) return;

    // Helper lambda to get/set effects list for a channel
    auto getEffectsList = [this](const QUuid& chId, QVector<QUuid>& listOut) -> bool {
        if (chId.isNull()) {
            listOut = m_audioState->getMasterEffects();
            return true;
        }
        Node* n = getNode(chId);
        if (n && n->type == "SampleGroup") {
            listOut = static_cast<SampleGroup*>(n)->insertEffects;
            return true;
        } else if (n && n->type == "Bus") {
            listOut = static_cast<BusNode*>(n)->insertEffects;
            return true;
        }
        return false;
    };

    auto setEffectsList = [this](const QUuid& chId, const QVector<QUuid>& listIn) {
        if (chId.isNull()) {
            m_audioState->setMasterEffects(listIn);
        } else {
            Node* n = getNode(chId);
            if (n && n->type == "SampleGroup") {
                static_cast<SampleGroup*>(n)->insertEffects = listIn;
            } else if (n && n->type == "Bus") {
                static_cast<BusNode*>(n)->insertEffects = listIn;
            }
        }
    };

    QVector<QUuid> srcList, tgtList;
    if (!getEffectsList(srcChId, srcList)) return;
    
    // Remove from source list
    int srcIdx = srcList.indexOf(srcFxId);
    if (srcIdx == -1) return;
    srcList.removeAt(srcIdx);
    
    if (srcChId == tgtChId) {
        tgtList = srcList; // They are the same list, already removed
    } else {
        if (!getEffectsList(tgtChId, tgtList)) return;
    }
    
    // Insert into target list
    if (tgtFxId.isNull()) {
        // Append to end
        tgtList.append(srcFxId);
    } else {
        // Insert before target
        int tgtIdx = tgtList.indexOf(tgtFxId);
        if (tgtIdx == -1) tgtList.append(srcFxId);
        else tgtList.insert(tgtIdx, srcFxId);
    }
    
    // Write back
    if (srcChId != tgtChId) {
        setEffectsList(srcChId, srcList);
    }
    setEffectsList(tgtChId, tgtList);
    
    // Notify
    setDirty(true);
    // Let's trigger a full UI refresh by notifying both nodes changed, or just a project loaded refresh.
    // For simplicity, we can emit nodeModified for the channels
    if (!srcChId.isNull()) emit nodeModified(srcChId, "insertEffects");
    if (!tgtChId.isNull() && srcChId != tgtChId) emit nodeModified(tgtChId, "insertEffects");
    // If master is involved, there's no specific node ID, we might need a general signal or just force a refresh
    emit projectDirtyChanged(m_isDirty); // This doesn't trigger channel strip update directly
    
    // Hack: We can just emit projectLoaded to rebuild the whole mixer for now.
    // That's a bit heavy, let's just emit nodeModified with empty QUuid for master
    if (srcChId.isNull()) emit nodeModified(QUuid(), "masterEffects");
    if (tgtChId.isNull() && !srcChId.isNull()) emit nodeModified(QUuid(), "masterEffects");
}
