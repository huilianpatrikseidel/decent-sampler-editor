#pragma once
#include "IInspectorStrategy.h"
#include <QUuid>

class UiComponent;

class UiComponentInspectorStrategy : public IInspectorStrategy {
public:
    UiComponentInspectorStrategy(const UiComponent* comp, const QUuid& id);
    void buildForm(QFormLayout* layout, PropertiesInspector* inspector) override;

private:
    void buildDataBindingForm(const UiComponent* comp, QFormLayout* layout, PropertiesInspector* inspector);

    const UiComponent* m_comp;
    QUuid m_id;
};
