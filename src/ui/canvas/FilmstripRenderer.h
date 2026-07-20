#pragma once
#include <QPixmap>
#include <QString>
#include <QRect>

#include "../designer/UiDesignerExport.h"

class UIDESIGNER_EXPORT FilmstripRenderer {
public:
    FilmstripRenderer();
    ~FilmstripRenderer();

    void load(const QString& imagePath, int frameCount, bool isVertical = true);
    
    // Returns the QPixmap and the exact QRect to draw based on a value from 0.0 to 1.0
    // Returns true if successfully loaded and calculated
    bool getFrame(double value, QPixmap& outPixmap, QRect& outSourceRect) const;
    
    bool isValid() const { return !m_pixmap.isNull() && m_frameCount > 0; }
    int getFrameCount() const { return m_frameCount; }

private:
    QPixmap m_pixmap;
    int m_frameCount;
    bool m_isVertical;
    int m_frameWidth;
    int m_frameHeight;
};
