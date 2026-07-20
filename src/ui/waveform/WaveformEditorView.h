#pragma once
#include <QWidget>
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QMenu>
#include "WaveformCache.h"
#include "../../audio/AudioAnalyzer.h"

class WaveformEditorView : public QWidget {
    Q_OBJECT
public:
    explicit WaveformEditorView(QWidget* parent = nullptr);
    ~WaveformEditorView();

    void loadAudio(const QString& filepath);
    
signals:
    void loopPointsChanged(double startMs, double endMs);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;

private slots:
    void onCacheReady(bool success);
    void onLoopDetected(const QString& filepath, double startPct, double endPct);
    void onLoopDetectionFailed(const QString& filepath);

private:
    void updatePolygon();

    WaveformCache* m_cache = nullptr;
    AudioAnalyzer* m_analyzer = nullptr;
    QString m_currentFilepath;
    QPolygonF m_waveformPolygon;
    
    // UI Handles (0.0 to 1.0 representing normalized position)
    double m_loopStartPos = 0.4;
    double m_loopEndPos = 0.8;
    double m_inPos = 0.2;
    double m_outPos = 0.9;
    
    enum DragState { None, DraggingLoopStart, DraggingLoopEnd, DraggingIn, DraggingOut };
    DragState m_dragState = None;
    
};
