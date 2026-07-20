#include "SynthCardBase.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

SynthCardBase::SynthCardBase(const QString& titleText, std::function<void()> onDelete, QWidget* parent) 
    : QWidget(parent) {
    setObjectName("NeumorphicCard");
    setAttribute(Qt::WA_StyledBackground, true);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(15, 15, 15, 15);
    layout->setSpacing(10);
    
    QHBoxLayout* headerLayout = new QHBoxLayout();
    
    QWidget* leftContainer = new QWidget();
    leftContainer->setObjectName("CardLeftContainer");
    leftContainer->setStyleSheet("QWidget#CardLeftContainer { background-color: transparent; }");
    leftContainer->setFixedSize(24, 24);
    QHBoxLayout* leftLayout = new QHBoxLayout(leftContainer);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    
    m_enableToggle = new QPushButton();
    m_enableToggle->setObjectName("CardEnableToggle");
    m_enableToggle->setCheckable(true);
    m_enableToggle->setFixedSize(12, 12);
    m_enableToggle->setCursor(Qt::PointingHandCursor);
    connect(m_enableToggle, &QPushButton::toggled, this, &SynthCardBase::toggled);
    m_enableToggle->hide();
    
    leftLayout->addWidget(m_enableToggle, 0, Qt::AlignCenter);
    headerLayout->addWidget(leftContainer);
    
    m_titleLabel = new QLabel(titleText);
    m_titleLabel->setObjectName("ModuleHeader");
    m_titleLabel->setAlignment(Qt::AlignCenter);
    headerLayout->addWidget(m_titleLabel, 1);
    
    if (onDelete) {
        QPushButton* btnDel = new QPushButton("X");
        btnDel->setStyleSheet("background-color: transparent;  font-weight: bold; font-size: 16px; border: none;");
        btnDel->setFixedSize(24, 24);
        connect(btnDel, &QPushButton::clicked, this, onDelete);
        headerLayout->addWidget(btnDel, 0, Qt::AlignRight);
    }
    
    layout->addLayout(headerLayout);
    // Content container
    m_contentContainer = new QWidget();
    m_contentContainer->setObjectName("CardContentContainer");
    m_contentContainer->setStyleSheet("QWidget#CardContentContainer { background-color: transparent; }");
    m_contentLayout = new QVBoxLayout(m_contentContainer);
    m_contentLayout->setContentsMargins(0, 0, 0, 0); // Inner margins handled by cards if needed
    layout->addWidget(m_contentContainer);
}

QVBoxLayout* SynthCardBase::contentLayout() const {
    return m_contentLayout;
}

void SynthCardBase::setTitle(const QString& title) {
    if (m_titleLabel) m_titleLabel->setText(title);
}

void SynthCardBase::setEnableToggle(bool show, bool isChecked) {
    if (m_enableToggle) {
        m_enableToggle->setVisible(show);
        m_enableToggle->setChecked(isChecked);
    }
}

void SynthCardBase::setToggleState(bool checked) {
    if (m_enableToggle) {
        QSignalBlocker blocker(m_enableToggle);
        m_enableToggle->setChecked(checked);
    }
}

void SynthCardBase::setCardContentEnabled(bool enabled) {
    if (m_contentContainer) {
        m_contentContainer->setEnabled(enabled);
    }
}

void SynthCardBase::setToggleEnabled(bool enabled) {
    if (m_enableToggle) {
        m_enableToggle->setEnabled(enabled);
    }
}

