#pragma once

#include <QColor>
#include <QIcon>
#include <QPixmap>
#include <QWidget>

class QLabel;
class QListWidget;
class QListWidgetItem;
class QComboBox;
class QLineEdit;
class QPushButton;
class ApplicationController;

class LibraryView : public QWidget {
    Q_OBJECT
public:
    explicit LibraryView(ApplicationController* controller, QWidget* parent = nullptr);
    ~LibraryView() = default;

signals:
    void requestTabChange(int index);

public slots:
    void loadProjects();
    void onNewProjectClicked();
    void onProjectDoubleClicked(QListWidgetItem* item);
    void showContextMenu(const QPoint& pos);

private slots:
    void applyFilter();

private:
    ApplicationController* m_controller;

    QListWidget* m_projectGrid;
    QComboBox* m_sortCombo;
    QLineEdit* m_searchBox;
    QPushButton* m_newBtn;
    QPushButton* m_importBtn;
    QLabel* m_emptyState = nullptr;

    void setupUi();

    // Cards used to render as a flat black rectangle. Until projects carry a real
    // rendered preview, derive a stable one from the name so every card is distinct
    // and recognisable at a glance.
    static QPixmap makeThumbnail(const QString& projectName);
    static QIcon makeSearchIcon(const QColor& color);
    void updateEmptyState();
};
