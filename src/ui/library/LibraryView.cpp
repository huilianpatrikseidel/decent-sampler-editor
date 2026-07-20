#include "LibraryView.h"
#include "../../core/ApplicationController.h"
#include "../../core/ThemePalette.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QListWidgetItem>
#include <QIcon>
#include <QPixmap>
#include <QInputDialog>
#include <QDir>
#include <QRegularExpression>
#include <QMenu>
#include <QAction>
#include <QMessageBox>

LibraryView::LibraryView(ApplicationController* controller, QWidget* parent)
    : QWidget(parent), m_controller(controller)
{
    setupUi();
    loadProjects();
}

void LibraryView::setupUi() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(40, 40, 40, 40);
    mainLayout->setSpacing(20);

    // --- Top Bar ---
    QHBoxLayout* topBar = new QHBoxLayout();
    
    QLabel* titleLabel = new QLabel("Libraries");
    titleLabel->setObjectName("LibraryTitleLabel");
    topBar->addWidget(titleLabel);
    
    topBar->addStretch();
    
    m_searchBox = new QLineEdit();
    m_searchBox->setPlaceholderText("Search libraries...");
    m_searchBox->setFixedWidth(200);
    m_searchBox->setStyleSheet("padding: 6px; border-radius: 4px;");
    topBar->addWidget(m_searchBox);
    
    m_sortCombo = new QComboBox();
    m_sortCombo->addItems({"Sort by Last Modified", "Sort by Name", "Sort by Date Created"});
    m_sortCombo->setStyleSheet("padding: 6px;");
    topBar->addWidget(m_sortCombo);
    
    m_importBtn = new QPushButton("Import");
    m_importBtn->setStyleSheet("padding: 6px 16px;");
    topBar->addWidget(m_importBtn);
    
    m_newBtn = new QPushButton("New Project");
    m_newBtn->setStyleSheet("background-color: #ff6600; color: white; padding: 6px 16px; font-weight: bold;");
    topBar->addWidget(m_newBtn);

    mainLayout->addLayout(topBar);

    // --- Main Grid ---
    m_projectGrid = new QListWidget();
    m_projectGrid->setViewMode(QListView::IconMode);
    m_projectGrid->setIconSize(QSize(200, 120));
    m_projectGrid->setResizeMode(QListView::Adjust);
    m_projectGrid->setSpacing(20);
    m_projectGrid->setMovement(QListView::Static);
    m_projectGrid->setObjectName("LibraryProjectGrid");
    
    m_projectGrid->setContextMenuPolicy(Qt::CustomContextMenu);
    
    mainLayout->addWidget(m_projectGrid);
    
    connect(m_newBtn, &QPushButton::clicked, this, &LibraryView::onNewProjectClicked);
    connect(m_projectGrid, &QListWidget::itemDoubleClicked, this, &LibraryView::onProjectDoubleClicked);
    connect(m_projectGrid, &QListWidget::customContextMenuRequested, this, &LibraryView::showContextMenu);
}

void LibraryView::loadProjects() {
    m_projectGrid->clear();
    
    DatabaseManager* db = m_controller->getDatabaseManager();
    if (!db) return;
    
    QList<ProjectRecord> projects = db->getProjects();
    
    QPixmap defaultThumb(200, 120);
    defaultThumb.fill(ThemePalette::color("qss_color_15", QColor("#1e1e1e")));
    
    for (const auto& rec : projects) {
        QListWidgetItem* item = new QListWidgetItem();
        item->setIcon(QIcon(defaultThumb));
        item->setText(rec.name + "\nModified: " + rec.modifiedAt.toString("dd/MM/yyyy"));
        item->setTextAlignment(Qt::AlignLeft | Qt::AlignBottom);
        item->setSizeHint(QSize(220, 180));
        item->setData(Qt::UserRole, rec.id);
        m_projectGrid->addItem(item);
    }
}

void LibraryView::onNewProjectClicked() {
    bool ok;
    QString name = QInputDialog::getText(this, "New Project", "Project Name:", QLineEdit::Normal, "", &ok);
    
    if (ok && !name.isEmpty()) {
        DatabaseManager* db = m_controller->getDatabaseManager();
        if (db) {
            QString folderName = name;
            folderName.remove(QRegularExpression("[\\\\/:*?\"<>|]"));
            
            QDir dir(db->getProjectsDirectory());
            QString projectPath = dir.filePath(folderName);
            dir.mkpath(projectPath); // Media pool folder
            
            int id = db->addProject(name, "", "");
            if (id != -1) {
                ProjectManager* pm = m_controller->getProjectManager();
                pm->createNewProject(name);
                pm->setCurrentProjectId(id);
                pm->saveProject();
                
                emit requestTabChange(1); // Switch to Sampling tab
                loadProjects(); // Refresh grid
            }
        }
    }
}

void LibraryView::onProjectDoubleClicked(QListWidgetItem* item) {
    if (!item) return;
    int id = item->data(Qt::UserRole).toInt();
    
    DatabaseManager* db = m_controller->getDatabaseManager();
    if (db) {
        ProjectRecord rec = db->getProject(id);
        ProjectManager* pm = m_controller->getProjectManager();
        if (pm->loadProject(rec.projectData, rec.id)) {
            pm->setCurrentProjectName(rec.name);
            emit requestTabChange(1); // Switch to Sampling tab
        }
    }
}

void LibraryView::showContextMenu(const QPoint& pos) {
    QListWidgetItem* item = m_projectGrid->itemAt(pos);
    if (!item) return;
    
    QMenu menu(this);
    QAction* renameAction = menu.addAction("Rename Project");
    QAction* deleteAction = menu.addAction("Delete Project");
    
    QAction* selected = menu.exec(m_projectGrid->mapToGlobal(pos));
    
    if (selected == renameAction) {
        int id = item->data(Qt::UserRole).toInt();
        DatabaseManager* db = m_controller->getDatabaseManager();
        if (db) {
            ProjectRecord rec = db->getProject(id);
            bool ok;
            QString newName = QInputDialog::getText(this, "Rename Project", "New Name:", QLineEdit::Normal, rec.name, &ok);
            if (ok && !newName.isEmpty() && newName != rec.name) {
                rec.name = newName;
                db->updateProject(rec);
                loadProjects();
            }
        }
    } else if (selected == deleteAction) {
        int id = item->data(Qt::UserRole).toInt();
        DatabaseManager* db = m_controller->getDatabaseManager();
        if (db) {
            ProjectRecord rec = db->getProject(id);
            // Delete media pool folder
            QString folderName = rec.name;
            folderName.remove(QRegularExpression("[\\\\/:*?\"<>|]"));
            QString projectPath = QDir(db->getProjectsDirectory()).filePath(folderName);
            QDir dir(projectPath);
            if (dir.exists()) {
                dir.removeRecursively();
            }
            
            // Delete from DB
            db->deleteProject(id);
            loadProjects(); // Refresh grid
        }
    }
}
