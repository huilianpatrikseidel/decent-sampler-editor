#pragma once

#include "ui/mapper/MapperLibExport.h"

class QPainter;
class QRectF;

class MAPPERLIB_EXPORT MappingGridPainter {
public:
    static void paintBackground(QPainter* painter, const QRectF& rect, double noteWidth, double velocityHeight);
    static void paintForeground(QPainter* painter, const QRectF& viewRect, double velocityHeight);
};
