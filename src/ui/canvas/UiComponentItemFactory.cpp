#include "UiComponentItemFactory.h"
#include "UiComponentItems.h"

UiComponentItem* UiComponentItemFactory::create(const UiComponent* comp, QGraphicsItem* parent) {
    if (comp->type == "UiKnob") return new UiKnobItem(comp, parent);
    if (comp->type == "UiSlider") return new UiSliderItem(comp, parent);
    if (comp->type == "UiButton") return new UiButtonItem(comp, parent);
    if (comp->type == "UiMenu") return new UiMenuItem(comp, parent);
    if (comp->type == "UiXYPad") return new UiXYPadItem(comp, parent);
    if (comp->type == "UiOscilloscope") return new UiOscilloscopeItem(comp, parent);
    if (comp->type == "UiKeyboard") return new UiKeyboardItem(comp, parent);
    if (comp->type == "UiTab") return new UiTabItem(comp, parent);
    if (comp->type == "UiLabel") return new UiLabelItem(comp, parent);
    if (comp->type == "UiShape") return new UiShapeItem(comp, parent);
    if (comp->type == "UiImage") return new UiImageItem(comp, parent);
    
    // Fallback if unknown
    return new UiShapeItem(comp, parent);
}
