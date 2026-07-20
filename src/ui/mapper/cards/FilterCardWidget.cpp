#include "FilterCardWidget.h"
#include "../../../commands/ModifyPropertyCommand.h"
#include "../../components/FilterSectionWidget.h"
#include "../../components/ModSourceHelper.h"
#include "../../components/SynthKnobWidget.h"
#include <QVBoxLayout>

FilterCardWidget::FilterCardWidget(ProjectManager* pm, SampleGroup* sg, const QUuid& oscId, QWidget* parent)
    : SynthCardBase("Filter", nullptr, parent), m_pm(pm), m_oscId(oscId) {
    
    QString initType = sg->filterType;
    if (initType.length() > 0) initType[0] = initType[0].toUpper();
    if (initType.isEmpty()) initType = "Lowpass";
    setTitle(QString("Filter - %1").arg(initType));
    
    QVBoxLayout* layout = contentLayout();
    
    m_filterWidget = new FilterSectionWidget();
    m_filterWidget->setFilterType(sg->filterType);
    m_filterWidget->setCutoff(sg->filterCutoff);
    m_filterWidget->setResonance(sg->filterResonance);
    m_filterWidget->setEnvDepth(sg->filterEnvDepth);
    m_filterWidget->setKeyTrack(sg->filterKeyTrack);
    m_filterWidget->setEnabled(sg->filterEnabled);
    
    setEnableToggle(true, sg->filterEnabled);
    connect(this, &SynthCardBase::toggled, this, [this, oscId](bool checked) {
        if (m_isUpdating) return;
        
        Node* node = m_pm->getNode(oscId);
        if (node && node->type == "SampleGroup") {
            SampleGroup* sgNode = static_cast<SampleGroup*>(node);
            QJsonObject oldJson = sgNode->toJson();
            SampleGroup newSg = *sgNode;
            newSg.filterEnabled = checked;
            m_pm->getUndoStack()->push(new ModifyPropertyCommand(m_pm, oscId, "filterEnabled", oldJson, newSg.toJson()));
        }
        m_filterWidget->setEnabled(checked);
    });
    
    auto provider = ModSourceHelper::createProvider(pm);
    m_filterWidget->setModSourceProvider(provider);
    
    auto setupFilterModulation = [this, oscId, sg](SynthKnobWidget* knob, const QString& dest) {
        knob->setModDestination(dest);
        for (const auto& r : sg->routings) {
            if (r.destination == dest || r.destination.startsWith(dest + " - ")) {
                knob->setModRouting(r);
                break;
            }
        }
        
        connect(m_pm, &ProjectManager::nodeModified, knob, [this, knob, oscId, dest](QUuid id, const QString& prop) {
            if (id == oscId && prop == "routings") {
                Node* n = m_pm->getNode(oscId);
                if (n && n->type == "SampleGroup") {
                    SampleGroup* nSg = static_cast<SampleGroup*>(n);
                    bool found = false;
                    for (const ModRouting& r : nSg->routings) {
                        if (r.destination == dest || r.destination.startsWith(dest + " - ")) {
                            knob->setModRouting(r);
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        ModRouting emptyR;
                        emptyR.destination = dest;
                        knob->setModRouting(emptyR);
                    }
                }
            }
        });
        
        connect(knob, &SynthKnobWidget::modulationChanged, this, [this, oscId](const ModRouting& newR) {
            if (m_isUpdating) return;
            Node* n = m_pm->getNode(oscId);
            if (n && n->type == "SampleGroup") {
                SampleGroup* nSg = static_cast<SampleGroup*>(n);
                auto copy = nSg->routings;
                bool found = false;
                for (int i=0; i<copy.size(); ++i) {
                    if (copy[i].destination == newR.destination) {
                        if (newR.source.isEmpty()) {
                            copy.removeAt(i);
                        } else {
                            copy[i] = newR;
                        }
                        found = true;
                        break;
                    }
                }
                if (!found && !newR.source.isEmpty()) {
                    copy.append(newR);
                }
                
                QJsonArray oldArr, newArr;
                for (const auto& r : nSg->routings) oldArr.append(r.toJson());
                for (const auto& r : copy) newArr.append(r.toJson());
                m_pm->getUndoStack()->push(new ModifyPropertyCommand(m_pm, oscId, "routings", oldArr, newArr));
            }
        });
    };
    
    setupFilterModulation(m_filterWidget->cutoffKnob(), "Cutoff");
    setupFilterModulation(m_filterWidget->resonanceKnob(), "Resonance");
    
    connect(m_filterWidget, &FilterSectionWidget::filterChanged, this, [this, oscId](const QString& type, double cutoff, double resonance, double envDepth, double keyTrack) {
        if (m_isUpdating) return;
        
        Node* node = m_pm->getNode(oscId);
        if (node && node->type == "SampleGroup") {
            SampleGroup* sgNode = static_cast<SampleGroup*>(node);
            QJsonObject oldJson = sgNode->toJson();
            SampleGroup newSg = *sgNode;
            
            QString internalType = type;
            if (!internalType.isEmpty()) internalType[0] = internalType[0].toUpper();
            
            setTitle(QString("Filter - %1").arg(internalType));
            
            newSg.filterType = internalType;
            newSg.filterCutoff = cutoff;
            newSg.filterResonance = resonance;
            newSg.filterEnvDepth = envDepth;
            newSg.filterKeyTrack = keyTrack;
            
            m_pm->getUndoStack()->push(new ModifyPropertyCommand(m_pm, oscId, "", oldJson, newSg.toJson()));
        }
    });
    
    layout->addWidget(m_filterWidget);
    m_isUpdating = false;
}
