#include "WaveformEditorView.h"
#include <QPen>
#include <QBrush>
#include <QDebug>
#include <algorithm>
#include <cmath>
#include "../../core/ThemePalette.h"

WaveformEditorView::WaveformEditorView(QWidget* parent) : QWidget(parent) {
    m_cache = new WaveformCache(this);
    m_analyzer = new AudioAnalyzer(this);
    connect(m_cache, &WaveformCache::loadFinished, this, &WaveformEditorView::onCacheReady);
    connect(m_analyzer, &AudioAnalyzer::loopDetected, this, &WaveformEditorView::onLoopDetected);
    connect(m_analyzer, &AudioAnalyzer::loopDetectionFailed, this, &WaveformEditorView::onLoopDetectionFailed);
    setMinimumHeight(150);
}

WaveformEditorView::~WaveformEditorView() {}

void WaveformEditorView::loadAudio(const QString& filepath) {
    m_currentFilepath = filepath;
    m_cache->loadAudio(filepath);
    update();
}

void WaveformEditorView::onCacheReady(bool success) {
    if (success) {
        updatePolygon();
    }
    update();
}

void WaveformEditorView::updatePolygon() {
    if (m_cache->isReady()) {
        m_waveformPolygon = m_cache->getPolygon(width(), height());
    }
}

void WaveformEditorView::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    updatePolygon();
}

void WaveformEditorView::contextMenuEvent(QContextMenuEvent* event) {
    if (m_currentFilepath.isEmpty()) return;
    
    QMenu menu(this);
    QAction* autoLoopAction = menu.addAction("Auto-Loop (Find Best Phase Match)");
    
    QAction* selected = menu.exec(event->globalPos());
    if (selected == autoLoopAction) {
        // Run analyzer async
        m_analyzer->findBestLoopAsync(m_currentFilepath, 0.4, 0.95);
    }
}

void WaveformEditorView::onLoopDetected(const QString& filepath, double startPct, double endPct) {
    if (filepath == m_currentFilepath) {
        m_loopStartPos = startPct;
        m_loopEndPos = endPct;
        update();
        if (m_cache->isReady() && m_cache->getSampleRate() > 0) {
            double totalMs = (static_cast<double>(m_cache->getTotalFrames()) / m_cache->getSampleRate()) * 1000.0;
            emit loopPointsChanged(m_loopStartPos * totalMs, m_loopEndPos * totalMs);
        }
    }
}

void WaveformEditorView::onLoopDetectionFailed(const QString& filepath) {
    qDebug() << "Auto-Loop failed for" << filepath;
}

void WaveformEditorView::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Background
    painter.fillRect(rect(), ThemePalette::color("qss_color_1", QColor(30, 30, 35)));
    
    // Draw Waveform
    if (m_cache->isReady()) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(ThemePalette::color("qss_color_3", QColor(100, 150, 255, 180)));
        painter.drawPolygon(m_waveformPolygon);
    } else {
        painter.setPen(ThemePalette::color("qss_color_17", QColor(150, 150, 150)));
        painter.drawText(rect(), Qt::AlignCenter, "No Audio Selected");
        return;
    }
    
    // Draw Loop Handles
    int startX = m_loopStartPos * width();
    int endX = m_loopEndPos * width();
    int inX = m_inPos * width();
    int outX = m_outPos * width();
    
    // Shaded areas outside In/Out
    painter.fillRect(0, 0, inX, height(), ThemePalette::color("qss_color_30", QColor(0, 0, 0, 150)));
    painter.fillRect(outX, 0, width() - outX, height(), ThemePalette::color("qss_color_30", QColor(0, 0, 0, 150)));
    
    // Loop Handles (Green/Red)
    painter.setPen(QPen(ThemePalette::color("qss_color_26", Qt::green), 2));
    painter.drawLine(startX, 0, startX, height());
    
    painter.setPen(QPen(ThemePalette::color("qss_color_4", Qt::red), 2));
    painter.drawLine(endX, 0, endX, height());
    
    // In/Out Handles (Yellow/Orange)
    painter.setPen(QPen(ThemePalette::color("qss_color_3", Qt::yellow), 2, Qt::DashLine));
    painter.drawLine(inX, 0, inX, height());
    painter.setPen(QPen(ThemePalette::color("qss_color_6", QColor(255, 150, 0)), 2, Qt::DashLine));
    painter.drawLine(outX, 0, outX, height());
    
    // Crossfade Visual Curve
    painter.setPen(QPen(ThemePalette::color("qss_color_35", Qt::cyan), 1, Qt::DashLine));
    QPainterPath path;
    path.moveTo(endX, height()/2);
    path.quadTo((startX + endX)/2.0, height()/2 - 30, startX, height()/2);
    painter.drawPath(path);
    
    // Labels
    painter.setPen(ThemePalette::color("qss_color_8", Qt::white));
    painter.drawText(startX + 5, 15, "Loop Start");
    painter.drawText(endX - 60, 15, "Loop End");
    painter.drawText(inX + 5, height() - 5, "In Point");
    painter.drawText(outX - 55, height() - 5, "Out Point");
}

void WaveformEditorView::mousePressEvent(QMouseEvent* event) {
    int startX = m_loopStartPos * width();
    int endX = m_loopEndPos * width();
    int inX = m_inPos * width();
    int outX = m_outPos * width();
    int margin = 5;
    
    if (std::abs(event->pos().x() - startX) <= margin) m_dragState = DraggingLoopStart;
    else if (std::abs(event->pos().x() - endX) <= margin) m_dragState = DraggingLoopEnd;
    else if (std::abs(event->pos().x() - inX) <= margin) m_dragState = DraggingIn;
    else if (std::abs(event->pos().x() - outX) <= margin) m_dragState = DraggingOut;
}

void WaveformEditorView::mouseMoveEvent(QMouseEvent* event) {
    if (m_dragState == None) return;
    
    double normalizedPos = static_cast<double>(event->pos().x()) / width();
    normalizedPos = std::clamp(normalizedPos, 0.0, 1.0);
    
    if (m_dragState == DraggingLoopStart) {
        m_loopStartPos = std::min(normalizedPos, m_loopEndPos - 0.01);
    } else if (m_dragState == DraggingLoopEnd) {
        m_loopEndPos = std::max(normalizedPos, m_loopStartPos + 0.01);
    } else if (m_dragState == DraggingIn) {
        m_inPos = std::min(normalizedPos, m_outPos - 0.01);
    } else if (m_dragState == DraggingOut) {
        m_outPos = std::max(normalizedPos, m_inPos + 0.01);
    }
    
    update();
}

void WaveformEditorView::mouseReleaseEvent(QMouseEvent* event) {
    Q_UNUSED(event);
    if (m_dragState != None) {
        
        if (m_cache->isReady() && !m_cache->getPeaks().isEmpty()) {
            auto peaks = m_cache->getPeaks();
            auto snapToZero = [&](double& pos) {
                int peakIdx = std::clamp(static_cast<int>(pos * peaks.size()), 0, (int)peaks.size() - 1);
                int searchRadius = 50; 
                int bestIdx = peakIdx;
                double minAmp = 999.0;
                
                for (int i = std::max(0, peakIdx - searchRadius); i <= std::min((int)peaks.size() - 1, peakIdx + searchRadius); ++i) {
                    if (peaks[i].minVal <= 0.0f && peaks[i].maxVal >= 0.0f) {
                        double amp = std::abs(peaks[i].maxVal) + std::abs(peaks[i].minVal);
                        if (amp < minAmp) {
                            minAmp = amp;
                            bestIdx = i;
                        }
                    }
                }
                pos = static_cast<double>(bestIdx) / peaks.size();
            };
            
            if (m_dragState == DraggingLoopStart) snapToZero(m_loopStartPos);
            else if (m_dragState == DraggingLoopEnd) snapToZero(m_loopEndPos);
            else if (m_dragState == DraggingIn) snapToZero(m_inPos);
            else if (m_dragState == DraggingOut) snapToZero(m_outPos);
            
            update();
        }
        
        m_dragState = None;
        if (m_cache->isReady() && m_cache->getSampleRate() > 0) {
            double totalMs = (static_cast<double>(m_cache->getTotalFrames()) / m_cache->getSampleRate()) * 1000.0;
            emit loopPointsChanged(m_loopStartPos * totalMs, m_loopEndPos * totalMs);
        }
    }
}
