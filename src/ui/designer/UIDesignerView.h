#pragma once
#include <QWidget>

class ProjectManager;
class InstrumentCanvasView;

#include "UiDesignerExport.h"

class UIDESIGNER_EXPORT UIDesignerView : public QWidget {
    Q_OBJECT
public:
    explicit UIDesignerView(ProjectManager* pm, QWidget* parent = nullptr);
    InstrumentCanvasView* getCanvas() const { return m_uiCanvas; }
    QWidget* getLibraryWidget() const { return m_componentLibraryWidget; }

signals:
    void notePressed(int note, int vel);
    void noteReleased(int note);

private:
    InstrumentCanvasView* m_uiCanvas;
    QWidget* m_componentLibraryWidget;
    ProjectManager* m_pm;
};
