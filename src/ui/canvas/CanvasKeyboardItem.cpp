#include "CanvasKeyboardItem.h"
#include "CanvasKeyboardPainter.h"
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <cmath>

CanvasKeyboardItem::CanvasKeyboardItem(ProjectManager* pm, QGraphicsItem* parent)
    : QGraphicsObject(parent), m_pm(pm), m_padding(6), m_gap(6),
      m_wheelW(16), m_sideBtnW(16), m_kbStartNote(36), m_activeMouseNote(-1) {
    setZValue(100);
    setAcceptedMouseButtons(Qt::LeftButton);
    int w = m_pm ? m_pm->getUiState()->getWidth() : 812;
    m_rect = QRectF(0, 0, w, 75);
}

void CanvasKeyboardItem::setGeometry(const QRectF& rect) {
    prepareGeometryChange();
    m_rect = rect;
}

void CanvasKeyboardItem::setLinearMode(bool active, double keyWidth) {
    m_isLinearMode = active;
    m_linearKeyWidth = keyWidth;
    update();
}



QRectF CanvasKeyboardItem::boundingRect() const {
    return m_rect;
}

void CanvasKeyboardItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);
    
    if (m_isLinearMode) {
        CanvasKeyboardPainter::paintLinear(painter, m_pm, m_rect, m_linearKeyWidth, m_activeMouseNote);
    } else {
        CanvasKeyboardPainter::paintStandard(painter, m_pm, m_rect, m_padding, m_gap, m_wheelW, m_sideBtnW, m_kbStartNote, m_activeMouseNote);
    }
}

void CanvasKeyboardItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (m_isLinearMode) {
        int note = getMidiNoteAt(event->pos(), m_rect.width());
        if (note >= 0 && note <= 127) {
            m_activeMouseNote = note;
            emit notePressed(note, 100);
            update();
        }
        event->accept();
        return;
    }
    
    int w = m_rect.width();
    int h = m_rect.height();
    QPointF pos = event->pos();
    
    double wheelsY = m_padding;
    double wheelsH = h - (m_padding * 2);
    
    double pitchX = m_padding;
    double modX = pitchX + m_wheelW + m_gap;
    double leftBtnX = modX + m_wheelW + m_gap;
    double rightBtnX = w - m_padding - m_sideBtnW;
    
    int globalLo = 21;
    int globalHi = 108;
    QColor kbColor("#ffffff");
    
    std::vector<int> allWhiteNotes;
    for (int n = globalLo; n <= globalHi; ++n) {
        int rem = n % 12;
        if (rem == 0 || rem == 2 || rem == 4 || rem == 5 || rem == 7 || rem == 9 || rem == 11) {
            allWhiteNotes.push_back(n);
        }
    }
    if (allWhiteNotes.empty()) {
        for (int n = 36; n <= 84; ++n) {
            int rem = n % 12;
            if (rem == 0 || rem == 2 || rem == 4 || rem == 5 || rem == 7 || rem == 9 || rem == 11) {
                allWhiteNotes.push_back(n);
            }
        }
    }
    
    int currentIdx = 0;
    for (int i = 0; i < (int)allWhiteNotes.size(); ++i) {
        if (allWhiteNotes[i] >= m_kbStartNote) {
            currentIdx = i;
            break;
        }
    }
    
    if (pos.x() >= leftBtnX && pos.x() <= leftBtnX + m_sideBtnW && pos.y() >= wheelsY && pos.y() <= wheelsY + wheelsH) {
        int newIdx = std::max(0, currentIdx - 7);
        m_kbStartNote = allWhiteNotes[newIdx];
        update();
        event->accept();
        return;
    }
    
    if (pos.x() >= rightBtnX && pos.x() <= rightBtnX + m_sideBtnW && pos.y() >= wheelsY && pos.y() <= wheelsY + wheelsH) {
        int maxVisible = 36;
        int maxIdx = std::max(0, (int)allWhiteNotes.size() - maxVisible);
        int newIdx = std::min(maxIdx, currentIdx + 7);
        m_kbStartNote = allWhiteNotes[newIdx];
        update();
        event->accept();
        return;
    }
    
    int note = getMidiNoteAt(pos, w);
    if (note >= 0 && note <= 127) {
        m_activeMouseNote = note;
        emit notePressed(note, 100);
        update();
    }
    event->accept();
}

void CanvasKeyboardItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (m_activeMouseNote != -1) {
        int w = m_rect.width();
        int note = getMidiNoteAt(event->pos(), w);
        if (note != m_activeMouseNote) {
            emit noteReleased(m_activeMouseNote);
            if (note >= 0 && note <= 127) {
                m_activeMouseNote = note;
                emit notePressed(note, 100);
            } else {
                m_activeMouseNote = -1;
            }
            update();
        }
        event->accept();
    }
}

void CanvasKeyboardItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if (m_activeMouseNote != -1) {
        emit noteReleased(m_activeMouseNote);
        m_activeMouseNote = -1;
        update();
        event->accept();
    }
}

int CanvasKeyboardItem::getMidiNoteAt(const QPointF& pos, int w) const {
    if (m_isLinearMode) {
        int note = std::floor(pos.x() / m_linearKeyWidth);
        if (note >= 0 && note <= 127) return note;
        return -1;
    }
    
    int h = m_rect.height();
    double wheelsY = m_padding;
    double wheelsH = h - (m_padding * 2);
    
    double pitchX = m_padding;
    double modX = pitchX + m_wheelW + m_gap;
    double leftBtnX = modX + m_wheelW + m_gap;
    double rightBtnX = w - m_padding - m_sideBtnW;
    double kbStartX = leftBtnX + m_sideBtnW + m_gap;
    double kbDrawW = rightBtnX - m_gap - kbStartX;
    
    int globalLo = 21;
    int globalHi = 108;
    QColor kbColor("#ffffff");
    int startNoteTemp = m_kbStartNote;
    std::vector<int> visibleWhiteNotes = CanvasKeyboardPainter::getVisibleWhiteNotes(m_pm, startNoteTemp, globalLo, globalHi, kbColor);
    int numWhiteKeys = visibleWhiteNotes.size();
    
    if (pos.y() < wheelsY || pos.y() > wheelsY + wheelsH) return -1;
    if (pos.x() < kbStartX || pos.x() > rightBtnX - m_gap) return -1;
    
    for (int i = 0; i < numWhiteKeys; ++i) {
        int midiNote = visibleWhiteNotes[i];
        int blackNote = midiNote + 1;
        int rem = blackNote % 12;
        bool isBlack = (rem == 1 || rem == 3 || rem == 6 || rem == 8 || rem == 10);
        
        if (isBlack && blackNote <= globalHi && i < numWhiteKeys - 1) {
            int x1 = kbStartX + (i * kbDrawW) / numWhiteKeys;
            int x2 = kbStartX + ((i + 1) * kbDrawW) / numWhiteKeys;
            int kW = x2 - x1;
            
            double blackKeyW = kW * 0.55; 
            double blackKeyH = wheelsH * 0.62;
            
            QRectF bKeyRect(x2 - (blackKeyW/2), wheelsY, blackKeyW, blackKeyH);
            if (bKeyRect.contains(pos)) return blackNote;
        }
    }
    
    for (int i = 0; i < numWhiteKeys; ++i) {
        int x1 = kbStartX + (i * kbDrawW) / numWhiteKeys;
        int x2 = kbStartX + ((i + 1) * kbDrawW) / numWhiteKeys;
        int kW = x2 - x1;
        
        QRectF wKeyRect(x1, wheelsY, kW, wheelsH);
        if (wKeyRect.contains(pos)) return visibleWhiteNotes[i];
    }
    
    return -1;
}


