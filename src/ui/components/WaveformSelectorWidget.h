#pragma once
#include <QWidget>
#include <QPushButton>
#include <QButtonGroup>
#include <QVector>
#include <QHBoxLayout>


#include "ui/components/UiComponentsLibExport.h"
class UICOMPONENTSLIB_EXPORT WaveformSelectorWidget : public QWidget {
    Q_OBJECT
public:
    explicit WaveformSelectorWidget(QWidget* parent = nullptr);
    
    void setWaveform(const QString& waveform);
    QString waveform() const { return m_currentWaveform; }
    
    QHBoxLayout* graphLayout() const { return m_graphLayout; }

signals:
    void waveformChanged(const QString& waveform);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    struct WaveformEntry {
        QString id;       // "sine", "saw", "square", "triangle", "noise", "pluck", "wavetable"
        QString label;    // Display label
    };
    
    QVector<WaveformEntry> m_waveforms;
    QButtonGroup* m_buttonGroup;
    QVector<QPushButton*> m_buttons;
    QString m_currentWaveform = "saw";
    QWidget* m_previewWidget;
    QHBoxLayout* m_graphLayout;
    
    void createButtons();
    
public:
    // Public so the inner WaveformPreviewWidget class can access it
    double waveformSample(const QString& type, double phase) const;
};
