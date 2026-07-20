#include "ModRoutingWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <QStackedWidget>
#include <QDoubleSpinBox>
#include <QPushButton>
#include "RangeSliderWidget.h"
#include "CurveGraphWidget.h"

ModRoutingWidget::ModRoutingWidget(const ModRouting& routing, const QStringList& sources, const QStringList& destinations, bool lockDestination, QWidget* parent)
    : QWidget(parent), m_routing(routing) 
{
    // Make the entire widget a vertical card
    setObjectName("ModRoutingCard");
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("#ModRoutingCard { background: #222; border-radius: 6px; border: 1px solid #333; }");
    setFixedWidth(220);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(6);

    // Header: Source and Delete button
    QHBoxLayout* headerLayout = new QHBoxLayout();
    m_sourceCombo = new QComboBox();
    m_sourceCombo->setStyleSheet("QComboBox { background-color: #1a1a1c; border: 1px solid #a832ff; border-radius: 4px; padding: 2px; color: #d088ff; font-weight: bold; }");
    m_sourceCombo->addItem("None");
    for (const QString& src : sources) m_sourceCombo->addItem(src);
    m_sourceCombo->setCurrentText(m_routing.source.isEmpty() ? "None" : m_routing.source);
    headerLayout->addWidget(m_sourceCombo, 1);

    if (!lockDestination) {
        QPushButton* delBtn = new QPushButton("×");
        delBtn->setStyleSheet("color: #ff4444; font-weight: bold; border: none; background: transparent; font-size: 16px;");
        delBtn->setFixedSize(20, 20);
        connect(delBtn, &QPushButton::clicked, this, &ModRoutingWidget::deleteRequested);
        headerLayout->addWidget(delBtn);
    }
    mainLayout->addLayout(headerLayout);

    // Arrow pointing down
    QLabel* arrow = new QLabel("↓");
    arrow->setAlignment(Qt::AlignCenter);
    arrow->setStyleSheet("color: #666; font-size: 14px;");
    mainLayout->addWidget(arrow);

    // Destination
    if (!lockDestination) {
        m_destCombo = new QComboBox();
        m_destCombo->setStyleSheet("QComboBox { background-color: #1a1a1c; border: 1px solid #ff8800; border-radius: 4px; padding: 2px; color: #ffaa33; font-weight: bold; }");
        m_destCombo->addItems(destinations);
        for (const QString& d : destinations) {
            if (d.startsWith(m_routing.destination)) {
                m_destCombo->setCurrentText(d);
                break;
            }
        }
        mainLayout->addWidget(m_destCombo);
    } else {
        m_destCombo = nullptr;
    }

    // Range Slider
    m_rangeSlider = new RangeSliderWidget();
    m_rangeSlider->setRange(-1.0, 1.0); // Default, updated dynamically
    m_rangeSlider->setValues(0.0, m_routing.amount);
    mainLayout->addWidget(m_rangeSlider);

    QFrame* sep = new QFrame();
    sep->setFrameShape(QFrame::HLine);
    sep->setStyleSheet("color: #444;");
    mainLayout->addWidget(sep);

    // Advanced Settings Grid (Bipolar, Mode, Curve)
    QGridLayout* grid = new QGridLayout();
    grid->setContentsMargins(0, 0, 0, 0);
    grid->setSpacing(4);

    m_bipolarCheck = new QCheckBox("Bipolar");
    m_bipolarCheck->setChecked(m_routing.bipolar);
    m_bipolarCheck->setStyleSheet("color: #aaa; font-size: 10px;");
    grid->addWidget(m_bipolarCheck, 0, 0);

    m_behaviorCombo = new QComboBox();
    m_behaviorCombo->addItems({"add", "multiply", "set"});
    m_behaviorCombo->setCurrentText(m_routing.modBehavior);
    m_behaviorCombo->setStyleSheet("QComboBox { font-size: 10px; background-color: #1a1a1c; border: 1px solid #444; border-radius: 4px; padding: 1px; color: #ccc; }");
    grid->addWidget(new QLabel("Mode:"), 0, 1);
    grid->addWidget(m_behaviorCombo, 0, 2);

    m_translationCombo = new QComboBox();
    m_translationCombo->addItems({"linear", "table", "fixed_value"});
    m_translationCombo->setCurrentText(m_routing.translation);
    m_translationCombo->setStyleSheet("QComboBox { font-size: 10px; background-color: #1a1a1c; border: 1px solid #444; border-radius: 4px; padding: 1px; color: #ccc; }");
    grid->addWidget(new QLabel("Curve:"), 1, 1);
    grid->addWidget(m_translationCombo, 1, 2);
    
    mainLayout->addLayout(grid);

    // Stacked Widget for Translation UI
    m_translationStack = new QStackedWidget();
    
    QWidget* emptyPage = new QWidget();
    m_translationStack->addWidget(emptyPage); // 0 = linear
    
    m_curveGraph = new CurveGraphWidget();
    m_curveGraph->setTableString(m_routing.translationTable);
    m_translationStack->addWidget(m_curveGraph); // 1 = table
    
    QWidget* fixedValPage = new QWidget();
    QHBoxLayout* fvLayout = new QHBoxLayout(fixedValPage);
    fvLayout->setContentsMargins(0, 0, 0, 0);
    fvLayout->addWidget(new QLabel("Value:"));
    m_fixedValueSpin = new QDoubleSpinBox();
    m_fixedValueSpin->setRange(-100000, 100000);
    m_fixedValueSpin->setDecimals(4);
    m_fixedValueSpin->setValue(m_routing.translationValue);
    m_fixedValueSpin->setStyleSheet("QDoubleSpinBox { background-color: #1a1a1c; border: 1px solid #444; border-radius: 4px; color: #ccc; }");
    fvLayout->addWidget(m_fixedValueSpin);
    fvLayout->addStretch();
    m_translationStack->addWidget(fixedValPage); // 2 = fixed_value

    mainLayout->addWidget(m_translationStack);

    // Initial state
    updateTranslationUI();

    connect(m_sourceCombo, &QComboBox::currentTextChanged, this, [this]() { emitChange(); emit routingEditingFinished(getRouting()); });
    if (m_destCombo) {
        connect(m_destCombo, &QComboBox::currentTextChanged, this, [this](const QString& fullDest) {
            QString dest = fullDest.split(" - ").first();
            double minLim = -100.0, maxLim = 100.0;
            if (dest == "Pan") { minLim = -1.0; maxLim = 1.0; }
            else if (dest == "Volume") { minLim = -100.0; maxLim = 12.0; }
            else if (dest == "Pitch") { minLim = -2400.0; maxLim = 2400.0; }
            else if (dest == "Cutoff") { minLim = 20.0; maxLim = 20000.0; }
            else if (dest == "Resonance") { minLim = 0.0; maxLim = 100.0; }
            else if (dest.startsWith("AmpEnv")) { minLim = 0.0; maxLim = 10.0; }
            
            setParameterLimits(minLim, maxLim);
            emitChange();
            emit routingEditingFinished(getRouting());
        });
        
        // Trigger initially
        double minLim = -100.0, maxLim = 100.0;
        QString dest = m_destCombo->currentText().split(" - ").first();
        if (dest == "Pan") { minLim = -1.0; maxLim = 1.0; }
        else if (dest == "Volume") { minLim = -100.0; maxLim = 12.0; }
        else if (dest == "Pitch") { minLim = -2400.0; maxLim = 2400.0; }
        else if (dest == "Cutoff") { minLim = 20.0; maxLim = 20000.0; }
        else if (dest == "Resonance") { minLim = 0.0; maxLim = 100.0; }
        else if (dest.startsWith("AmpEnv")) { minLim = 0.0; maxLim = 10.0; }
        setParameterLimits(minLim, maxLim);
    }
    
    connect(m_rangeSlider, &RangeSliderWidget::rangeChanged, this, [this](double lower, double upper) {
        if (m_isUpdatingUI) return;
        
        double newBase = m_baseValue;
        double newAmount = m_routing.amount;
        
        if (m_bipolarCheck->isChecked()) {
            newBase = (lower + upper) / 2.0;
            newAmount = (upper - lower) / 2.0;
        } else {
            if (m_routing.amount < 0) {
                newBase = upper;
                newAmount = lower - upper; // negative amount
            } else {
                newBase = lower;
                newAmount = upper - lower;
            }
        }
        
        m_baseValue = newBase;
        m_routing.amount = newAmount;
        
        emit baseValueChanged(newBase);
        emitChange();
    });
    
    connect(m_rangeSlider, &RangeSliderWidget::rangeEditingFinished, this, [this](double lower, double upper) {
        if (m_isUpdatingUI) return;
        emit baseValueEditingFinished(m_baseValue);
        emit routingEditingFinished(getRouting());
    });
    
    connect(m_bipolarCheck, &QCheckBox::stateChanged, this, [this]() { emitChange(); emit routingEditingFinished(getRouting()); });
    connect(m_behaviorCombo, &QComboBox::currentTextChanged, this, [this]() { emitChange(); emit routingEditingFinished(getRouting()); });
    connect(m_translationCombo, &QComboBox::currentTextChanged, this, [this]() {
        updateTranslationUI();
        emitChange();
        emit routingEditingFinished(getRouting());
    });
    
    connect(m_curveGraph, &CurveGraphWidget::tableChanged, this, [this]() { emitChange(); emit routingEditingFinished(getRouting()); });
    connect(m_fixedValueSpin, &QDoubleSpinBox::valueChanged, this, [this]() { emitChange(); emit routingEditingFinished(getRouting()); });
}

void ModRoutingWidget::updateTranslationUI() {
    QString trans = m_translationCombo->currentText();
    if (trans == "table") {
        m_translationStack->setCurrentIndex(1);
        m_translationStack->setVisible(true);
    } else if (trans == "fixed_value") {
        m_translationStack->setCurrentIndex(2);
        m_translationStack->setVisible(true);
    } else {
        m_translationStack->setCurrentIndex(0);
        m_translationStack->setVisible(false);
    }
    
    // Adjust parent size to wrap tightly around visible elements
    if (QWidget* win = window()) {
        win->adjustSize();
    }
}

ModRouting ModRoutingWidget::getRouting() const {
    ModRouting r = m_routing;
    
    QString src = m_sourceCombo->currentText();
    if (src == "None") r.source = "";
    else r.source = src;

    if (m_destCombo) {
        QString fullDest = m_destCombo->currentText();
        r.destination = fullDest.split(" - ").first();
    }
    
    // Amount is handled by rangeSlider connections updating m_routing directly
    r.amount = m_routing.amount;
    r.bipolar = m_bipolarCheck->isChecked();
    r.modBehavior = m_behaviorCombo->currentText();
    r.translation = m_translationCombo->currentText();
    r.translationTable = m_curveGraph->getTableString();
    r.translationValue = m_fixedValueSpin->value();

    return r;
}

void ModRoutingWidget::emitChange() {
    if (m_isUpdatingUI) return;
    
    ModRouting r = getRouting();
    emit routingChanged(r);
}

void ModRoutingWidget::setRouting(const ModRouting& r) {
    m_isUpdatingUI = true;
    m_routing = r;
    m_sourceCombo->setCurrentText(r.source.isEmpty() ? "None" : r.source);
    
    if (m_destCombo) {
        bool found = false;
        for (int i = 0; i < m_destCombo->count(); ++i) {
            QString itemText = m_destCombo->itemText(i);
            if (itemText == r.destination || itemText.startsWith(r.destination + " - ")) {
                m_destCombo->setCurrentIndex(i);
                found = true;
                break;
            }
        }
        if (!found) m_destCombo->setCurrentText(r.destination);
    }
    
    if (r.bipolar) {
        m_rangeSlider->setValues(m_baseValue - r.amount, m_baseValue + r.amount);
    } else {
        double val1 = m_baseValue;
        double val2 = m_baseValue + r.amount;
        if (val1 > val2) std::swap(val1, val2);
        m_rangeSlider->setValues(val1, val2);
    }
    
    m_bipolarCheck->setChecked(r.bipolar);
    m_behaviorCombo->setCurrentText(r.modBehavior);
    m_translationCombo->setCurrentText(r.translation);
    m_curveGraph->setTableString(r.translationTable);
    m_fixedValueSpin->setValue(r.translationValue);
    updateTranslationUI();
    m_isUpdatingUI = false;
}

void ModRoutingWidget::setBaseValue(double base) {
    m_baseValue = base;
    setRouting(m_routing);
}

void ModRoutingWidget::setParameterLimits(double minLimit, double maxLimit) {
    m_rangeSlider->setRange(minLimit, maxLimit);
}
