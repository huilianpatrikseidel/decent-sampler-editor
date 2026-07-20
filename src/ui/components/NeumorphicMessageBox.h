#pragma once
#include "NeumorphicDialog.h"
#include <QMessageBox>
#include <QString>


#include "ui/components/UiComponentsLibExport.h"
class QLabel;
class QHBoxLayout;

class UICOMPONENTSLIB_EXPORT NeumorphicMessageBox : public NeumorphicDialog {
    Q_OBJECT
public:
    explicit NeumorphicMessageBox(QWidget* parent = nullptr);

    void setText(const QString& text);
    void setInformativeText(const QString& text);
    void setStandardButtons(QMessageBox::StandardButtons buttons);
    void setDefaultButton(QMessageBox::StandardButton button);
    
    QMessageBox::StandardButton execDialog();

    static QMessageBox::StandardButton question(QWidget* parent, const QString& title, const QString& text, QMessageBox::StandardButtons buttons = QMessageBox::Yes | QMessageBox::No, QMessageBox::StandardButton defaultBtn = QMessageBox::No);
    static QMessageBox::StandardButton warning(QWidget* parent, const QString& title, const QString& text, QMessageBox::StandardButtons buttons = QMessageBox::Ok, QMessageBox::StandardButton defaultBtn = QMessageBox::Ok);
    static QMessageBox::StandardButton information(QWidget* parent, const QString& title, const QString& text, QMessageBox::StandardButtons buttons = QMessageBox::Ok, QMessageBox::StandardButton defaultBtn = QMessageBox::Ok);
    static QMessageBox::StandardButton critical(QWidget* parent, const QString& title, const QString& text, QMessageBox::StandardButtons buttons = QMessageBox::Ok, QMessageBox::StandardButton defaultBtn = QMessageBox::Ok);

private:
    QLabel* m_textLabel;
    QLabel* m_infoLabel;
    QHBoxLayout* m_btnLayout;
    QMessageBox::StandardButton m_clickedButton;
};
