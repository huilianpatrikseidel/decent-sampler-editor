#pragma once
#include <QJsonObject>

class AudioStateModel;

class AudioStateSerializer {
public:
    static QJsonObject serialize(const AudioStateModel* audioState);
    static void deserialize(AudioStateModel* audioState, const QJsonObject& json);
};
