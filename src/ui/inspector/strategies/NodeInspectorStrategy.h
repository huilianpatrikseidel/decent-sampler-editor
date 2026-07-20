#pragma once
#include "IInspectorStrategy.h"
#include <QUuid>

class Node;

class NodeInspectorStrategy : public IInspectorStrategy {
public:
    NodeInspectorStrategy(const Node* node, const QUuid& id);
    void buildForm(QFormLayout* layout, PropertiesInspector* inspector) override;

private:
    const Node* m_node;
    QUuid m_id;
};
