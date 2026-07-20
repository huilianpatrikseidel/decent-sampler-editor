#include "NoteSequenceEditorView.h"
#include <QLabel>
#include <QMessageBox>
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QDebug>
#include <algorithm>
#include <QIcon>
#include <QPixmap>
#include "../../audio/AudioEngine.h"
#include "../../audio/AudioMessage.h"

#include "SequenceGraphWidget.h"
#include "../../audio/PlaybackSequencer.h"
#include "../../core/ThemePalette.h"

NoteSequenceEditorView::NoteSequenceEditorView(ApplicationController* appCtrl, QWidget* parent) 
    : QWidget(parent), m_appCtrl(appCtrl), m_pm(appCtrl->getProjectManager()) {
    
    connect(m_appCtrl->getPlaybackSequencer(), &PlaybackSequencer::stepChanged, this, [this](int step) {
        m_graphWidget->setActiveStep(step);
    });
    
    buildUi();
    updateUi();
    connect(m_pm, &ProjectManager::projectLoaded, this, &NoteSequenceEditorView::updateUi);
    connect(m_pm->getAudioState(), &AudioStateModel::audioStateModified, this, [this](){
        if (!m_spinBpm->hasFocus()) {
            m_spinBpm->setValue(m_pm->getAudioState()->getEditorBpm());
        }
    });
}

NoteSequenceEditorView::~NoteSequenceEditorView() {
    if (m_appCtrl->getPlaybackSequencer()->isPlaying()) {
        m_appCtrl->getPlaybackSequencer()->stop();
    }
}

void NoteSequenceEditorView::buildUi() {
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(10);
    
    QHBoxLayout* topBar = new QHBoxLayout();
    topBar->addWidget(new QLabel("Sequence:"));
    m_seqCombo = new QComboBox();
    m_seqCombo->setFixedWidth(200);
    topBar->addWidget(m_seqCombo);
    
    m_btnAddSeq = new QPushButton("+ New Sequence");
    topBar->addWidget(m_btnAddSeq);
    
    topBar->addSpacing(20);
    
    // Play/Stop
    m_btnPlay = new QPushButton("⏵ Play");
    m_btnPlay->setFixedWidth(80);
    m_btnPlay->setStyleSheet("background-color: #33aa33; color: white; font-weight: bold; border-radius: 4px; padding: 5px;");
    topBar->addWidget(m_btnPlay);
    
    topBar->addSpacing(10);
    
    // Target Group
    topBar->addWidget(new QLabel("Target:"));
    m_comboTargetGroup = new QComboBox();
    m_comboTargetGroup->setIconSize(QSize(55, 16));
    topBar->addWidget(m_comboTargetGroup);
    
    // BPM
    topBar->addWidget(new QLabel("BPM:"));
    m_spinBpm = new QDoubleSpinBox();
    m_spinBpm->setRange(20, 300);
    m_spinBpm->setValue(m_pm->getAudioState()->getEditorBpm());
    topBar->addWidget(m_spinBpm);
    
    // Time Sig
    topBar->addWidget(new QLabel("Time Sig:"));
    m_spinTsNum = new QSpinBox(); m_spinTsNum->setRange(1, 16); m_spinTsNum->setValue(4);
    topBar->addWidget(m_spinTsNum);
    topBar->addWidget(new QLabel("/"));
    m_comboTsDen = new QComboBox(); 
    m_comboTsDen->addItems({"1", "2", "4", "8", "16", "32"});
    m_comboTsDen->setCurrentText("4");
    topBar->addWidget(m_comboTsDen);
    
    // Rate
    topBar->addWidget(new QLabel("Rate:"));
    m_comboRate = new QComboBox();
    m_comboRate->addItems({"1/1 (Whole)", "1/2 (Half)", "1/4 (Quarter)", "1/8 (Eighth)", "1/16 (Sixteenth)"});
    m_comboRate->setCurrentIndex(2); // Quarter default
    topBar->addWidget(m_comboRate);
    
    topBar->addStretch();
    layout->addLayout(topBar);
    
    m_graphWidget = new SequenceGraphWidget(this);
    layout->addWidget(m_graphWidget, 1); // 1 = stretch factor
    
    QHBoxLayout* bottomBar = new QHBoxLayout();
    m_btnAddStep = new QPushButton("+ Add Step");
    m_btnRemoveStep = new QPushButton("- Remove Step");
    
    QLabel* lblHelp = new QLabel("<b>Controls:</b> Click/Drag: <i>Velocity</i> | Shift+Drag: <i>Pitch</i> | Ctrl+Drag: <i>Length</i> | Scroll: <i>Pitch</i>");
    lblHelp->setStyleSheet("color: #888888; font-size: 11px;");
    lblHelp->setAlignment(Qt::AlignCenter);
    
    m_btnSave = new QPushButton("Save Changes");
    
    bottomBar->addWidget(m_btnAddStep);
    bottomBar->addWidget(m_btnRemoveStep);
    bottomBar->addWidget(lblHelp, 1); // stretch
    bottomBar->addWidget(m_btnSave);
    layout->addLayout(bottomBar);
    
    connect(m_btnAddSeq, &QPushButton::clicked, this, [this]() {
        QVector<NoteSequence> seqs = m_pm->getAudioState()->getNoteSequences();
        NoteSequence s;
        s.name = QString("Seq %1").arg(seqs.size() + 1);
        seqs.append(s);
        m_pm->getAudioState()->setNoteSequences(seqs);
        updateUi();
        m_seqCombo->setCurrentIndex(seqs.size() - 1);
    });
    
    connect(m_seqCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &NoteSequenceEditorView::sequenceSelected);
    
    connect(m_btnAddStep, &QPushButton::clicked, this, &NoteSequenceEditorView::addStep);
    connect(m_btnRemoveStep, &QPushButton::clicked, this, &NoteSequenceEditorView::removeStep);
    connect(m_btnSave, &QPushButton::clicked, this, &NoteSequenceEditorView::saveChanges);
    connect(m_graphWidget, &SequenceGraphWidget::stepsModified, this, &NoteSequenceEditorView::saveChanges);
    connect(m_btnPlay, &QPushButton::clicked, this, &NoteSequenceEditorView::togglePlayback);
    
    connect(m_spinBpm, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double v){
        m_pm->getAudioState()->setEditorBpm(v);
        if (m_appCtrl->getPlaybackSequencer()->isPlaying()) togglePlayback(); // restart to apply new rate
    });
    
    auto tsChanged = [this](int){ saveChanges(); };
    connect(m_spinTsNum, QOverload<int>::of(&QSpinBox::valueChanged), this, tsChanged);
    connect(m_comboTsDen, QOverload<int>::of(&QComboBox::currentIndexChanged), this, tsChanged);
    connect(m_comboRate, QOverload<int>::of(&QComboBox::currentIndexChanged), this, tsChanged);
}

void NoteSequenceEditorView::updateUi() {
    m_seqCombo->blockSignals(true);
    m_seqCombo->clear();
    const QVector<NoteSequence>& seqs = m_pm->getAudioState()->getNoteSequences();
    for (const auto& seq : seqs) {
        m_seqCombo->addItem(seq.name);
    }
    m_seqCombo->blockSignals(false);
    
    // Update target groups
    QString currentTarget = m_comboTargetGroup->currentText();
    m_comboTargetGroup->blockSignals(true);
    m_comboTargetGroup->clear();
    for (const auto& pair : m_pm->getAllNodes()) {
        if (pair.second->type == "SampleGroup") {
            SampleGroup* sg = static_cast<SampleGroup*>(pair.second.get());
            if (sg->isOscillator) continue; // Exclude oscillators from global Target dropdown
            
            QPixmap pix(55, 16);
            pix.fill(Qt::transparent);
            QPainter p(&pix);
            p.setRenderHint(QPainter::Antialiasing);
            p.setBrush(sg->isSynthContainer ? ThemePalette::color("qss_color_26", QColor(50, 220, 100)) : ThemePalette::color("qss_color_6", QColor(255, 150, 50)));
            p.setPen(Qt::NoPen);
            p.drawRoundedRect(pix.rect(), 3, 3);
            p.setPen(ThemePalette::color("qss_color_22", QColor(25, 25, 25))); // Dark text for contrast
            QFont f = p.font();
            f.setPointSize(7);
            f.setBold(true);
            p.setFont(f);
            p.drawText(pix.rect(), Qt::AlignCenter, sg->isSynthContainer ? "SYNTH" : "SAMPLER");
            p.end();
            QIcon icon(pix);
            
            QString displayName = sg->name;
            if (displayName.isEmpty()) displayName = "Group";
            m_comboTargetGroup->addItem(icon, displayName, sg->id.toString());
        }
    }
    int tIdx = m_comboTargetGroup->findText(currentTarget);
    if (tIdx >= 0) m_comboTargetGroup->setCurrentIndex(tIdx);
    else if (m_comboTargetGroup->count() > 0) m_comboTargetGroup->setCurrentIndex(0);
    m_comboTargetGroup->blockSignals(false);
    
    if (m_currentIndex >= 0 && m_currentIndex < seqs.size()) {
        m_seqCombo->setCurrentIndex(m_currentIndex);
    } else if (!seqs.isEmpty()) {
        m_seqCombo->setCurrentIndex(0);
        sequenceSelected(0);
    } else {
        populateSteps();
        m_currentIndex = -1;
    }
}

void NoteSequenceEditorView::sequenceSelected(int index) {
    m_currentIndex = index;
    populateSteps();
}

void NoteSequenceEditorView::populateSteps() {
    m_isPopulating = true;
    
    if (m_currentIndex < 0) {
        m_graphWidget->setSteps({});
        m_isPopulating = false;
        return;
    }
    
    const QVector<NoteSequence>& seqs = m_pm->getAudioState()->getNoteSequences();
    if (m_currentIndex >= seqs.size()) {
        m_graphWidget->setSteps({});
        m_isPopulating = false;
        return;
    }
    
    const NoteSequence& seq = seqs[m_currentIndex];
    m_graphWidget->setSteps(seq.steps);
    
    m_spinTsNum->blockSignals(true);
    m_comboTsDen->blockSignals(true);
    m_comboRate->blockSignals(true);
    
    m_spinTsNum->setValue(seq.timeSignatureNumerator);
    m_comboTsDen->setCurrentText(QString::number(seq.timeSignatureDenominator));
    
    // Map rate to combo index (4.0=0, 2.0=1, 1.0=2, 0.5=3, 0.25=4)
    if (seq.rate >= 4.0) m_comboRate->setCurrentIndex(0);
    else if (seq.rate >= 2.0) m_comboRate->setCurrentIndex(1);
    else if (seq.rate >= 1.0) m_comboRate->setCurrentIndex(2);
    else if (seq.rate >= 0.5) m_comboRate->setCurrentIndex(3);
    else m_comboRate->setCurrentIndex(4);
    
    m_spinTsNum->blockSignals(false);
    m_comboTsDen->blockSignals(false);
    m_comboRate->blockSignals(false);
    
    m_isPopulating = false;
}

void NoteSequenceEditorView::addStep() {
    if (m_currentIndex < 0) return;
    
    QVector<NoteSequence> seqs = m_pm->getAudioState()->getNoteSequences();
    NoteSequence& seq = seqs[m_currentIndex];
    
    SequenceStep newStep;
    newStep.position = seq.steps.size();
    newStep.velocity = 1.0;
    newStep.note = 60;
    newStep.length = 1.0;
    
    seq.steps.append(newStep);
    m_pm->getAudioState()->setNoteSequences(seqs);
    populateSteps();
}

void NoteSequenceEditorView::removeStep() {
    if (m_currentIndex < 0) return;
    
    QVector<NoteSequence> seqs = m_pm->getAudioState()->getNoteSequences();
    NoteSequence& seq = seqs[m_currentIndex];
    
    if (!seq.steps.isEmpty()) {
        seq.steps.removeLast();
        m_pm->getAudioState()->setNoteSequences(seqs);
        populateSteps();
    }
}

void NoteSequenceEditorView::saveChanges() {
    if (m_currentIndex < 0 || m_isPopulating) return;
    QVector<NoteSequence> seqs = m_pm->getAudioState()->getNoteSequences();
    if (m_currentIndex >= seqs.size()) return;
    
    seqs[m_currentIndex].steps = m_graphWidget->getSteps();
    seqs[m_currentIndex].timeSignatureNumerator = m_spinTsNum->value();
    seqs[m_currentIndex].timeSignatureDenominator = m_comboTsDen->currentText().toInt();
    
    // Map combo index to rate (0=4.0, 1=2.0, 2=1.0, 3=0.5, 4=0.25)
    double rate = 1.0;
    switch(m_comboRate->currentIndex()) {
        case 0: rate = 4.0; break;
        case 1: rate = 2.0; break;
        case 2: rate = 1.0; break;
        case 3: rate = 0.5; break;
        case 4: rate = 0.25; break;
    }
    seqs[m_currentIndex].rate = rate;
    
    // Disable saving layout to project file constantly on drag, but update model
    m_pm->getAudioState()->setNoteSequences(seqs);
}

void NoteSequenceEditorView::togglePlayback() {
    auto seq = m_appCtrl->getPlaybackSequencer();
    if (seq->isPlaying()) {
        seq->stop();
        updatePlaybackUi(false);
    } else {
        if (m_currentIndex < 0 || m_comboTargetGroup->count() == 0) return;
        
        QString targetGroupIdStr = m_comboTargetGroup->currentData().toString();
        QUuid targetGroupId = QUuid(targetGroupIdStr);
        seq->setTargetGroup(targetGroupId);
        
        double rate = 1.0;
        switch(m_comboRate->currentIndex()) {
            case 0: rate = 4.0; break;
            case 1: rate = 2.0; break;
            case 2: rate = 1.0; break;
            case 3: rate = 0.5; break;
            case 4: rate = 0.25; break;
        }
        
        seq->setSequenceAndRate(m_graphWidget->getSteps(), m_spinBpm->value(), rate);
        seq->play();
        updatePlaybackUi(true);
    }
}

void NoteSequenceEditorView::updatePlaybackUi(bool isPlaying) {
    if (isPlaying) {
        m_btnPlay->setText("⏹ Stop");
        m_btnPlay->setStyleSheet("background-color: #aa3333; color: white; font-weight: bold; border-radius: 4px; padding: 5px;");
    } else {
        m_btnPlay->setText("⏵ Play");
        m_btnPlay->setStyleSheet("background-color: #33aa33; color: white; font-weight: bold; border-radius: 4px; padding: 5px;");
        m_graphWidget->setActiveStep(-1);
    }
}

#include "NoteSequenceEditorView.moc"
