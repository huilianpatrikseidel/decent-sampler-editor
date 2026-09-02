#pragma once
#include <QWidget>
#include <QSvgRenderer>
#include <QString>


#include "ui/components/UiComponentsLibExport.h"
class UICOMPONENTSLIB_EXPORT FaderWidget : public QWidget {
    Q_OBJECT
public:
    FaderWidget(bool isMaster = false, QWidget* parent = nullptr);
    
    // Channel volumes are decibels everywhere in the model, so the fader speaks dB on
    // its public API. The knob position stays normalised internally for painting and
    // dragging, which is what the track geometry actually needs.
    double valueDb() const;
    void setValueDb(double db);

    static constexpr double kMaxDb = 6.0;      // top of travel
    static constexpr double kFloorDb = -60.0;  // just above the bottom; position 0 is silence
    static constexpr double kUnityPosition = 0.8; // where 0 dB sits along the track

signals:
    void valueChanged(double newValueDb);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    
private:
    double m_position;        // 0.0 (silence) .. 1.0 (kMaxDb)
    double m_defaultPosition; // unity, so a double-click returns to 0 dB
    bool m_dragging;
    int m_lastY;
    int m_capHeight;
    QSvgRenderer m_trackRenderer;
    QSvgRenderer m_capRenderer;
    
    int trackTopMargin() const { return m_capHeight / 2; }
    int trackBottomMargin() const { return m_capHeight / 2; }
    int trackLength() const { return height() - trackTopMargin() - trackBottomMargin(); }
    int valueToY() const;
    void updateValueFromY(int y);
    void setPosition(double pos);
    static double positionToDb(double pos);
    static double dbToPosition(double db);
};
