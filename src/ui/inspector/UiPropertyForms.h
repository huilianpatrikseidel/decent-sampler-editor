#pragma once
#include <QString>
#include <functional>
#include <map>

class UiComponent;
class PropertiesInspector;
class QFormLayout;

class UiPropertyForms {
public:
    using BuilderFunc = std::function<void(const UiComponent* comp, PropertiesInspector* inspector, QFormLayout* layout)>;
    static void init();
    static void registerBuilder(const QString& type, BuilderFunc builder);
    static void buildForm(const UiComponent* comp, PropertiesInspector* inspector, QFormLayout* layout);
    
private:
    static std::map<QString, BuilderFunc> s_builders;
    static bool s_initialized;
};
