#include "AutoMapper.h"
#include <QFileInfo>
#include <QRegularExpression>
#include <algorithm>
#include <QMap>

int AutoMapper::noteNameToMidi(const QString& noteName) {
    QRegularExpression re("^([A-Ga-g])([#b]?)(-?\\d+)");
    QRegularExpressionMatch match = re.match(noteName);
    if (!match.hasMatch()) return -1;
    
    QString note = match.captured(1).toUpper();
    QString acc = match.captured(2);
    int octave = match.captured(3).toInt();
    
    int base = 0;
    if (note == "C") base = 0;
    else if (note == "D") base = 2;
    else if (note == "E") base = 4;
    else if (note == "F") base = 5;
    else if (note == "G") base = 7;
    else if (note == "A") base = 9;
    else if (note == "B") base = 11;
    
    if (acc == "#") base++;
    else if (acc == "b") base--;
    
    // MIDI note 60 is C4 in most DAWs (sometimes C3). We'll assume C4 = 60.
    // C0 = 12, C-1 = 0
    return (octave + 1) * 12 + base;
}

AutoMapper::ParseResult AutoMapper::parseFilename(const QString& filePath) {
    ParseResult res;
    res.filePath = filePath;
    QFileInfo fi(filePath);
    QString baseName = fi.baseName();
    
    // 1. Parse Note (e.g. C4, D#3, Bb2)
    QRegularExpression noteRe("([A-Ga-g][#b]?-?\\d+)");
    QRegularExpressionMatch noteMatch = noteRe.match(baseName);
    if (noteMatch.hasMatch()) {
        res.rootNote = noteNameToMidi(noteMatch.captured(1));
    }
    
    // 2. Parse Velocity (e.g. v127, vel100, v001-030)
    QRegularExpression velRangeRe("v(?:el)?(\\d{1,3})-v?(?:el)?(\\d{1,3})", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch velRangeMatch = velRangeRe.match(baseName);
    if (velRangeMatch.hasMatch()) {
        res.loVel = velRangeMatch.captured(1).toInt();
        res.hiVel = velRangeMatch.captured(2).toInt();
    } else {
        QRegularExpression velSingleRe("v(?:el)?(\\d{1,3})", QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch velSingleMatch = velSingleRe.match(baseName);
        if (velSingleMatch.hasMatch()) {
            // Se apenas 1 velocity for provida, assume-se como o topo ou único. 
            // Para simplificar, deixamos cobrir todo o range ou fazemos algo mais inteligente no spread.
            int v = velSingleMatch.captured(1).toInt();
            res.loVel = 1; // Espalhará depois se tivermos multiplos no mesmo grupo de notas
            res.hiVel = v;
        }
    }
    
    // 3. Parse Round Robin (e.g. RR1, RR2, rr03)
    QRegularExpression rrRe("rr(\\d+)", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch rrMatch = rrRe.match(baseName);
    if (rrMatch.hasMatch()) {
        res.seqPosition = rrMatch.captured(1).toInt();
    } else {
        // Fallback for simple _1, _2 at the end
        QRegularExpression endNumRe("_(\\d+)$");
        QRegularExpressionMatch endNumMatch = endNumRe.match(baseName);
        if (endNumMatch.hasMatch()) {
            res.seqPosition = endNumMatch.captured(1).toInt();
        }
    }
    
    return res;
}

QVector<Zone> AutoMapper::mapFiles(const QStringList& filePaths, MappingMode mode, int defaultRoot) {
    QVector<Zone> finalZones;
    QVector<ParseResult> parsed;
    
    int currentRoot = defaultRoot; // Caso não encontre nota
    for (const QString& path : filePaths) {
        ParseResult r = parseFilename(path);
        if (r.rootNote == -1) {
            r.rootNote = currentRoot;
            if (mode == MappingMode::Percussive) currentRoot++; // Incrementa para baterias mapeadas no teclado
        }
        parsed.append(r);
    }
    
    // Ordenar por RootNote, Velocity e RR
    std::sort(parsed.begin(), parsed.end(), [](const ParseResult& a, const ParseResult& b) {
        if (a.rootNote != b.rootNote) return a.rootNote < b.rootNote;
        if (a.hiVel != b.hiVel) return a.hiVel < b.hiVel;
        return a.seqPosition < b.seqPosition;
    });
    
    // Corrigir Velocity Layers se estiverem sobrepostas ou mal definidas
    // Agrupa por root note
    QMap<int, QVector<ParseResult*>> noteGroups;
    for (int i = 0; i < parsed.size(); ++i) {
        noteGroups[parsed[i].rootNote].append(&parsed[i]);
    }
    
    for (auto it = noteGroups.begin(); it != noteGroups.end(); ++it) {
        auto& group = it.value();
        // Separar por RR (assumindo que RR estão bem divididos, vamos simplificar focando no RR 1 ou igual)
        // Agrupar por Velocity
        QVector<int> velTops;
        for (auto* p : group) {
            if (!velTops.contains(p->hiVel)) velTops.append(p->hiVel);
        }
        std::sort(velTops.begin(), velTops.end());
        
        for (auto* p : group) {
            int idx = velTops.indexOf(p->hiVel);
            p->loVel = (idx == 0) ? 1 : velTops[idx - 1] + 1;
        }
    }
    
    // Criar as zonas iniciais
    for (const auto& p : parsed) {
        Zone z;
        z.samplePath = p.filePath;
        z.rootNote = p.rootNote;
        z.loNote = p.rootNote;
        z.hiNote = p.rootNote;
        z.loVel = p.loVel;
        z.hiVel = p.hiVel;
        z.seqPosition = p.seqPosition;
        z.micLayer = p.micLayer;
        finalZones.append(z);
    }
    
    if (mode == MappingMode::Tonal) {
        applyTonalSpread(finalZones);
    }
    
    return finalZones;
}

void AutoMapper::applyTonalSpread(QVector<Zone>& zones) {
    if (zones.isEmpty()) return;
    
    // Agrupar zonas que compartilham mesma nota, pra esticar juntas
    QMap<int, QVector<Zone*>> noteGroups;
    for (int i = 0; i < zones.size(); ++i) {
        noteGroups[zones[i].rootNote].append(&zones[i]);
    }
    
    QList<int> sortedRoots = noteGroups.keys();
    std::sort(sortedRoots.begin(), sortedRoots.end());
    
    for (int i = 0; i < sortedRoots.size(); ++i) {
        int currentRoot = sortedRoots[i];
        
        int loBound = 0;
        int hiBound = 127;
        
        if (i > 0) {
            int prevRoot = sortedRoots[i - 1];
            loBound = prevRoot + (currentRoot - prevRoot) / 2 + 1;
        }
        
        if (i < sortedRoots.size() - 1) {
            int nextRoot = sortedRoots[i + 1];
            hiBound = currentRoot + (nextRoot - currentRoot) / 2;
        }
        
        for (Zone* z : noteGroups[currentRoot]) {
            z->loNote = loBound;
            z->hiNote = hiBound;
        }
    }
}
