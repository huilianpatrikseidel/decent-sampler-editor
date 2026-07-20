#include "FilmstripRenderer.h"
#include <cmath>

FilmstripRenderer::FilmstripRenderer() : m_frameCount(1), m_isVertical(true), m_frameWidth(0), m_frameHeight(0) {}
FilmstripRenderer::~FilmstripRenderer() {}

void FilmstripRenderer::load(const QString& imagePath, int frameCount, bool isVertical) {
    if (m_pixmap.load(imagePath)) {
        m_frameCount = frameCount > 0 ? frameCount : 1;
        m_isVertical = isVertical;
        
        if (m_isVertical) {
            m_frameWidth = m_pixmap.width();
            m_frameHeight = m_pixmap.height() / m_frameCount;
        } else {
            m_frameWidth = m_pixmap.width() / m_frameCount;
            m_frameHeight = m_pixmap.height();
        }
    } else {
        m_pixmap = QPixmap();
        m_frameCount = 1;
    }
}

bool FilmstripRenderer::getFrame(double value, QPixmap& outPixmap, QRect& outSourceRect) const {
    if (!isValid()) return false;
    
    if (value < 0.0) value = 0.0;
    if (value > 1.0) value = 1.0;
    
    int frameIndex = std::round(value * (m_frameCount - 1));
    
    outPixmap = m_pixmap;
    
    if (m_isVertical) {
        outSourceRect = QRect(0, frameIndex * m_frameHeight, m_frameWidth, m_frameHeight);
    } else {
        outSourceRect = QRect(frameIndex * m_frameWidth, 0, m_frameWidth, m_frameHeight);
    }
    
    return true;
}
