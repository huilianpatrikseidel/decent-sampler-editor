#pragma once
#include <QWidget>
#include <QString>
#include <vector>


#include "ui/components/UiComponentsLibExport.h"
class UICOMPONENTSLIB_EXPORT WavetableViewerWidget : public QWidget {
    Q_OBJECT
public:
    explicit WavetableViewerWidget(QWidget* parent = nullptr);

    void setWavFile(const QString& filepath);
    void setPosition(double pos); // 0.0 to 1.0

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void loadAudioFile();

    QString m_filepath;
    double m_position = 0.0;
    
    // We will extract frames (e.g. 100 frames, each 2048 samples) for 3D drawing
    std::vector<std::vector<float>> m_frames;
    bool m_loaded = false;
};
