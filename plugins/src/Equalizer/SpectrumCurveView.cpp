#include "SpectrumCurveView.h"
#include "EqualizerController.h"
#include "Biquad.h"
#include <cmath>

namespace VSTGUI {

SpectrumCurveView::SpectrumCurveView(const CRect& size, IControlListener* listener, int32_t tag)
    : CControl(size, listener, tag, nullptr) {
}

void SpectrumCurveView::draw(CDrawContext* context) {
    if (!m_controller) return;

    // Draw background
    context->setFillColor(CColor(30, 30, 30, 255));
    context->drawRect(getViewSize(), kDrawFilled);

    // Calculate magnitude response curve
    double sampleRate = 44100.0;

    CGraphicsPath* path = context->createGraphicsPath();
    if (path) {
        double width = getViewSize().getWidth();
        double height = getViewSize().getHeight();
        
        bool first = true;
        for (int x = 0; x < width; ++x) {
            double normalizedX = (double)x / width;
            double freq = 20.0 * std::pow(1000.0, normalizedX);

            double totalMag = 1.0;
            
            for (int b = 0; b < 24; ++b) {
                int baseId = b * 10;
                double enabled = m_controller->getParamNormalized(baseId + 0);
                if (enabled > 0.5) {
                    double normFreq = m_controller->getParamNormalized(baseId + 1);
                    double normGain = m_controller->getParamNormalized(baseId + 2);
                    double normQ = m_controller->getParamNormalized(baseId + 3);
                    double normType = m_controller->getParamNormalized(baseId + 4);

                    double bFreq = 20.0 * std::pow(1000.0, normFreq);
                    double bGain = (normGain * 60.0) - 30.0;
                    double bQ = 0.1 * std::pow(400.0, normQ);
                    FilterType bType = static_cast<FilterType>(std::round(normType * 6.0));

                    BiquadFilter tempFilter;
                    tempFilter.calculateCoefficients(bType, sampleRate, bFreq, bQ, bGain);
                    totalMag *= tempFilter.evaluateMagnitude(freq, sampleRate);
                }
            }
            
            double db = 20.0 * std::log10(totalMag > 1e-10 ? totalMag : 1e-10);
            double normalizedY = (db + 30.0) / 60.0;
            normalizedY = std::fmax(0.0, std::fmin(1.0, normalizedY));
            
            double y = getViewSize().top + height - (normalizedY * height);
            
            if (first) {
                path->beginSubpath(CPoint(getViewSize().left + x, y));
                first = false;
            } else {
                path->addLine(CPoint(getViewSize().left + x, y));
            }
        }
        
        context->setFrameColor(CColor(100, 200, 255, 255));
        context->setLineWidth(2);
        context->drawGraphicsPath(path, CDrawContext::kPathStroked);
        path->forget();
    }

    // Draw nodes
    for (int b = 0; b < 24; ++b) {
        int baseId = b * 10;
        double enabled = m_controller->getParamNormalized(baseId + 0);
        if (enabled > 0.5) {
            double normFreq = m_controller->getParamNormalized(baseId + 1);
            double normGain = m_controller->getParamNormalized(baseId + 2);
            
            double width = getViewSize().getWidth();
            double height = getViewSize().getHeight();
            
            double x = getViewSize().left + normFreq * width;
            double y = getViewSize().top + height - (normGain * height);
            
            CRect nodeRect(x - 6, y - 6, x + 6, y + 6);
            if (b == m_draggingBand) {
                context->setFillColor(CColor(255, 200, 100, 255));
            } else {
                context->setFillColor(CColor(200, 200, 200, 255));
            }
            context->drawEllipse(nodeRect, kDrawFilled);
        }
    }
}

CMouseEventResult SpectrumCurveView::onMouseDown(CPoint& where, const CButtonState& buttons) {
    if (buttons.isLeftButton()) {
        m_draggingBand = getBandAtPoint(where);
        if (m_draggingBand != -1) {
            m_controller->beginEdit(m_draggingBand * 10 + 1);
            m_controller->beginEdit(m_draggingBand * 10 + 2);
            return kMouseEventHandled;
        } else {
            for (int b = 0; b < 24; ++b) {
                int baseId = b * 10;
                if (m_controller->getParamNormalized(baseId + 0) < 0.5) {
                    m_controller->beginEdit(baseId + 0);
                    m_controller->setParamNormalized(baseId + 0, 1.0);
                    m_controller->performEdit(baseId + 0, 1.0);
                    m_controller->endEdit(baseId + 0);
                    
                    m_draggingBand = b;
                    m_controller->beginEdit(baseId + 1);
                    m_controller->beginEdit(baseId + 2);
                    updateParametersFromMouse(where);
                    return kMouseEventHandled;
                }
            }
        }
    }
    return kMouseEventNotHandled;
}

CMouseEventResult SpectrumCurveView::onMouseUp(CPoint& where, const CButtonState& buttons) {
    if (m_draggingBand != -1) {
        m_controller->endEdit(m_draggingBand * 10 + 1);
        m_controller->endEdit(m_draggingBand * 10 + 2);
        m_draggingBand = -1;
        invalid();
        return kMouseEventHandled;
    }
    return kMouseEventNotHandled;
}

CMouseEventResult SpectrumCurveView::onMouseMoved(CPoint& where, const CButtonState& buttons) {
    if (m_draggingBand != -1 && buttons.isLeftButton()) {
        updateParametersFromMouse(where);
        return kMouseEventHandled;
    }
    return kMouseEventNotHandled;
}

int SpectrumCurveView::getBandAtPoint(const CPoint& where) {
    if (!m_controller) return -1;
    double width = getViewSize().getWidth();
    double height = getViewSize().getHeight();

    for (int b = 0; b < 24; ++b) {
        int baseId = b * 10;
        if (m_controller->getParamNormalized(baseId + 0) > 0.5) {
            double normFreq = m_controller->getParamNormalized(baseId + 1);
            double normGain = m_controller->getParamNormalized(baseId + 2);
            
            double x = getViewSize().left + normFreq * width;
            double y = getViewSize().top + height - (normGain * height);
            
            double dx = where.x - x;
            double dy = where.y - y;
            if (dx*dx + dy*dy <= 64.0) {
                return b;
            }
        }
    }
    return -1;
}

void SpectrumCurveView::updateParametersFromMouse(const CPoint& where) {
    if (!m_controller || m_draggingBand == -1) return;
    
    double width = getViewSize().getWidth();
    double height = getViewSize().getHeight();
    
    double normFreq = (where.x - getViewSize().left) / width;
    double normGain = (getViewSize().top + height - where.y) / height;
    
    normFreq = std::fmax(0.0, std::fmin(1.0, normFreq));
    normGain = std::fmax(0.0, std::fmin(1.0, normGain));
    
    m_controller->setParamNormalized(m_draggingBand * 10 + 1, normFreq);
    m_controller->performEdit(m_draggingBand * 10 + 1, normFreq);
    
    m_controller->setParamNormalized(m_draggingBand * 10 + 2, normGain);
    m_controller->performEdit(m_draggingBand * 10 + 2, normGain);
    
    invalid();
}

class SpectrumCurveViewCreator : public ViewCreatorAdapter {
public:
    SpectrumCurveViewCreator() {
        UIViewFactory::registerViewCreator(*this);
    }
    IdStringPtr getViewName() const override { return "SpectrumCurveView"; }
    IdStringPtr getBaseViewName() const override { return "CControl"; }
    CView* create(const UIAttributes& attributes, const IUIDescription* description) const override {
        return new SpectrumCurveView(CRect(0, 0, 0, 0), nullptr, -1);
    }
};

static SpectrumCurveViewCreator __gSpectrumCurveViewCreator;

} // namespace VSTGUI
