#include "GlobalLfoCardWidget.h"
#include "../../../commands/ModifyPropertyCommand.h"
#include "../../../commands/RemoveNodeCommand.h"
#include "../../components/SynthKnobWidget.h"
#include "../../components/LFOGraphWidget.h"
#include "../../components/ModSourceHelper.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QButtonGroup>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QtMath>

GlobalLfoCardWidget::GlobalLfoCardWidget(ProjectManager* pm, const LFO& lfoData, QWidget* parent)
    : SynthCardBase(lfoData.name, [pm, lfoData]() { pm->getUndoStack()->push(new RemoveLfoCommand(pm, lfoData)); }, parent),
      m_pm(pm) {
      
    QString initShape = lfoData.shape;
    if (initShape.length() > 0) initShape[0] = initShape[0].toUpper();
    if (initShape.isEmpty()) initShape = "Sine";
    setTitle(QString("%1 - %2").arg(lfoData.name).arg(initShape));
    
    setEnableToggle(true, lfoData.enabled);
    connect(this, &SynthCardBase::toggled, this, [this, lfoId = lfoData.id](bool checked) {
        if (m_isUpdating) return;
        saveLfoParams(lfoId, "enabled", checked);
    });
    
    QVBoxLayout* lfoLayout = contentLayout();
    QUuid lfoId = lfoData.id;
    
    QHBoxLayout* shapeLayout = new QHBoxLayout();
    shapeLayout->setContentsMargins(0, 0, 0, 0);
    shapeLayout->setSpacing(4);
    
    QButtonGroup* shapeGroup = new QButtonGroup(this);
    QStringList shapes = {"sine", "triangle", "square", "saw"};
    QStringList shapeLabels = {"SIN", "TRI", "SQR", "SAW"};
    
    shapeLayout->addStretch(); // Center buttons
    
    for (int i = 0; i < shapes.size(); ++i) {
        QPushButton* btn = new QPushButton();
        btn->setObjectName("NeumorphicToggle");
        btn->setCheckable(true);
        btn->setFixedSize(44, 36);

        QString shape = shapes[i];
        auto createPix = [shape](bool checked) {
            int w = 24, h = 24;
            QPixmap pix(w, h);
            pix.fill(Qt::transparent);
            QPainter p(&pix);
            p.setRenderHint(QPainter::Antialiasing);
            QPen pen(checked ? QColor("#ff6600") : QColor("#888888"), 1.5);
            p.setPen(pen);
            
            QPainterPath path;
            const double midY = h / 2.0;
            const double amp = (h - 4) / 2.0;
            
            auto sample = [](const QString& t, double phase) {
                double frac = phase - qFloor(phase);
                if (t == "sine") return qSin(frac * 2.0 * M_PI);
                if (t == "saw") return 1.0 - 2.0 * frac;
                if (t == "square") return (frac < 0.5) ? 1.0 : -1.0;
                if (t == "triangle") {
                    if (frac < 0.25) return 4.0 * frac;
                    else if (frac < 0.75) return 2.0 - 4.0 * frac;
                    else return -4.0 + 4.0 * frac;
                }
                return 0.0;
            };
            
            for (int x = 0; x <= w; ++x) {
                double phase = (static_cast<double>(x) / w) * 1.5;
                double s = sample(shape, phase);
                double y = midY - s * amp;
                if (x == 0) path.moveTo(x, y); else path.lineTo(x, y);
            }
            p.drawPath(path);
            return pix;
        };
        
        QIcon icon;
        icon.addPixmap(createPix(false), QIcon::Normal, QIcon::Off);
        icon.addPixmap(createPix(true), QIcon::Normal, QIcon::On);
        btn->setIcon(icon);
        btn->setIconSize(QSize(16, 16));

        btn->setChecked(lfoData.shape.toLower() == shapes[i]);
        shapeGroup->addButton(btn, i);
        shapeLayout->addWidget(btn);
    }
    shapeLayout->addStretch(); // Center buttons
    
    QVBoxLayout* lfoInner = new QVBoxLayout();
    lfoInner->setContentsMargins(0, 0, 0, 0);
    lfoInner->setSpacing(6);
    lfoInner->addLayout(shapeLayout);
    
    LFOGraphWidget* lfoGraph = new LFOGraphWidget();
    lfoGraph->setParameters(lfoData.shape, lfoData.frequency, 0.0);
    lfoInner->addWidget(lfoGraph);
    
    lfoLayout->addLayout(lfoInner);
    
    QWidget* controlsBlock = new QWidget();
    controlsBlock->setObjectName("NeumorphicControls");
    controlsBlock->setAttribute(Qt::WA_StyledBackground, true);
    controlsBlock->setFixedHeight(85);
    QHBoxLayout* lfoControls = new QHBoxLayout(controlsBlock);
    lfoControls->setContentsMargins(0, 10, 0, 10);
    lfoControls->setSpacing(16);
    
    lfoControls->addStretch();
    
    connect(shapeGroup, QOverload<int>::of(&QButtonGroup::idClicked), this, [this, lfoId, shapes, lfoData](int id) {
        if (m_isUpdating) return;
        QString shape = shapes[id];
        saveLfoParams(lfoId, "shape", shape);
        
        QString shapeName = shape;
        if (shapeName.length() > 0) shapeName[0] = shapeName[0].toUpper();
        setTitle(QString("%1 - %2").arg(lfoData.name).arg(shapeName));
    });
    
    auto addLfoKnob = [&](const QString& label, double min, double max, double val) -> SynthKnobWidget* {
        QVBoxLayout* kl = new QVBoxLayout();
        kl->setSpacing(2);
        QLabel* nl = new QLabel(label);
        nl->setStyleSheet("font-size: 12px;  font-weight: 600;");
        kl->addWidget(nl, 0, Qt::AlignCenter);
        
        auto provider = ModSourceHelper::createProvider(pm);
        
        SynthKnobWidget* knob = new SynthKnobWidget();
        knob->setModSourceProvider(provider);
        knob->setFixedSize(36, 36);
        knob->setMinimum(min); knob->setMaximum(max);
        
        double def = 0.0;
        if (label == "Rate") def = 1.0;
        else if (label == "Depth") def = 1.0;
        knob->setDefaultValue(def);
        
        knob->setValue(val);
        kl->addWidget(knob, 0, Qt::AlignCenter);
        
        QLabel* valLabel = new QLabel(QString::number(val, 'f', 2));
        valLabel->setStyleSheet("font-size: 10px;  font-family: Consolas, monospace;");
        valLabel->setAlignment(Qt::AlignCenter);
        kl->addWidget(valLabel, 0, Qt::AlignCenter);
        
        connect(knob, &SynthKnobWidget::valueChanged, valLabel, [valLabel](double v) {
            valLabel->setText(QString::number(v, 'f', 2));
        });
        
        lfoControls->addLayout(kl);
        return knob;
    };
    
    SynthKnobWidget* rateKnob = addLfoKnob("Rate", 0.1, 20.0, lfoData.frequency);
    SynthKnobWidget* depthKnob = addLfoKnob("Depth", 0.0, 1.0, lfoData.amount);
    SynthKnobWidget* phaseKnob = addLfoKnob("Phase", 0.0, 1.0, 0.0);
    
    lfoControls->addStretch();
    
    lfoLayout->addWidget(controlsBlock);
    
    connect(rateKnob, &SynthKnobWidget::valueChanged, this, [lfoGraph, phaseKnob, shapeGroup, shapes](double v) {
        int shapeId = shapeGroup->checkedId();
        QString shape = (shapeId >= 0 && shapeId < shapes.size()) ? shapes[shapeId] : "sine";
        lfoGraph->setParameters(shape, v, phaseKnob->value());
    });
    connect(phaseKnob, &SynthKnobWidget::valueChanged, this, [lfoGraph, rateKnob, shapeGroup, shapes](double v) {
        int shapeId = shapeGroup->checkedId();
        QString shape = (shapeId >= 0 && shapeId < shapes.size()) ? shapes[shapeId] : "sine";
        lfoGraph->setParameters(shape, rateKnob->value(), v);
    });
    connect(shapeGroup, QOverload<int>::of(&QButtonGroup::idClicked), this, [lfoGraph, rateKnob, phaseKnob, shapes](int id) {
        QString shape = (id >= 0 && id < shapes.size()) ? shapes[id] : "sine";
        lfoGraph->setParameters(shape, rateKnob->value(), phaseKnob->value());
    });
    
    connect(rateKnob, &SynthKnobWidget::valueChanged, this, [this, lfoId](double v) {
        if (m_isUpdating) return;
        saveLfoParams(lfoId, "frequency", v);
    });
    connect(depthKnob, &SynthKnobWidget::valueChanged, this, [this, lfoId](double v) {
        if (m_isUpdating) return;
        saveLfoParams(lfoId, "amount", v);
    });
    
    m_isUpdating = false;
}

void GlobalLfoCardWidget::saveLfoParams(const QUuid& lfoId, const QString& key, const QVariant& value) {
    auto lfos = m_pm->getAudioState()->getGlobalLfos();
    for (int i = 0; i < lfos.size(); ++i) {
        if (lfos[i].id == lfoId) {
            LFO lfo = lfos[i];
            if (key == "shape") lfo.shape = value.toString();
            else if (key == "frequency") lfo.frequency = value.toDouble();
            else if (key == "amount") lfo.amount = value.toDouble();
            else if (key == "scope") lfo.scope = value.toString();
            m_pm->getUndoStack()->push(new ModifyLfoCommand(m_pm, lfoId, lfos[i], lfo));
            return;
        }
    }
}
