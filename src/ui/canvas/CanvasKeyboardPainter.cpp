#include "CanvasKeyboardPainter.h"
#include "core/ProjectManager.h"
#include "core/models/AudioNodes.h"
#include "core/models/UiComponents.h"
#include "core/AppSettings.h"
#include "../../core/ThemePalette.h"
#include <QSet>
#include <QMap>
#include <QFont>
#include <algorithm>

QColor CanvasKeyboardPainter::getGroupColor(int groupIndex) {
    static const QVector<QColor> colors = {
        QColor(160, 190, 220, 160), QColor(170, 210, 230, 160),
        QColor(210, 230, 210, 160), QColor(245, 235, 200, 160),
        QColor(240, 210, 180, 160), QColor(220, 190, 210, 160),
        QColor(200, 210, 240, 160)
    };
    return colors[groupIndex % colors.size()];
}

std::vector<int> CanvasKeyboardPainter::getVisibleWhiteNotes(ProjectManager* pm, int& kbStartNote, int& globalLo, int& globalHi, QColor& kbColor) {
    globalLo = 36; 
    globalHi = 84; 
    kbColor = QColor("#ffffff");
    
    if (pm) {
        for (const auto& pair : pm->getAllUiComponents()) {
            if (pair.second->type == "UiKeyboard") {
                auto kbComp = static_cast<UiKeyboard*>(pair.second.get());
                globalLo = kbComp->loNote;
                globalHi = kbComp->hiNote;
                if (!kbComp->colorHex.isEmpty()) kbColor = QColor(kbComp->colorHex);
                break;
            }
        }
    }
    
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
    
    int maxVisible = 36;
    if ((int)allWhiteNotes.size() <= maxVisible) return allWhiteNotes;
    
    int idx = 0;
    for (int i = 0; i < (int)allWhiteNotes.size(); ++i) {
        if (allWhiteNotes[i] >= kbStartNote) { idx = i; break; }
    }
    
    if (idx < 0) idx = 0;
    if (idx > (int)allWhiteNotes.size() - maxVisible) idx = (int)allWhiteNotes.size() - maxVisible;
    
    kbStartNote = allWhiteNotes[idx];
    
    std::vector<int> visible;
    for (int i = 0; i < maxVisible; ++i) {
        visible.push_back(allWhiteNotes[idx + i]);
    }
    return visible;
}

void CanvasKeyboardPainter::paintLinear(QPainter* painter, ProjectManager* pm, const QRectF& rect, double linearKeyWidth, int activeMouseNote) {
    int h = rect.height();
    painter->setRenderHint(QPainter::Antialiasing, false);
    
    std::vector<SampleGroup*> sgs;
    if (pm) {
        for (const auto& pair : pm->getAllNodes()) {
            if (pair.second->type == "SampleGroup") {
                sgs.push_back(static_cast<SampleGroup*>(pair.second.get()));
            }
        }
        std::sort(sgs.begin(), sgs.end(), [](SampleGroup* a, SampleGroup* b) {
            return a->name < b->name;
        });
    }

    QMap<int, QColor> noteColors;
    QSet<int> keySwitches;
    for (size_t gIdx = 0; gIdx < sgs.size(); ++gIdx) {
        SampleGroup* sg = sgs[gIdx];
        QColor grpColor = getGroupColor(gIdx);
        for (const Zone& z : sg->zones) {
            for (int n = z.loNote; n <= z.hiNote; ++n) {
                if (n >= 0 && n <= 127) noteColors[n] = grpColor;
            }
            if (z.keySwitchNote >= 0 && z.keySwitchNote <= 127) {
                keySwitches.insert(z.keySwitchNote);
            }
        }
    }

    painter->fillRect(rect, ThemePalette::color("keyboard_bg"));
    
    painter->setPen(QPen(ThemePalette::color("keyboard_border"), 1));
    for (int i = 0; i <= 127; ++i) {
        int noteInOctave = i % 12;
        bool isWhite = (noteInOctave == 0 || noteInOctave == 2 || noteInOctave == 4 || 
                        noteInOctave == 5 || noteInOctave == 7 || noteInOctave == 9 || noteInOctave == 11);
        if (isWhite) {
            double x = i * linearKeyWidth;
            QRectF keyRect(x, 0, linearKeyWidth, h);
            
            QColor brushColor = QColor("#ffffff");
            if (keySwitches.contains(i)) brushColor = QColor(144, 238, 144, 200);
            else if (noteColors.contains(i)) brushColor = noteColors[i];
            if (i == activeMouseNote) brushColor = brushColor.darker(125);
            
            painter->setBrush(brushColor);
            painter->drawRect(keyRect);
            
            if (noteInOctave == 0) {
                painter->setPen(QColor("#333333"));
                QFont font = painter->font(); font.setFamily("Segoe UI"); font.setPixelSize(9); font.setBold(false); painter->setFont(font);
                int octave = (i / 12) - 2;
                painter->drawText(keyRect.adjusted(2, 0, 0, -2), Qt::AlignBottom | Qt::AlignLeft, QString("C%1").arg(octave));
                painter->setPen(QPen(QColor("#a0a0a0"), 1));
            }
        }
    }
    
    painter->setPen(QPen(QColor("#111111"), 1));
    for (int i = 0; i <= 127; ++i) {
        int noteInOctave = i % 12;
        bool isBlack = (noteInOctave == 1 || noteInOctave == 3 || noteInOctave == 6 || 
                        noteInOctave == 8 || noteInOctave == 10);
        if (isBlack) {
            double x = i * linearKeyWidth;
            QRectF keyRect(x, 0, linearKeyWidth, h * 0.62);
            
            QColor brushColor = ThemePalette::color("keyboard_black_keys");
            if (keySwitches.contains(i)) brushColor = QColor(100, 200, 100);
            else if (noteColors.contains(i)) brushColor = noteColors[i].darker(140);
            if (i == activeMouseNote) brushColor = brushColor.darker(150);
            
            painter->setBrush(brushColor);
            painter->drawRect(keyRect);
        }
    }
}

void CanvasKeyboardPainter::paintStandard(QPainter* painter, ProjectManager* pm, const QRectF& rect, int padding, int gap, double wheelW, double sideBtnW, int kbStartNote, int activeMouseNote) {
    int w = rect.width();
    int h = rect.height();
    painter->setRenderHint(QPainter::Antialiasing, false);
    
    std::vector<SampleGroup*> sgs;
    if (pm) {
        for (const auto& pair : pm->getAllNodes()) {
            if (pair.second->type == "SampleGroup") {
                sgs.push_back(static_cast<SampleGroup*>(pair.second.get()));
            }
        }
        std::sort(sgs.begin(), sgs.end(), [](SampleGroup* a, SampleGroup* b) {
            return a->name < b->name;
        });
    }

    QMap<int, QColor> noteColors;
    QSet<int> keySwitches;
    for (size_t gIdx = 0; gIdx < sgs.size(); ++gIdx) {
        SampleGroup* sg = sgs[gIdx];
        QColor grpColor = getGroupColor(gIdx);
        for (const Zone& z : sg->zones) {
            for (int n = z.loNote; n <= z.hiNote; ++n) {
                if (n >= 0 && n <= 127) noteColors[n] = grpColor;
            }
            if (z.keySwitchNote >= 0 && z.keySwitchNote <= 127) {
                keySwitches.insert(z.keySwitchNote);
            }
        }
    }
    
    double wheelsStartX = padding;
    double wheelsY = padding;
    double wheelsH = h - (padding * 2);
    
    int globalLo = 21;
    int globalHi = 108;
    QColor kbColor("#ffffff");
    std::vector<int> visibleWhiteNotes = getVisibleWhiteNotes(pm, kbStartNote, globalLo, globalHi, kbColor);
    int numWhiteKeys = visibleWhiteNotes.size();
    
    double pitchX = wheelsStartX;
    double modX = pitchX + wheelW + gap;
    double leftBtnX = modX + wheelW + gap;
    double rightBtnX = w - padding - sideBtnW;
    double kbStartX = leftBtnX + sideBtnW + gap;
    double kbDrawW = rightBtnX - gap - kbStartX;
    
    painter->setPen(QPen(ThemePalette::color("keyboard_panel_border"), 1));
    painter->setBrush(ThemePalette::color("keyboard_panel_bg"));
    
    painter->drawRect(QRectF(pitchX, wheelsY, wheelW, wheelsH));
    painter->fillRect(QRectF(pitchX, wheelsY + (wheelsH/2) - 1, wheelW, 2), ThemePalette::color("keyboard_wheel_track"));
    
    painter->drawRect(QRectF(modX, wheelsY, wheelW, wheelsH));
    painter->fillRect(QRectF(modX, wheelsY + wheelsH - 8, wheelW, 2), ThemePalette::color("keyboard_wheel_track"));
    
    painter->drawRect(QRectF(leftBtnX, wheelsY, sideBtnW, wheelsH));
    painter->drawRect(QRectF(rightBtnX, wheelsY, sideBtnW, wheelsH));
    
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(Qt::NoPen);
    painter->setBrush(ThemePalette::color("keyboard_wheel_bg"));
    
    QPolygonF leftTri;
    leftTri << QPointF(leftBtnX + 10, wheelsY + (wheelsH/2) - 5)
            << QPointF(leftBtnX + 10, wheelsY + (wheelsH/2) + 5)
            << QPointF(leftBtnX + 4, wheelsY + (wheelsH/2));
    painter->drawPolygon(leftTri);
    
    QPolygonF rightTri;
    rightTri << QPointF(rightBtnX + 6, wheelsY + (wheelsH/2) - 5)
             << QPointF(rightBtnX + 6, wheelsY + (wheelsH/2) + 5)
             << QPointF(rightBtnX + 12, wheelsY + (wheelsH/2));
    painter->drawPolygon(rightTri);
    painter->setRenderHint(QPainter::Antialiasing, false);
    
    painter->setPen(QPen(ThemePalette::color("keyboard_border"), 1));
    for (int i = 0; i < numWhiteKeys; ++i) {
        int x1 = kbStartX + (i * kbDrawW) / numWhiteKeys;
        int x2 = kbStartX + ((i + 1) * kbDrawW) / numWhiteKeys;
        int kW = x2 - x1;
        
        QRect keyRect(x1, wheelsY, kW, wheelsH);
        int midiNote = visibleWhiteNotes[i];
        
        QColor brushColor = kbColor;
        if (keySwitches.contains(midiNote)) brushColor = QColor(144, 238, 144, 200);
        else if (noteColors.contains(midiNote)) brushColor = noteColors[midiNote];
        
        if (midiNote == activeMouseNote) brushColor = brushColor.darker(125);
        
        painter->setBrush(brushColor);
        painter->drawRect(keyRect);
    }
    
    QFont f = painter->font();
    for (int i = 0; i < numWhiteKeys; ++i) {
        int midiNote = visibleWhiteNotes[i];
        if (midiNote % 12 == 0) {
            int x1 = kbStartX + (i * kbDrawW) / numWhiteKeys;
            int x2 = kbStartX + ((i + 1) * kbDrawW) / numWhiteKeys;
            int kW = x2 - x1;
            QRect keyRect(x1, wheelsY, kW, wheelsH);
            
            f.setFamily("Segoe UI");
            f.setPixelSize(9); f.setBold(false); painter->setFont(f);
            painter->setPen(QColor("#333333"));
            int oct = (midiNote / 12) - 2; 
            painter->drawText(keyRect.adjusted(2, 0, 0, -2), Qt::AlignBottom | Qt::AlignLeft, QString("C%1").arg(oct));
        }
    }
    
    painter->setPen(QPen(QColor("#111111"), 1));
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
            
            QColor brushColor = ThemePalette::color("keyboard_black_keys");
            if (keySwitches.contains(blackNote)) brushColor = QColor(100, 200, 100);
            else if (noteColors.contains(blackNote)) brushColor = noteColors[blackNote].darker(140);
            
            if (blackNote == activeMouseNote) brushColor = brushColor.darker(150);
            
            painter->setBrush(brushColor);
            painter->drawRect(bKeyRect);
        }
    }
}
