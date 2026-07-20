#include "CustomTitleBar.h"
#include <QStyle>
#include <QApplication>
#include <QMenu>
#include <QAction>

CustomTitleBar::CustomTitleBar(QWidget *parent)
    : QWidget(parent), m_menuBar(nullptr), m_tabBar(nullptr) {
    
    // Set fixed height standard for Windows native title bars (approx 40px)
    setFixedHeight(40);
    
    // Create layout: [Title] [Menu] <stretch> [Min] [Max] [Close]
    // The tab bar is NOT in this layout — it's positioned absolutely via resizeEvent.
    m_mainLayout = new QHBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);
    
    // Left: Title
    m_mainLayout->addSpacing(15);
    
    m_titleButton = new QPushButton("decent sampler editor", this);
    m_titleButton->setObjectName("AppTitleButton");
    m_titleButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    m_titleButton->setCursor(Qt::ArrowCursor);
    
    QMenu* titleMenu = new QMenu(this);
    QAction* settingsAction = titleMenu->addAction("Configurações");
    QAction* aboutAction = titleMenu->addAction("Sobre");
    
    connect(settingsAction, &QAction::triggered, this, &CustomTitleBar::settingsRequested);
    connect(aboutAction, &QAction::triggered, this, &CustomTitleBar::aboutRequested);
    
    connect(m_titleButton, &QPushButton::clicked, this, [this, titleMenu]() {
        QPoint pos = m_titleButton->mapToGlobal(QPoint(0, m_titleButton->height()));
        titleMenu->exec(pos);
    });
    
    m_mainLayout->addWidget(m_titleButton, 0, Qt::AlignBaseline);
    
    // Right: Buttons
    m_btnMinimize = new QPushButton("—", this);
    m_btnMinimize->setObjectName("btnMinimize");
    m_btnMaximize = new QPushButton("□", this);
    m_btnMaximize->setObjectName("btnMaximize");
    m_btnClose = new QPushButton("✕", this);
    m_btnClose->setObjectName("btnClose");
    
    // Styles moved to QSS
                            
    m_btnMinimize->setFixedSize(46, 40);
    m_btnMaximize->setFixedSize(46, 40);
    m_btnClose->setFixedSize(46, 40);
    
    // Handled by QSS
    
    // Minimize and Close are handled via Qt signals (nativeHitTest returns 0 for them).
    // Maximize is handled via Win32 WM_NCLBUTTONDOWN/UP (nativeHitTest returns HTMAXBUTTON)
    connect(m_btnMinimize, &QPushButton::clicked, this, &CustomTitleBar::minimizeRequested);
    connect(m_btnClose, &QPushButton::clicked, this, &CustomTitleBar::closeRequested);
    
    // Stretch pushes buttons to the right
    m_mainLayout->addStretch(1);
    
    m_mainLayout->addWidget(m_btnMinimize);
    m_mainLayout->addWidget(m_btnMaximize);
    m_mainLayout->addWidget(m_btnClose);
}

void CustomTitleBar::setMenuBar(QMenuBar* menuBar) {
    if (m_menuBar) {
        m_mainLayout->removeWidget(m_menuBar);
    }
    m_menuBar = menuBar;
    if (m_menuBar) {
        m_menuBar->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        m_mainLayout->insertWidget(2, m_menuBar);
        m_mainLayout->setAlignment(m_menuBar, Qt::AlignBaseline);
        
        m_menuBar->setObjectName("AppMenuBar");
    }
}

void CustomTitleBar::setTabBar(QTabBar* tabBar) {
    m_tabBar = tabBar;
    if (!m_tabBar) return;
    
    // Parent to this widget but do NOT add to layout.
    // Position is managed manually in resizeEvent for true centering.
    m_tabBar->setParent(this);
    m_tabBar->setExpanding(false);
    m_tabBar->setDrawBase(false);
    
    m_tabBar->setObjectName("MainTabBar");
    
    m_tabBar->raise();
    
    centerTabBar();
}

void CustomTitleBar::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    if (layout()) {
        layout()->activate(); // Force layout geometry update before calculations
    }
    centerTabBar();
}

void CustomTitleBar::centerTabBar() {
    if (!m_tabBar) return;
    QSize tabSize = m_tabBar->sizeHint();
    
    int leftBound = 0;
    if (m_menuBar) {
        leftBound = m_menuBar->geometry().right() + 15;
    } else if (m_titleButton) {
        leftBound = m_titleButton->geometry().right() + 15;
    }
    
    int rightBound = m_btnMinimize->geometry().left() - 15;
    
    int availableWidth = qMax(0, rightBound - leftBound);
    int targetWidth = qMin(tabSize.width(), availableWidth);
    
    int idealX = (width() - tabSize.width()) / 2;
    int x = idealX;
    
    if (idealX < leftBound || idealX + tabSize.width() > rightBound) {
        x = leftBound + (availableWidth - targetWidth) / 2;
    }
    
    int y = (height() - tabSize.height()) / 2;
    m_tabBar->setGeometry(x, y, targetWidth, tabSize.height());
}

void CustomTitleBar::setTitle(const QString& title) {
    m_titleButton->setText(title);
}

void CustomTitleBar::updateMaximizeButton(bool isMaximized) {
    m_btnMaximize->setText(isMaximized ? "❐" : "□");
}

void CustomTitleBar::setMaximizeButtonHovered(bool hovered) {
    m_btnMaximize->setProperty("hovered", hovered);
    m_btnMaximize->style()->unpolish(m_btnMaximize);
    m_btnMaximize->style()->polish(m_btnMaximize);
}

void CustomTitleBar::setMaximizeButtonPressed(bool pressed) {
    m_btnMaximize->setProperty("pressed", pressed);
    m_btnMaximize->style()->unpolish(m_btnMaximize);
    m_btnMaximize->style()->polish(m_btnMaximize);
}

int CustomTitleBar::nativeHitTest(const QPoint& pos) const {
    // Minimize and Close: return 0 so Qt handles them via QPushButton signals
    if (m_btnMinimize->geometry().contains(pos)) return 0;
    if (m_btnClose->geometry().contains(pos)) return 0;
    
    // Maximize: return HTMAXBUTTON so Windows enables Snap Layouts flyout on hover
    if (m_btnMaximize->geometry().contains(pos)) return 9; // HTMAXBUTTON
    
    // Title Button (Menu): return 0 so Qt handles click to open menu
    if (m_titleButton && m_titleButton->geometry().contains(pos)) {
        return 0;
    }
    
    // Tab bar: return 0 so Qt handles tab clicks normally
    if (m_tabBar && m_tabBar->geometry().contains(pos)) {
        return 0;
    }
    
    // Check if it's over the menu bar
    if (m_menuBar && m_menuBar->geometry().contains(pos)) {
        return 0;
    }
    
    return 2; // HTCAPTION
}
