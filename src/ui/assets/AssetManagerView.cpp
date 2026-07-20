#include "AssetManagerView.h"
#include <QFileDialog>
#include <QStandardPaths>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QRegularExpression>
#include "../../core/ProjectManager.h"

AssetManagerView::AssetManagerView(ProjectManager* pm, QWidget* parent)
    : QWidget(parent), m_pm(pm) {
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(5, 5, 5, 5);
    
    QLabel* title = new QLabel("<b>Asset Manager</b>");
    title->setStyleSheet("padding: 4px; background: #2d2d30;");
    layout->addWidget(title);
    
    QPushButton* btnOpen = new QPushButton("Select Assets Folder...");
    connect(btnOpen, &QPushButton::clicked, this, &AssetManagerView::onOpenFolderClicked);
    
    QPushButton* btnProject = new QPushButton("Project Assets");
    connect(btnProject, &QPushButton::clicked, this, [this]() {
        QString appData = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QString projName = m_pm->getCurrentProjectName();
        projName.remove(QRegularExpression("[\\\\/:*?\"<>|]"));
        QString projPath = QDir(appData).filePath("Databases/Libraries/Projects/" + projName);
        QDir dir(projPath);
        if (!dir.exists()) dir.mkpath(".");
        
        m_currentPath = projPath;
        m_listView->setRootIndex(m_fileModel->setRootPath(m_currentPath));
    });
    
    QHBoxLayout* topBtns = new QHBoxLayout();
    topBtns->addWidget(btnOpen);
    topBtns->addWidget(btnProject);
    layout->addLayout(topBtns);
    
    m_fileModel = new QFileSystemModel(this);
    m_fileModel->setNameFilters({"*.png", "*.jpg", "*.jpeg"});
    m_fileModel->setNameFilterDisables(false);
    
    m_listView = new QListView();
    m_listView->setModel(m_fileModel);
    m_listView->setViewMode(QListView::IconMode);
    m_listView->setIconSize(QSize(64, 64));
    m_listView->setGridSize(QSize(80, 80));
    m_listView->setResizeMode(QListView::Adjust);
    m_listView->setDragEnabled(true); // Allow dragging images to Canvas
    
    layout->addWidget(m_listView);
    
    // Default path
    m_currentPath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    m_listView->setRootIndex(m_fileModel->setRootPath(m_currentPath));
}

void AssetManagerView::onOpenFolderClicked() {
    QString dir = QFileDialog::getExistingDirectory(this, "Select Assets Folder", m_currentPath);
    if (!dir.isEmpty()) {
        m_currentPath = dir;
        m_listView->setRootIndex(m_fileModel->setRootPath(m_currentPath));
    }
}