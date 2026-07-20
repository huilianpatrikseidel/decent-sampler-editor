#pragma once
#include "UiComponentItem.h"
#include <QFont>

class UiKnobItem : public UiComponentItem {
public:
    UiKnobItem(const UiComponent* comp, QGraphicsItem* p = nullptr);
    void paintComponent(QPainter* painter) override;
    void updateComponentSpecifics(const UiComponent* comp) override;
private:
    FilmstripRenderer m_renderer;
    double m_currentValue = 0.0;
};

class UiSliderItem : public UiComponentItem {
public:
    UiSliderItem(const UiComponent* comp, QGraphicsItem* p = nullptr);
    void paintComponent(QPainter* painter) override;
    void updateComponentSpecifics(const UiComponent* comp) override;
private:
    double m_currentValue = 0.0;
};

class UiButtonItem : public UiComponentItem {
public:
    UiButtonItem(const UiComponent* comp, QGraphicsItem* p = nullptr);
    void paintComponent(QPainter* painter) override;
    void updateComponentSpecifics(const UiComponent* comp) override;
private:
    QString m_imagePath;
    QImage m_imageCache;
};

class UiMenuItem : public UiComponentItem {
public:
    UiMenuItem(const UiComponent* comp, QGraphicsItem* p = nullptr);
    void paintComponent(QPainter* painter) override;
};

class UiXYPadItem : public UiComponentItem {
public:
    UiXYPadItem(const UiComponent* comp, QGraphicsItem* p = nullptr);
    void paintComponent(QPainter* painter) override;
};

class UiOscilloscopeItem : public UiComponentItem {
public:
    UiOscilloscopeItem(const UiComponent* comp, QGraphicsItem* p = nullptr);
    void paintComponent(QPainter* painter) override;
};

class UiKeyboardItem : public UiComponentItem {
public:
    UiKeyboardItem(const UiComponent* comp, QGraphicsItem* p = nullptr);
    void paintComponent(QPainter* painter) override;
};

class UiTabItem : public UiComponentItem {
public:
    UiTabItem(const UiComponent* comp, QGraphicsItem* p = nullptr);
    void paintComponent(QPainter* painter) override;
};

class UiLabelItem : public UiComponentItem {
public:
    UiLabelItem(const UiComponent* comp, QGraphicsItem* p = nullptr);
    void paintComponent(QPainter* painter) override;
    void updateComponentSpecifics(const UiComponent* comp) override;
private:
    QString m_text;
    QString m_font;
    int m_fontSize;
    QString m_colorHex;
};

class UiShapeItem : public UiComponentItem {
public:
    UiShapeItem(const UiComponent* comp, QGraphicsItem* p = nullptr);
    void paintComponent(QPainter* painter) override;
    void updateComponentSpecifics(const UiComponent* comp) override;
private:
    QString m_shapeType;
    QString m_fillColorHex;
    QString m_strokeColorHex;
    int m_strokeWidth;
};

class UiImageItem : public UiComponentItem {
public:
    UiImageItem(const UiComponent* comp, QGraphicsItem* p = nullptr);
    void paintComponent(QPainter* painter) override;
    void updateComponentSpecifics(const UiComponent* comp) override;
private:
    QString m_imagePath;
    QImage m_imageCache;
};
