#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <functional>
#include <QString>


#include "ui/mapper/MapperLibExport.h"
class MAPPERLIB_EXPORT SynthCardBase : public QWidget {
    Q_OBJECT
public:
    explicit SynthCardBase(const QString& titleText, std::function<void()> onDelete = nullptr, QWidget* parent = nullptr);
    QVBoxLayout* contentLayout() const;
    void setTitle(const QString& title);
    
    void setEnableToggle(bool show, bool isChecked = true);
    void setToggleState(bool checked);
    void setToggleEnabled(bool enabled);
    void setCardContentEnabled(bool enabled);

signals:
    void toggled(bool enabled);
    
protected:
    QLabel* m_titleLabel;
    class QPushButton* m_enableToggle;
    QWidget* m_contentContainer;
private:
    QVBoxLayout* m_contentLayout;
};
