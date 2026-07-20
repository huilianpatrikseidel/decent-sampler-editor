#include "WaveformSelectorWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QPainterPath>
#include <QtMath>
#include "../../core/ThemePalette.h"

// ---------------------------------------------------------------------------
// Preview widget that draws the waveform
// ---------------------------------------------------------------------------
class WaveformPreviewWidget : public QWidget {
public:
    explicit WaveformPreviewWidget(WaveformSelectorWidget* owner, QWidget* parent = nullptr)
        : QWidget(parent), m_owner(owner)
    {
        setObjectName("NeumorphicInner");
        setAttribute(Qt::WA_StyledBackground, true);
        setFixedHeight(140);
        setMinimumWidth(200);
    }

protected:
    void paintEvent(QPaintEvent* /*event*/) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);

        QPainterPath clipPath;
        clipPath.addRoundedRect(this->rect(), 8, 8);
        p.setClipPath(clipPath);

        const QString waveId = m_owner->waveform();
        QColor accent = ThemePalette::color("qss_color_6", QColor(0xFF, 0x66, 0x00));
        QColor orange = accent;
        if (!isEnabled()) orange.setAlpha(80);
        const int w = width();
        const int h = height();
        const double midY = h / 2.0;
        const double amp  = (h - 20) / 2.0;   // leave 10 px padding top/bottom
        const int numCycles = 2;

        if (waveId == "wavetable") {
            // Draw 3 offset sine waves to suggest wavetable frames
            int alphaScale = isEnabled() ? 1 : 0;
            QColor dimOrange = accent;
            dimOrange.setAlpha(isEnabled() ? 60 : 20);
            QColor midOrange = accent;
            midOrange.setAlpha(isEnabled() ? 120 : 40);

            auto drawSineOffset = [&](double yOff, double phaseOff, const QColor& col) {
                QPainterPath path;
                for (int x = 0; x < w; ++x) {
                    double phase = (static_cast<double>(x) / w) * numCycles;
                    double angle = (phase + phaseOff) * 2.0 * M_PI;
                    double y = (midY + yOff) - qSin(angle) * (amp * 0.6);
                    if (x == 0) path.moveTo(x, y);
                    else        path.lineTo(x, y);
                }
                QPen pen(col, 1.5);
                p.setPen(pen);
                p.drawPath(path);
            };

            drawSineOffset(-8, 0.3, dimOrange);
            drawSineOffset(-4, 0.15, midOrange);
            drawSineOffset(0, 0.0, orange);
        } else {
            // Normal waveform drawing – 2 cycles
            QPainterPath path;
            for (int x = 0; x < w; ++x) {
                double phase = (static_cast<double>(x) / w) * numCycles;
                double sample = m_owner->waveformSample(waveId, phase);
                double y = midY - sample * amp;
                if (x == 0) path.moveTo(x, y);
                else        path.lineTo(x, y);
            }
            QPen pen(orange, 2.0);
            p.setPen(pen);
            p.drawPath(path);
        }

        // Faint centre line
        QPen centrePen(ThemePalette::color("qss_color_23", QColor(255, 255, 255, 30)), 1.0, Qt::DashLine);
        p.setPen(centrePen);
        p.drawLine(0, static_cast<int>(midY), w, static_cast<int>(midY));
    }

private:
    WaveformSelectorWidget* m_owner;

    // Allow the owner's helper to be called from here
    friend class WaveformSelectorWidget;
};

// ---------------------------------------------------------------------------
// WaveformSelectorWidget
// ---------------------------------------------------------------------------
WaveformSelectorWidget::WaveformSelectorWidget(QWidget* parent)
    : QWidget(parent)
{
    m_waveforms = {
        { "sine",      "SIN" },
        { "saw",       "SAW" },
        { "square",    "SQR" },
        { "triangle",  "TRI" },
        { "noise",     "NOI" },
        { "pluck",     "PLK" },
        { "wavetable", "WT"  }
    };

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(6);

    // --- Button row ---
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->setContentsMargins(0, 0, 0, 0);
    btnLayout->setSpacing(4);

    m_buttonGroup = new QButtonGroup(this);
    m_buttonGroup->setExclusive(true);
    
    btnLayout->addStretch(); // Center buttons

    for (int i = 0; i < m_waveforms.size(); ++i) {
        QPushButton* btn = new QPushButton();
        btn->setObjectName("NeumorphicToggle");
        btn->setCheckable(true);
        btn->setFixedSize(44, 36);

        QString shape = m_waveforms[i].id;
        auto createPix = [this, shape](bool checked) {
            int w = 24, h = 24;
            QPixmap pix(w, h);
            pix.fill(Qt::transparent);
            QPainter p(&pix);
            p.setRenderHint(QPainter::Antialiasing);
            QColor onCol = ThemePalette::color("qss_color_6", QColor("#ff6600"));
            if (!isEnabled()) onCol.setAlpha(80);
            QPen pen(checked ? onCol : (isEnabled() ? ThemePalette::color("qss_color_17", QColor("#888888")) : ThemePalette::color("qss_color_25", QColor("#444444"))), 1.5);
            p.setPen(pen);
            
            QPainterPath path;
            const double midY = h / 2.0;
            const double amp = (h - 4) / 2.0;
            if (shape == "wavetable") {
                for (int x = 0; x <= w; ++x) {
                    double phase = (static_cast<double>(x) / w) * 2.0;
                    double y = midY - qSin(phase * 2.0 * M_PI) * (amp*0.5) - qSin(phase * 4.0 * M_PI) * (amp*0.25);
                    if (x == 0) path.moveTo(x, y); else path.lineTo(x, y);
                }
                QPainterPath path2;
                for (int x = 0; x <= w; ++x) {
                    double phase = (static_cast<double>(x) / w) * 2.0;
                    double y = midY - qSin(phase * 2.0 * M_PI + 1.0) * (amp*0.5) - qSin(phase * 4.0 * M_PI) * (amp*0.25) + 4;
                    if (x == 0) path2.moveTo(x, y); else path2.lineTo(x, y);
                }
                QColor onMod = ThemePalette::color("qss_color_6", QColor(255,102,0));
                onMod.setAlpha(isEnabled() ? 100 : 30);
                QColor offMod = ThemePalette::color("qss_color_35", QColor(92,116,140));
                offMod.setAlpha(isEnabled() ? 100 : 30);
                p.setPen(QPen(checked ? onMod : offMod, 1.0));
                p.drawPath(path2);
                p.setPen(pen);
            } else {
                for (int x = 0; x <= w; ++x) {
                    double phase = (static_cast<double>(x) / w) * 1.5;
                    double sample = waveformSample(shape, phase);
                    double y = midY - sample * amp;
                    if (x == 0) path.moveTo(x, y); else path.lineTo(x, y);
                }
            }
            p.drawPath(path);
            return pix;
        };
        
        QIcon icon;
        icon.addPixmap(createPix(false), QIcon::Normal, QIcon::Off);
        icon.addPixmap(createPix(true), QIcon::Normal, QIcon::On);
        btn->setIcon(icon);
        btn->setIconSize(QSize(16, 16));

        if (m_waveforms[i].id == m_currentWaveform)
            btn->setChecked(true);

        m_buttonGroup->addButton(btn, i);
        m_buttons.append(btn);
        btnLayout->addWidget(btn);
    }
    btnLayout->addStretch();
    mainLayout->addLayout(btnLayout);

    // --- Waveform preview ---
    m_graphLayout = new QHBoxLayout();
    m_graphLayout->setContentsMargins(0, 0, 0, 0);
    m_graphLayout->setSpacing(10);
    
    m_previewWidget = new WaveformPreviewWidget(this, this);
    m_graphLayout->addWidget(m_previewWidget, 1); // stretch = 1
    
    mainLayout->addLayout(m_graphLayout);

    // --- Connections ---
    connect(m_buttonGroup, &QButtonGroup::idClicked, this, [this](int id) {
        if (id >= 0 && id < m_waveforms.size()) {
            m_currentWaveform = m_waveforms[id].id;
            m_previewWidget->update();
            emit waveformChanged(m_currentWaveform);
        }
    });
}

void WaveformSelectorWidget::setWaveform(const QString& waveform) {
    if (m_currentWaveform == waveform)
        return;

    m_currentWaveform = waveform;
    for (int i = 0; i < m_waveforms.size(); ++i) {
        if (m_waveforms[i].id == waveform) {
            m_buttons[i]->setChecked(true);
            break;
        }
    }
    m_previewWidget->update();
}

void WaveformSelectorWidget::paintEvent(QPaintEvent* event) {
    QWidget::paintEvent(event);
}

double WaveformSelectorWidget::waveformSample(const QString& type, double phase) const {
    // phase is in [0, numCycles) – extract the fractional cycle
    double t = phase - qFloor(phase);  // 0..1 within one cycle
    double angle = t * 2.0 * M_PI;

    if (type == "sine") {
        return qSin(angle);
    }
    if (type == "saw") {
        return 1.0 - 2.0 * t;   // ramp from +1 to -1
    }
    if (type == "square") {
        return (t < 0.5) ? 1.0 : -1.0;
    }
    if (type == "triangle") {
        if (t < 0.25)      return  4.0 * t;
        else if (t < 0.75) return  2.0 - 4.0 * t;
        else                return -4.0 + 4.0 * t;
    }
    if (type == "noise") {
        // Deterministic pseudo-random based on phase for stability
        uint32_t seed = static_cast<uint32_t>(phase * 10000.0);
        seed ^= (seed << 13);
        seed ^= (seed >> 17);
        seed ^= (seed << 5);
        return (static_cast<double>(seed % 10000) / 5000.0) - 1.0;
    }
    if (type == "pluck") {
        return qSin(angle) * qExp(-phase * 3.0);
    }

    return 0.0;
}

// Make WaveformPreviewWidget's MOC happy – the inner class is Q_OBJECT-free,
// so no separate moc include is needed.
