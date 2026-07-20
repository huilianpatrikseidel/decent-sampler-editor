#include "UiPropertyForms.h"
#include "PropertiesInspector.h"
#include "core/models/AudioNodes.h"
#include "core/models/UiComponents.h"
#include "core/models/GraphStructures.h"
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>

std::map<QString, UiPropertyForms::BuilderFunc> UiPropertyForms::s_builders;
bool UiPropertyForms::s_initialized = false;

void UiPropertyForms::registerBuilder(const QString& type, BuilderFunc builder) {
    s_builders[type] = builder;
}

static void buildDataBindingSection(const UiComponent* comp, PropertiesInspector* ins, QFormLayout* lay) {
    QComboBox* targetNodeCombo = new QComboBox();
    targetNodeCombo->addItem("None", QUuid().toString());
    
    auto pm = ins->getProjectManager();
    const auto& allNodes = pm->getAllNodes();
    
    int currentIndex = 0;
    int idx = 1;
    for (const auto& pair : allNodes) {
        const auto& node = pair.second;
        if (node->type == "SampleGroup" || node->type == "Delay" || node->type == "Reverb" || node->type == "Filter") {
            targetNodeCombo->addItem(node->name, node->id.toString());
            if (node->id == comp->targetNodeId) currentIndex = idx;
            idx++;
        }
    }
    targetNodeCombo->setCurrentIndex(currentIndex);
    
    QComboBox* targetPropCombo = new QComboBox();
    
    auto updatePropCombo = [pm, targetPropCombo, comp](const QUuid& nodeId) {
        targetPropCombo->clear();
        if (nodeId.isNull()) return;
        
        Node* node = pm->getNode(nodeId);
        if (!node) return;
        
        if (node->type == "SampleGroup") {
            targetPropCombo->addItems({"volume", "pan", "tuning", "attack", "decay", "sustain", "release", "seqMode", "loCC64", "hiCC64"});
        } else if (node->type == "Delay") {
            targetPropCombo->addItems({"time", "feedback"});
        } else if (node->type == "Reverb") {
            targetPropCombo->addItems({"size", "damp"});
        } else if (node->type == "Filter") {
            targetPropCombo->addItems({"cutoff", "resonance"});
        }
        
        int pIdx = targetPropCombo->findText(comp->targetProperty);
        if (pIdx >= 0) targetPropCombo->setCurrentIndex(pIdx);
        else if (targetPropCombo->count() > 0) targetPropCombo->setCurrentIndex(0);
    };
    
    updatePropCombo(comp->targetNodeId);
    
    QObject::connect(targetNodeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [ins, comp, targetNodeCombo, updatePropCombo](int index) {
        QUuid newId(targetNodeCombo->itemData(index).toString());
        ins->notifyUiPropertyChanged(comp->id, "targetNodeId", comp->targetNodeId.toString(), newId.toString());
        updatePropCombo(newId);
        
        // Also update the property right away if it changed
        if (QComboBox* propCombo = targetNodeCombo->parentWidget()->findChild<QComboBox*>("targetPropCombo")) {
            ins->notifyUiPropertyChanged(comp->id, "targetProperty", comp->targetProperty, propCombo->currentText());
        }
    });
    
    targetPropCombo->setObjectName("targetPropCombo"); // For finding it later
    QObject::connect(targetPropCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [ins, comp, targetPropCombo](int index) {
        if (index >= 0) {
            ins->notifyUiPropertyChanged(comp->id, "targetProperty", comp->targetProperty, targetPropCombo->currentText());
        }
    });
    
    lay->addRow("<b>[ Data Binding ]</b>", new QLabel(""));
    lay->addRow("Target Node", targetNodeCombo);
    lay->addRow("Target Property", targetPropCombo);
    
    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    lay->addRow(line);
}

void UiPropertyForms::init() {
    if (s_initialized) return;
    s_initialized = true;
    
    registerBuilder("UiKnob", [](const UiComponent* c, PropertiesInspector* ins, QFormLayout* lay) {
        buildDataBindingSection(c, ins, lay);
        const UiKnob* comp = static_cast<const UiKnob*>(c);
        QLineEdit* pathEdit = new QLineEdit(comp->filmstripPath);
        QObject::connect(pathEdit, &QLineEdit::editingFinished, [ins, comp, pathEdit]() {
            ins->notifyUiPropertyChanged(comp->id, "filmstripPath", "", pathEdit->text());
        });
        lay->addRow("Filmstrip PNG", pathEdit);
        
        QSpinBox* framesBox = new QSpinBox(); framesBox->setRange(1, 1000); framesBox->setValue(comp->frameCount);
        QObject::connect(framesBox, &QSpinBox::editingFinished, [ins, comp, framesBox]() {
            ins->notifyUiPropertyChanged(comp->id, "frameCount", 0, framesBox->value());
        });
        lay->addRow("Frames", framesBox);
        
        QSlider* valSlider = new QSlider(Qt::Horizontal); valSlider->setRange(0, 100); valSlider->setValue(comp->currentValue * 100);
        QObject::connect(valSlider, &QSlider::sliderReleased, [ins, comp, valSlider]() {
            ins->notifyUiPropertyChanged(comp->id, "currentValue", 0.0, valSlider->value() / 100.0);
        });
        lay->addRow("Test Value", valSlider);
    });

    registerBuilder("UiSlider", [](const UiComponent* c, PropertiesInspector* ins, QFormLayout* lay) {
        buildDataBindingSection(c, ins, lay);
        const UiSlider* comp = static_cast<const UiSlider*>(c);
        QLineEdit* pathEdit = new QLineEdit(comp->filmstripPath);
        QObject::connect(pathEdit, &QLineEdit::editingFinished, [ins, comp, pathEdit]() {
            ins->notifyUiPropertyChanged(comp->id, "filmstripPath", "", pathEdit->text());
        });
        lay->addRow("Filmstrip PNG", pathEdit);
        
        QSpinBox* framesBox = new QSpinBox(); framesBox->setRange(1, 1000); framesBox->setValue(comp->frameCount);
        QObject::connect(framesBox, &QSpinBox::editingFinished, [ins, comp, framesBox]() {
            ins->notifyUiPropertyChanged(comp->id, "frameCount", 0, framesBox->value());
        });
        lay->addRow("Frames", framesBox);
        
        QSlider* valSlider = new QSlider(Qt::Horizontal); valSlider->setRange(0, 100); valSlider->setValue(comp->currentValue * 100);
        QObject::connect(valSlider, &QSlider::sliderReleased, [ins, comp, valSlider]() {
            ins->notifyUiPropertyChanged(comp->id, "currentValue", 0.0, valSlider->value() / 100.0);
        });
        lay->addRow("Test Value", valSlider);
    });
    
    registerBuilder("UiShape", [](const UiComponent* c, PropertiesInspector* ins, QFormLayout* lay) {
        const UiShape* comp = static_cast<const UiShape*>(c);
        QLineEdit* fillEdit = new QLineEdit(comp->fillColorHex);
        QObject::connect(fillEdit, &QLineEdit::editingFinished, [ins, comp, fillEdit]() {
            ins->notifyUiPropertyChanged(comp->id, "fillColorHex", "", fillEdit->text());
        });
        lay->addRow("Fill Color", fillEdit);
        
        QLineEdit* strokeEdit = new QLineEdit(comp->strokeColorHex);
        QObject::connect(strokeEdit, &QLineEdit::editingFinished, [ins, comp, strokeEdit]() {
            ins->notifyUiPropertyChanged(comp->id, "strokeColorHex", "", strokeEdit->text());
        });
        lay->addRow("Stroke Color", strokeEdit);
        
        QSpinBox* strokeWBox = new QSpinBox(); strokeWBox->setRange(0, 100); strokeWBox->setValue(comp->strokeWidth);
        QObject::connect(strokeWBox, &QSpinBox::editingFinished, [ins, comp, strokeWBox]() {
            ins->notifyUiPropertyChanged(comp->id, "strokeWidth", 0, strokeWBox->value());
        });
        lay->addRow("Stroke Width", strokeWBox);
    });
    
    registerBuilder("UiMenu", [](const UiComponent* c, PropertiesInspector* ins, QFormLayout* lay) {
        const UiMenu* comp = static_cast<const UiMenu*>(c);
        QLineEdit* optsEdit = new QLineEdit(comp->options.join(", "));
        QObject::connect(optsEdit, &QLineEdit::editingFinished, [ins, comp, optsEdit]() {
            ins->notifyUiPropertyChanged(comp->id, "options", "", optsEdit->text());
        });
        lay->addRow("Options (comma sep)", optsEdit);
        
        QLineEdit* bgEdit = new QLineEdit(comp->bgColorHex);
        QObject::connect(bgEdit, &QLineEdit::editingFinished, [ins, comp, bgEdit]() {
            ins->notifyUiPropertyChanged(comp->id, "bgColorHex", "", bgEdit->text());
        });
        lay->addRow("Background", bgEdit);
        
        QLineEdit* textEdit = new QLineEdit(comp->textColorHex);
        QObject::connect(textEdit, &QLineEdit::editingFinished, [ins, comp, textEdit]() {
            ins->notifyUiPropertyChanged(comp->id, "textColorHex", "", textEdit->text());
        });
        lay->addRow("Text Color", textEdit);
    });

    registerBuilder("UiXYPad", [](const UiComponent* c, PropertiesInspector* ins, QFormLayout* lay) {
        const UiXYPad* comp = static_cast<const UiXYPad*>(c);
        QLineEdit* bgEdit = new QLineEdit(comp->fillColorHex);
        QObject::connect(bgEdit, &QLineEdit::editingFinished, [ins, comp, bgEdit]() {
            ins->notifyUiPropertyChanged(comp->id, "fillColorHex", "", bgEdit->text());
        });
        lay->addRow("Fill Color", bgEdit);
        
        QLineEdit* indEdit = new QLineEdit(comp->indicatorColorHex);
        QObject::connect(indEdit, &QLineEdit::editingFinished, [ins, comp, indEdit]() {
            ins->notifyUiPropertyChanged(comp->id, "indicatorColorHex", "", indEdit->text());
        });
        lay->addRow("Indicator", indEdit);
    });

    registerBuilder("UiOscilloscope", [](const UiComponent* c, PropertiesInspector* ins, QFormLayout* lay) {
        const UiOscilloscope* comp = static_cast<const UiOscilloscope*>(c);
        QLineEdit* bgEdit = new QLineEdit(comp->bgColorHex);
        QObject::connect(bgEdit, &QLineEdit::editingFinished, [ins, comp, bgEdit]() {
            ins->notifyUiPropertyChanged(comp->id, "bgColorHex", "", bgEdit->text());
        });
        lay->addRow("Background", bgEdit);
        
        QLineEdit* lineEdit = new QLineEdit(comp->lineColorHex);
        QObject::connect(lineEdit, &QLineEdit::editingFinished, [ins, comp, lineEdit]() {
            ins->notifyUiPropertyChanged(comp->id, "lineColorHex", "", lineEdit->text());
        });
        lay->addRow("Line Color", lineEdit);
    });

    registerBuilder("UiKeyboard", [](const UiComponent* c, PropertiesInspector* ins, QFormLayout* lay) {
        const UiKeyboard* comp = static_cast<const UiKeyboard*>(c);
        QSpinBox* loBox = new QSpinBox(); loBox->setRange(0, 127); loBox->setValue(comp->loNote);
        QObject::connect(loBox, &QSpinBox::editingFinished, [ins, comp, loBox]() {
            ins->notifyUiPropertyChanged(comp->id, "loNote", 0, loBox->value());
        });
        lay->addRow("Low Note", loBox);
        
        QSpinBox* hiBox = new QSpinBox(); hiBox->setRange(0, 127); hiBox->setValue(comp->hiNote);
        QObject::connect(hiBox, &QSpinBox::editingFinished, [ins, comp, hiBox]() {
            ins->notifyUiPropertyChanged(comp->id, "hiNote", 0, hiBox->value());
        });
        lay->addRow("High Note", hiBox);
        
        QLineEdit* colorEdit = new QLineEdit(comp->colorHex);
        QObject::connect(colorEdit, &QLineEdit::editingFinished, [ins, comp, colorEdit]() {
            ins->notifyUiPropertyChanged(comp->id, "colorHex", "", colorEdit->text());
        });
        lay->addRow("Color", colorEdit);
    });

    registerBuilder("UiTab", [](const UiComponent* c, PropertiesInspector* ins, QFormLayout* lay) {
        const UiTab* comp = static_cast<const UiTab*>(c);
        QLineEdit* pgsEdit = new QLineEdit(comp->pages.join(", "));
        QObject::connect(pgsEdit, &QLineEdit::editingFinished, [ins, comp, pgsEdit]() {
            ins->notifyUiPropertyChanged(comp->id, "pages", "", pgsEdit->text());
        });
        lay->addRow("Pages (comma sep)", pgsEdit);
    });

    registerBuilder("UiButton", [](const UiComponent* c, PropertiesInspector* ins, QFormLayout* lay) {
        buildDataBindingSection(c, ins, lay);
        const UiButton* comp = static_cast<const UiButton*>(c);
        QLineEdit* imgOnEdit = new QLineEdit(comp->imagePathOn);
        QObject::connect(imgOnEdit, &QLineEdit::editingFinished, [ins, comp, imgOnEdit]() {
            ins->notifyUiPropertyChanged(comp->id, "imagePathOn", comp->imagePathOn, imgOnEdit->text());
        });
        lay->addRow("Image ON", imgOnEdit);
        
        QLineEdit* imgOffEdit = new QLineEdit(comp->imagePathOff);
        QObject::connect(imgOffEdit, &QLineEdit::editingFinished, [ins, comp, imgOffEdit]() {
            ins->notifyUiPropertyChanged(comp->id, "imagePathOff", comp->imagePathOff, imgOffEdit->text());
        });
        lay->addRow("Image OFF", imgOffEdit);
        
        QComboBox* modeCombo = new QComboBox();
        modeCombo->addItem("Trigger (Push)");
        modeCombo->addItem("Toggle (Switch)");
        modeCombo->setCurrentIndex(comp->isToggle ? 1 : 0);
        QObject::connect(modeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [ins, comp](int index) {
            ins->notifyUiPropertyChanged(comp->id, "isToggle", comp->isToggle, index == 1);
        });
        lay->addRow("Button Mode", modeCombo);
    });

    registerBuilder("UiLabel", [](const UiComponent* c, PropertiesInspector* ins, QFormLayout* lay) {
        const UiLabel* comp = static_cast<const UiLabel*>(c);
        QLineEdit* txtEdit = new QLineEdit(comp->text);
        QObject::connect(txtEdit, &QLineEdit::editingFinished, [ins, comp, txtEdit]() {
            ins->notifyUiPropertyChanged(comp->id, "text", comp->text, txtEdit->text());
        });
        lay->addRow("Text", txtEdit);
        
        QLineEdit* colEdit = new QLineEdit(comp->colorHex);
        QObject::connect(colEdit, &QLineEdit::editingFinished, [ins, comp, colEdit]() {
            ins->notifyUiPropertyChanged(comp->id, "colorHex", comp->colorHex, colEdit->text());
        });
        lay->addRow("Color (Hex)", colEdit);
    });
}

void UiPropertyForms::buildForm(const UiComponent* comp, PropertiesInspector* inspector, QFormLayout* layout) {
    if (!s_initialized) init();
    
    auto it = s_builders.find(comp->type);
    if (it != s_builders.end()) {
        it->second(comp, inspector, layout);
    }
}
