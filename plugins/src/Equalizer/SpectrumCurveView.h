#pragma once
#include "vstgui/lib/controls/ccontrol.h"
#include "vstgui/uidescription/uiviewfactory.h"

class EqualizerController;

namespace VSTGUI {

class SpectrumCurveView : public CControl {
public:
    SpectrumCurveView(const CRect& size, IControlListener* listener, int32_t tag);
    ~SpectrumCurveView() override = default;

    void draw(CDrawContext* context) override;
    CMouseEventResult onMouseDown(CPoint& where, const CButtonState& buttons) override;
    CMouseEventResult onMouseUp(CPoint& where, const CButtonState& buttons) override;
    CMouseEventResult onMouseMoved(CPoint& where, const CButtonState& buttons) override;

    void setController(EqualizerController* controller) { m_controller = controller; }

    CLASS_METHODS(SpectrumCurveView, CControl)

protected:
    int getBandAtPoint(const CPoint& where);
    void updateParametersFromMouse(const CPoint& where);
    
    int m_draggingBand = -1;
    EqualizerController* m_controller = nullptr;
};

} // namespace VSTGUI
