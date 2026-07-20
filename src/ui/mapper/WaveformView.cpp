#include "WaveformView.h"
#include <QPainter>
#include <QCursor>
#include <QMenu>
#include <QAction>
#include "../../core/ThemePalette.h"
#include "../../core/commands/ModifyZonePropertyCommand.h"
#include "WaveformAudioLoader.h"
#include "../../thirdparty/miniaudio/miniaudio.h"

WaveformView::WaveformView(ApplicationController* controller, QWidget* parent) 
    : QWidget(parent), m_controller(controller) 
{
    setMinimumHeight(100);
    setMouseTracking(true);
    m_loadingText = "No sample selected";
    
    m_audioLoader = new WaveformAudioLoader(this);
    
    connect(m_audioLoader, &WaveformAudioLoader::loadStarted, this, [this](const QString& text) {
        m_loadingText = text;
        update();
    });
    
    connect(m_audioLoader, &WaveformAudioLoader::loadFailed, this, [this](const QString& errorMsg) {
        m_loadingText = errorMsg;
        emit waveformLoaded();
        update();
    });
    
    connect(m_audioLoader, &WaveformAudioLoader::loadCompleted, this, [this](std::vector<float> peaks, long long totalFrames) {
        m_peaks = std::move(peaks);
        m_totalFrames = totalFrames;
        m_loadingText = "";
        emit waveformLoaded();
        update();
    });
}

WaveformView::~WaveformView() {
    m_audioLoader->cancel();
}

void WaveformView::setZone(const QUuid& sgId, int zoneIndex, const Zone* zone) {
    if (!zone) {
        m_currentSgId = QUuid();
        m_currentZoneIndex = -1;
        m_filepath = "";
        
        m_audioLoader->cancel();
        
        m_peaks.clear();
        m_totalFrames = 0;
        m_loadingText = "No sample selected";
        update();
        return;
    }
    
    m_currentSgId = sgId;
    m_currentZoneIndex = zoneIndex;
    m_zoneData = *zone;
    
    if (m_filepath == zone->samplePath) {
        update(); // just redraw bounds
        return; 
    }
    
    m_filepath = zone->samplePath;
    m_peaks.clear();
    m_totalFrames = 0;
    update();
    
    m_audioLoader->load(m_filepath);
}

int WaveformView::frameToX(long long frame) const {
    if (m_totalFrames == 0) return 0;
    return static_cast<int>((static_cast<double>(frame) / m_totalFrames) * width());
}

long long WaveformView::xToFrame(int x) const {
    if (width() == 0) return 0;
    long long frame = static_cast<long long>((static_cast<double>(x) / width()) * m_totalFrames);
    if (frame < 0) frame = 0;
    if (frame > m_totalFrames) frame = m_totalFrames;
    return frame;
}

bool WaveformView::isPointNearLine(int px, int lx) const {
    return std::abs(px - lx) < 5;
}

void WaveformView::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    
    painter.fillRect(rect(), ThemePalette::color("qss_color_22"));
    painter.setPen(QPen(ThemePalette::color("qss_color_9"), 1));
    painter.drawRect(rect().adjusted(0, 0, -1, -1));
    
    if (!m_loadingText.isEmpty()) {
        painter.setPen(ThemePalette::color("qss_color_17"));
        painter.drawText(rect(), Qt::AlignCenter, m_loadingText);
        return;
    }
    
    if (m_peaks.empty()) return;
    
    // 1. Draw Waveform
    painter.setPen(QPen(ThemePalette::color("qss_color_26", QColor(100, 200, 100, 180)), 1));
    int w = width();
    int h = height();
    int midY = h / 2;
    
    for (int x = 0; x < w; ++x) {
        int peakIdx = (x * m_peaks.size()) / w;
        if (peakIdx >= m_peaks.size()) break;
        float amp = m_peaks[peakIdx];
        int drawH = static_cast<int>(amp * midY);
        painter.drawLine(x, midY - drawH, x, midY + drawH);
    }
    
    // 2. Draw Regions Out of Bounds (Darkened)
    int sx = frameToX(m_zoneData.sampleStart);
    long long endFrame = m_zoneData.sampleEnd > 0 ? m_zoneData.sampleEnd : m_totalFrames;
    int ex = frameToX(endFrame);
    
    QColor dimColor = ThemePalette::color("qss_color_1");
    dimColor.setAlpha(150);
    painter.fillRect(0, 0, sx, h, dimColor);
    painter.fillRect(ex, 0, w - ex, h, dimColor);
    
    // 3. Draw Loop Area
    if (m_zoneData.loopEnabled) {
        int lsx = frameToX(m_zoneData.loopStart);
        int lex = frameToX(m_zoneData.loopEnd > 0 ? m_zoneData.loopEnd : m_totalFrames);
        QColor loopColor = ThemePalette::color("qss_color_23"); // typically a light orange/yellow transparent
        painter.fillRect(lsx, 0, lex - lsx, h, loopColor);
    }
    
    // 4. Draw Markers
    QPen sPen(ThemePalette::color("qss_color_6", QColor(255, 200, 50)), 2);
    painter.setPen(sPen);
    painter.drawLine(sx, 0, sx, h);
    painter.drawLine(ex, 0, ex, h);
    
    QPen lPen(ThemePalette::color("qss_color_26", QColor(100, 255, 100)), 2);
    painter.setPen(lPen);
    int lsx = frameToX(m_zoneData.loopStart);
    painter.drawLine(lsx, 0, lsx, h);
    
    QPen lePen(ThemePalette::color("qss_color_4", QColor(255, 100, 100)), 2);
    painter.setPen(lePen);
    int lex = frameToX(m_zoneData.loopEnd > 0 ? m_zoneData.loopEnd : m_totalFrames);
    painter.drawLine(lex, 0, lex, h);
    
    // Draw Crossfade Region
    if (m_zoneData.loopEnabled && m_zoneData.loopCrossfade > 0) {
        int cfx = frameToX(m_zoneData.loopEnd > 0 ? (m_zoneData.loopEnd - m_zoneData.loopCrossfade) : (m_totalFrames - m_zoneData.loopCrossfade));
        painter.fillRect(cfx, 0, lex - cfx, h, ThemePalette::color("qss_color_8", QColor(255, 255, 255, 40)));
        
        // Draw diagonal line to indicate crossfade
        painter.setPen(QPen(ThemePalette::color("qss_color_8", QColor(255, 255, 255, 100)), 1, Qt::DashLine));
        painter.drawLine(cfx, h, lex, 0);
    }
    
    // Draw Handles (little boxes at top/bottom of loop lines)
    auto drawHandle = [&](int x, const QColor& color, bool top) {
        painter.setBrush(color);
        painter.setPen(Qt::black);
        if (top) painter.drawRect(x - 5, 0, 10, 10);
        else painter.drawRect(x - 5, h - 10, 10, 10);
    };
    
    drawHandle(lsx, ThemePalette::color("qss_color_26", QColor(100, 255, 100)), true);
    drawHandle(lex, ThemePalette::color("qss_color_4", QColor(255, 100, 100)), false);
}

void WaveformView::mousePressEvent(QMouseEvent* event) {
    if (m_peaks.empty()) return;
    
    int x = event->pos().x();
    int sx = frameToX(m_zoneData.sampleStart);
    int ex = frameToX(m_zoneData.sampleEnd > 0 ? m_zoneData.sampleEnd : m_totalFrames);
    int lsx = frameToX(m_zoneData.loopStart);
    int lex = frameToX(m_zoneData.loopEnd > 0 ? m_zoneData.loopEnd : m_totalFrames);
    
    m_dragging = None;
    if (isPointNearLine(x, sx)) m_dragging = SampleStart;
    else if (isPointNearLine(x, ex)) m_dragging = SampleEnd;
    else if (isPointNearLine(x, lsx)) m_dragging = LoopStart;
    else if (isPointNearLine(x, lex)) m_dragging = LoopEnd;
}

void WaveformView::mouseMoveEvent(QMouseEvent* event) {
    if (m_peaks.empty()) return;
    
    int x = event->pos().x();
    
    if (m_dragging == None) {
        int sx = frameToX(m_zoneData.sampleStart);
        int ex = frameToX(m_zoneData.sampleEnd > 0 ? m_zoneData.sampleEnd : m_totalFrames);
        int lsx = frameToX(m_zoneData.loopStart);
        int lex = frameToX(m_zoneData.loopEnd > 0 ? m_zoneData.loopEnd : m_totalFrames);
        
        if (isPointNearLine(x, sx) || isPointNearLine(x, ex) || 
            isPointNearLine(x, lsx) || isPointNearLine(x, lex)) {
            setCursor(Qt::SizeHorCursor);
        } else {
            setCursor(Qt::ArrowCursor);
        }
    } else {
        long long frame = xToFrame(x);
        if (m_dragging == SampleStart) m_zoneData.sampleStart = frame;
        else if (m_dragging == SampleEnd) m_zoneData.sampleEnd = frame;
        else if (m_dragging == LoopStart) m_zoneData.loopStart = frame;
        else if (m_dragging == LoopEnd) m_zoneData.loopEnd = frame;
        update();
    }
}

void WaveformView::mouseReleaseEvent(QMouseEvent* event) {
    if (m_dragging != None) {
        long long frame = xToFrame(event->pos().x());
        
        // Zero-Crossing Detection using miniaudio in GUI thread (blocking, but for a tiny radius it's fast)
        if (frame > 0 && !m_filepath.isEmpty()) {
            ma_decoder decoder;
            ma_decoder_config config = ma_decoder_config_init(ma_format_f32, 1, 0);
#ifdef _WIN32
            if (ma_decoder_init_file_w(m_filepath.toStdWString().c_str(), &config, &decoder) == MA_SUCCESS) {
#else
            if (ma_decoder_init_file(m_filepath.toUtf8().constData(), &config, &decoder) == MA_SUCCESS) {
#endif
                long long searchRadius = 1000;
                long long startFrame = std::max(0LL, frame - searchRadius);
                ma_decoder_seek_to_pcm_frame(&decoder, startFrame);
                
                std::vector<float> chunk(searchRadius * 2);
                ma_uint64 framesRead = 0;
                ma_decoder_read_pcm_frames(&decoder, chunk.data(), chunk.size(), &framesRead);
                
                if (framesRead > 1) {
                    long long bestFrame = frame;
                    long long minDistance = searchRadius + 1;
                    
                    for (ma_uint64 i = 0; i < framesRead - 1; ++i) {
                        float a = chunk[i];
                        float b = chunk[i+1];
                        
                        if (a <= 0.0f && b > 0.0f) {
                            long long currentAbsoluteFrame = startFrame + i;
                            long long dist = std::abs(currentAbsoluteFrame - frame);
                            if (dist < minDistance) {
                                minDistance = dist;
                                bestFrame = currentAbsoluteFrame;
                            }
                        }
                    }
                    if (minDistance <= searchRadius) {
                        frame = bestFrame;
                    }
                }
                ma_decoder_uninit(&decoder);
            }
        }

        QString prop;
        long long oldV = 0;
        
        ProjectManager* pm = m_controller->getProjectManager();
        Node* node = pm->getNode(m_currentSgId);
        if (node && node->type == "SampleGroup") {
            SampleGroup* sg = static_cast<SampleGroup*>(node);
            if (m_currentZoneIndex >= 0 && m_currentZoneIndex < sg->zones.size()) {
                Zone& z = sg->zones[m_currentZoneIndex];
                
                if (m_dragging == SampleStart) { prop = "sampleStart"; oldV = z.sampleStart; }
                else if (m_dragging == SampleEnd) { prop = "sampleEnd"; oldV = z.sampleEnd; }
                else if (m_dragging == LoopStart) { prop = "loopStart"; oldV = z.loopStart; }
                else if (m_dragging == LoopEnd) { prop = "loopEnd"; oldV = z.loopEnd; }
                
                pm->getUndoStack()->push(new ModifyZonePropertyCommand(pm, m_currentSgId, m_currentZoneIndex, prop, oldV, frame));
            }
        }
        
        m_dragging = None;
    }
}
