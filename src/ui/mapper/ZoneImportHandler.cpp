#include "ZoneImportHandler.h"
#include "MappingEditorView.h"
#include "core/ProjectManager.h"
#include "commands/AddNodeCommand.h"
#include "commands/ZoneCommands.h"
#include "audio/AudioAnalyzer.h"
#include "core/models/AudioNodes.h"
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>
#include <QFileInfo>

#include "core/AutoMapper.h"
#include "MappingEditorContainer.h"

void ZoneImportHandler::handleDrop(MappingEditorView* view, ProjectManager* pm, AudioAnalyzer* analyzer, QUuid& currentSgId, QDropEvent* event, double noteWidth, double velocityHeight) {
    if (!event->mimeData()->hasUrls()) return;
    
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty()) return;
    
    QStringList filePaths;
    for (const QUrl& url : urls) {
        QString filePath = url.toLocalFile();
        if (filePath.endsWith(".wav", Qt::CaseInsensitive) || 
            filePath.endsWith(".flac", Qt::CaseInsensitive) || 
            filePath.endsWith(".mp3", Qt::CaseInsensitive)) {
            filePaths.append(filePath);
        }
    }
    
    if (!filePaths.isEmpty()) {
        MappingEditorContainer* container = nullptr;
        QWidget* parent = view->parentWidget();
        while (parent) {
            container = qobject_cast<MappingEditorContainer*>(parent);
            if (container) break;
            parent = parent->parentWidget();
        }
        
        if (container) {
            container->showImportOverlay(filePaths);
        }
    }
    
    event->acceptProposedAction();
}
