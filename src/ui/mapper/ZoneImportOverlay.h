#pragma once
#include <QWidget>
#include <QStringList>
#include <QUuid>
#include "../../core/ProjectManager.h"

#include <QTableWidget>
#include <QListWidget>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QStringList>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>


#include "ui/mapper/MapperLibExport.h"
struct MAPPERLIB_EXPORT VelocityBucket {
    int loVel = 1;
    int hiVel = 127;
    QStringList files;
};

class MAPPERLIB_EXPORT FileDropListWidget : public QListWidget {
    Q_OBJECT
public:
    explicit FileDropListWidget(QWidget* parent = nullptr) : QListWidget(parent) {
        setAcceptDrops(true);
        setSelectionMode(QAbstractItemView::ExtendedSelection);
    }
signals:
    void filesDropped(const QStringList& files);
protected:
    void dragEnterEvent(QDragEnterEvent* event) override {
        if (event->mimeData()->hasUrls()) {
            event->acceptProposedAction();
        }
    }
    void dragMoveEvent(QDragMoveEvent* event) override {
        event->acceptProposedAction();
    }
    void dropEvent(QDropEvent* event) override {
        QStringList files;
        for (const QUrl& url : event->mimeData()->urls()) {
            if (url.isLocalFile()) {
                QString path = url.toLocalFile();
                if (path.endsWith(".wav", Qt::CaseInsensitive) || 
                    path.endsWith(".mp3", Qt::CaseInsensitive) || 
                    path.endsWith(".flac", Qt::CaseInsensitive)) {
                    files.append(path);
                }
            }
        }
        if (!files.isEmpty()) {
            emit filesDropped(files);
        }
        event->acceptProposedAction();
    }
};

class MAPPERLIB_EXPORT ZoneImportOverlay : public QWidget {
    Q_OBJECT
public:
    explicit ZoneImportOverlay(ProjectManager* pm, QWidget* parent = nullptr);

    void prepareForImport(const QStringList& files);

signals:
    void importConfirmed(const QVector<VelocityBucket>& buckets, QUuid targetGroupId, int mappingMode, int defaultRoot);
    void importCancelled();

protected:
    void paintEvent(QPaintEvent* event) override;

private slots:
    void onVelocityZoneSelected(int index);
    void onFilesDropped(const QStringList& files);
    void updatePreview();

private:
    ProjectManager* m_pm;
    QVector<VelocityBucket> m_buckets;
    int m_currentBucketIndex = 0;
    
    QWidget* m_panelWidget;
    QComboBox* m_groupCombo; // Usando combo para grupos para economizar espaço
    QListWidget* m_velZoneList;
    QPushButton* m_btnAddSplit;
    FileDropListWidget* m_fileList; // Custom para drag&drop
    
    QTableWidget* m_previewTable;
    QComboBox* m_modeCombo;
    QSpinBox* m_defaultRootSpin;
    
    QPushButton* m_importBtn;
    QPushButton* m_cancelBtn;

    void setupUi();
    void updateGroupCombo();
    void refreshVelocityList();
    void refreshFileList();
};
