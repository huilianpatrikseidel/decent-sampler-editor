#include "NodePropertyForms.h"
#include "PropertiesInspector.h"
#include "core/models/AudioNodes.h"
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QComboBox>
#include <QLabel>
#include "AdsrEditorView.h"

std::map<QString, NodePropertyForms::BuilderFunc> NodePropertyForms::s_builders;
bool NodePropertyForms::s_initialized = false;

void NodePropertyForms::registerBuilder(const QString& type, BuilderFunc builder) {
    s_builders[type] = builder;
}

void NodePropertyForms::init() {
    if (s_initialized) return;
    s_initialized = true;
    
    registerBuilder("SampleGroup", [](const Node* n, const QUuid& id, PropertiesInspector* ins, QFormLayout* lay) {
        const SampleGroup* sg = static_cast<const SampleGroup*>(n);
        
        QSlider* volumeSlider = new QSlider(Qt::Horizontal);
        volumeSlider->setRange(-60, 12);
        volumeSlider->setValue(sg->volume);
        QObject::connect(volumeSlider, &QSlider::sliderReleased, [ins, id, sg, volumeSlider]() {
            ins->notifyPropertyChanged(id, "volume", sg->volume, volumeSlider->value());
        });
        lay->addRow("Volume (dB)", volumeSlider);
        
        QSlider* panSlider = new QSlider(Qt::Horizontal);
        panSlider->setRange(-100, 100);
        panSlider->setValue(sg->pan * 100.0);
        QObject::connect(panSlider, &QSlider::sliderReleased, [ins, id, sg, panSlider]() {
            ins->notifyPropertyChanged(id, "pan", sg->pan, panSlider->value() / 100.0);
        });
        lay->addRow("Pan", panSlider);
        
        QComboBox* triggerCombo = new QComboBox();
        triggerCombo->addItems({"attack", "release", "first", "legato"});
        triggerCombo->setCurrentText(sg->trigger);
        QObject::connect(triggerCombo, &QComboBox::currentTextChanged, [ins, id, sg](const QString& text) {
            ins->notifyPropertyChanged(id, "trigger", sg->trigger, text);
        });
        lay->addRow("Trigger Mode", triggerCombo);
        
        QLineEdit* customTagsEdit = new QLineEdit(sg->customTags);
        customTagsEdit->setPlaceholderText("e.g. open_hihat");
        QObject::connect(customTagsEdit, &QLineEdit::editingFinished, [ins, id, sg, customTagsEdit]() {
            ins->notifyPropertyChanged(id, "customTags", sg->customTags, customTagsEdit->text());
        });
        lay->addRow("Custom Tags", customTagsEdit);
        
        QLineEdit* silencedByEdit = new QLineEdit(sg->silencedByTags);
        silencedByEdit->setPlaceholderText("e.g. open_hihat");
        QObject::connect(silencedByEdit, &QLineEdit::editingFinished, [ins, id, sg, silencedByEdit]() {
            ins->notifyPropertyChanged(id, "silencedByTags", sg->silencedByTags, silencedByEdit->text());
        });
        lay->addRow("Silenced By Tags", silencedByEdit);
        
        QComboBox* silencingModeCombo = new QComboBox();
        silencingModeCombo->addItems({"fast", "normal"});
        silencingModeCombo->setCurrentText(sg->silencingMode);
        QObject::connect(silencingModeCombo, &QComboBox::currentTextChanged, [ins, id, sg](const QString& text) {
            ins->notifyPropertyChanged(id, "silencingMode", sg->silencingMode, text);
        });
        lay->addRow("Silencing Mode", silencingModeCombo);
        
        lay->addRow(new QLabel("<b>ADSR Envelope</b>"));
        AdsrEditorView* adsrView = new AdsrEditorView();
        adsrView->setAdsr(sg->ampEnv, sg->id);
        QObject::connect(adsrView, &AdsrEditorView::adsrChanged, [ins, id, sg](const QUuid& uid, const ADSR& newAdsr) {
            ins->notifyPropertyChanged(id, "ampEnv", sg->ampEnv.toJson(), newAdsr.toJson());
        });
        lay->addRow(adsrView);
    });

    registerBuilder("Bus", [](const Node* n, const QUuid& id, PropertiesInspector* ins, QFormLayout* lay) {
        const BusNode* bus = static_cast<const BusNode*>(n);
        QSlider* volSlider = new QSlider(Qt::Horizontal);
        volSlider->setRange(0, 100);
        volSlider->setValue(bus->volume * 100);
        QObject::connect(volSlider, &QSlider::valueChanged, [ins, id, bus](int val) {
            ins->notifyPropertyChanged(id, "volume", bus->volume, val / 100.0);
        });
        lay->addRow("Volume", volSlider);
    });

    registerBuilder("Delay", [](const Node* n, const QUuid& id, PropertiesInspector* ins, QFormLayout* lay) {
        const DelayNode* d = static_cast<const DelayNode*>(n);
        QSlider* timeSlider = new QSlider(Qt::Horizontal); timeSlider->setRange(0, 100); timeSlider->setValue(d->time * 100);
        QObject::connect(timeSlider, &QSlider::valueChanged, [ins, id, d](int val){ ins->notifyPropertyChanged(id, "time", d->time, val / 100.0); });
        lay->addRow("Time", timeSlider);
        QSlider* fbkSlider = new QSlider(Qt::Horizontal); fbkSlider->setRange(0, 100); fbkSlider->setValue(d->feedback * 100);
        QObject::connect(fbkSlider, &QSlider::valueChanged, [ins, id, d](int val){ ins->notifyPropertyChanged(id, "feedback", d->feedback, val / 100.0); });
        lay->addRow("Feedback", fbkSlider);
    });

    registerBuilder("Reverb", [](const Node* n, const QUuid& id, PropertiesInspector* ins, QFormLayout* lay) {
        const ReverbNode* r = static_cast<const ReverbNode*>(n);
        QSlider* sizeSlider = new QSlider(Qt::Horizontal); sizeSlider->setRange(0, 100); sizeSlider->setValue(r->size * 100);
        QObject::connect(sizeSlider, &QSlider::valueChanged, [ins, id, r](int val){ ins->notifyPropertyChanged(id, "size", r->size, val / 100.0); });
        lay->addRow("Size", sizeSlider);
        QSlider* dampSlider = new QSlider(Qt::Horizontal); dampSlider->setRange(0, 100); dampSlider->setValue(r->damp * 100);
        QObject::connect(dampSlider, &QSlider::valueChanged, [ins, id, r](int val){ ins->notifyPropertyChanged(id, "damp", r->damp, val / 100.0); });
        lay->addRow("Damp", dampSlider);
        
        QLineEdit* irEdit = new QLineEdit(r->irPath);
        irEdit->setPlaceholderText("Convolution IR (.wav)");
        QObject::connect(irEdit, &QLineEdit::editingFinished, [ins, id, r, irEdit]() {
            ins->notifyPropertyChanged(id, "irPath", r->irPath, irEdit->text());
        });
        lay->addRow("IR Path", irEdit);
    });

    registerBuilder("Filter", [](const Node* n, const QUuid& id, PropertiesInspector* ins, QFormLayout* lay) {
        const FilterNode* f = static_cast<const FilterNode*>(n);
        QSlider* cutoffSlider = new QSlider(Qt::Horizontal); cutoffSlider->setRange(0, 20000); cutoffSlider->setValue(f->cutoff);
        QObject::connect(cutoffSlider, &QSlider::valueChanged, [ins, id, f](int val){ ins->notifyPropertyChanged(id, "cutoff", f->cutoff, static_cast<double>(val)); });
        lay->addRow("Cutoff", cutoffSlider);
    });
}

void NodePropertyForms::buildForm(const Node* node, const QUuid& nodeId, PropertiesInspector* inspector, QFormLayout* layout) {
    if (!s_initialized) init();
    
    auto it = s_builders.find(node->type);
    if (it != s_builders.end()) {
        it->second(node, nodeId, inspector, layout);
    }
}
