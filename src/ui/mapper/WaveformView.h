#pragma once
#include <QWidget>
#include <QString>
#include <vector>
#include <QUuid>
#include <QMouseEvent>
#include "../../core/ProjectManager.h"
#include "../../core/ApplicationController.h"


#include "ui/mapper/MapperLibExport.h"
class WaveformAudioLoader;

class MAPPERLIB_EXPORT WaveformView : public QWidget {
    Q_OBJECT
public:
    explicit WaveformView(ApplicationController* controller, QWidget* parent = nullptr);
    ~WaveformView();

public slots:
    void setZone(const QUuid& sgId, int zoneIndex, const Zone* zone);

signals:
    void waveformLoaded();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    ApplicationController* m_controller;
    QUuid m_currentSgId;
    int m_currentZoneIndex = -1;
    Zone m_zoneData; 
    
    QString m_filepath;
    std::vector<float> m_peaks;
    long long m_totalFrames = 0;
    QString m_loadingText;

    WaveformAudioLoader* m_audioLoader;

    enum DragHandle { None, SampleStart, SampleEnd, LoopStart, LoopEnd };
    DragHandle m_dragging = None;
    
    bool isPointNearLine(int px, int lx) const;

    int frameToX(long long frame) const;
    long long xToFrame(int x) const;
};
