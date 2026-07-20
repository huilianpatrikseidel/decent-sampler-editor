#pragma once
#include <QWidget>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSlider>
#include <QSpinBox>
#include "AdsrEditorView.h"
#include "core/ProjectManager.h"
#include <QUuid>
#include "NodePropertyForms.h"

class PropertiesInspector : public QWidget {
    Q_OBJECT
public:
    explicit PropertiesInspector(ProjectManager* pm, QWidget* parent = nullptr);

public slots:
    void setNode(const QUuid& id);
    void setUiComponent(const QUuid& id);
    void setZone(const QUuid& groupId, int zoneIndex);
    
    void onNodeModified(const QUuid& id, const QString& propertyName);
    void onUiComponentModified(const QUuid& id, const QString& propertyName);
    void onCanvasSizeChanged(int width, int height);

    void notifyUiPropertyChanged(const QUuid& id, const QString& propertyName, const QVariant& oldVal, const QVariant& newVal) {
        emit uiPropertyChanged(id, propertyName, oldVal, newVal);
    }
    void notifyPropertyChanged(const QUuid& id, const QString& propertyName, const QVariant& oldVal, const QVariant& newVal) {
        emit propertyChanged(id, propertyName, oldVal, newVal);
    }
    ProjectManager* getProjectManager() const { return m_pm; }
    bool isUpdatingUI() const { return m_isUpdatingUI; }

signals:
    void canvasSizePropertyChanged(int width, int height);
    void canvasBgPropertyChanged(const QString& bgImg, int bgMode);
    void propertyChanged(const QUuid& id, const QString& propertyName, const QVariant& oldVal, const QVariant& newVal);
    void uiPropertyChanged(const QUuid& id, const QString& propertyName, const QVariant& oldVal, const QVariant& newVal);
    void zonePropertyChanged(const QUuid& groupId, int zoneIndex, const QString& propertyName, const QVariant& oldVal, const QVariant& newVal);

private:
    ProjectManager* m_pm;
    QUuid m_currentNodeId;
    QUuid m_currentUiCompId;
    QUuid m_currentZoneGroupId;
    int m_currentZoneIndex = -1;
    QFormLayout* m_layout;
    
    QSlider* m_volumeSlider;
    QSlider* m_panSlider;
    
    void clearForm();
    bool m_isUpdatingUI = false;
};
