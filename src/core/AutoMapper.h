#pragma once
#include <QStringList>
#include <QVector>
#include "models/AudioNodes.h"

#include "CoreLibExport.h"

class CORELIB_EXPORT AutoMapper {
public:
    enum class MappingMode {
        Tonal,
        Percussive
    };

    struct ParseResult {
        QString filePath;
        int rootNote = -1;
        int loVel = 1;
        int hiVel = 127;
        int seqPosition = 1;
        QString micLayer = "Close";
    };

    static QVector<Zone> mapFiles(const QStringList& filePaths, MappingMode mode = MappingMode::Tonal, int defaultRoot = 60);

private:
    static ParseResult parseFilename(const QString& filePath);
    static int noteNameToMidi(const QString& noteName);
    static void applyTonalSpread(QVector<Zone>& zones);
};
