#pragma once
#include "IInspectorStrategy.h"

class UiStateModel;

class CanvasInspectorStrategy : public IInspectorStrategy {
public:
    CanvasInspectorStrategy(UiStateModel* uiState);
    void buildForm(QFormLayout* layout, PropertiesInspector* inspector) override;

private:
    UiStateModel* m_uiState;
};
