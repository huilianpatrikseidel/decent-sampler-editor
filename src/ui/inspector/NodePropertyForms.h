#pragma once
#include <QString>
#include <functional>
#include <map>
#include <QUuid>

class Node;
class PropertiesInspector;
class QFormLayout;

class NodePropertyForms {
public:
    using BuilderFunc = std::function<void(const Node* node, const QUuid& nodeId, PropertiesInspector* inspector, QFormLayout* layout)>;
    static void init();
    static void registerBuilder(const QString& type, BuilderFunc builder);
    static void buildForm(const Node* node, const QUuid& nodeId, PropertiesInspector* inspector, QFormLayout* layout);
    
private:
    static std::map<QString, BuilderFunc> s_builders;
    static bool s_initialized;
};
