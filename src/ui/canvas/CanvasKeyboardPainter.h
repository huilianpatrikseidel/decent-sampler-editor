#pragma once
#include <QPainter>
#include <QRectF>
#include <QColor>
#include <vector>

class ProjectManager;

class CanvasKeyboardPainter {
public:
    static void paintLinear(QPainter* painter, ProjectManager* pm, const QRectF& rect, double linearKeyWidth, int activeMouseNote);
    static void paintStandard(QPainter* painter, ProjectManager* pm, const QRectF& rect, int padding, int gap, double wheelW, double sideBtnW, int kbStartNote, int activeMouseNote);
    
    static QColor getGroupColor(int groupIndex);
    static std::vector<int> getVisibleWhiteNotes(ProjectManager* pm, int& kbStartNote, int& globalLo, int& globalHi, QColor& kbColor);
};
