#include "FilterSectionWidget.h"
#include "SynthKnobWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QtMath>

// ---------------------------------------------------------------------------
// Inner widget that draws the frequency response curve
// ---------------------------------------------------------------------------
class FilterResponseWidget : public QWidget {
public:
    explicit FilterResponseWidget(FilterSectionWidget* owner, QWidget* parent = nullptr)
        : QWidget(parent), m_owner(owner)
    {
        setObjectName("NeumorphicInner");
        setAttribute(Qt::WA_StyledBackground, true);
        setFixedHeight(120);
        setMinimumWidth(200);
    }

protected:
    void paintEvent(QPaintEvent* /*event*/) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);

        QPainterPath clipPath;
        clipPath.addRoundedRect(this->rect(), 8, 8);
        p.setClipPath(clipPath);

        const QColor orange = isEnabled() ? QColor(0xFF, 0x66, 0x00) : QColor(0xFF, 0x66, 0x00, 80);
        const int w = width();
        const int h = height();
        const double pad = 6.0;
        const double amp = (h - 2.0 * pad) / 2.0;
        const double midY = h / 2.0;

        // Map cutoff (20-20000 Hz) into normalised x [0,1] using log scale
        double cutoff = m_owner->cutoff();
        double cutoffNorm = (qLn(cutoff) - qLn(20.0)) / (qLn(20000.0) - qLn(20.0));
        cutoffNorm = qBound(0.0, cutoffNorm, 1.0);

        double res = m_owner->resonance();
        // Scale resonance to a visual peak factor (1.0 = no peak, up to ~3.0)
        double peakFactor = 1.0 + (res - 0.1) / 9.9 * 2.0;

        QString type = m_owner->filterType().toLower();

        // Q factor (0.5 is very damped, higher is more resonant)
        // Map user resonance [0.1, 10.0] to Q [0.5, 10.0]
        double Q_factor = qMax(0.5, res);

        QPainterPath path;
        for (int px = 0; px < w; ++px) {
            double x = static_cast<double>(px) / w;  // 0..1
            
            // f_ratio = f / f_cutoff
            double f_ratio = qExp((x - cutoffNorm) * qLn(20000.0 / 20.0));
            double f_ratio_sq = f_ratio * f_ratio;
            
            // Biquad magnitude response denominator
            double denom = qSqrt(qPow(1.0 - f_ratio_sq, 2.0) + qPow(f_ratio / Q_factor, 2.0));
            if (denom < 1e-6) denom = 1e-6;
            
            double gain = 1.0;
            if (type == "lowpass") {
                gain = 1.0 / denom;
            } else if (type == "highpass") {
                gain = f_ratio_sq / denom;
            } else if (type == "bandpass") {
                gain = (f_ratio / Q_factor) / denom;
            } else if (type == "notch") {
                gain = qAbs(1.0 - f_ratio_sq) / denom;
            }
            
            // Convert to dB
            double gain_dB = 20.0 * qLn(gain + 1e-5) / qLn(10.0);
            
            // Map to Y: 0 dB is at 25% from the top
            double y_0dB = pad + (h - 2.0 * pad) * 0.25;
            // 48 dB dynamic range below 0 dB
            double px_per_dB = (h - pad - y_0dB) / 48.0;
            
            double y = y_0dB - gain_dB * px_per_dB;
            y = qBound(pad, y, h - pad);
            
            if (px == 0) path.moveTo(px, y);
            else         path.lineTo(px, y);
        }

        QPen pen(orange, 2.0);
        p.setPen(pen);
        p.drawPath(path);

        // Faint cutoff vertical line
        int cutoffX = static_cast<int>(cutoffNorm * w);
        QPen dashed(QColor(255, 255, 255, 40), 1.0, Qt::DashLine);
        p.setPen(dashed);
        p.drawLine(cutoffX, 0, cutoffX, h);
    }

private:
    FilterSectionWidget* m_owner;
    friend class FilterSectionWidget;
};

// ---------------------------------------------------------------------------
// FilterSectionWidget
// ---------------------------------------------------------------------------
FilterSectionWidget::FilterSectionWidget(QWidget* parent)
    : QWidget(parent)
{
    setStyleSheet("FilterSectionWidget { background: transparent; }");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(10);

    // --- Filter type buttons ---
    struct TypeEntry { QString id; QString label; };
    QVector<TypeEntry> types = {
        { "lowpass",  "LP" },
        { "highpass", "HP" },
        { "bandpass", "BP" },
        { "notch",    "NT" }
    };

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->setContentsMargins(0, 0, 0, 0);
    btnLayout->setSpacing(4);

    m_typeGroup = new QButtonGroup(this);
    m_typeGroup->setExclusive(true);
    
    btnLayout->addStretch(); // Center buttons

    for (int i = 0; i < types.size(); ++i) {
        QPushButton* btn = new QPushButton();
        btn->setObjectName("NeumorphicToggle");
        btn->setCheckable(true);
        btn->setFixedSize(44, 36);

        QString shape = types[i].id;
        auto createPix = [shape](bool checked) {
            int w = 24, h = 24;
            QPixmap pix(w, h);
            pix.fill(Qt::transparent);
            QPainter p(&pix);
            p.setRenderHint(QPainter::Antialiasing);
            QPen pen(checked ? QColor("#ff6600") : QColor("#888888"), 1.5);
            p.setPen(pen);
            
            QPainterPath path;
            if (shape == "lowpass") {
                path.moveTo(2, h/4); path.lineTo(w/2 - 2, h/4);
                path.cubicTo(w/2 + 2, h/4, w/2 + 4, h-4, w-2, h-4);
            } else if (shape == "highpass") {
                path.moveTo(2, h-4); path.cubicTo(w/2 - 4, h-4, w/2 - 2, h/4, w/2 + 2, h/4);
                path.lineTo(w-2, h/4);
            } else if (shape == "bandpass") {
                path.moveTo(2, h-4); path.cubicTo(w/2 - 4, h-4, w/2, 2, w/2 + 4, h-4);
                path.lineTo(w-2, h-4);
            } else if (shape == "notch") {
                path.moveTo(2, h/4); path.lineTo(w/4, h/4);
                path.cubicTo(w/2 - 4, h/4, w/2, h-4, w/2 + 4, h/4);
                path.lineTo(w-2, h/4);
            }
            p.drawPath(path);
            return pix;
        };
        
        QIcon icon;
        icon.addPixmap(createPix(false), QIcon::Normal, QIcon::Off);
        icon.addPixmap(createPix(true), QIcon::Normal, QIcon::On);
        btn->setIcon(icon);
        btn->setIconSize(QSize(16, 16));

        if (types[i].id == m_filterType)
            btn->setChecked(true);

        btn->setProperty("filterId", types[i].id);
        m_typeGroup->addButton(btn, i);
        m_typeButtons.append(btn);
        btnLayout->addWidget(btn);
    }
    btnLayout->addStretch();
    mainLayout->addLayout(btnLayout);

    // --- Response curve preview ---
    m_responseWidget = new FilterResponseWidget(this, this);
    mainLayout->addWidget(m_responseWidget);
    
    // --- Knobs ---
    QWidget* controlsBlock = new QWidget();
    controlsBlock->setObjectName("NeumorphicControls");
    controlsBlock->setAttribute(Qt::WA_StyledBackground, true);
    controlsBlock->setFixedHeight(85);
    QHBoxLayout* knobLayout = new QHBoxLayout(controlsBlock);
    knobLayout->setContentsMargins(0, 10, 0, 10);
    knobLayout->setSpacing(8);
    
    knobLayout->addStretch(); // Center the knobs

    // Cutoff knob
    QVBoxLayout* cutoffLayout = new QVBoxLayout();
    cutoffLayout->setSpacing(2);
    QLabel* cutoffLabel = new QLabel("Cutoff");
    cutoffLabel->setStyleSheet("font-size: 12px;  font-weight: 600;");
    cutoffLayout->addWidget(cutoffLabel, 0, Qt::AlignCenter);
    
    m_cutoffKnob = new SynthKnobWidget();
    m_cutoffKnob->setFixedSize(36, 36);
    m_cutoffKnob->setMinimum(20.0);
    m_cutoffKnob->setMaximum(20000.0);
    m_cutoffKnob->setLogarithmic(true);
    m_cutoffKnob->setDefaultValue(20000.0);
    m_cutoffKnob->setValue(20000.0);
    cutoffLayout->addWidget(m_cutoffKnob, 0, Qt::AlignCenter);
    
    QLabel* cutoffVal = new QLabel(QString::number(20000.0, 'f', 0));
    cutoffVal->setStyleSheet("font-size: 10px;  font-family: Consolas, monospace;");
    cutoffVal->setAlignment(Qt::AlignCenter);
    cutoffLayout->addWidget(cutoffVal, 0, Qt::AlignCenter);
    connect(m_cutoffKnob, &SynthKnobWidget::valueChanged, cutoffVal, [cutoffVal](double v) {
        cutoffVal->setText(QString::number(v, 'f', 0));
    });
    
    knobLayout->addLayout(cutoffLayout);

    // Resonance knob
    QVBoxLayout* resLayout = new QVBoxLayout();
    resLayout->setSpacing(2);
    QLabel* resLabel = new QLabel("Resonance");
    resLabel->setStyleSheet("font-size: 12px;  font-weight: 600;");
    resLayout->addWidget(resLabel, 0, Qt::AlignCenter);
    
    m_resonanceKnob = new SynthKnobWidget();
    m_resonanceKnob->setFixedSize(36, 36);
    m_resonanceKnob->setMinimum(0.1);
    m_resonanceKnob->setMaximum(10.0);
    m_resonanceKnob->setDefaultValue(1.0);
    m_resonanceKnob->setValue(1.0);
    resLayout->addWidget(m_resonanceKnob, 0, Qt::AlignCenter);
    
    QLabel* resVal = new QLabel(QString::number(1.0, 'f', 2));
    resVal->setStyleSheet("font-size: 10px;  font-family: Consolas, monospace;");
    resVal->setAlignment(Qt::AlignCenter);
    resLayout->addWidget(resVal, 0, Qt::AlignCenter);
    connect(m_resonanceKnob, &SynthKnobWidget::valueChanged, resVal, [resVal](double v) {
        resVal->setText(QString::number(v, 'f', 2));
    });
    
    knobLayout->addLayout(resLayout);
    
    // Env Depth knob
    QVBoxLayout* envLayout = new QVBoxLayout();
    envLayout->setSpacing(2);
    QLabel* envLabel = new QLabel("Env Depth");
    envLabel->setStyleSheet("font-size: 12px;  font-weight: 600;");
    envLayout->addWidget(envLabel, 0, Qt::AlignCenter);
    
    m_envDepthKnob = new SynthKnobWidget();
    m_envDepthKnob->setFixedSize(36, 36);
    m_envDepthKnob->setMinimum(-1.0);
    m_envDepthKnob->setMaximum(1.0);
    m_envDepthKnob->setDefaultValue(0.0);
    m_envDepthKnob->setValue(0.0);
    envLayout->addWidget(m_envDepthKnob, 0, Qt::AlignCenter);
    
    QLabel* envVal = new QLabel(QString::number(0.0, 'f', 2));
    envVal->setStyleSheet("font-size: 10px;  font-family: Consolas, monospace;");
    envVal->setAlignment(Qt::AlignCenter);
    envLayout->addWidget(envVal, 0, Qt::AlignCenter);
    connect(m_envDepthKnob, &SynthKnobWidget::valueChanged, envVal, [envVal](double v) {
        envVal->setText(QString::number(v, 'f', 2));
    });
    
    knobLayout->addLayout(envLayout);
    
    // KeyTrack knob
    QVBoxLayout* ktLayout = new QVBoxLayout();
    ktLayout->setSpacing(2);
    QLabel* ktLabel = new QLabel("KeyTrack");
    ktLabel->setStyleSheet("font-size: 12px;  font-weight: 600;");
    ktLayout->addWidget(ktLabel, 0, Qt::AlignCenter);
    
    m_keyTrackKnob = new SynthKnobWidget();
    m_keyTrackKnob->setFixedSize(36, 36);
    m_keyTrackKnob->setMinimum(0.0);
    m_keyTrackKnob->setMaximum(1.0);
    m_keyTrackKnob->setDefaultValue(0.0);
    m_keyTrackKnob->setValue(0.0);
    ktLayout->addWidget(m_keyTrackKnob, 0, Qt::AlignCenter);
    
    QLabel* ktVal = new QLabel(QString::number(0.0, 'f', 2));
    ktVal->setStyleSheet("font-size: 10px;  font-family: Consolas, monospace;");
    ktVal->setAlignment(Qt::AlignCenter);
    ktLayout->addWidget(ktVal, 0, Qt::AlignCenter);
    connect(m_keyTrackKnob, &SynthKnobWidget::valueChanged, ktVal, [ktVal](double v) {
        ktVal->setText(QString::number(v, 'f', 2));
    });
    
    knobLayout->addLayout(ktLayout);

    knobLayout->addStretch();
    mainLayout->addWidget(controlsBlock);

    // --- Connections ---
    connect(m_typeGroup, &QButtonGroup::idClicked, this, [this](int id) {
        if (id >= 0 && id < m_typeButtons.size()) {
            m_filterType = m_typeButtons[id]->property("filterId").toString();
            m_responseWidget->update();
            emitChanged();
        }
    });

    connect(m_cutoffKnob, &SynthKnobWidget::valueChanged, this, [this]() {
        m_responseWidget->update();
        emitChanged();
    });

    connect(m_resonanceKnob, &SynthKnobWidget::valueChanged, this, [this]() {
        m_responseWidget->update();
        emitChanged();
    });
    
    connect(m_envDepthKnob, &SynthKnobWidget::valueChanged, this, [this]() {
        emitChanged();
    });
    
    connect(m_keyTrackKnob, &SynthKnobWidget::valueChanged, this, [this]() {
        emitChanged();
    });
    
    mainLayout->addStretch();
}

// --- Public setters -----------------------------------------------------------

void FilterSectionWidget::setFilterType(const QString& type) {
    if (m_filterType == type)
        return;
    m_filterType = type;
    for (int i = 0; i < m_typeButtons.size(); ++i) {
        if (m_typeButtons[i]->property("filterId").toString() == type) {
            m_typeButtons[i]->setChecked(true);
            break;
        }
    }
    m_responseWidget->update();
}

void FilterSectionWidget::setCutoff(double freq) {
    m_cutoffKnob->setValue(freq);
    m_responseWidget->update();
}

void FilterSectionWidget::setResonance(double res) {
    m_resonanceKnob->setValue(res);
    m_responseWidget->update();
}

void FilterSectionWidget::setEnvDepth(double depth) {
    m_envDepthKnob->setValue(depth);
}

void FilterSectionWidget::setKeyTrack(double track) {
    m_keyTrackKnob->setValue(track);
}

void FilterSectionWidget::setEnabled(bool enabled) {
    m_enabled = enabled;
    QWidget::setEnabled(enabled);
}

void FilterSectionWidget::setModSourceProvider(ModSourceProvider provider) {
    m_cutoffKnob->setModSourceProvider(provider);
    m_resonanceKnob->setModSourceProvider(provider);
    m_envDepthKnob->setModSourceProvider(provider);
    m_keyTrackKnob->setModSourceProvider(provider);
}

// --- Public getters -----------------------------------------------------------

QString FilterSectionWidget::filterType() const {
    return m_filterType;
}

double FilterSectionWidget::cutoff() const {
    return m_cutoffKnob->value();
}

double FilterSectionWidget::resonance() const {
    return m_resonanceKnob->value();
}

double FilterSectionWidget::envDepth() const {
    return m_envDepthKnob->value();
}

double FilterSectionWidget::keyTrack() const {
    return m_keyTrackKnob->value();
}

// --- Protected / private ------------------------------------------------------

void FilterSectionWidget::paintEvent(QPaintEvent* event) {
    QWidget::paintEvent(event);
}

void FilterSectionWidget::emitChanged() {
    emit filterChanged(m_filterType, m_cutoffKnob->value(), m_resonanceKnob->value(), m_envDepthKnob->value(), m_keyTrackKnob->value());
}
