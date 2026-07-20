#pragma once
#include <QWidget>
#include <QSvgRenderer>
#include <functional>
#include <QStringList>
#include "../../core/models/GraphStructures.h"


#include "ui/components/UiComponentsLibExport.h"
class UICOMPONENTSLIB_EXPORT SynthKnobWidget : public QWidget {
    Q_OBJECT
    Q_PROPERTY(double value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(double minimum READ minimum WRITE setMinimum)
    Q_PROPERTY(double maximum READ maximum WRITE setMaximum)
public:
    explicit SynthKnobWidget(QWidget* parent = nullptr);
    
    double value() const { return m_value; }
    void setValue(double val);
    
    double minimum() const { return m_minimum; }
    void setMinimum(double min);
    
    double maximum() const { return m_maximum; }
    void setMaximum(double max);
    
    double defaultValue() const { return m_defaultValue; }
    void setDefaultValue(double val) { m_defaultValue = val; }
    
    bool isLogarithmic() const { return m_isLogarithmic; }
    void setLogarithmic(bool log) { m_isLogarithmic = log; update(); }
    
    bool isBipolarDisplayEnabled() const { return m_isBipolarDisplayEnabled; }
    void setBipolarDisplayEnabled(bool enabled) { m_isBipolarDisplayEnabled = enabled; update(); }
    
    // Modulation properties
    QString modDestination() const { return m_modDestination; }
    void setModDestination(const QString& dest) { m_modDestination = dest; }
    
    ModRouting modRouting() const { return m_modRouting; }
    void setModRouting(const ModRouting& routing);
    
    using ModSourceProvider = std::function<QStringList()>;
    void setModSourceProvider(ModSourceProvider provider) { m_modSourceProvider = provider; }
    
signals:
    void valueChanged(double newValue);
    void modulationChanged(const ModRouting& routing);
    void modRoutingUpdatedExternally(const ModRouting& routing);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;
    
private:
    double m_value;
    double m_virtualValue;
    double m_defaultValue;
    double m_minimum;
    double m_maximum;
    bool m_isLogarithmic = false;
    bool m_isBipolarDisplayEnabled = true;
    
    bool m_dragging;
    bool m_modDragging;
    int m_lastX;
    int m_lastY;
    QSvgRenderer m_baseRenderer;
    QSvgRenderer m_indicatorRenderer;
    
    QString m_modDestination;
    ModRouting m_modRouting;
    ModSourceProvider m_modSourceProvider;
};
