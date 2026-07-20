#ifndef EFFECT_EDITOR_DIALOG_H
#define EFFECT_EDITOR_DIALOG_H

#include "../components/NeumorphicDialog.h"
#include <QUuid>


#include "ui/mixer/MixerLibExport.h"
class ProjectManager;
class ApplicationController;
class Node;

class MIXERLIB_EXPORT EffectEditorDialog : public NeumorphicDialog {
    Q_OBJECT
public:
    explicit EffectEditorDialog(ApplicationController* appCtrl, const QUuid& fxId, QWidget* parent = nullptr);
    
private:
    void buildUiForNode(Node* node);
    
    ApplicationController* m_appCtrl;
    ProjectManager* m_pm;
    QUuid m_fxId;
    bool m_updating = false;
};

#endif // EFFECT_EDITOR_DIALOG_H
