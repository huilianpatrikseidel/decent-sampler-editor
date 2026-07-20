#include "PresetListWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QInputDialog>
#include <QApplication>
#include "../../core/ApplicationController.h"
#include "../../core/ProjectManager.h"
#include "../../core/PresetManager.h"
#include "../canvas/InstrumentCanvasView.h"

PresetListWidget::PresetListWidget(ApplicationController* controller, InstrumentCanvasView* canvas, QWidget* parent)
    : QWidget(parent), m_controller(controller), m_canvas(canvas) {
    
    setMinimumWidth(250);
    QVBoxLayout* layout = new QVBoxLayout(this);
    
    m_presetList = new QListWidget();
    m_presetList->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    layout->addWidget(m_presetList);
    
    QHBoxLayout* btnLayout = new QHBoxLayout();
    m_btnAddPreset = new QPushButton("+");
    m_btnRemovePreset = new QPushButton("-");
    btnLayout->addWidget(m_btnAddPreset);
    btnLayout->addWidget(m_btnRemovePreset);
    layout->addLayout(btnLayout);
    
    connect(m_btnAddPreset, &QPushButton::clicked, [this]() {
        bool ok;
        QString text = QInputDialog::getText(this, "New Preset", "Preset Name:", QLineEdit::Normal, "New Preset", &ok);
        if (ok && !text.isEmpty()) {
            m_controller->getProjectManager()->getPresetManager()->addPreset(text);
            updateList();
        }
    });
    
    connect(m_btnRemovePreset, &QPushButton::clicked, [this]() {
        int idx = m_presetList->currentRow();
        if (idx >= 0 && m_controller->getProjectManager()->getPresetManager()->getPresetCount() > 1) {
            m_controller->getProjectManager()->getPresetManager()->removePreset(idx);
            updateList();
        }
    });

    connect(m_presetList, &QListWidget::currentRowChanged, [this](int currentRow) {
        if (currentRow >= 0 && m_presetList->signalsBlocked() == false) {
            QApplication::setOverrideCursor(Qt::WaitCursor);
            double vx, vy, vzoom;
            m_canvas->getViewport(vx, vy, vzoom);
            m_controller->getProjectManager()->getUiState()->setViewport(vx, vy, vzoom);
            
            m_controller->getProjectManager()->getPresetManager()->switchPreset(currentRow);
            
            m_controller->getProjectManager()->getUiState()->getViewport(vx, vy, vzoom);
            m_canvas->setViewport(vx, vy, vzoom);
            QApplication::restoreOverrideCursor();
        }
    });
    
    connect(m_presetList, &QListWidget::itemChanged, [this](QListWidgetItem* item) {
        if (!m_presetList->signalsBlocked()) {
            int row = m_presetList->row(item);
            if (row >= 0) {
                m_controller->getProjectManager()->getPresetManager()->renamePreset(row, item->text());
            }
        }
    });
    
    connect(m_controller->getProjectManager(), &ProjectManager::projectLoaded, this, &PresetListWidget::updateList);
    
    updateList();
}

void PresetListWidget::updateList() {
    if (!m_presetList) return;
    m_presetList->blockSignals(true);
    m_presetList->clear();
    
    ProjectManager* pm = m_controller->getProjectManager();
    int count = pm->getPresetManager()->getPresetCount();
    for (int i = 0; i < count; ++i) {
        QListWidgetItem* item = new QListWidgetItem(pm->getPresetManager()->getPresetName(i));
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        m_presetList->addItem(item);
    }
    m_presetList->setCurrentRow(pm->getPresetManager()->getActivePresetIndex());
    m_btnRemovePreset->setEnabled(count > 1);
    m_presetList->blockSignals(false);
}
