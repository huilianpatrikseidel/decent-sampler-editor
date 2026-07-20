#pragma once
#include <QWidget>
#include <QUuid>


#include "ui/mixer/MixerLibExport.h"
class ProjectManager;
class ApplicationController;
class QScrollArea;
class QVBoxLayout;
class QPushButton;

class MIXERLIB_EXPORT FxRackWidget : public QWidget {
    Q_OBJECT
public:
    explicit FxRackWidget(ApplicationController* appCtrl, const QUuid& sgId, QWidget* parent = nullptr);

    void updateFromNode();

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    ApplicationController* m_appCtrl;
    ProjectManager* m_pm;
    QUuid m_sgId;

    QScrollArea* m_fxScrollArea;
    QWidget* m_fxContainer;
    QVBoxLayout* m_fxContainerLayout;
    QPushButton* m_btnAddFx;
};
