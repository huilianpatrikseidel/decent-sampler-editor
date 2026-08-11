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

        // Read-only summary. The editable controls (volume, pan, trigger, tags,
        // silencing, ADSR, filter, envelopes...) live in the "Group Settings" panel,
        // so they are not duplicated here — the inspector just reflects the selection.
        const QString kind = (sg->isSynthContainer || sg->isOscillator) ? "Synth Group" : "Sampler Group";
        lay->addRow("Type", new QLabel(kind));
        lay->addRow("Zones", new QLabel(QString::number(static_cast<int>(sg->zones.size()))));
        lay->addRow("Volume", new QLabel(QString::number(sg->volume, 'f', 1) + " dB"));
        lay->addRow("Pan", new QLabel(QString::number(sg->pan, 'f', 2)));
        lay->addRow("Trigger", new QLabel(sg->trigger));

        QLabel* hint = new QLabel("Edit parameters in the Group Settings panel below.");
        hint->setWordWrap(true);
        hint->setStyleSheet("color: #888888; font-style: italic;");
        lay->addRow(hint);
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
