#pragma once
#include <memory>
#include <QJsonObject>
#include "../models/UiComponents.h"

#include "../CoreLibExport.h"

class CORELIB_EXPORT UiComponentFactory {
public:
    static std::unique_ptr<UiComponent> fromJson(const QJsonObject& obj);
};
