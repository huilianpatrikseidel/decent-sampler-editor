#include <QApplication>
#include <QThread>
#include <QIcon>
#include <QFontDatabase>
#include <QStyleHints>
#include "core/ApplicationController.h"
#include "core/InitWorker.h"
#include "ui/MainWindow.h"
#include "ui/SplashScreen.h"
#include "export/BundleExporter.h"
#include "ui/canvas/UiComponentItemFactory.h"
#include <QGraphicsScene>
#include <QPainter>
#include <QImage>

extern const char* APP_BUILD_NUMBER;

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    app.setApplicationName("Decent Sampler & SFZ IDE");
    app.setApplicationVersion(QString("1.0.%1").arg(APP_BUILD_NUMBER));
    app.setStyle("Fusion");
    app.styleHints()->setColorScheme(Qt::ColorScheme::Dark);

    // Load custom fonts
    QFontDatabase::addApplicationFont(":/ui/Jost-Regular.ttf");
    
    // Removed hardcoded QPalette, handled by ThemeManager

    // Show splash screen immediately
    SplashScreen splash;
    splash.show();
    app.processEvents();

    // Set up export rasterizer for headless background baking
    BundleExporter::setRasterizer([](const ProjectManager* pmTemp) -> QImage {
        QImage bgImage(812, 375, QImage::Format_ARGB32);
        bgImage.fill(QColor("#2B2B2B"));
        
        QGraphicsScene scene;
        scene.setSceneRect(0, 0, 812, 375);
        for (const auto& pair : pmTemp->getAllUiComponents()) {
            const auto& comp = pair.second;
            if (comp->type != "UiKnob") {
                auto item = UiComponentItemFactory::create(comp.get());
                scene.addItem(item);
            }
        }
        
        QPainter painter(&bgImage);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::TextAntialiasing);
        scene.render(&painter);
        painter.end();
        
        return bgImage;
    });

    // Create the controller (lightweight — no heavy init yet)
    ApplicationController controller;

    // Run initialization on a worker thread
    QThread workerThread;
    InitWorker worker(&controller);
    worker.moveToThread(&workerThread);

    // Connect worker signals to splash (cross-thread, queued connection)
    QObject::connect(&worker, &InitWorker::progressChanged, 
                     &splash, &SplashScreen::setProgress);
    QObject::connect(&splash, &SplashScreen::cancelRequested, 
                     &worker, &InitWorker::cancel);

    // When worker finishes: create MainWindow, close splash, quit thread
    MainWindow* mainWindow = nullptr;
    QObject::connect(&worker, &InitWorker::finished, &app, [&](bool success) {
        if (worker.isCancelled() || !success) {
            splash.close();
            workerThread.quit();
            return; // no main window, app will quit automatically if no active windows
        }
        
        mainWindow = new MainWindow(&controller);
        mainWindow->show();
        
        splash.close();
        workerThread.quit();
    });

    // Start work when thread starts
    QObject::connect(&workerThread, &QThread::started, &worker, &InitWorker::run);

    workerThread.start();
    int ret = app.exec();

    workerThread.wait();
    delete mainWindow;
    
    return ret;
}
