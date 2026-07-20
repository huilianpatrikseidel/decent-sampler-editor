#pragma once
#include <QWidget>
#include "../../core/ApplicationController.h"
#include "../canvas/NodeGraphView.h"

#include "NodeMapExport.h"

class NODEMAP_EXPORT NodeMapView : public QWidget {
    Q_OBJECT
public:
    explicit NodeMapView(ApplicationController* controller, QWidget *parent = nullptr);
    
private:
    ApplicationController* m_controller;
    NodeGraphView* m_graphView;
};
