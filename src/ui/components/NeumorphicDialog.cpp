#include "NeumorphicDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>
#include "../../core/AppSettings.h"
#include "../../core/ThemePalette.h"

NeumorphicDialog::NeumorphicDialog(QWidget* parent) : QDialog(parent) {
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(45, 45, 45, 45);
    mainLayout->setSpacing(0);
    
    QWidget* card = new QWidget(this);
    card->setObjectName("NeumorphicCard");
    card->setAttribute(Qt::WA_StyledBackground, true);
    // Style handled by QSS #NeumorphicCard
    
    QVBoxLayout* cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(1, 1, 1, 1);
    cardLayout->setSpacing(0);
    
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(50);
    shadow->setColor(ThemePalette::color("qss_color_30", QColor(0, 0, 0, 180)));
    shadow->setOffset(0, 8);
    card->setGraphicsEffect(shadow);
    
    // Title bar
    QWidget* titleBar = new QWidget(card);
    titleBar->setObjectName("NeumorphicTitleBar");
    titleBar->setFixedHeight(30);
    // Style handled by QSS #NeumorphicTitleBar
    
    QHBoxLayout* titleLayout = new QHBoxLayout(titleBar);
    titleLayout->setContentsMargins(15, 0, 0, 0);
    
    m_titleLabel = new QLabel("");
    m_titleLabel->setObjectName("NeumorphicDialogTitle");
    titleLayout->addWidget(m_titleLabel);
    
    titleLayout->addStretch();
    
    m_closeButton = new QPushButton("×");
    m_closeButton->setFixedSize(30, 30);
    m_closeButton->setObjectName("NeumorphicDialogCloseBtn");
    connect(m_closeButton, &QPushButton::clicked, this, &NeumorphicDialog::onCloseClicked);
    titleLayout->addWidget(m_closeButton);
    
    cardLayout->addWidget(titleBar);
    
    // Content area
    QWidget* contentWidget = new QWidget(card);
    contentWidget->setStyleSheet("background: transparent;");
    m_contentLayout = new QVBoxLayout(contentWidget);
    m_contentLayout->setContentsMargins(20, 20, 20, 20);
    m_contentLayout->setSpacing(15);
    cardLayout->addWidget(contentWidget, 1);
    
    mainLayout->addWidget(card);
}

void NeumorphicDialog::setDialogTitle(const QString& title) {
    m_titleLabel->setText(title);
}

void NeumorphicDialog::onCloseClicked() {
    reject();
}

void NeumorphicDialog::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_dragPos = event->globalPosition().toPoint() - frameGeometry().topLeft();
        event->accept();
    }
}

void NeumorphicDialog::mouseMoveEvent(QMouseEvent* event) {
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPosition().toPoint() - m_dragPos);
        event->accept();
    }
}
