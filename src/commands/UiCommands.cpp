#include "UiCommands.h"

// Add
AddUiComponentCommand::AddUiComponentCommand(ProjectManager* pm, std::unique_ptr<UiComponent> comp)
    : m_pm(pm), m_comp(std::move(comp)) {
    m_id = m_comp->id;
    setText("Add UI Component");
}
void AddUiComponentCommand::undo() { m_comp = m_pm->removeUiComponent(m_id); }
void AddUiComponentCommand::redo() { m_pm->addUiComponent(std::move(m_comp)); }

// Remove
RemoveUiComponentCommand::RemoveUiComponentCommand(ProjectManager* pm, const QUuid& id)
    : m_pm(pm), m_id(id) { setText("Remove UI Component"); }
void RemoveUiComponentCommand::undo() { m_pm->addUiComponent(std::move(m_savedComp)); }
void RemoveUiComponentCommand::redo() { m_savedComp = m_pm->removeUiComponent(m_id); }

// Move
MoveUiComponentCommand::MoveUiComponentCommand(ProjectManager* pm, const QUuid& id, const QPointF& oldPos, const QPointF& newPos)
    : m_pm(pm), m_id(id), m_oldPos(oldPos), m_newPos(newPos) { setText("Move UI Component"); }
void MoveUiComponentCommand::undo() {
    if (auto c = m_pm->getUiComponent(m_id)) { c->x = m_oldPos.x(); c->y = m_oldPos.y(); m_pm->setUiComponentModified(m_id, "position"); }
}
void MoveUiComponentCommand::redo() {
    if (auto c = m_pm->getUiComponent(m_id)) { c->x = m_newPos.x(); c->y = m_newPos.y(); m_pm->setUiComponentModified(m_id, "position"); }
}
bool MoveUiComponentCommand::mergeWith(const QUndoCommand *command) {
    if (command->id() != id()) return false;
    const MoveUiComponentCommand* cmd = static_cast<const MoveUiComponentCommand*>(command);
    if (cmd->m_id != m_id) return false;
    m_newPos = cmd->m_newPos; return true;
}

// Modify
ModifyUiPropertyCommand::ModifyUiPropertyCommand(ProjectManager* pm, const QUuid& id, const QString& prop, const QVariant& oldV, const QVariant& newV)
    : m_pm(pm), m_id(id), m_propertyPath(prop), m_oldVal(oldV), m_newVal(newV) { setText(QString("Change %1").arg(prop)); }
void ModifyUiPropertyCommand::undo() { applyValue(m_oldVal); }
void ModifyUiPropertyCommand::redo() { applyValue(m_newVal); }
void ModifyUiPropertyCommand::applyValue(const QVariant& val) {
    auto c = m_pm->getUiComponent(m_id);
    if (!c) return;
    
    if (m_propertyPath == "name") {
        c->name = val.toString();
    } else if (m_propertyPath == "targetNodeId") {
        c->targetNodeId = val.toUuid();
    } else if (m_propertyPath == "targetProperty") {
        c->targetProperty = val.toString();
    } else if (c->type == "UiKnob") {
        auto k = static_cast<UiKnob*>(c);
        if (m_propertyPath == "currentValue") k->currentValue = val.toDouble();
        else if (m_propertyPath == "filmstripPath") k->filmstripPath = val.toString();
        else if (m_propertyPath == "frameCount") k->frameCount = val.toInt();
    } else if (c->type == "UiSlider") {
        auto s = static_cast<UiSlider*>(c);
        if (m_propertyPath == "currentValue") s->currentValue = val.toDouble();
        else if (m_propertyPath == "filmstripPath") s->filmstripPath = val.toString();
        else if (m_propertyPath == "frameCount") s->frameCount = val.toInt();
    } else if (c->type == "UiButton") {
        auto b = static_cast<UiButton*>(c);
        if (m_propertyPath == "imagePathOn") b->imagePathOn = val.toString();
        else if (m_propertyPath == "imagePathOff") b->imagePathOff = val.toString();
        else if (m_propertyPath == "isToggle") b->isToggle = val.toBool();
        else if (m_propertyPath == "isPressed") b->isPressed = val.toBool();
    } else if (c->type == "UiLabel") {
        auto l = static_cast<UiLabel*>(c);
        if (m_propertyPath == "text") l->text = val.toString();
        else if (m_propertyPath == "colorHex") l->colorHex = val.toString();
    } else if (c->type == "UiShape") {
        auto s = static_cast<UiShape*>(c);
        if (m_propertyPath == "fillColorHex") s->fillColorHex = val.toString();
        else if (m_propertyPath == "strokeColorHex") s->strokeColorHex = val.toString();
        else if (m_propertyPath == "strokeWidth") s->strokeWidth = val.toInt();
    }
    m_pm->setUiComponentModified(m_id, m_propertyPath);
}
bool ModifyUiPropertyCommand::mergeWith(const QUndoCommand *command) {
    if (command->id() != id()) return false;
    const ModifyUiPropertyCommand* cmd = static_cast<const ModifyUiPropertyCommand*>(command);
    if (cmd->m_id != m_id || cmd->m_propertyPath != m_propertyPath) return false;
    m_newVal = cmd->m_newVal; return true;
}

// Canvas Size
ModifyCanvasSizeCommand::ModifyCanvasSizeCommand(ProjectManager* pm, int oldW, int oldH, int newW, int newH)
    : m_pm(pm), m_oldW(oldW), m_oldH(oldH), m_newW(newW), m_newH(newH) {
    setText("Change Canvas Size");
}
void ModifyCanvasSizeCommand::undo() {
    m_pm->getUiState()->setWidth(m_oldW);
    m_pm->getUiState()->setHeight(m_oldH);
}
void ModifyCanvasSizeCommand::redo() {
    m_pm->getUiState()->setWidth(m_newW);
    m_pm->getUiState()->setHeight(m_newH);
}
bool ModifyCanvasSizeCommand::mergeWith(const QUndoCommand *command) {
    if (command->id() != id()) return false;
    const ModifyCanvasSizeCommand* cmd = static_cast<const ModifyCanvasSizeCommand*>(command);
    m_newW = cmd->m_newW;
    m_newH = cmd->m_newH;
    return true;
}

// Canvas Background
ModifyCanvasBgCommand::ModifyCanvasBgCommand(ProjectManager* pm, const QString& oldImg, const QString& newImg, BgMode oldMode, BgMode newMode)
    : m_pm(pm), m_oldImg(oldImg), m_newImg(newImg), m_oldMode(oldMode), m_newMode(newMode) {
    setText("Change Canvas Background");
}
void ModifyCanvasBgCommand::undo() {
    m_pm->getUiState()->setBgImage(m_oldImg);
    m_pm->getUiState()->setBgMode(m_oldMode);
}
void ModifyCanvasBgCommand::redo() {
    m_pm->getUiState()->setBgImage(m_newImg);
    m_pm->getUiState()->setBgMode(m_newMode);
}
bool ModifyCanvasBgCommand::mergeWith(const QUndoCommand *command) {
    if (command->id() != id()) return false;
    const ModifyCanvasBgCommand* cmd = static_cast<const ModifyCanvasBgCommand*>(command);
    m_newImg = cmd->m_newImg;
    m_newMode = cmd->m_newMode;
    return true;
}
