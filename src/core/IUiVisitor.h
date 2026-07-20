#pragma once

class UiKnob;
class UiSlider;
class UiButton;
class UiLabel;
class UiShape;
class UiImage;
class UiMenu;
class UiXYPad;
class UiOscilloscope;
class UiKeyboard;
class UiTab;
class UiLine;
class UiMultiFrameImage;

class IUiVisitor {
public:
    virtual ~IUiVisitor() = default;
    virtual void visit(const UiKnob* knob) = 0;
    virtual void visit(const UiSlider* slider) = 0;
    virtual void visit(const UiButton* btn) = 0;
    virtual void visit(const UiLabel* lbl) = 0;
    virtual void visit(const UiShape* shape) = 0;
    virtual void visit(const UiImage* img) = 0;
    virtual void visit(const UiMenu* menu) = 0;
    virtual void visit(const UiXYPad* pad) = 0;
    virtual void visit(const UiOscilloscope* osc) = 0;
    virtual void visit(const UiKeyboard* kb) = 0;
    virtual void visit(const UiTab* tab) = 0;
    virtual void visit(const UiLine* line) = 0;
    virtual void visit(const UiMultiFrameImage* mimg) = 0;
};
