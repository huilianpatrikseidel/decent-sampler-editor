#pragma once
#include <QWidget>

class QListWidget;
class QPushButton;
class ApplicationController;
class InstrumentCanvasView;

class PresetListWidget : public QWidget {
    Q_OBJECT
public:
    PresetListWidget(ApplicationController* controller, InstrumentCanvasView* canvas, QWidget* parent = nullptr);
    void updateList();

private:
    ApplicationController* m_controller;
    InstrumentCanvasView* m_canvas;
    QListWidget* m_presetList;
    QPushButton* m_btnAddPreset;
    QPushButton* m_btnRemovePreset;
};
