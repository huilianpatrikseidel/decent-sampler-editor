#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QTabBar>
#include <QResizeEvent>


#include "ui/components/UiComponentsLibExport.h"
class UICOMPONENTSLIB_EXPORT CustomTitleBar : public QWidget {
    Q_OBJECT
public:
    explicit CustomTitleBar(QWidget *parent = nullptr);
    
    void setMenuBar(QMenuBar* menuBar);
    void setTitle(const QString& title);
    void updateMaximizeButton(bool isMaximized);
    
    // Embed a QTabBar centered in the title bar.
    // The tab bar is positioned absolutely (not in the layout) via resizeEvent,
    // so it stays perfectly centered regardless of left/right content sizes.
    void setTabBar(QTabBar* tabBar);
    
    // Win32 non-client area visual state management for the maximize button.
    void setMaximizeButtonHovered(bool hovered);
    void setMaximizeButtonPressed(bool pressed);
    
    // Returns HTMAXBUTTON, HTCAPTION, or 0
    int nativeHitTest(const QPoint& pos) const;

protected:
    void resizeEvent(QResizeEvent* event) override;

signals:
    void minimizeRequested();
    void maximizeRestoreRequested();
    void closeRequested();
    
    // New menu signals
    void settingsRequested();
    void aboutRequested();

private:
    void centerTabBar();
    
    QHBoxLayout* m_mainLayout;
    QPushButton* m_titleButton;
    QMenuBar* m_menuBar;
    QTabBar* m_tabBar;
    
    QPushButton* m_btnMinimize;
    QPushButton* m_btnMaximize;
    QPushButton* m_btnClose;
    
    QString m_btnStyle;
};
