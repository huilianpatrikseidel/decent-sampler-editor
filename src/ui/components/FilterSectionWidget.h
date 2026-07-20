#pragma once
#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QButtonGroup>
#include <functional>

#include "ui/components/UiComponentsLibExport.h"
class SynthKnobWidget;

class UICOMPONENTSLIB_EXPORT FilterSectionWidget : public QWidget {
    Q_OBJECT
public:
    explicit FilterSectionWidget(QWidget* parent = nullptr);
    
    void setFilterType(const QString& type);
    void setCutoff(double freq);
    void setResonance(double res);
    void setEnvDepth(double depth);
    void setKeyTrack(double track);
    void setEnabled(bool enabled);
    
    QString filterType() const;
    double cutoff() const;
    double resonance() const;
    double envDepth() const;
    double keyTrack() const;
    
    using ModSourceProvider = std::function<QStringList()>;
    void setModSourceProvider(ModSourceProvider provider);
    
    SynthKnobWidget* cutoffKnob() const { return m_cutoffKnob; }
    SynthKnobWidget* resonanceKnob() const { return m_resonanceKnob; }

signals:
    void filterChanged(const QString& type, double cutoff, double resonance, double envDepth, double keyTrack);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QButtonGroup* m_typeGroup;
    QVector<QPushButton*> m_typeButtons;
    SynthKnobWidget* m_cutoffKnob;
    SynthKnobWidget* m_resonanceKnob;
    SynthKnobWidget* m_envDepthKnob;
    SynthKnobWidget* m_keyTrackKnob;
    QWidget* m_responseWidget;  // For frequency response curve
    QString m_filterType = "lowpass";
    bool m_enabled = true;
    
    void emitChanged();
};
