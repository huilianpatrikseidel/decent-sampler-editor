#include "UiComponentFactory.h"

std::unique_ptr<UiComponent> UiComponentFactory::fromJson(const QJsonObject& uiObj) {
    QString type = uiObj["type"].toString();
    
    std::unique_ptr<UiComponent> comp;
    if (type == "UiKnob") {
        auto knob = std::make_unique<UiKnob>();
        knob->filmstripPath = uiObj["filmstripPath"].toString();
        knob->frameCount = uiObj["frameCount"].toInt();
        knob->isVertical = uiObj["isVertical"].toBool();
        knob->currentValue = uiObj["currentValue"].toDouble();
        comp = std::move(knob);
    } else if (type == "UiSlider") {
        auto slider = std::make_unique<UiSlider>();
        slider->filmstripPath = uiObj["filmstripPath"].toString();
        slider->frameCount = uiObj["frameCount"].toInt();
        slider->isVertical = uiObj["isVertical"].toBool();
        slider->currentValue = uiObj["currentValue"].toDouble();
        comp = std::move(slider);
    } else if (type == "UiButton") {
        auto button = std::make_unique<UiButton>();
        button->imagePathOn = uiObj["imagePathOn"].toString();
        button->imagePathOff = uiObj["imagePathOff"].toString();
        button->isToggle = uiObj["isToggle"].toBool();
        button->isPressed = uiObj["isPressed"].toBool();
        comp = std::move(button);
    } else if (type == "UiLabel") {
        auto lbl = std::make_unique<UiLabel>();
        lbl->text = uiObj["text"].toString();
        lbl->font = uiObj["font"].toString();
        lbl->fontSize = uiObj["fontSize"].toInt();
        lbl->colorHex = uiObj["colorHex"].toString();
        comp = std::move(lbl);
    } else if (type == "UiShape") {
        auto shape = std::make_unique<UiShape>();
        shape->shapeType = uiObj["shapeType"].toString();
        shape->fillColorHex = uiObj["fillColorHex"].toString();
        shape->strokeColorHex = uiObj["strokeColorHex"].toString();
        shape->strokeWidth = uiObj["strokeWidth"].toInt(1);
        comp = std::move(shape);
    } else if (type == "UiImage") {
        auto img = std::make_unique<UiImage>();
        img->imagePath = uiObj["imagePath"].toString();
        comp = std::move(img);
    } else if (type == "UiKeyboard") {
        auto kb = std::make_unique<UiKeyboard>();
        kb->loNote = uiObj["loNote"].toInt(0);
        kb->hiNote = uiObj["hiNote"].toInt(127);
        kb->colorHex = uiObj["colorHex"].toString("#FFFFFF");
        comp = std::move(kb);
    } else if (type == "UiLine") {
        auto line = std::make_unique<UiLine>();
        line->color = uiObj["color"].toString("#FFFFFF");
        line->thickness = uiObj["thickness"].toInt(1);
        comp = std::move(line);
    } else if (type == "UiMultiFrameImage") {
        auto mimg = std::make_unique<UiMultiFrameImage>();
        mimg->imagePath = uiObj["imagePath"].toString();
        mimg->numFrames = uiObj["numFrames"].toInt(1);
        mimg->orientation = uiObj["orientation"].toString("vertical");
        comp = std::move(mimg);
    } else if (type == "UiMenu") {
        auto menu = std::make_unique<UiMenu>();
        QJsonArray optArr = uiObj["options"].toArray();
        for (int o=0; o<optArr.size(); ++o) menu->options.append(optArr[o].toString());
        menu->font = uiObj["font"].toString("Arial");
        menu->fontSize = uiObj["fontSize"].toInt(12);
        menu->textColorHex = uiObj["textColorHex"].toString("#FFFFFF");
        menu->bgColorHex = uiObj["bgColorHex"].toString("#222222");
        comp = std::move(menu);
    } else if (type == "UiXYPad") {
        auto pad = std::make_unique<UiXYPad>();
        pad->fillColorHex = uiObj["fillColorHex"].toString("#000000");
        pad->indicatorColorHex = uiObj["indicatorColorHex"].toString("#00FF00");
        pad->currentX = uiObj["currentX"].toDouble(0.5);
        pad->currentY = uiObj["currentY"].toDouble(0.5);
        comp = std::move(pad);
    } else if (type == "UiOscilloscope") {
        auto osc = std::make_unique<UiOscilloscope>();
        osc->lineColorHex = uiObj["lineColorHex"].toString("#00FF00");
        osc->bgColorHex = uiObj["bgColorHex"].toString("#000000");
        comp = std::move(osc);
    } else if (type == "UiTab") {
        auto tab = std::make_unique<UiTab>();
        QJsonArray pgArr = uiObj["pages"].toArray();
        for (int o=0; o<pgArr.size(); ++o) tab->pages.append(pgArr[o].toString());
        comp = std::move(tab);
    }
    
    if (comp) {
        comp->id = QUuid(uiObj["id"].toString());
        comp->name = uiObj["name"].toString();
        comp->x = uiObj["x"].toDouble();
        comp->y = uiObj["y"].toDouble();
        comp->width = uiObj["width"].toDouble();
        comp->height = uiObj["height"].toDouble();
        
        comp->targetNodeId = QUuid(uiObj["targetNodeId"].toString());
        comp->targetProperty = uiObj["targetProperty"].toString();
    }
    
    return comp;
}
