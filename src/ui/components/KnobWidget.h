#pragma once
#include <QWidget>
#include <QSvgRenderer>
#include <functional>
#include <QStringList>


#include "ui/components/UiComponentsLibExport.h"
class UICOMPONENTSLIB_EXPORT KnobWidget : public QWidget {
    Q_OBJECT
    Q_PROPERTY(double value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(double minimum READ minimum WRITE setMinimum)
    Q_PROPERTY(double maximum READ maximum WRITE setMaximum)
public:
    explicit KnobWidget(QWidget* parent = nullptr);
    
    double value() const { return m_value; }
    void setValue(double val);
    
    double minimum() const { return m_minimum; }
    void setMinimum(double min);
    
    double maximum() const { return m_maximum; }
    void setMaximum(double max);
    
    double defaultValue() const { return m_defaultValue; }
    void setDefaultValue(double val) { m_defaultValue = val; }
    
    // Modulation properties
    QString modSource() const { return m_modSource; }
    void setModSource(const QString& source);
    
    double modAmount() const { return m_modAmount; }
    void setModAmount(double amount);
    
    using ModSourceProvider = std::function<QStringList()>;
    void setModSourceProvider(ModSourceProvider provider) { m_modSourceProvider = provider; }
    
signals:
    void valueChanged(double newValue);
    void modulationChanged(QString source, double amount);

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
    
    bool m_dragging;
    bool m_modDragging;
    int m_lastY;
    QSvgRenderer m_baseRenderer;
    QSvgRenderer m_indicatorRenderer;
    
    QString m_modSource;
    double m_modAmount; // from -1.0 to 1.0 (relative to range)
    ModSourceProvider m_modSourceProvider;
};
