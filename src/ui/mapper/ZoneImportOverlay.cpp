#include "ZoneImportOverlay.h"
#include "../../core/AutoMapper.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QFileInfo>
#include <QPainter>
#include <QGraphicsDropShadowEffect>
#include <QHeaderView>
#include <QInputDialog>
#include "../../core/ThemePalette.h"

ZoneImportOverlay::ZoneImportOverlay(ProjectManager* pm, QWidget* parent) 
    : QWidget(parent), m_pm(pm) {
    // Default single bucket
    m_buckets.append(VelocityBucket{1, 127, {}});
    setupUi();
}

void ZoneImportOverlay::setupUi() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);
    
    // The central panel
    m_panelWidget = new QWidget(this);
    m_panelWidget->setFixedSize(900, 650);
    m_panelWidget->setStyleSheet("QWidget { background-color: #252526; border-radius: 8px; border: 1px solid #3e3e42; }");
    
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(20);
    shadow->setColor(QColor(0, 0, 0, 160));
    shadow->setOffset(0, 4);
    m_panelWidget->setGraphicsEffect(shadow);
    
    QVBoxLayout* panelLayout = new QVBoxLayout(m_panelWidget);
    panelLayout->setContentsMargins(20, 20, 20, 20);
    panelLayout->setSpacing(15);
    
    QLabel* titleLabel = new QLabel("<b>Advanced Sample Import</b>");
    titleLabel->setStyleSheet("font-size: 18px; border: none; color: white;");
    panelLayout->addWidget(titleLabel);
    
    // Top config row
    QHBoxLayout* configLayout = new QHBoxLayout();
    configLayout->addWidget(new QLabel("Target Group:"));
    m_groupCombo = new QComboBox();
    m_groupCombo->setStyleSheet("border: 1px solid #3e3e42; padding: 4px;");
    configLayout->addWidget(m_groupCombo);
    
    configLayout->addSpacing(20);
    configLayout->addWidget(new QLabel("Mapping Mode:"));
    m_modeCombo = new QComboBox();
    m_modeCombo->addItems({"Tonal (Stretches Zones)", "Percussive (Single Notes)"});
    m_modeCombo->setStyleSheet("border: 1px solid #3e3e42; padding: 4px;");
    configLayout->addWidget(m_modeCombo);
    
    configLayout->addSpacing(20);
    configLayout->addWidget(new QLabel("Default Root:"));
    m_defaultRootSpin = new QSpinBox();
    m_defaultRootSpin->setRange(0, 127);
    m_defaultRootSpin->setValue(60);
    m_defaultRootSpin->setStyleSheet("border: 1px solid #3e3e42; padding: 4px;");
    configLayout->addWidget(m_defaultRootSpin);
    
    configLayout->addStretch();
    panelLayout->addLayout(configLayout);
    
    // 3 Columns Layout
    QHBoxLayout* columnsLayout = new QHBoxLayout();
    columnsLayout->setSpacing(10);
    
    // Column 2: Velocity Zones
    QVBoxLayout* col2Layout = new QVBoxLayout();
    col2Layout->addWidget(new QLabel("<b>Velocity Zones</b>"));
    m_velZoneList = new QListWidget();
    m_velZoneList->setStyleSheet("QListWidget { background-color: #1e1e1e; border: 1px solid #3e3e42; padding: 4px; outline: none; } "
                                 "QListWidget::item { padding: 8px; border-bottom: 1px solid #2d2d30; } "
                                 "QListWidget::item:selected { background-color: #ff6600; color: white; }");
    connect(m_velZoneList, &QListWidget::currentRowChanged, this, &ZoneImportOverlay::onVelocityZoneSelected);
    col2Layout->addWidget(m_velZoneList);
    
    m_btnAddSplit = new QPushButton("+ Add Velocity Zone");
    m_btnAddSplit->setStyleSheet("padding: 6px; border: 1px solid #3e3e42;");
    connect(m_btnAddSplit, &QPushButton::clicked, this, [this]() {
        // Quick split action for simplicity
        VelocityBucket nb;
        nb.loVel = 1;
        nb.hiVel = 127;
        m_buckets.append(nb);
        refreshVelocityList();
        m_velZoneList->setCurrentRow(m_buckets.size() - 1);
    });
    col2Layout->addWidget(m_btnAddSplit);
    columnsLayout->addLayout(col2Layout, 1);
    
    // Column 3: Samples
    QVBoxLayout* col3Layout = new QVBoxLayout();
    col3Layout->addWidget(new QLabel("<b>Samples (Drag & Drop Here)</b>"));
    m_fileList = new FileDropListWidget();
    m_fileList->setStyleSheet("QListWidget { background-color: #1e1e1e; border: 1px dashed #ff6600; padding: 4px; } "
                              "QListWidget::item { padding: 4px; }");
    connect(m_fileList, &FileDropListWidget::filesDropped, this, &ZoneImportOverlay::onFilesDropped);
    
    // Add delete key shortcut for samples
    connect(m_fileList, &QListWidget::itemSelectionChanged, this, [this]() {
        // In a full implementation, we'd add key event overrides for backspace/delete
    });
    
    col3Layout->addWidget(m_fileList);
    
    QPushButton* btnClearFiles = new QPushButton("Clear Selected");
    btnClearFiles->setStyleSheet("padding: 6px; border: 1px solid #3e3e42;");
    connect(btnClearFiles, &QPushButton::clicked, this, [this]() {
        if (m_currentBucketIndex >= 0 && m_currentBucketIndex < m_buckets.size()) {
            QList<QListWidgetItem*> items = m_fileList->selectedItems();
            for (auto* item : items) {
                QString filename = item->text();
                // Find in bucket and remove
                for (int i=0; i < m_buckets[m_currentBucketIndex].files.size(); ++i) {
                    if (QFileInfo(m_buckets[m_currentBucketIndex].files[i]).fileName() == filename) {
                        m_buckets[m_currentBucketIndex].files.removeAt(i);
                        break;
                    }
                }
            }
            refreshFileList();
            updatePreview();
        }
    });
    col3Layout->addWidget(btnClearFiles);
    
    columnsLayout->addLayout(col3Layout, 2);
    
    panelLayout->addLayout(columnsLayout, 2);
    
    // Preview Area
    panelLayout->addWidget(new QLabel("<b>Mapping Preview</b>"));
    m_previewTable = new QTableWidget(0, 4);
    m_previewTable->setHorizontalHeaderLabels({"File", "Note", "Velocity Range", "RR Pos"});
    m_previewTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_previewTable->setStyleSheet("QTableWidget { background-color: #1e1e1e; border: 1px solid #3e3e42; gridline-color: #3e3e42; } "
                                  "QHeaderView::section { background-color: #2d2d30; padding: 4px; border: 1px solid #3e3e42; }");
    m_previewTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_previewTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    panelLayout->addWidget(m_previewTable, 1);
    
    // Buttons
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    
    m_cancelBtn = new QPushButton("Cancel");
    m_cancelBtn->setStyleSheet("QPushButton { border: 1px solid #3e3e42; padding: 6px 20px; border-radius: 3px; } QPushButton:hover { background-color: #3e3e42; }");
    connect(m_cancelBtn, &QPushButton::clicked, this, &ZoneImportOverlay::importCancelled);
    btnLayout->addWidget(m_cancelBtn);
    
    m_importBtn = new QPushButton("Import");
    m_importBtn->setStyleSheet("QPushButton { background-color: #ff6600; color: white; border: none; padding: 6px 20px; border-radius: 3px; font-weight: bold; } QPushButton:hover { background-color: #ff8533; }");
    connect(m_importBtn, &QPushButton::clicked, this, [this]() {
        QUuid targetGroupId = m_groupCombo->currentData().toUuid();
        emit importConfirmed(m_buckets, targetGroupId, m_modeCombo->currentIndex(), m_defaultRootSpin->value());
    });
    btnLayout->addWidget(m_importBtn);
    
    panelLayout->addLayout(btnLayout);
    mainLayout->addWidget(m_panelWidget);
    
    connect(m_modeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ZoneImportOverlay::updatePreview);
    connect(m_defaultRootSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &ZoneImportOverlay::updatePreview);
}

void ZoneImportOverlay::updateGroupCombo() {
    m_groupCombo->clear();
    m_groupCombo->addItem("Create New Group", QUuid());
    for (const auto& pair : m_pm->getAllNodes()) {
        if (pair.second->type == "SampleGroup") {
            m_groupCombo->addItem(pair.second->name.isEmpty() ? "Unnamed Group" : pair.second->name, pair.second->id);
        }
    }
}

void ZoneImportOverlay::refreshVelocityList() {
    m_velZoneList->clear();
    for (int i = 0; i < m_buckets.size(); ++i) {
        QString text = QString("Zone %1 (Vel: %2 - %3) [%4 samples]")
            .arg(i+1)
            .arg(m_buckets[i].loVel, 3, 10, QChar('0'))
            .arg(m_buckets[i].hiVel, 3, 10, QChar('0'))
            .arg(m_buckets[i].files.size());
        m_velZoneList->addItem(text);
    }
}

void ZoneImportOverlay::refreshFileList() {
    m_fileList->clear();
    if (m_currentBucketIndex >= 0 && m_currentBucketIndex < m_buckets.size()) {
        for (const QString& f : m_buckets[m_currentBucketIndex].files) {
            m_fileList->addItem(QFileInfo(f).fileName());
        }
    }
}

void ZoneImportOverlay::updatePreview() {
    m_previewTable->setRowCount(0);
    
    int mode = m_modeCombo->currentIndex();
    int root = m_defaultRootSpin->value();
    
    for (const auto& bucket : m_buckets) {
        if (bucket.files.isEmpty()) continue;
        
        // Use AutoMapper just to preview what it will do
        // Note: For actual mapping, we will parse them and force the bucket velocities
        QVector<Zone> mapped = AutoMapper::mapFiles(bucket.files, (AutoMapper::MappingMode)mode, root);
        
        for (const Zone& z : mapped) {
            int row = m_previewTable->rowCount();
            m_previewTable->insertRow(row);
            m_previewTable->setItem(row, 0, new QTableWidgetItem(QFileInfo(z.samplePath).fileName()));
            m_previewTable->setItem(row, 1, new QTableWidgetItem(QString("Note %1").arg(z.rootNote)));
            m_previewTable->setItem(row, 2, new QTableWidgetItem(QString("%1 - %2").arg(bucket.loVel).arg(bucket.hiVel)));
            m_previewTable->setItem(row, 3, new QTableWidgetItem(QString::number(z.seqPosition)));
        }
    }
}

void ZoneImportOverlay::onVelocityZoneSelected(int index) {
    m_currentBucketIndex = index;
    refreshFileList();
}

void ZoneImportOverlay::onFilesDropped(const QStringList& files) {
    if (m_currentBucketIndex >= 0 && m_currentBucketIndex < m_buckets.size()) {
        for (const QString& f : files) {
            if (!m_buckets[m_currentBucketIndex].files.contains(f)) {
                m_buckets[m_currentBucketIndex].files.append(f);
            }
        }
        refreshVelocityList();
        m_velZoneList->setCurrentRow(m_currentBucketIndex); // Maintain selection
        refreshFileList();
        updatePreview();
    }
}

void ZoneImportOverlay::prepareForImport(const QStringList& files) {
    // Reset state
    m_buckets.clear();
    // Create an initial bucket
    VelocityBucket b1;
    b1.loVel = 1; b1.hiVel = 127;
    b1.files = files;
    m_buckets.append(b1);
    
    m_currentBucketIndex = 0;
    updateGroupCombo();
    refreshVelocityList();
    m_velZoneList->setCurrentRow(0);
    refreshFileList();
    updatePreview();
}

void ZoneImportOverlay::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.fillRect(rect(), ThemePalette::color("qss_color_30", QColor(0, 0, 0, 180)));
}
