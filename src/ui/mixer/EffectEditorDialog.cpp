#include "EffectEditorDialog.h"
#include "../../core/ProjectManager.h"
#include "../../core/models/AudioNodes.h"
#include "../../commands/ModifyPropertyCommand.h"
#include <QFormLayout>
#include <QDial>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QSlider>
#include "Vst3EditorWidget.h"
#include "../../audio/Vst3Host.h"

#include "../../core/ApplicationController.h"

EffectEditorDialog::EffectEditorDialog(ApplicationController* appCtrl, const QUuid& fxId, QWidget* parent)
    : NeumorphicDialog(parent), m_appCtrl(appCtrl), m_pm(appCtrl->getProjectManager()), m_fxId(fxId)
{
    // Set modern styling for the dialog itself
    setStyleSheet("QLabel { color: white; }");
    
    Node* node = m_pm->getNode(fxId);
    if (node) {
        setDialogTitle(node->type + " Editor - " + node->name);
        buildUiForNode(node);
    } else {
        setDialogTitle("Effect Not Found");
    }
}

void EffectEditorDialog::buildUiForNode(Node* node) {
    std::shared_ptr<void> rawHost = m_appCtrl->getVstPluginManager()->getVstHost(m_fxId);
    std::shared_ptr<Vst3Host> host = std::static_pointer_cast<Vst3Host>(rawHost);
    
    if (host) {
        Vst3EditorWidget* vstWidget = new Vst3EditorWidget(host, this);
        contentLayout()->addWidget(vstWidget);
        
        // Resize dialog to fit the widget exactly
        // NeumorphicDialog has margins, so we add 40px width and 76px height (10+10 main, 20+20 content, 36 titlebar)
        setFixedSize(vstWidget->size().width() + 60, vstWidget->size().height() + 96);
    } else {
        QLabel* fallback = new QLabel("Failed to load VST3 Interface for: " + node->type);
        fallback->setAlignment(Qt::AlignCenter);
        contentLayout()->addWidget(fallback);
    }
}
