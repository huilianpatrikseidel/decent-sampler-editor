#include "EnvelopeCardWidget.h"
#include "../../../commands/ModifyPropertyCommand.h"
#include "../../components/SynthKnobWidget.h"
#include "../../components/EnvelopeGraphWidget.h"
#include "../../components/ModSourceHelper.h"
#include <QHBoxLayout>
#include <QLabel>

EnvelopeCardWidget::EnvelopeCardWidget(ProjectManager* pm, SampleGroup* sg, int oscCount, const QUuid& oscId, QWidget* parent)
    : SynthCardBase(QString("Envelopes (ADSR %1)").arg(oscCount), nullptr, parent),
      m_pm(pm) {
      
    QVBoxLayout* envLayout = contentLayout();
    
    auto provider = ModSourceHelper::createProvider(pm);
    
    SynthKnobWidget *aa, *ad, *as, *ar, *ma, *md, *ms, *mr;
    
    auto addEnv = [&](const QString& label, const ADSR& envVal, SynthKnobWidget*& a, SynthKnobWidget*& d, SynthKnobWidget*& s, SynthKnobWidget*& r) {
        QVBoxLayout* l = new QVBoxLayout();
        if (label == "Modulation") {
            l->setContentsMargins(0, 8, 0, 0);
        } else {
            l->setContentsMargins(0, 0, 0, 0);
        }
        l->setSpacing(10);
        
        QLabel* title = new QLabel(label);
        title->setStyleSheet(" font-weight: 600; font-size: 11px;");
        l->addWidget(title, 0, Qt::AlignCenter);
        
        EnvelopeGraphWidget* envGraph = new EnvelopeGraphWidget();
        envGraph->setParameters(envVal.attack, envVal.decay, envVal.sustain, envVal.release);
        l->addWidget(envGraph);
        envLayout->addLayout(l);
        
        QWidget* controlsBlock = new QWidget();
        controlsBlock->setObjectName("NeumorphicControls");
        controlsBlock->setAttribute(Qt::WA_StyledBackground, true);
        controlsBlock->setFixedHeight(85);
        QHBoxLayout* knobs = new QHBoxLayout(controlsBlock);
        knobs->setContentsMargins(0, 10, 0, 10);
        knobs->setSpacing(16);
        
        knobs->addStretch();
        
        auto addK = [&](const QString& name, SynthKnobWidget*& knob, double max, double val, QLabel*& valLabelOut, const QString& modDest) {
            QVBoxLayout* kl = new QVBoxLayout();
            kl->setSpacing(2);
            QLabel* nl = new QLabel(name);
            nl->setStyleSheet("font-size: 12px;  font-weight: 600;");
            kl->addWidget(nl, 0, Qt::AlignCenter);
            knob = new SynthKnobWidget();
            knob->setModSourceProvider(provider);
            knob->setFixedSize(36, 36);
            knob->setMinimum(0); knob->setMaximum(max);
            
            // Set reasonable defaults based on ADSR
            double def = 0.0;
            if (name == "Sustain") def = 1.0;
            knob->setDefaultValue(def);
            
            knob->setValue(val);
            kl->addWidget(knob, 0, Qt::AlignCenter);
            
            QLabel* valLabel = new QLabel(QString::number(val, 'f', 2));
            valLabel->setStyleSheet("font-size: 10px;  font-family: Consolas, monospace;");
            valLabel->setAlignment(Qt::AlignCenter);
            kl->addWidget(valLabel, 0, Qt::AlignCenter);
            knobs->addLayout(kl);
            
            valLabelOut = valLabel;
            
            if (!modDest.isEmpty()) {
                knob->setModDestination(modDest);
                for (const ModRouting& r : sg->routings) {
                    if (r.destination == modDest || r.destination.startsWith(modDest + " - ")) {
                        knob->setModRouting(r);
                        break;
                    }
                }
                
                connect(m_pm, &ProjectManager::nodeModified, knob, [this, knob, oscId, modDest](QUuid id, const QString& prop) {
                    if (id == oscId && prop == "routings") {
                        Node* n = m_pm->getNode(oscId);
                        if (n && n->type == "SampleGroup") {
                            SampleGroup* sg = static_cast<SampleGroup*>(n);
                            bool found = false;
                            for (const ModRouting& r : sg->routings) {
                                if (r.destination == modDest || r.destination.startsWith(modDest + " - ")) {
                                    knob->setModRouting(r);
                                    found = true;
                                    break;
                                }
                            }
                            if (!found) {
                                ModRouting emptyR;
                                emptyR.destination = modDest;
                                knob->setModRouting(emptyR);
                            }
                        }
                    }
                });

                connect(knob, &SynthKnobWidget::modulationChanged, this, [this, oscId](const ModRouting& newR) {
                    if (m_isUpdating) return;
                    Node* n = m_pm->getNode(oscId);
                    if (n && n->type == "SampleGroup") {
                        SampleGroup* sg = static_cast<SampleGroup*>(n);
                        auto copy = sg->routings;
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
                        for (const auto& r : sg->routings) oldArr.append(r.toJson());
                        for (const auto& r : copy) newArr.append(r.toJson());
                        m_pm->getUndoStack()->push(new ModifyPropertyCommand(m_pm, oscId, "routings", oldArr, newArr));
                    }
                });
            }
        };
        
        QLabel *l_a, *l_d, *l_s, *l_r;
        QString prefix = (label == "Amplitude") ? "AmpEnv_" : ""; // only amplitude envelope can be modulated via AmpEnv_*
        addK("Attack", a, 10.0, envVal.attack, l_a, prefix.isEmpty() ? "" : prefix + "Attack"); 
        addK("Decay", d, 10.0, envVal.decay, l_d, prefix.isEmpty() ? "" : prefix + "Decay"); 
        addK("Sustain", s, 1.0, envVal.sustain, l_s, prefix.isEmpty() ? "" : prefix + "Sustain"); 
        addK("Release", r, 10.0, envVal.release, l_r, prefix.isEmpty() ? "" : prefix + "Release");
        knobs->addStretch();
        
        // Now that all knobs are created, we can safely capture them by value (their addresses won't change)
        auto onKnobMove = [envGraph, a, d, s, r](double atk, double dcy, double sus, double rel) {
            if (envGraph) envGraph->setParameters(atk, dcy, sus, rel);
        };
        
        connect(a, &SynthKnobWidget::valueChanged, this, [l_a, onKnobMove, d, s, r](double v) {
            l_a->setText(QString::number(v, 'f', 2));
            onKnobMove(v, d->value(), s->value(), r->value());
        });
        connect(d, &SynthKnobWidget::valueChanged, this, [l_d, onKnobMove, a, s, r](double v) {
            l_d->setText(QString::number(v, 'f', 2));
            onKnobMove(a->value(), v, s->value(), r->value());
        });
        connect(s, &SynthKnobWidget::valueChanged, this, [l_s, onKnobMove, a, d, r](double v) {
            l_s->setText(QString::number(v, 'f', 2));
            onKnobMove(a->value(), d->value(), v, r->value());
        });
        connect(r, &SynthKnobWidget::valueChanged, this, [l_r, onKnobMove, a, d, s](double v) {
            l_r->setText(QString::number(v, 'f', 2));
            onKnobMove(a->value(), d->value(), s->value(), v);
        });
        
        envLayout->addWidget(controlsBlock);
    };
    
    addEnv("Amplitude", sg->ampEnv, aa, ad, as, ar);
    addEnv("Modulation", sg->modEnv, ma, md, ms, mr);
    envLayout->addStretch();
    
    auto onEnvK = [this, oscId, aa, ad, as, ar, ma, md, ms, mr]() {
        if (m_isUpdating) return;
        Node* node = m_pm->getNode(oscId);
        if (node && node->type == "SampleGroup") {
            SampleGroup* sg = static_cast<SampleGroup*>(node);
            QJsonObject oldJson = sg->toJson();
            SampleGroup newSg = *sg;
            
            newSg.ampEnv.attack = aa->value();
            newSg.ampEnv.decay = ad->value();
            newSg.ampEnv.sustain = as->value();
            newSg.ampEnv.release = ar->value();
            
            newSg.modEnv.attack = ma->value();
            newSg.modEnv.decay = md->value();
            newSg.modEnv.sustain = ms->value();
            newSg.modEnv.release = mr->value();
            
            m_pm->getUndoStack()->push(new ModifyPropertyCommand(m_pm, oscId, "", oldJson, newSg.toJson()));
        }
    };
    connect(aa, &SynthKnobWidget::valueChanged, this, onEnvK);
    connect(ad, &SynthKnobWidget::valueChanged, this, onEnvK);
    connect(as, &SynthKnobWidget::valueChanged, this, onEnvK);
    connect(ar, &SynthKnobWidget::valueChanged, this, onEnvK);
    connect(ma, &SynthKnobWidget::valueChanged, this, onEnvK);
    connect(md, &SynthKnobWidget::valueChanged, this, onEnvK);
    connect(ms, &SynthKnobWidget::valueChanged, this, onEnvK);
    connect(mr, &SynthKnobWidget::valueChanged, this, onEnvK);
    
    m_isUpdating = false;
}
