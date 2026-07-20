#include "NodeMapView.h"
#include <QVBoxLayout>
#include "../../commands/AddNodeCommand.h"
#include "../../commands/MoveNodeCommand.h"
#include "../../commands/RemoveNodeCommand.h"
#include "../../commands/ConnectionCommands.h"

NodeMapView::NodeMapView(ApplicationController* controller, QWidget *parent) 
    : QWidget(parent), m_controller(controller) 
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    
    m_graphView = new NodeGraphView(controller->getProjectManager(), this);
    layout->addWidget(m_graphView);
    
    ProjectManager* pm = controller->getProjectManager();
    
    connect(pm, &ProjectManager::nodeAdded, m_graphView, &NodeGraphView::onNodeAdded);
    connect(pm, &ProjectManager::nodeRemoved, m_graphView, &NodeGraphView::onNodeRemoved);
    connect(pm, &ProjectManager::nodeModified, m_graphView, &NodeGraphView::onNodeModified);
    connect(pm, &ProjectManager::connectionAdded, m_graphView, &NodeGraphView::onConnectionAdded);
    connect(pm, &ProjectManager::connectionRemoved, m_graphView, &NodeGraphView::onConnectionRemoved);
    
    connect(m_graphView, &NodeGraphView::requestMoveNode, this, [pm](const QUuid& id, const QPointF& oldPos, const QPointF& newPos){
        pm->getUndoStack()->push(new MoveNodeCommand(pm, id, oldPos, newPos));
    });
    
    connect(m_graphView, &NodeGraphView::requestAddNode, this, [pm](const QString& type, const QPointF& pos){
        std::unique_ptr<Node> node;
        if (type == "SampleGroup") { node = std::make_unique<SampleGroup>(); node->name = "Group"; }
        else if (type == "Bus") { node = std::make_unique<BusNode>(); node->name = "Bus"; }
        else if (type == "Delay") { node = std::make_unique<DelayNode>(); node->name = "Delay"; }
        else if (type == "Reverb") { node = std::make_unique<ReverbNode>(); node->name = "Reverb"; }
        else if (type == "Filter") { node = std::make_unique<FilterNode>(); node->name = "Filter"; }
        
        if (node) {
            pm->getUndoStack()->push(new AddNodeCommand(pm, std::move(node), pos));
        }
    });
    
    connect(m_graphView, &NodeGraphView::requestRemoveNode, this, [pm](const QUuid& id){
        pm->getUndoStack()->push(new RemoveNodeCommand(pm, id));
    });
    
    connect(m_graphView, &NodeGraphView::requestAddConnection, this, [pm](const Connection& conn){
        pm->getUndoStack()->push(new AddConnectionCommand(pm, conn));
    });
    
    connect(m_graphView, &NodeGraphView::requestRemoveConnection, this, [pm](const Connection& conn){
        pm->getUndoStack()->push(new RemoveConnectionCommand(pm, conn));
    });
    
    // Initial sync
    for (const auto& pair : pm->getAllNodes()) {
        m_graphView->onNodeAdded(pair.first);
    }
    for (const auto& conn : pm->getAllConnections()) {
        m_graphView->onConnectionAdded(conn);
    }
}
