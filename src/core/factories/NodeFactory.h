#pragma once
#include <memory>
#include <QJsonObject>
#include "../models/AudioNodes.h"

#include "../CoreLibExport.h"

class CORELIB_EXPORT NodeFactory {
public:
    static std::unique_ptr<Node> fromJson(const QJsonObject& obj);
};
