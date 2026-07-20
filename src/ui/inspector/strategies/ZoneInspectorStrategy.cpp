#include "ZoneInspectorStrategy.h"
#include "../PropertiesInspector.h"
#include "../../../core/models/GraphStructures.h"
#include <QSpinBox>
#include <QLabel>
#include <QString>

ZoneInspectorStrategy::ZoneInspectorStrategy(const Zone* zone, const QUuid& groupId, int zoneIndex)
    : m_zone(zone), m_groupId(groupId), m_zoneIndex(zoneIndex) {}

void ZoneInspectorStrategy::buildForm(QFormLayout* layout, PropertiesInspector* inspector) {
    if (!m_zone) return;

    layout->addRow(new QLabel(QString("<b>Zone %1</b>").arg(m_zoneIndex)));
    
    auto createSpinBox = [this, layout, inspector](const QString& label, int value, int min, int max, const QString& propName) {
        QSpinBox* spin = new QSpinBox();
        spin->setRange(min, max);
        spin->setValue(value);
        layout->addRow(label, spin);
        QObject::connect(spin, &QSpinBox::valueChanged, [this, inspector, propName, spin](int v) {
            if (!inspector->isUpdatingUI()) {
                emit inspector->zonePropertyChanged(m_groupId, m_zoneIndex, propName, spin->value(), v);
            }
        });
        return spin;
    };
    
    createSpinBox("Low Note", m_zone->loNote, 0, 127, "loNote");
    createSpinBox("High Note", m_zone->hiNote, 0, 127, "hiNote");
    createSpinBox("Root Note", m_zone->rootNote, 0, 127, "rootNote");
    createSpinBox("Low Vel", m_zone->loVel, 1, 127, "loVel");
    createSpinBox("High Vel", m_zone->hiVel, 1, 127, "hiVel");
}
