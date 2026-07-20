#include "WavetableViewerWidget.h"
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include "../../thirdparty/miniaudio/miniaudio.h"
#include "../../core/ThemePalette.h"

WavetableViewerWidget::WavetableViewerWidget(QWidget* parent) : QWidget(parent) {
    setMinimumHeight(120);
}

void WavetableViewerWidget::setWavFile(const QString& filepath) {
    if (m_filepath != filepath) {
        m_filepath = filepath;
        loadAudioFile();
        update();
    }
}

void WavetableViewerWidget::setPosition(double pos) {
    if (m_position != pos) {
        m_position = pos;
        update();
    }
}

void WavetableViewerWidget::loadAudioFile() {
    m_frames.clear();
    m_loaded = false;
    
    if (m_filepath.isEmpty()) return;
    
    ma_decoder decoder;
    ma_decoder_config config = ma_decoder_config_init(ma_format_f32, 1, 0); 
    
#ifdef _WIN32
    ma_result result = ma_decoder_init_file_w(m_filepath.toStdWString().c_str(), &config, &decoder);
#else
    ma_result result = ma_decoder_init_file(m_filepath.toUtf8().constData(), &config, &decoder);
#endif

    if (result != MA_SUCCESS) {
        return;
    }
    
    ma_uint64 totalFrames = 0;
    ma_decoder_get_length_in_pcm_frames(&decoder, &totalFrames);
    
    if (totalFrames == 0) {
        ma_decoder_uninit(&decoder);
        return;
    }
    
    std::vector<float> buffer(totalFrames);
    ma_uint64 framesRead = 0;
    ma_decoder_read_pcm_frames(&decoder, buffer.data(), totalFrames, &framesRead);
    ma_decoder_uninit(&decoder);
    
    // We want to extract approx 64 frames for the 3D view.
    // Assuming typical wavetable size is 2048 per frame
    const size_t FRAME_SIZE = 2048;
    int numFrames = totalFrames / FRAME_SIZE;
    
    if (numFrames > 128) {
        // Just take 128 evenly spaced frames
        numFrames = 128;
    }
    if (numFrames == 0 && totalFrames > 0) {
        numFrames = 1;
    }
    
    size_t step = totalFrames / numFrames;
    if (step == 0) step = 1;
    
    for (int i = 0; i < numFrames; ++i) {
        size_t start = i * step;
        std::vector<float> frameData(FRAME_SIZE, 0.0f);
        for (size_t s = 0; s < FRAME_SIZE; ++s) {
            if (start + s < buffer.size()) {
                frameData[s] = buffer[start + s];
            }
        }
        
        // Downsample the 2048 frame to say 128 points for faster drawing
        std::vector<float> visualFrame(128, 0.0f);
        size_t ds_step = FRAME_SIZE / 128;
        for (size_t s = 0; s < 128; ++s) {
            visualFrame[s] = frameData[s * ds_step];
        }
        
        m_frames.push_back(visualFrame);
    }
    
    m_loaded = true;
}

void WavetableViewerWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect rect = this->rect().adjusted(5, 5, -5, -5);
    
    painter.fillRect(rect, ThemePalette::color("qss_color_1", QColor(25, 25, 28)));
    painter.setPen(ThemePalette::color("qss_color_17", QColor(50, 50, 55)));
    painter.drawRect(rect);
    
    if (!m_loaded || m_frames.empty()) {
        painter.setPen(ThemePalette::color("qss_color_35", Qt::gray));
        painter.drawText(rect, Qt::AlignCenter, "No Wavetable Loaded");
        return;
    }
    
    // Draw 3D stacked frames
    int numFrames = m_frames.size();
    
    // Y offset per frame to create pseudo-3D
    double yOffsetPerFrame = (rect.height() * 0.4) / numFrames;
    double xOffsetPerFrame = (rect.width() * 0.2) / numFrames; // Slight skew
    
    double baseWidth = rect.width() - (xOffsetPerFrame * numFrames);
    double baseHeight = rect.height() * 0.5; // Amplitude scale
    
    int currentFrameIdx = qBound(0, (int)(m_position * (numFrames - 1)), numFrames - 1);
    
    // Draw from back to front
    for (int i = numFrames - 1; i >= 0; --i) {
        const auto& frame = m_frames[i];
        
        double yBase = rect.bottom() - (yOffsetPerFrame * i);
        double xStart = rect.left() + (xOffsetPerFrame * i);
        
        QPainterPath path;
        path.moveTo(xStart, yBase);
        
        int points = frame.size();
        for (int p = 0; p < points; ++p) {
            double nx = static_cast<double>(p) / (points - 1);
            double x = xStart + (nx * baseWidth);
            // frame[p] is -1 to 1
            double y = yBase - (frame[p] * baseHeight);
            path.lineTo(x, y);
        }
        
        QPainterPath fillPath = path;
        fillPath.lineTo(xStart + baseWidth, yBase);
        fillPath.lineTo(xStart, yBase);
        fillPath.closeSubpath();
        
        // Coloring
        if (i == currentFrameIdx) {
            QColor highlightCol = ThemePalette::color("qss_color_6", QColor(255, 100, 0));
            highlightCol.setAlpha(180);
            painter.fillPath(fillPath, highlightCol); // Highlight active frame
            painter.setPen(QPen(ThemePalette::color("qss_color_6", QColor(255, 150, 0)), 2));
            painter.drawPath(path);
        } else {
            QColor bgCol = ThemePalette::color("qss_color_35", QColor(0, 150, 255));
            bgCol.setAlpha(20 + (i * 2));
            painter.fillPath(fillPath, bgCol); // Background frames
            QColor bgPen = ThemePalette::color("qss_color_35", QColor(0, 150, 255));
            bgPen.setAlpha(100);
            painter.setPen(bgPen);
            painter.drawPath(path);
        }
    }
}
