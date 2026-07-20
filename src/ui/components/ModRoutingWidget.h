#pragma once
#include <QWidget>
#include <QComboBox>
#include <QLabel>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QStackedWidget>
#include <QPushButton>
#include "../../core/models/GraphStructures.h"
#include "CurveGraphWidget.h"
#include "RangeSliderWidget.h"


#include "ui/components/UiComponentsLibExport.h"
class UICOMPONENTSLIB_EXPORT ModRoutingWidget : public QWidget {
    Q_OBJECT
public:
    explicit ModRoutingWidget(const ModRouting& routing, const QStringList& sources, const QStringList& destinations, bool lockDestination = false, QWidget* parent = nullptr);

    ModRouting getRouting() const;
    void setRouting(const ModRouting& r);
    void setParameterLimits(double minLimit, double maxLimit);
    void setBaseValue(double base);

signals:
    void routingChanged(const ModRouting& newRouting);
    void routingEditingFinished(const ModRouting& newRouting);
    void baseValueChanged(double newBase);
    void baseValueEditingFinished(double newBase);
    void deleteRequested();

private slots:
    void emitChange();
    void updateTranslationUI();

private:
    ModRouting m_routing;
    
    QComboBox* m_sourceCombo;
    QComboBox* m_destCombo;
    RangeSliderWidget* m_rangeSlider;
    
    QCheckBox* m_bipolarCheck;
    QComboBox* m_behaviorCombo;
    
    QComboBox* m_translationCombo;
    QStackedWidget* m_translationStack;
    
    CurveGraphWidget* m_curveGraph;
    QDoubleSpinBox* m_fixedValueSpin;
    
    double m_baseValue = 0.0;
    
    bool m_isUpdatingUI = false;
};
