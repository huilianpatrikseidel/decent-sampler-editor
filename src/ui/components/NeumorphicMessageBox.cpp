#include "NeumorphicMessageBox.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStyle>
#include <QApplication>

NeumorphicMessageBox::NeumorphicMessageBox(QWidget* parent) : NeumorphicDialog(parent), m_clickedButton(QMessageBox::NoButton) {
    m_textLabel = new QLabel("");
    m_textLabel->setStyleSheet(" font-size: 14px;");
    m_textLabel->setWordWrap(true);
    contentLayout()->addWidget(m_textLabel);
    
    m_infoLabel = new QLabel("");
    m_infoLabel->setStyleSheet(" font-size: 12px;");
    m_infoLabel->setWordWrap(true);
    m_infoLabel->setVisible(false);
    contentLayout()->addWidget(m_infoLabel);
    
    m_btnLayout = new QHBoxLayout();
    m_btnLayout->addStretch();
    m_btnLayout->setSpacing(10);
    contentLayout()->addLayout(m_btnLayout);
}

void NeumorphicMessageBox::setText(const QString& text) { m_textLabel->setText(text); }
void NeumorphicMessageBox::setInformativeText(const QString& text) { 
    m_infoLabel->setText(text); 
    m_infoLabel->setVisible(!text.isEmpty());
}

void NeumorphicMessageBox::setStandardButtons(QMessageBox::StandardButtons buttons) {
    QLayoutItem* item;
    while ((item = m_btnLayout->takeAt(0)) != nullptr) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }
    m_btnLayout->addStretch();
    
    auto addIf = [&](QMessageBox::StandardButton b, const QString& txt) {
        if (buttons & b) {
            QPushButton* btn = new QPushButton(txt);
            btn->setProperty("msgButtonType", static_cast<int>(b));
            btn->setObjectName("NeumorphicButton");
            btn->setMinimumWidth(80);
            btn->setFixedHeight(32);
            connect(btn, &QPushButton::clicked, this, [this, b]() {
                m_clickedButton = b;
                accept();
            });
            m_btnLayout->addWidget(btn);
        }
    };
    
    addIf(QMessageBox::Save, "Save");
    addIf(QMessageBox::Discard, "Discard");
    addIf(QMessageBox::Yes, "Yes");
    addIf(QMessageBox::No, "No");
    addIf(QMessageBox::Ok, "OK");
    addIf(QMessageBox::Cancel, "Cancel");
}

void NeumorphicMessageBox::setDefaultButton(QMessageBox::StandardButton button) {
    for (int i = 0; i < m_btnLayout->count(); ++i) {
        QWidget* w = m_btnLayout->itemAt(i)->widget();
        if (QPushButton* btn = qobject_cast<QPushButton*>(w)) {
            if (btn->property("msgButtonType").toInt() == static_cast<int>(button)) {
                btn->setObjectName("NeumorphicButtonAction");
                btn->style()->unpolish(btn);
                btn->style()->polish(btn);
                btn->setFocus();
            }
        }
    }
}

QMessageBox::StandardButton NeumorphicMessageBox::execDialog() {
    exec();
    return m_clickedButton;
}

QMessageBox::StandardButton NeumorphicMessageBox::question(QWidget* parent, const QString& title, const QString& text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultBtn) {
    NeumorphicMessageBox msg(parent);
    msg.setDialogTitle(title);
    msg.setText(text);
    msg.setStandardButtons(buttons);
    msg.setDefaultButton(defaultBtn);
    return msg.execDialog();
}

QMessageBox::StandardButton NeumorphicMessageBox::warning(QWidget* parent, const QString& title, const QString& text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultBtn) {
    NeumorphicMessageBox msg(parent);
    msg.setDialogTitle(title);
    msg.setText(text);
    msg.setStandardButtons(buttons);
    msg.setDefaultButton(defaultBtn);
    return msg.execDialog();
}

QMessageBox::StandardButton NeumorphicMessageBox::information(QWidget* parent, const QString& title, const QString& text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultBtn) {
    NeumorphicMessageBox msg(parent);
    msg.setDialogTitle(title);
    msg.setText(text);
    msg.setStandardButtons(buttons);
    msg.setDefaultButton(defaultBtn);
    return msg.execDialog();
}

QMessageBox::StandardButton NeumorphicMessageBox::critical(QWidget* parent, const QString& title, const QString& text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultBtn) {
    NeumorphicMessageBox msg(parent);
    msg.setDialogTitle(title);
    msg.setText(text);
    msg.setStandardButtons(buttons);
    msg.setDefaultButton(defaultBtn);
    return msg.execDialog();
}
