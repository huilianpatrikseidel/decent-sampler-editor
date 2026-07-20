#include "LegatoCardWidget.h"
#include "../../../core/models/nodes/SampleGroupNode.h"
#include "../../components/SynthKnobWidget.h"
#include "../../components/ModSourceHelper.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include "../../../commands/ModifyPropertyCommand.h"
#include "../../../commands/RemoveNodeCommand.h"

LegatoCardWidget::LegatoCardWidget(ProjectManager* pm, SampleGroup* sg, const QUuid& oscId, QWidget* parent)
    : SynthCardBase("Legato & Glide", [pm, oscId](){ /* Optional remove lambda, not needed here usually */ }, parent), m_pm(pm), m_sg(sg), m_oscId(oscId)
{
    QVBoxLayout* layout = contentLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    
    setEnableToggle(true, sg->legatoEnabled);
    connect(this, &SynthCardBase::toggled, this, [this](bool checked) {
        QJsonObject oldJson = m_sg->toJson();
        QJsonObject newJson = oldJson;
        newJson["legatoEnabled"] = checked;
        m_pm->getUndoStack()->push(new ModifyPropertyCommand(m_pm, m_oscId, "legatoEnabled", oldJson, newJson));
        
        m_legatoBtn->setText(checked ? "LEGATO" : "MONO");
        m_legatoBtn->setChecked(checked);
    });
    
    // Create an inner container like the other cards
    QWidget* block = new QWidget();
    block->setObjectName("NeumorphicControls");
    block->setAttribute(Qt::WA_StyledBackground, true);
    block->setFixedHeight(85);
    
    QHBoxLayout* blockLayout = new QHBoxLayout(block);
    blockLayout->setContentsMargins(0, 10, 0, 10);
    blockLayout->setSpacing(30);
    blockLayout->setAlignment(Qt::AlignCenter);
    
    // Legato Button
    QVBoxLayout* btnLayout = new QVBoxLayout();
    btnLayout->setSpacing(4);
    QLabel* legatoLabel = new QLabel("Mode");
    legatoLabel->setStyleSheet("font-size: 12px;  font-weight: 600; background: transparent;");
    btnLayout->addWidget(legatoLabel, 0, Qt::AlignCenter);
    
    m_legatoBtn = new QPushButton(sg->legatoEnabled ? "LEGATO" : "MONO");
    m_legatoBtn->setObjectName("NeumorphicToggle");
    m_legatoBtn->setCheckable(true);
    m_legatoBtn->setChecked(sg->legatoEnabled);
    m_legatoBtn->setFixedSize(60, 36);
    
    m_legatoBtn->setStyleSheet(
        "QPushButton {"
        "   border: 1px solid transparent; background-"
        "}"
        "QPushButton:checked {"
        "   font-weight: bold; border: 1px solid #f60; background-"
        "}"
        "QPushButton:disabled {"
        "   background-"
        "}"
        "QPushButton:checked:disabled {"
        "  color: rgba(255, 102, 0, 80); border: 1px solid rgba(255, 102, 0, 80); background-"
        "}"
    );
    
    btnLayout->addWidget(m_legatoBtn, 0, Qt::AlignCenter);
    blockLayout->addLayout(btnLayout);
    
    // Glide Time Knob
    QVBoxLayout* glideLayout = new QVBoxLayout();
    glideLayout->setSpacing(2);
    QLabel* glideLabel = new QLabel("Glide");
    glideLabel->setStyleSheet("font-size: 12px;  font-weight: 600; background: transparent;");
    glideLayout->addWidget(glideLabel, 0, Qt::AlignCenter);
    
    m_glideKnob = new SynthKnobWidget();
    m_glideKnob->setFixedSize(36, 36);
    m_glideKnob->setMinimum(0.0);
    m_glideKnob->setMaximum(1.0);
    m_glideKnob->setDefaultValue(0.0);
    m_glideKnob->setValue(sg->glideTime);
    
    auto provider = ModSourceHelper::createProvider(pm);
    m_glideKnob->setModSourceProvider(provider);
    
    glideLayout->addWidget(m_glideKnob, 0, Qt::AlignCenter);
    
    QLabel* glideVal = new QLabel(QString::number(sg->glideTime, 'f', 2) + "s");
    glideVal->setStyleSheet("font-size: 10px;  font-family: Consolas, monospace; background: transparent;");
    glideVal->setAlignment(Qt::AlignCenter);
    glideLayout->addWidget(glideVal, 0, Qt::AlignCenter);
    
    blockLayout->addLayout(glideLayout);
    
    layout->addWidget(block);
    layout->addStretch();
    
    // Connections
    connect(m_legatoBtn, &QPushButton::toggled, this, [this](bool checked) {
        m_legatoBtn->setText(checked ? "LEGATO" : "MONO");
        
        QJsonObject oldJson = m_sg->toJson();
        QJsonObject newJson = oldJson;
        newJson["legatoEnabled"] = checked;
        m_pm->getUndoStack()->push(new ModifyPropertyCommand(m_pm, m_oscId, "legatoEnabled", oldJson, newJson));
    });
    
    connect(m_glideKnob, &SynthKnobWidget::valueChanged, this, [this, glideVal](double v) {
        glideVal->setText(QString::number(v, 'f', 2) + "s");
        
        QJsonObject oldJson = m_sg->toJson();
        QJsonObject newJson = oldJson;
        newJson["glideTime"] = v;
        m_pm->getUndoStack()->push(new ModifyPropertyCommand(m_pm, m_oscId, "glideTime", oldJson, newJson));
    });
}
