#include "MenuManager.h"
#include <QMenuBar>
#include <QAction>
#include <QFileDialog>
#include "../components/NeumorphicMessageBox.h"
#include <QProgressDialog>
#include <QThread>
#include <QApplication>
#include "../../export/ExportWorker.h"
#include "../../core/ProjectManager.h"

MenuManager::MenuManager(QMainWindow* mainWindow, ProjectManager* pm) 
    : QObject(mainWindow), m_mainWindow(mainWindow), m_pm(pm) {}

void MenuManager::buildMenus() {
    QMenu* fileMenu = m_mainWindow->menuBar()->addMenu("File");
    fileMenu->setStyleSheet(qApp->styleSheet());
    
    QAction* actionSave = new QAction("Save Project", this);
    actionSave->setShortcut(QKeySequence::Save);
    connect(actionSave, &QAction::triggered, [this]() {
        if (m_pm->saveProject()) {
            // Success indication? Status bar message would be good here if we had one.
        } else {
            NeumorphicMessageBox::warning(m_mainWindow, "Save Failed", "Could not save the project.");
        }
    });
    fileMenu->addAction(actionSave);
    fileMenu->addSeparator();
    
    QAction* actionExportDs = new QAction("Export to DecentSampler (.dslibrary)", this);
    connect(actionExportDs, &QAction::triggered, [this]() {
        QString path = QFileDialog::getSaveFileName(m_mainWindow, "Export DecentSampler", "", "DecentSampler Library (*.dslibrary)");
        if (!path.isEmpty()) {
            if (!path.endsWith(".dslibrary")) path += ".dslibrary";
            
            QProgressDialog* progress = new QProgressDialog("Exporting bundle, please wait...", QString(), 0, 0, m_mainWindow);
            progress->setWindowModality(Qt::WindowModal);
            progress->setAttribute(Qt::WA_DeleteOnClose);
            progress->show();
            
            QThread* thread = new QThread(m_mainWindow);
            ExportWorker* worker = new ExportWorker(ExportWorker::ExportType::DecentSampler, m_pm, path);
            worker->moveToThread(thread);
            
            connect(thread, &QThread::started, worker, &ExportWorker::process);
            connect(worker, &ExportWorker::finished, m_mainWindow, [this, progress, path, thread, worker](bool success, const QString& err) {
                progress->close();
                if (err.isEmpty()) {
                    NeumorphicMessageBox::information(m_mainWindow, "Export Success", "Successfully exported to:\n" + path);
                } else {
                    NeumorphicMessageBox::critical(m_mainWindow, "Export Failed", err);
                }
                thread->quit();
                thread->wait();
                worker->deleteLater();
                thread->deleteLater();
            });
            thread->start();
        }
    });
    fileMenu->addAction(actionExportDs);
    
    QAction* actionExportSfz = new QAction("Export to SFZ (.sfz)", this);
    connect(actionExportSfz, &QAction::triggered, [this]() {
        QString path = QFileDialog::getSaveFileName(m_mainWindow, "Export SFZ", "", "SFZ Instrument (*.sfz)");
        if (!path.isEmpty()) {
            if (!path.endsWith(".sfz")) path += ".sfz";
            
            QProgressDialog* progress = new QProgressDialog("Exporting SFZ, please wait...", QString(), 0, 0, m_mainWindow);
            progress->setWindowModality(Qt::WindowModal);
            progress->setAttribute(Qt::WA_DeleteOnClose);
            progress->show();
            
            QThread* thread = new QThread(m_mainWindow);
            ExportWorker* worker = new ExportWorker(ExportWorker::ExportType::SFZ, m_pm, path);
            worker->moveToThread(thread);
            
            connect(thread, &QThread::started, worker, &ExportWorker::process);
            connect(worker, &ExportWorker::finished, m_mainWindow, [this, progress, path, thread, worker](bool success, const QString& err) {
                progress->close();
                if (err.isEmpty()) {
                    NeumorphicMessageBox::information(m_mainWindow, "Export Success", "Successfully exported to: " + path);
                } else {
                    NeumorphicMessageBox::critical(m_mainWindow, "Export Failed", err);
                }
                thread->quit();
                thread->wait();
                worker->deleteLater();
                thread->deleteLater();
            });
            thread->start();
        }
    });
    fileMenu->addAction(actionExportSfz);
    
    QMenu* editMenu = m_mainWindow->menuBar()->addMenu("Edit");
    editMenu->setStyleSheet(qApp->styleSheet());
    QAction* undoAction = m_pm->getUndoStack()->createUndoAction(m_mainWindow, "Undo");
    undoAction->setShortcuts(QKeySequence::Undo);
    QAction* redoAction = m_pm->getUndoStack()->createRedoAction(m_mainWindow, "Redo");
    redoAction->setShortcuts(QKeySequence::Redo);
    editMenu->addAction(undoAction);
    editMenu->addAction(redoAction);
}
