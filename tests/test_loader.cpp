#include "core/ProjectManager.h"
#include "core/ProjectSerializer.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <iostream>

int main() {
    ProjectManager pm;
    QFile file("last_project.json");
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject root = doc.object();
        std::cout << "Version: " << root["version"].toString().toStdString() << std::endl;
        QJsonArray presets = root["presets"].toArray();
        std::cout << "Presets count: " << presets.size() << std::endl;
        if (presets.size() > 0) {
            QJsonObject state = presets[0].toObject()["state"].toObject();
            QJsonArray nodes = state["nodes"].toArray();
            std::cout << "Nodes count in JSON: " << nodes.size() << std::endl;
        }
        pm.loadProject(QString::fromUtf8(data), 1);
        std::cout << "Nodes loaded in ProjectManager: " << pm.getAllNodes().size() << std::endl;
    }
    return 0;
}
