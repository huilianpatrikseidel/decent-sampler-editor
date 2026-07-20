#include "UiStateSerializer.h"
#include "../ProjectManager.h"
#include "../models/ProjectStates.h"
#include "../factories/UiComponentFactory.h"
#include <QJsonArray>

QJsonObject UiStateSerializer::serialize(const ProjectManager* pm) {
    QJsonObject state;
    auto ui = pm->getUiState();
    
    double vx, vy, vz;
    ui->getViewport(vx, vy, vz);
    state["viewportX"] = vx;
    state["viewportY"] = vy;
    state["viewportZoom"] = vz;
    
    state["uiWidth"] = ui->getWidth();
    state["uiHeight"] = ui->getHeight();
    state["uiBgImage"] = ui->getBgImage();
    state["uiBgMode"] = static_cast<int>(ui->getBgMode());
    
    QJsonArray uiArray;
    for (const auto& pair : pm->getAllUiComponents()) {
        uiArray.append(pair.second->toJson());
    }
    state["components"] = uiArray;
    
    return state;
}

void UiStateSerializer::deserialize(ProjectManager* pm, const QJsonObject& json) {
    auto ui = pm->getUiState();

    if (json.contains("viewportX") && json.contains("viewportY") && json.contains("viewportZoom")) {
        ui->setViewport(json["viewportX"].toDouble(), json["viewportY"].toDouble(), json["viewportZoom"].toDouble());
    }
    if (json.contains("uiWidth")) ui->setWidth(json["uiWidth"].toInt());
    if (json.contains("uiHeight")) ui->setHeight(json["uiHeight"].toInt());
    if (json.contains("uiBgImage")) ui->setBgImage(json["uiBgImage"].toString());
    if (json.contains("uiBgMode")) ui->setBgMode(static_cast<BgMode>(json["uiBgMode"].toInt()));
    
    if (json.contains("components")) {
        QJsonArray uiArray = json["components"].toArray();
        for (int i = 0; i < uiArray.size(); ++i) {
            auto comp = UiComponentFactory::fromJson(uiArray[i].toObject());
            if (comp) {
                pm->addUiComponent(std::move(comp));
            }
        }
    }
}
