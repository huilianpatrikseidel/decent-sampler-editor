#pragma once
#include <QDialog>
#include <QString>
#include <QPoint>


#include "ui/components/UiComponentsLibExport.h"
class QLabel;
class QPushButton;
class QVBoxLayout;

class UICOMPONENTSLIB_EXPORT NeumorphicDialog : public QDialog {
    Q_OBJECT
public:
    explicit NeumorphicDialog(QWidget* parent = nullptr);
    void setDialogTitle(const QString& title);
    
    // Returns the layout where derived classes should add their content
    QVBoxLayout* contentLayout() const { return m_contentLayout; }

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    
    virtual void onCloseClicked();

private:
    QLabel* m_titleLabel;
    QPushButton* m_closeButton;
    QVBoxLayout* m_contentLayout;
    QPoint m_dragPos;
};
