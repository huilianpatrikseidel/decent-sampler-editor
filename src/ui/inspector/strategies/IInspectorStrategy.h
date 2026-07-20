#pragma once

class QFormLayout;
class PropertiesInspector;

class IInspectorStrategy {
public:
    virtual ~IInspectorStrategy() = default;
    virtual void buildForm(QFormLayout* layout, PropertiesInspector* inspector) = 0;
};
