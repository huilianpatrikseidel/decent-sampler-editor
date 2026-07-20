#include <QGuiApplication>
#include <QImage>
#include <QPainter>
#include <QSvgRenderer>

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    if (argc < 3) return 1;
    QSvgRenderer renderer(QString::fromUtf8(argv[1]));
    QImage image(256, 256, QImage::Format_ARGB32);
    image.fill(0x00000000); // transparent
    QPainter painter(&image);
    renderer.render(&painter);
    painter.end();
    
    // Qt can write ICO files
    image.save(QString::fromUtf8(argv[2]), "ICO");
    return 0;
}
