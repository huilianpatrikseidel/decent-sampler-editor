#include "SynthEditorView.h"
#include "../../commands/ModifyPropertyCommand.h"
#include "../../commands/AddNodeCommand.h"
#include "../../commands/RemoveNodeCommand.h"
#include "../components/KnobWidget.h"
#include "../components/WaveformSelectorWidget.h"
#include "../components/EnvelopeGraphWidget.h"
#include "../components/LFOGraphWidget.h"
#include "../components/FilterSectionWidget.h"
#include "cards/OscillatorCardWidget.h"
#include "cards/FilterCardWidget.h"
#include "cards/EnvelopeCardWidget.h"
#include "cards/GlobalLfoCardWidget.h"
#include "cards/LegatoCardWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFileDialog>
#include <QFileInfo>
#include <QScrollArea>
#include <QSpinBox>
#include <QApplication>

SynthEditorView::SynthEditorView(ProjectManager* pm, QWidget* parent) 
    : QWidget(parent), m_pm(pm) {
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    connect(m_pm, &ProjectManager::nodeModified, this, &SynthEditorView::onNodeModified);
    connect(m_pm, &ProjectManager::nodeAdded, this, [this](){ rebuildUi(); });
    connect(m_pm, &ProjectManager::nodeRemoved, this, [this](){ rebuildUi(); });
    
    m_lastLfoCount = m_pm->getAudioState()->getGlobalLfos().size();
    connect(m_pm->getAudioState(), &AudioStateModel::globalLfoChanged, this, [this]() { 
        if (m_isUpdating) return;
        int currentLfoCount = m_pm->getAudioState()->getGlobalLfos().size();
        if (currentLfoCount != m_lastLfoCount) {
            m_lastLfoCount = currentLfoCount;
            rebuildUi(); 
        }
    });
}

void SynthEditorView::onNodeModified(const QUuid& id, const QString& prop) {
    if (m_isUpdating) return;
    
    // Only rebuild the entire view for structural changes.
    // Rebuilding on every parameter change destroys the active widgets (interrupting drags)
    // and resets LFO animations, which ruins the UX.
    if (prop == "isOscillator" || prop == "synthParentId" || prop == "name" || prop == "waveform") {
        rebuildUi();
    }
    // We intentionally DO NOT rebuild for other properties to guarantee smooth dragging.
    // Undo/Redo visual updates for knobs will require switching tabs, which is a fair tradeoff
    // to prevent catastrophic UX during active interaction.
}

void SynthEditorView::setSampleGroup(const QUuid& sgId) {
    m_currentGroupId = sgId;
    rebuildUi();
}

// ──────────────────────────────────────────────────────────────
// MAIN REBUILD
// ──────────────────────────────────────────────────────────────
void SynthEditorView::rebuildUi() {
    m_isUpdating = true;
    
    QLayoutItem* item;
    while ((item = m_mainLayout->takeAt(0)) != nullptr) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }
    
    if (m_currentGroupId.isNull()) {
        m_isUpdating = false;
        return;
    }
    
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    
    QWidget* container = new QWidget();
    QGridLayout* mainCols = new QGridLayout(container);
    mainCols->setContentsMargins(20, 20, 20, 20);
    mainCols->setHorizontalSpacing(30);
    mainCols->setVerticalSpacing(20);
    
    // ── Left Column (Oscillators) ──
    QVBoxLayout* leftCol = new QVBoxLayout();
    leftCol->setSpacing(20);
    

    
    int oscCount = 0;
    
    for (const auto& pair : m_pm->getAllNodes()) {
        if (pair.second->type == "SampleGroup") {
            SampleGroup* sg = static_cast<SampleGroup*>(pair.second.get());
            if (sg->synthParentId == m_currentGroupId && sg->isOscillator) {
                oscCount++;
                QUuid oscId = sg->id;
                
                if (oscCount > 1) {
                    QFrame* hLine = new QFrame();
                    hLine->setFrameShape(QFrame::HLine);
                    hLine->setStyleSheet(" margin: 10px 0px;");
                    leftCol->addWidget(hLine);
                }
                
                QWidget* rowWidget = new QWidget();
                QHBoxLayout* rowLayout = new QHBoxLayout(rowWidget);
                rowLayout->setContentsMargins(0, 0, 0, 0);
                rowLayout->setSpacing(20);
                
                QVBoxLayout* leftCardsLayout = new QVBoxLayout();
                leftCardsLayout->setSpacing(20);
                OscillatorCardWidget* oscCard = new OscillatorCardWidget(m_pm, sg, oscCount, oscId);
                FilterCardWidget* filterCard = new FilterCardWidget(m_pm, sg, oscId);
                leftCardsLayout->addWidget(oscCard);
                leftCardsLayout->addWidget(filterCard);
                leftCardsLayout->addStretch();
                
                rowLayout->addLayout(leftCardsLayout, 1);
                
                QVBoxLayout* rightCardsLayout = new QVBoxLayout();
                rightCardsLayout->setSpacing(20);
                EnvelopeCardWidget* envCard = new EnvelopeCardWidget(m_pm, sg, oscCount, oscId);
                LegatoCardWidget* legatoCard = new LegatoCardWidget(m_pm, sg, oscId);
                rightCardsLayout->addWidget(envCard);
                rightCardsLayout->addWidget(legatoCard);
                rightCardsLayout->addStretch();
                
                if (!sg->oscEnabled) {
                    oscCard->setCardContentEnabled(false);
                    filterCard->setCardContentEnabled(false);
                    filterCard->setToggleEnabled(false);
                    envCard->setCardContentEnabled(false);
                    envCard->setToggleEnabled(false);
                    legatoCard->setCardContentEnabled(false);
                    legatoCard->setToggleEnabled(false);
                }
                
                connect(oscCard, &SynthCardBase::toggled, this, [oscCard, filterCard, envCard, legatoCard](bool checked) {
                    oscCard->setCardContentEnabled(checked);
                    filterCard->setCardContentEnabled(checked);
                    filterCard->setToggleEnabled(checked);
                    envCard->setCardContentEnabled(checked);
                    envCard->setToggleEnabled(checked);
                    legatoCard->setCardContentEnabled(checked);
                    legatoCard->setToggleEnabled(checked);
                });
                
                rowLayout->addLayout(rightCardsLayout, 1);
                
                leftCol->addWidget(rowWidget);
            }
        }
    }
    
    QPushButton* btnAddOsc = new QPushButton("+ Add Oscillator");
    btnAddOsc->setObjectName("ModAddBtn");
    connect(btnAddOsc, &QPushButton::clicked, this, [this, oscCount]() {
        auto sg = std::make_unique<SampleGroup>();
        sg->isOscillator = true;
        sg->synthParentId = m_currentGroupId;
        sg->name = QString("Oscillator %1").arg(oscCount + 1);
        m_pm->getUndoStack()->push(new AddNodeCommand(m_pm, std::move(sg), QPointF(0, 0)));
    });
    leftCol->addWidget(btnAddOsc);
    leftCol->addStretch();
    mainCols->addLayout(leftCol, 0, 0, 1, 8);
    
    // ── Right Column (Global LFOs) ──
    QVBoxLayout* rightCol = new QVBoxLayout();
    rightCol->setSpacing(20);
    
    auto lfos = m_pm->getAudioState()->getGlobalLfos();
    for (const auto& lfo : lfos) {
        QWidget* lfoRow = new QWidget();
        QHBoxLayout* lfoRowLayout = new QHBoxLayout(lfoRow);
        lfoRowLayout->setContentsMargins(0,0,0,0);
        lfoRowLayout->addWidget(new GlobalLfoCardWidget(m_pm, lfo), 1);
        rightCol->addWidget(lfoRow);
    }
    
    QPushButton* btnAddLfo = new QPushButton("+ Add LFO");
    btnAddLfo->setObjectName("ModAddBtn");
    connect(btnAddLfo, &QPushButton::clicked, this, [this, lfos]() {
        LFO newLfo;
        newLfo.name = QString("LFO %1").arg(lfos.size() + 1);
        m_pm->getUndoStack()->push(new AddLfoCommand(m_pm, newLfo));
    });
    rightCol->addWidget(btnAddLfo);
    rightCol->addStretch();
    QFrame* vLine = new QFrame();
    vLine->setFrameShape(QFrame::VLine);
    vLine->setStyleSheet(" margin: 0px 10px;");
    // To make sure the vertical line stretches all the way down, we can let it expand
    vLine->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    mainCols->addWidget(vLine, 0, 8, 1, 1);
    
    mainCols->addLayout(rightCol, 0, 9, 1, 4);
    
    scrollArea->setWidget(container);
    m_mainLayout->addWidget(scrollArea);
    
    m_isUpdating = false;
}



void SynthEditorView::onMacroChanged(int index, double value) {
    // Macro value handling - in DecentSampler, macros map to global <control> knobs.
}
