#ifndef GROUP_DIALOGS_H
#define GROUP_DIALOGS_H

#include "../components/NeumorphicDialog.h"
#include <QLineEdit>
#include <QComboBox>
#include <QListWidget>
#include <QPushButton>
#include "../../core/ProjectManager.h"


#include "ui/mapper/MapperLibExport.h"
class MAPPERLIB_EXPORT CreateGroupDialog : public NeumorphicDialog {
    Q_OBJECT
public:
    QLineEdit* m_nameEdit;
    QComboBox* m_typeCombo;
    
    explicit CreateGroupDialog(QWidget* parent = nullptr);
    
    QString getName() const;
    bool isSynth() const;
};

class MAPPERLIB_EXPORT GroupManagerDialog : public NeumorphicDialog {
    Q_OBJECT
public:
    ProjectManager* m_pm;
    QListWidget* m_list;
    QLineEdit* m_nameEdit;
    QComboBox* m_typeCombo;
    QPushButton* m_btnDelete;
    QUuid m_currentId;
    bool m_isUpdating = false;
    
    explicit GroupManagerDialog(ProjectManager* pm, QWidget* parent = nullptr);
    void populateList();
};

#endif // GROUP_DIALOGS_H
