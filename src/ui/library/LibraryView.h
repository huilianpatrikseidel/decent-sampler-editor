#pragma once

#include <QWidget>

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

private:
    ApplicationController* m_controller;
    
    QListWidget* m_projectGrid;
    QComboBox* m_sortCombo;
    QLineEdit* m_searchBox;
    QPushButton* m_newBtn;
    QPushButton* m_importBtn;

    void setupUi();
};
