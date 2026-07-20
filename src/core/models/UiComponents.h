#pragma once
#include <QString>
#include <QUuid>
#include <QJsonObject>
#include <QJsonArray>
#include <QStringList>
#include "../IUiVisitor.h"

// ==========================================
// UI Component Definitions for WYSIWYG
// ==========================================

#include "../CoreLibExport.h"

struct CORELIB_EXPORT UiComponent {
    QUuid id = QUuid::createUuid();
    QString type;
    QString name;
    double x = 0.0, y = 0.0;
    double width = 50.0, height = 50.0;
    int zIndex = 0;
    
    // Data Binding target
    QUuid targetNodeId;
    QString targetProperty;

    virtual ~UiComponent() = default;
    
    virtual void accept(IUiVisitor* v) const {}

    virtual QJsonObject toJson() const {
        QJsonObject obj;
        obj["id"] = id.toString();
        obj["type"] = type;
        obj["name"] = name;
        obj["x"] = x;
        obj["y"] = y;
        obj["width"] = width;
        obj["height"] = height;
        obj["zIndex"] = zIndex;
        obj["targetNodeId"] = targetNodeId.toString();
        obj["targetProperty"] = targetProperty;
        return obj;
    }
};

struct UiKnob : public UiComponent {
    QString filmstripPath;
    int frameCount = 1;
    bool isVertical = true;
    double currentValue = 0.0; // 0.0 to 1.0

    void accept(IUiVisitor* v) const override { v->visit(this); }
    UiKnob() { type = "UiKnob"; width = 60; height = 60; }
    
    QJsonObject toJson() const override {
        QJsonObject obj = UiComponent::toJson();
        obj["filmstripPath"] = filmstripPath;
        obj["frameCount"] = frameCount;
        obj["isVertical"] = isVertical;
        obj["currentValue"] = currentValue;
        return obj;
    }
};

struct UiSlider : public UiComponent {
    QString filmstripPath;
    int frameCount = 1;
    bool isVertical = true;
    double currentValue = 0.0; // 0.0 to 1.0
    
    void accept(IUiVisitor* v) const override { v->visit(this); }
    UiSlider() { type = "UiSlider"; width = 30; height = 120; }
    
    QJsonObject toJson() const override {
        QJsonObject obj = UiComponent::toJson();
        obj["filmstripPath"] = filmstripPath;
        obj["frameCount"] = frameCount;
        obj["isVertical"] = isVertical;
        obj["currentValue"] = currentValue;
        return obj;
    }
};

struct UiButton : public UiComponent {
    QString imagePathOn;
    QString imagePathOff;
    bool isToggle = false;
    bool isPressed = false;
    
    void accept(IUiVisitor* v) const override { v->visit(this); }
    UiButton() { type = "UiButton"; width = 80; height = 30; }
    
    QJsonObject toJson() const override {
        QJsonObject obj = UiComponent::toJson();
        obj["imagePathOn"] = imagePathOn;
        obj["imagePathOff"] = imagePathOff;
        obj["isToggle"] = isToggle;
        obj["isPressed"] = isPressed;
        return obj;
    }
};

struct UiLabel : public UiComponent {
    QString text;
    QString font = "Arial";
    int fontSize = 12;
    QString colorHex = "#FFFFFF";

    void accept(IUiVisitor* v) const override { v->visit(this); }
    UiLabel() { type = "UiLabel"; width = 100; height = 25; }
    
    QJsonObject toJson() const override {
        QJsonObject obj = UiComponent::toJson();
        obj["text"] = text;
        obj["font"] = font;
        obj["fontSize"] = fontSize;
        obj["colorHex"] = colorHex;
        return obj;
    }
};

struct UiShape : public UiComponent {
    QString shapeType = "Rectangle"; // "Rectangle" or "Ellipse"
    QString fillColorHex = "#404040";
    QString strokeColorHex = "#808080";
    int strokeWidth = 1;
    
    void accept(IUiVisitor* v) const override { v->visit(this); }
    UiShape() { type = "UiShape"; }
    QJsonObject toJson() const override {
        QJsonObject obj = UiComponent::toJson();
        obj["shapeType"] = shapeType;
        obj["fillColorHex"] = fillColorHex;
        obj["strokeColorHex"] = strokeColorHex;
        obj["strokeWidth"] = strokeWidth;
        return obj;
    }
};

struct UiImage : public UiComponent {
    QString imagePath;
    
    void accept(IUiVisitor* v) const override { v->visit(this); }
    UiImage() { type = "UiImage"; }
    QJsonObject toJson() const override {
        QJsonObject obj = UiComponent::toJson();
        obj["imagePath"] = imagePath;
        return obj;
    }
};

struct UiMultiFrameImage : public UiComponent {
    QString imagePath;
    int numFrames = 1;
    QString orientation = "vertical";
    
    void accept(IUiVisitor* v) const override { v->visit(this); }
    UiMultiFrameImage() { type = "UiMultiFrameImage"; }
    QJsonObject toJson() const override {
        QJsonObject obj = UiComponent::toJson();
        obj["imagePath"] = imagePath;
        obj["numFrames"] = numFrames;
        obj["orientation"] = orientation;
        return obj;
    }
};

struct UiLine : public UiComponent {
    QString color = "#FFFFFF";
    int thickness = 1;
    
    void accept(IUiVisitor* v) const override { v->visit(this); }
    UiLine() { type = "UiLine"; }
    QJsonObject toJson() const override {
        QJsonObject obj = UiComponent::toJson();
        obj["color"] = color;
        obj["thickness"] = thickness;
        return obj;
    }
};

struct UiMenu : public UiComponent {
    QStringList options;
    QString font = "Arial";
    int fontSize = 12;
    QString textColorHex = "#FFFFFF";
    QString bgColorHex = "#222222";
    
    void accept(IUiVisitor* v) const override { v->visit(this); }
    UiMenu() { type = "UiMenu"; width = 120; height = 24; }
    QJsonObject toJson() const override {
        QJsonObject obj = UiComponent::toJson();
        QJsonArray opts;
        for (const QString& o : options) opts.append(o);
        obj["options"] = opts;
        obj["font"] = font;
        obj["fontSize"] = fontSize;
        obj["textColorHex"] = textColorHex;
        obj["bgColorHex"] = bgColorHex;
        return obj;
    }
};

struct UiXYPad : public UiComponent {
    QString fillColorHex = "#000000";
    QString indicatorColorHex = "#00FF00";
    double currentX = 0.5;
    double currentY = 0.5;
    
    void accept(IUiVisitor* v) const override { v->visit(this); }
    UiXYPad() { type = "UiXYPad"; width = 100; height = 100; }
    QJsonObject toJson() const override {
        QJsonObject obj = UiComponent::toJson();
        obj["fillColorHex"] = fillColorHex;
        obj["indicatorColorHex"] = indicatorColorHex;
        obj["currentX"] = currentX;
        obj["currentY"] = currentY;
        return obj;
    }
};

struct UiOscilloscope : public UiComponent {
    QString lineColorHex = "#00FF00";
    QString bgColorHex = "#000000";
    
    void accept(IUiVisitor* v) const override { v->visit(this); }
    UiOscilloscope() { type = "UiOscilloscope"; width = 150; height = 50; }
    QJsonObject toJson() const override {
        QJsonObject obj = UiComponent::toJson();
        obj["lineColorHex"] = lineColorHex;
        obj["bgColorHex"] = bgColorHex;
        return obj;
    }
};

struct UiKeyboard : public UiComponent {
    int loNote = 0;
    int hiNote = 127;
    QString colorHex = "#FFFFFF";
    
    void accept(IUiVisitor* v) const override { v->visit(this); }
    UiKeyboard() { type = "UiKeyboard"; width = 400; height = 60; }
    QJsonObject toJson() const override {
        QJsonObject obj = UiComponent::toJson();
        obj["loNote"] = loNote;
        obj["hiNote"] = hiNote;
        obj["colorHex"] = colorHex;
        return obj;
    }
};

struct UiTab : public UiComponent {
    QStringList pages;
    
    void accept(IUiVisitor* v) const override { v->visit(this); }
    UiTab() { type = "UiTab"; width = 200; height = 30; }
    QJsonObject toJson() const override {
        QJsonObject obj = UiComponent::toJson();
        QJsonArray pgs;
        for (const QString& p : pages) pgs.append(p);
        obj["pages"] = pgs;
        return obj;
    }
};
