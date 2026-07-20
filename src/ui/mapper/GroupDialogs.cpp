#include "GroupDialogs.h"
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include "../components/NeumorphicMessageBox.h"
#include "../../commands/ModifyPropertyCommand.h"
#include "../../commands/RemoveNodeCommand.h"

CreateGroupDialog::CreateGroupDialog(QWidget* parent) : NeumorphicDialog(parent) {
    setDialogTitle("Create New Group");
    setMinimumWidth(300);
    
    QFormLayout* layout = new QFormLayout();
    
    m_nameEdit = new QLineEdit();
    layout->addRow("Name:", m_nameEdit);
    
    m_typeCombo = new QComboBox();
    m_typeCombo->addItem("Sampler", false);
    m_typeCombo->addItem("Synthesizer", true);
    layout->addRow("Type:", m_typeCombo);
    
    QDialogButtonBox* bbox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(bbox, &QDialogButtonBox::accepted, this, &NeumorphicDialog::accept);
    connect(bbox, &QDialogButtonBox::rejected, this, &NeumorphicDialog::reject);
    layout->addRow(bbox);
    
    contentLayout()->addLayout(layout);
}

QString CreateGroupDialog::getName() const { return m_nameEdit->text(); }
bool CreateGroupDialog::isSynth() const { return m_typeCombo->currentData().toBool(); }

GroupManagerDialog::GroupManagerDialog(ProjectManager* pm, QWidget* parent) : NeumorphicDialog(parent), m_pm(pm) {
    setDialogTitle("Group Manager");
    resize(600, 400);
    
    QHBoxLayout* mainLayout = new QHBoxLayout();
    QSplitter* splitter = new QSplitter(Qt::Horizontal);
    
    m_list = new QListWidget();
    splitter->addWidget(m_list);
    
    QWidget* sidebar = new QWidget();
    QVBoxLayout* sideLayout = new QVBoxLayout(sidebar);
    
    QFormLayout* form = new QFormLayout();
    m_nameEdit = new QLineEdit();
    form->addRow("Name:", m_nameEdit);
    
    m_typeCombo = new QComboBox();
    m_typeCombo->addItem("Sampler", false);
    m_typeCombo->addItem("Synthesizer", true);
    m_typeCombo->setEnabled(false); // Locked after creation
    form->addRow("Type:", m_typeCombo);
    
    sideLayout->addLayout(form);
    sideLayout->addStretch();
    
    m_btnDelete = new QPushButton("Delete Group");
    m_btnDelete->setStyleSheet("background-color: transparent; border: 1px solid #ff4444; color: #ff4444; font-weight: bold; padding: 5px;");
    sideLayout->addWidget(m_btnDelete);
    
    splitter->addWidget(sidebar);
    splitter->setSizes({250, 350});
    mainLayout->addWidget(splitter);
    contentLayout()->addLayout(mainLayout);
    
    populateList();
    
    connect(m_list, &QListWidget::currentItemChanged, this, [this](QListWidgetItem* current, QListWidgetItem*) {
        if (!current) {
            m_currentId = QUuid();
            m_nameEdit->clear();
            m_nameEdit->setEnabled(false);
            m_typeCombo->setCurrentIndex(-1);
            m_btnDelete->setEnabled(false);
            return;
        }
        m_currentId = current->data(Qt::UserRole).toUuid();
        Node* n = m_pm->getNode(m_currentId);
        if (n && n->type == "SampleGroup") {
            SampleGroup* sg = static_cast<SampleGroup*>(n);
            m_isUpdating = true;
            m_nameEdit->setEnabled(true);
            m_btnDelete->setEnabled(true);
            m_nameEdit->setText(sg->name);
            m_typeCombo->setCurrentIndex(sg->isSynthContainer ? 1 : 0);
            m_isUpdating = false;
        }
    });
    
    connect(m_nameEdit, &QLineEdit::editingFinished, this, [this]() {
        if (m_isUpdating || m_currentId.isNull()) return;
        Node* n = m_pm->getNode(m_currentId);
        if (n && n->type == "SampleGroup") {
            SampleGroup* sg = static_cast<SampleGroup*>(n);
            if (sg->name != m_nameEdit->text()) {
                m_pm->getUndoStack()->push(new ModifyPropertyCommand(m_pm, m_currentId, "name", sg->name, m_nameEdit->text()));
            }
        }
    });
    
    connect(m_btnDelete, &QPushButton::clicked, this, [this]() {
        if (m_currentId.isNull()) return;
        SampleGroup* sg = static_cast<SampleGroup*>(m_pm->getNode(m_currentId));
        QMessageBox::StandardButton resBtn = NeumorphicMessageBox::question(this, "Delete Group",
            QString("Are you sure you want to delete group '%1'?").arg(sg ? sg->name : ""),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
            
        if (resBtn == QMessageBox::Yes) {
            QUuid idToDelete = m_currentId;
            m_list->setCurrentItem(nullptr); 
            
            m_pm->getUndoStack()->push(new RemoveNodeCommand(m_pm, idToDelete));
            QVector<QUuid> childrenToRemove;
            for (const auto& pair : m_pm->getAllNodes()) {
                if (pair.second->type == "SampleGroup") {
                    SampleGroup* child = static_cast<SampleGroup*>(pair.second.get());
                    if (child->synthParentId == idToDelete) childrenToRemove.append(child->id);
                }
            }
            for (const QUuid& cid : childrenToRemove) {
                m_pm->getUndoStack()->push(new RemoveNodeCommand(m_pm, cid));
            }
        }
    });
    
    connect(m_pm, &ProjectManager::nodeModified, this, [this](const QUuid& id, const QString&) {
        if (id == m_currentId) {
            Node* n = m_pm->getNode(m_currentId);
            if (n && n->type == "SampleGroup") {
                SampleGroup* sg = static_cast<SampleGroup*>(n);
                m_isUpdating = true;
                m_nameEdit->setText(sg->name);
                m_typeCombo->setCurrentIndex(sg->isSynthContainer ? 1 : 0);
                m_isUpdating = false;
            }
        }
        populateList();
    });
    connect(m_pm, &ProjectManager::nodeAdded, this, &GroupManagerDialog::populateList);
    connect(m_pm, &ProjectManager::nodeRemoved, this, &GroupManagerDialog::populateList);
}

void GroupManagerDialog::populateList() {
    QUuid selId = m_currentId;
    m_list->blockSignals(true);
    m_list->clear();
    for (const auto& pair : m_pm->getAllNodes()) {
        if (pair.second->type == "SampleGroup") {
            SampleGroup* sg = static_cast<SampleGroup*>(pair.second.get());
            if (!sg->synthParentId.isNull()) continue; // Skip oscillators
            
            QListWidgetItem* item = new QListWidgetItem(sg->name.isEmpty() ? "Group" : sg->name);
            item->setData(Qt::UserRole, sg->id);
            m_list->addItem(item);
            if (sg->id == selId) {
                m_list->setCurrentItem(item);
            }
        }
    }
    m_list->blockSignals(false);
}
