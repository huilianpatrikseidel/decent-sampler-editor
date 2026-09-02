#include "LibraryView.h"
#include "../../core/ApplicationController.h"
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
#include <QLinearGradient>
#include <QPainter>
#include <QPainterPath>
#include <QFileInfo>
#include <algorithm>

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
    m_searchBox->setClearButtonEnabled(true);
    m_searchBox->addAction(makeSearchIcon(m_searchBox->palette().color(QPalette::PlaceholderText)),
                           QLineEdit::LeadingPosition);
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

    m_emptyState = new QLabel();
    m_emptyState->setObjectName("LibraryEmptyState");
    m_emptyState->setAlignment(Qt::AlignCenter);
    m_emptyState->setWordWrap(true);
    m_emptyState->hide();
    mainLayout->addWidget(m_emptyState, 1);

    connect(m_newBtn, &QPushButton::clicked, this, &LibraryView::onNewProjectClicked);
    connect(m_projectGrid, &QListWidget::itemDoubleClicked, this, &LibraryView::onProjectDoubleClicked);
    connect(m_projectGrid, &QListWidget::customContextMenuRequested, this, &LibraryView::showContextMenu);

    // The search field and the sort selector were both inert; wiring them here is what
    // makes the search icon above honest.
    connect(m_searchBox, &QLineEdit::textChanged, this, &LibraryView::applyFilter);
    connect(m_sortCombo, &QComboBox::currentIndexChanged, this, &LibraryView::loadProjects);
}

QPixmap LibraryView::makeThumbnail(const QString& projectName) {
    constexpr int kWidth = 200;
    constexpr int kHeight = 120;

    // Hash the name so a project keeps the same colour across sessions and machines.
    const uint hash = qHash(projectName.trimmed().toLower(), 0x5eed);
    const int hue = static_cast<int>(hash % 360u);

    QPixmap pixmap(kWidth, kHeight);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath clip;
    clip.addRoundedRect(QRectF(0, 0, kWidth, kHeight), 8, 8);
    painter.setClipPath(clip);

    QLinearGradient gradient(0, 0, kWidth, kHeight);
    gradient.setColorAt(0.0, QColor::fromHsv(hue, 150, 105));
    gradient.setColorAt(1.0, QColor::fromHsv((hue + 40) % 360, 170, 60));
    painter.fillPath(clip, gradient);

    // Initials: first letter of the first two words, so "Deep Strings" reads as "DS".
    QString initials;
    const QStringList words = projectName.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
    for (const QString& word : words) {
        initials += word.at(0).toUpper();
        if (initials.size() == 2) break;
    }
    if (initials.isEmpty()) initials = QStringLiteral("?");

    QFont font = painter.font();
    font.setPointSize(34);
    font.setBold(true);
    painter.setFont(font);
    painter.setPen(QColor(255, 255, 255, 190));
    painter.drawText(QRect(0, 0, kWidth, kHeight), Qt::AlignCenter, initials);

    return pixmap;
}

QIcon LibraryView::makeSearchIcon(const QColor& color) {
    QPixmap pixmap(16, 16);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen(color.isValid() ? color : QColor(150, 150, 150));
    pen.setWidthF(1.6);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(QPointF(6.5, 6.5), 4.5, 4.5);
    painter.drawLine(QPointF(10.0, 10.0), QPointF(14.0, 14.0));

    return QIcon(pixmap);
}

void LibraryView::applyFilter() {
    const QString needle = m_searchBox->text().trimmed();
    for (int i = 0; i < m_projectGrid->count(); ++i) {
        QListWidgetItem* item = m_projectGrid->item(i);
        const QString name = item->data(Qt::UserRole + 1).toString();
        item->setHidden(!needle.isEmpty() && !name.contains(needle, Qt::CaseInsensitive));
    }
    updateEmptyState();
}

void LibraryView::updateEmptyState() {
    int visible = 0;
    for (int i = 0; i < m_projectGrid->count(); ++i) {
        if (!m_projectGrid->item(i)->isHidden()) ++visible;
    }

    if (visible > 0) {
        m_emptyState->hide();
        m_projectGrid->show();
        return;
    }

    m_emptyState->setText(m_projectGrid->count() > 0
        ? tr("Nothing matches \"%1\".").arg(m_searchBox->text().trimmed())
        : tr("No libraries yet.\n\nStart one with \"New Project\", or bring in an existing "
             "instrument with \"Import\"."));
    m_projectGrid->hide();
    m_emptyState->show();
}

void LibraryView::loadProjects() {
    m_projectGrid->clear();
    
    DatabaseManager* db = m_controller->getDatabaseManager();
    if (!db) return;
    
    QList<ProjectRecord> projects = db->getProjects();

    switch (m_sortCombo->currentIndex()) {
        case 1: // Name
            std::sort(projects.begin(), projects.end(), [](const ProjectRecord& a, const ProjectRecord& b) {
                return a.name.localeAwareCompare(b.name) < 0;
            });
            break;
        case 2: // Date created, newest first
            std::sort(projects.begin(), projects.end(), [](const ProjectRecord& a, const ProjectRecord& b) {
                return a.createdAt > b.createdAt;
            });
            break;
        default: // Last modified, newest first
            std::sort(projects.begin(), projects.end(), [](const ProjectRecord& a, const ProjectRecord& b) {
                return a.modifiedAt > b.modifiedAt;
            });
            break;
    }

    for (const auto& rec : projects) {
        QListWidgetItem* item = new QListWidgetItem();

        // A stored preview wins when one exists; otherwise fall back to a generated card.
        QPixmap thumb;
        if (!rec.thumbnailPath.isEmpty() && QFileInfo::exists(rec.thumbnailPath)) {
            thumb.load(rec.thumbnailPath);
        }
        if (thumb.isNull()) {
            thumb = makeThumbnail(rec.name);
        } else {
            thumb = thumb.scaled(200, 120, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        }

        item->setIcon(QIcon(thumb));
        item->setText(rec.name + "\nModified: " + rec.modifiedAt.toString("dd/MM/yyyy"));
        item->setTextAlignment(Qt::AlignLeft | Qt::AlignBottom);
        item->setSizeHint(QSize(220, 180));
        item->setData(Qt::UserRole, rec.id);
        item->setData(Qt::UserRole + 1, rec.name); // searched against by applyFilter()
        item->setToolTip(rec.name);
        m_projectGrid->addItem(item);
    }

    applyFilter(); // keeps an active search applied across reloads, and sets the empty state
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
