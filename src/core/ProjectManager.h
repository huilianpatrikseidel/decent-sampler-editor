#pragma once
#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QUndoStack>
#include <QPointF>
#include <QTimer>
#include <memory>
#include <unordered_map>
#include <vector>
#include "PresetManager.h"
#include "models/AudioNodes.h"
#include "models/UiComponents.h"
#include "models/GraphStructures.h"
#include "models/ProjectStates.h"

class Node;
class UiComponent;
class INodeVisitor;

#include "CoreLibExport.h"

class CORELIB_EXPORT ProjectManager : public QObject {
    Q_OBJECT
    friend class ProjectSerializer;
public:
    explicit ProjectManager(QObject *parent = nullptr);

signals:
    void nodeAdded(const QUuid& id);
    void nodeRemoved(const QUuid& id);
    void nodeModified(const QUuid& id, const QString& propertyName);
    void connectionAdded(const Connection& conn);
    void connectionRemoved(const Connection& conn);
    
    void uiComponentAdded(const QUuid& id);
    void uiComponentRemoved(const QUuid& id);
    void uiComponentModified(const QUuid& id, const QString& propertyName);
    
    void projectLoaded();
    void projectSaved();
    void projectSavedWithData(int id, const QString& projectData);
    void projectDirtyChanged(bool isDirty);

public:
    bool createNewProject(const QString &projectName);
    bool loadProject(const QString &projectData, int id = -1);
    bool saveProject();
    
    int getCurrentProjectId() const { return m_currentProjectId; }
    void setCurrentProjectId(int id) { m_currentProjectId = id; }
    
    void clearProject();
    
    QString getCurrentProjectName() const { return m_currentProjectName; }
    void setCurrentProjectName(const QString& name) { m_currentProjectName = name; }
    
    PresetManager* getPresetManager() { return m_presetManager.get(); }
    const PresetManager* getPresetManager() const { return m_presetManager.get(); }
    
    UiStateModel* getUiState() const { return m_uiState.get(); }
    AudioStateModel* getAudioState() const { return m_audioState.get(); }
    
    // Node clearing without clearing presets (for switching presets)
    void clearProjectNodes();
    
    // Gestão do QUndoStack nativa do projeto
    QUndoStack* getUndoStack() { return &m_undoStack; }

    // Gerenciamento de Memória Moderna C++17 (Smart Pointers)
    void addNode(std::unique_ptr<Node> node);
    
    Node* getNode(const QUuid& id) const;
    std::unique_ptr<Node> removeNode(const QUuid& id);
    
    void setNodePosition(const QUuid& id, double x, double y);
    QPointF getNodePosition(const QUuid& id) const;
    
    void addUiComponent(std::unique_ptr<UiComponent> comp);
    UiComponent* getUiComponent(const QUuid& id) const;
    std::unique_ptr<UiComponent> removeUiComponent(const QUuid& id);
    void setUiComponentModified(const QUuid& id, const QString& propertyName);
    
    const std::unordered_map<QUuid, std::unique_ptr<Node>>& getAllNodes() const { return m_nodes; }
    const std::unordered_map<QUuid, std::unique_ptr<UiComponent>>& getAllUiComponents() const { return m_uiComponents; }
    
    void addConnection(const Connection& conn);
    void removeConnection(const Connection& conn);
    bool canConnect(const Connection& conn) const;
    std::vector<Connection> getConnectionsForNode(const QUuid& id) const;
    const std::vector<Connection>& getAllConnections() const { return m_connections; }
    
    void setNodeModified(const QUuid& id, const QString& propertyName);
    void setDirty(bool dirty);
    bool isDirty() const { return m_isDirty; }
    
    void reorderEffect(const QUuid& srcFxId, const QUuid& srcChId, const QUuid& tgtFxId, const QUuid& tgtChId);


private:
    bool m_isDirty = false;
    QString m_currentProjectName;
    int m_currentProjectId = -1;
    QTimer* m_autoSaveTimer = nullptr;
    
    std::unique_ptr<PresetManager> m_presetManager;
    std::unique_ptr<UiStateModel> m_uiState;
    std::unique_ptr<AudioStateModel> m_audioState;
    
    std::unordered_map<QUuid, std::unique_ptr<Node>> m_nodes; 
    std::unordered_map<QUuid, QPointF> m_nodePositions;
    std::unordered_map<QUuid, std::unique_ptr<UiComponent>> m_uiComponents;
    std::vector<Connection> m_connections;
    QUndoStack m_undoStack;
};
