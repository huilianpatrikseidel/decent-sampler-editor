#include "MappingEditorContainer.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSplitter>
#include <QFileDialog>
#include "../../commands/AddNodeCommand.h"
#include "../../commands/ZoneCommands.h"
#include "../../core/AutoMapper.h"
#include "../../commands/ModifyPropertyCommand.h"
#include "../../commands/RemoveNodeCommand.h"
#include <QListWidget>
#include <QSplitter>
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QComboBox>
#include <QMessageBox>

#include "GroupDialogs.h"
#include <QIcon>
#include <QPixmap>
#include <QPainter>
#include "../../core/ThemePalette.h"

MappingEditorContainer::MappingEditorContainer(ProjectManager* pm, QWidget* parent) : QWidget(parent), m_pm(pm) {
    m_stackedLayout = new QStackedLayout(this);
    m_stackedLayout->setStackingMode(QStackedLayout::StackAll);
    
    m_mainViewContainer = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(m_mainViewContainer);
    layout->setContentsMargins(0, 0, 0, 0);
    
    QHBoxLayout* topBar = new QHBoxLayout();
    
    QLabel* groupLabel = new QLabel("Group:");
    groupLabel->setContentsMargins(10, 0, 0, 0);
    topBar->addWidget(groupLabel);
    
    m_groupCombo = new QComboBox();
    m_groupCombo->setMinimumWidth(150);
    m_groupCombo->setIconSize(QSize(55, 16));
    topBar->addWidget(m_groupCombo);
    
    m_btnEditGroup = new QPushButton("Edit");
    topBar->addWidget(m_btnEditGroup);
    
    m_btnAddGroup = new QPushButton("+ Add Group");
    topBar->addWidget(m_btnAddGroup);
    
    m_btnImportSamples = new QPushButton("Import Samples");
    m_btnImportSamples->setStyleSheet("background-color: #ff6600; color: white; padding: 4px 12px; font-weight: bold; border-radius: 2px;");
    topBar->addWidget(m_btnImportSamples);
    
    topBar->addStretch();
    
    layout->addLayout(topBar);
    
    m_modeStack = new QStackedWidget();
    
    m_view = new MappingEditorView(pm, this);
    m_view->setMinimumSize(100, 100);
    
    m_synthView = new SynthEditorView(pm, this);
    
    m_modeStack->addWidget(m_view); // index 0
    m_modeStack->addWidget(m_synthView); // index 1
    
    layout->addWidget(m_modeStack);
    
    m_stackedLayout->addWidget(m_mainViewContainer);
    
    // The Overlay
    m_importOverlay = new ZoneImportOverlay(pm);
    m_importOverlay->hide();
    m_stackedLayout->addWidget(m_importOverlay);
    
    // Pass zone selection out
    connect(m_view, static_cast<void (MappingEditorView::*)(const QUuid&, int)>(&MappingEditorView::zoneSelected), this, [this](const QUuid& sgId, int index) {
        emit zoneSelected(sgId, index);
    });
    
    connect(m_view, &MappingEditorView::notePressed, this, &MappingEditorContainer::notePressed);
    connect(m_view, &MappingEditorView::noteReleased, this, &MappingEditorContainer::noteReleased);
    
    connect(m_btnAddGroup, &QPushButton::clicked, this, [this]() {
        CreateGroupDialog dlg(this);
        // Pre-fill default name
        int count = 1;
        for (const auto& pair : m_pm->getAllNodes()) {
            if (pair.second->type == "SampleGroup") count++;
        }
        dlg.m_nameEdit->setText(QString("Group %1").arg(count));
        
        if (dlg.exec() == QDialog::Accepted) {
            auto sg = std::make_unique<SampleGroup>();
            sg->name = dlg.getName();
            if (sg->name.isEmpty()) sg->name = QString("Group %1").arg(count);
            sg->isSynthContainer = dlg.isSynth();
            
            QUuid newId = sg->id;
            m_pm->getUndoStack()->push(new AddNodeCommand(m_pm, std::move(sg), QPointF(0, 0)));
            setSampleGroup(newId);
        }
    });
    
    connect(m_btnEditGroup, &QPushButton::clicked, this, [this]() {
        QUuid currentId = m_groupCombo->currentData().toUuid();
        GroupManagerDialog dlg(m_pm, this);
        if (!currentId.isNull()) {
            for(int i=0; i<dlg.m_list->count(); ++i) {
                if (dlg.m_list->item(i)->data(Qt::UserRole).toUuid() == currentId) {
                    dlg.m_list->setCurrentItem(dlg.m_list->item(i));
                    break;
                }
            }
        }
        dlg.exec();
    });
    
    connect(m_btnImportSamples, &QPushButton::clicked, this, [this]() {
        QStringList files = QFileDialog::getOpenFileNames(this, "Select Audio Files", "", "Audio Files (*.wav *.flac *.mp3)");
        if (!files.isEmpty()) {
            showImportOverlay(files);
        }
    });
    
    // Handle overlay logic
    connect(m_importOverlay, &ZoneImportOverlay::importCancelled, this, [this]() {
        m_importOverlay->hide();
        m_stackedLayout->setCurrentIndex(0);
    });
    
    connect(m_importOverlay, &ZoneImportOverlay::importConfirmed, this, [this](const QVector<VelocityBucket>& buckets, QUuid targetGroupId, int mode, int root) {
        m_importOverlay->hide();
        m_stackedLayout->setCurrentIndex(0);
        
        if (targetGroupId.isNull()) {
            auto newSg = std::make_unique<SampleGroup>();
            int count = 1;
            for (const auto& pair : m_pm->getAllNodes()) {
                if (pair.second->type == "SampleGroup") count++;
            }
            newSg->name = QString("Group %1").arg(count);
            targetGroupId = newSg->id;
            m_pm->getUndoStack()->push(new AddNodeCommand(m_pm, std::move(newSg), QPointF(0, 0)));
            setSampleGroup(targetGroupId);
        }
        
        AutoMapper::MappingMode mapMode = (mode == 0) ? AutoMapper::MappingMode::Tonal : AutoMapper::MappingMode::Percussive;
        
        QVector<Zone> allMappedZones;
        for (const auto& bucket : buckets) {
            if (bucket.files.isEmpty()) continue;
            QVector<Zone> mapped = AutoMapper::mapFiles(bucket.files, mapMode, root);
            for (Zone& z : mapped) {
                // Force bucket velocities
                z.loVel = bucket.loVel;
                z.hiVel = bucket.hiVel;
                allMappedZones.append(z);
            }
        }
        
        if (!allMappedZones.isEmpty()) {
            m_pm->getUndoStack()->push(new AddMultipleZonesCommand(m_pm, targetGroupId, allMappedZones));
        }
    });
    
    connect(m_groupCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
        if (index >= 0) {
            QUuid id = m_groupCombo->itemData(index).toUuid();
            setSampleGroup(id);
        }
    });
    
    // removed m_modeCombo connect
    
    connect(m_pm, &ProjectManager::nodeAdded, this, &MappingEditorContainer::updateGroupList);
    connect(m_pm, &ProjectManager::nodeRemoved, this, &MappingEditorContainer::updateGroupList);
    connect(m_pm, &ProjectManager::nodeModified, this, [this](const QUuid& id, const QString& prop) {
        if (prop == "name" || prop == "isSynthContainer" || prop.isEmpty()) {
            updateGroupList();
        }
    });
    
    // Allows mapping editor view to signal it auto-created a group
    connect(m_view, &MappingEditorView::groupAutoCreated, this, [this](const QUuid& sgId) {
        updateGroupList();
        setSampleGroup(sgId);
    });
    
    updateGroupList();
}

void MappingEditorContainer::updateGroupList() {
    QUuid current = m_groupCombo->currentData().toUuid();
    m_groupCombo->blockSignals(true);
    m_groupCombo->clear();
    
    int indexToSelect = -1;
    for (const auto& pair : m_pm->getAllNodes()) {
        if (pair.second->type == "SampleGroup") {
            SampleGroup* sg = static_cast<SampleGroup*>(pair.second.get());
            if (!sg->synthParentId.isNull()) continue; // Do not show child oscillators
            
            QPixmap pix(55, 16);
            pix.fill(Qt::transparent);
            QPainter p(&pix);
            p.setRenderHint(QPainter::Antialiasing);
            p.setBrush(sg->isSynthContainer ? ThemePalette::color("synth_tag_bg", QColor(50, 220, 100)) : ThemePalette::color("sampler_tag_bg", QColor(255, 150, 50)));
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
            
            QString name = sg->name.isEmpty() ? "Group" : sg->name;
            m_groupCombo->addItem(icon, name, sg->id);
            
            if (sg->id == current) {
                indexToSelect = m_groupCombo->count() - 1;
            }
        }
    }
    m_groupCombo->blockSignals(false);
    
    if (indexToSelect >= 0) {
        m_groupCombo->blockSignals(true);
        m_groupCombo->setCurrentIndex(indexToSelect);
        m_groupCombo->blockSignals(false);
    } else if (m_groupCombo->count() > 0) {
        m_groupCombo->blockSignals(true);
        m_groupCombo->setCurrentIndex(0);
        m_groupCombo->blockSignals(false);
    }
    
    QUuid newCurrent = m_groupCombo->currentData().toUuid();
    if (newCurrent != current) {
        if (!newCurrent.isNull()) {
            setSampleGroup(newCurrent);
        } else {
            m_view->setSampleGroup(QUuid());
            m_synthView->setSampleGroup(QUuid());
        }
    }
}

void MappingEditorContainer::setSampleGroup(const QUuid& sgId) {
    m_groupCombo->blockSignals(true);
    for (int i = 0; i < m_groupCombo->count(); ++i) {
        if (m_groupCombo->itemData(i).toUuid() == sgId) {
            m_groupCombo->setCurrentIndex(i);
            break;
        }
    }
    m_groupCombo->blockSignals(false);
    
    m_view->setSampleGroup(sgId);
    m_synthView->setSampleGroup(sgId);
    
    Node* n = m_pm->getNode(sgId);
    if (n && n->type == "SampleGroup") {
        SampleGroup* sg = static_cast<SampleGroup*>(n);
        if (sg->isSynthContainer) {
            m_modeStack->setCurrentIndex(1);
        } else {
            m_modeStack->setCurrentIndex(0);
        }
    }
    
    emit groupSelected(sgId);
}

void MappingEditorContainer::showImportOverlay(const QStringList& files) {
    m_importOverlay->prepareForImport(files);
    m_stackedLayout->setCurrentIndex(1);
    m_importOverlay->show();
}
