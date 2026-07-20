#pragma once
#include "IInspectorStrategy.h"
#include <QUuid>

class Zone;

class ZoneInspectorStrategy : public IInspectorStrategy {
public:
    ZoneInspectorStrategy(const Zone* zone, const QUuid& groupId, int zoneIndex);
    void buildForm(QFormLayout* layout, PropertiesInspector* inspector) override;

private:
    const Zone* m_zone;
    QUuid m_groupId;
    int m_zoneIndex;
};
