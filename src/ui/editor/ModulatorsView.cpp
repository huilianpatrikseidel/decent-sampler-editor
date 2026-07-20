#include "ModulatorsView.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QTableWidget>
#include <QHeaderView>
#include <QPushButton>
#include <QComboBox>
#include <QDoubleSpinBox>
#include "../../commands/ModifyPropertyCommand.h"
#include "../components/ModRoutingWidget.h"
#include "../components/ModSourceHelper.h"
#include <QJsonArray>
#include <QScrollBar>
#include <QTimer>

ModulatorsView::ModulatorsView(ProjectManager* pm, QWidget* parent) 
    : QWidget(parent), m_pm(pm) {
    
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    setLayout(m_layout);
    
    connect(m_pm, &ProjectManager::nodeModified, this, [this](const QUuid& id, const QString& prop) {
        if (m_isUpdating) return;
        if (prop == "routings" || prop == "isOscillator" || prop == "name" || prop == "synthParentId" ||
            prop == "pan" || prop == "volume" || prop == "tuning" || prop == "filterCutoff" || 
            prop == "filterResonance" || prop == "ampEnv") {
            updateForm();
        }
    });
    connect(m_pm, &ProjectManager::projectLoaded, this, &ModulatorsView::updateForm);
    
    updateForm();
}

void ModulatorsView::updateForm() {
    m_isUpdating = true;
    
    // Save scroll position before rebuild
    QScrollArea* oldScroll = findChild<QScrollArea*>();
    if (oldScroll && oldScroll->verticalScrollBar()) {
        m_savedScrollPos = oldScroll->verticalScrollBar()->value();
    }
    
    while (QLayoutItem* item = m_layout->takeAt(0)) {
        if (QWidget* w = item->widget()) w->deleteLater();
        delete item;
    }
    
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    
    QWidget* container = new QWidget();
    QVBoxLayout* scrollLayout = new QVBoxLayout(container);
    scrollLayout->setContentsMargins(15, 15, 15, 15);
    
    QLabel* title = new QLabel("<b>Global Modulation Matrix</b>");
    title->setStyleSheet("font-size: 16px; color: #eee; margin-bottom: 20px;");
    title->setAlignment(Qt::AlignCenter);
    scrollLayout->addWidget(title);
    
    // Collect all root groups
    QVector<SampleGroup*> rootGroups;
    for (const auto& pair : m_pm->getAllNodes()) {
        if (pair.second->type == "SampleGroup") {
            SampleGroup* sg = static_cast<SampleGroup*>(pair.second.get());
            if (sg->synthParentId.isNull()) {
                rootGroups.append(sg);
            }
        }
    }
    
    if (rootGroups.isEmpty()) {
        QLabel* emptyMsg = new QLabel("No synthesizers or groups found in project.");
        emptyMsg->setStyleSheet("color: #777;");
        emptyMsg->setAlignment(Qt::AlignCenter);
        scrollLayout->addWidget(emptyMsg);
        scrollLayout->addStretch();
        
        scrollArea->setWidget(container);
        m_layout->addWidget(scrollArea);
        
        m_isUpdating = false;
        return;
    }
    
    // Sort root groups by name
    std::sort(rootGroups.begin(), rootGroups.end(), [](SampleGroup* a, SampleGroup* b) {
        return a->name < b->name;
    });
    
    for (SampleGroup* rootSg : rootGroups) {
        QUuid rootId = rootSg->id;
        
        QWidget* oscSection = new QWidget();
        oscSection->setObjectName("NeumorphicCard");
        oscSection->setAttribute(Qt::WA_StyledBackground, true);
        QVBoxLayout* oscLayout = new QVBoxLayout(oscSection);
        oscLayout->setContentsMargins(15, 15, 15, 15);
        oscLayout->setSpacing(10);
        
        QString displayName = rootSg->name;
        if (displayName.isEmpty()) displayName = rootSg->isSynthContainer ? "Synth" : "Group";
        
        QHBoxLayout* headerLayout = new QHBoxLayout();
        
        QLabel* tagLabel = new QLabel(rootSg->isSynthContainer ? "SYNTH" : "SAMPLER");
        tagLabel->setStyleSheet(QString("background-color: %1; color: #111; font-weight: bold; font-size: 10px; padding: 2px 6px; border-radius: 4px;")
            .arg(rootSg->isSynthContainer ? "#32dc64" : "#ff9632"));
        tagLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
        headerLayout->addWidget(tagLabel);
        
        QLabel* oscTitle = new QLabel(displayName);
        oscTitle->setObjectName("ModuleHeader");
        headerLayout->addWidget(oscTitle);
        headerLayout->addStretch();
        
        oscLayout->addLayout(headerLayout);
        
        QHBoxLayout* slotsLayout = new QHBoxLayout();
        slotsLayout->setSpacing(10);
        
        // Collect tree (root + children)
        QVector<SampleGroup*> treeGroups;
        treeGroups.append(rootSg);
        for (const auto& pair : m_pm->getAllNodes()) {
            if (pair.second->type == "SampleGroup") {
                SampleGroup* child = static_cast<SampleGroup*>(pair.second.get());
                if (child->synthParentId == rootId) {
                    treeGroups.append(child);
                }
            }
        }
        
        for (SampleGroup* sg : treeGroups) {
            QUuid sgId = sg->id;
            bool isChild = (sg != rootSg);
            
            auto saveRoutings = [this, sgId](const QVector<ModRouting>& newR) {
                if (!m_isUpdating) {
                    Node* n = m_pm->getNode(sgId);
                    if (n && n->type == "SampleGroup") {
                        SampleGroup* sg = static_cast<SampleGroup*>(n);
                        QJsonArray oldArr, newArr;
                        for (const auto& r : sg->routings) oldArr.append(r.toJson());
                        for (const auto& r : newR) newArr.append(r.toJson());
                        m_pm->getUndoStack()->push(new ModifyPropertyCommand(m_pm, sgId, "routings", oldArr, newArr));
                    }
                }
            };
        
            for (int i = 0; i < sg->routings.size(); ++i) {
                const ModRouting& r = sg->routings[i];
                
                QWidget* slot = new QWidget();
                slot->setObjectName("NeumorphicInner");
                slot->setAttribute(Qt::WA_StyledBackground, true);
                
                QVBoxLayout* slotLayout = new QVBoxLayout(slot);
                slotLayout->setContentsMargins(10, 10, 10, 10);
                
                QStringList baseDestinations = {"Pitch", "Volume", "Pan", "Cutoff", "Resonance", "AmpEnv_Attack", "AmpEnv_Decay", "AmpEnv_Sustain", "AmpEnv_Release"};
                QStringList destinations;
                for (const QString& d : baseDestinations) {
                    if (isChild) destinations.append(QString("%1 - %2").arg(d, sg->name));
                    else destinations.append(d);
                }
                
                QStringList sources = ModSourceHelper::getAvailableSources(m_pm);
                ModRoutingWidget* routingWidget = new ModRoutingWidget(r, sources, destinations, false, slot);
                
                // Fetch the actual base value for this destination so the slider matches the knob
                auto getBaseValue = [sg](const QString& destStr) -> double {
                    QString d = destStr.split(" - ").first();
                    if (d == "Pan") return sg->pan;
                    if (d == "Volume") return sg->volume;
                    if (d == "Pitch") return sg->tuning;
                    if (d == "Cutoff") return sg->filterCutoff;
                    if (d == "Resonance") return sg->filterResonance;
                    if (d == "AmpEnv_Attack") return sg->ampEnv.attack;
                    if (d == "AmpEnv_Decay") return sg->ampEnv.decay;
                    if (d == "AmpEnv_Sustain") return sg->ampEnv.sustain;
                    if (d == "AmpEnv_Release") return sg->ampEnv.release;
                    return 0.0;
                };
                routingWidget->setBaseValue(getBaseValue(r.destination));

            
            connect(routingWidget, &ModRoutingWidget::routingEditingFinished, this, [sgId, i, saveRoutings, this](const ModRouting& newR) {
                if (m_isUpdating) return;
                Node* n = m_pm->getNode(sgId);
                if (n && n->type == "SampleGroup") {
                    SampleGroup* sg = static_cast<SampleGroup*>(n);
                    auto copy = sg->routings; 
                    copy[i] = newR; 
                    saveRoutings(copy);
                }
            }, Qt::QueuedConnection);
            
            connect(routingWidget, &ModRoutingWidget::baseValueEditingFinished, this, [sgId, i, this, routingWidget](double newBase) {
                if (m_isUpdating) return;
                Node* n = m_pm->getNode(sgId);
                if (n && n->type == "SampleGroup") {
                    SampleGroup* sg = static_cast<SampleGroup*>(n);
                    ModRouting r = routingWidget->getRouting();
                    
                    QJsonObject oldState = sg->toJson();
                    if (r.destination == "Pan") sg->pan = newBase;
                    else if (r.destination == "Volume") sg->volume = newBase;
                    else if (r.destination == "Pitch") sg->tuning = newBase;
                    else if (r.destination == "Cutoff") sg->filterCutoff = newBase;
                    else if (r.destination == "Resonance") sg->filterResonance = newBase;
                    else if (r.destination == "AmpEnv_Attack") sg->ampEnv.attack = newBase;
                    else if (r.destination == "AmpEnv_Decay") sg->ampEnv.decay = newBase;
                    else if (r.destination == "AmpEnv_Sustain") sg->ampEnv.sustain = newBase;
                    else if (r.destination == "AmpEnv_Release") sg->ampEnv.release = newBase;
                    
                    QJsonObject newState = sg->toJson();
                    m_pm->getUndoStack()->push(new ModifyPropertyCommand(m_pm, sgId, "", oldState, newState));
                }
            }, Qt::QueuedConnection);
            
            connect(routingWidget, &ModRoutingWidget::deleteRequested, this, [sgId, i, saveRoutings, this]() {
                Node* n = m_pm->getNode(sgId);
                if (n && n->type == "SampleGroup") {
                    SampleGroup* sg = static_cast<SampleGroup*>(n);
                    auto copy = sg->routings; 
                    copy.removeAt(i); 
                    saveRoutings(copy);
                }
            }, Qt::QueuedConnection);
            
                slotLayout->addWidget(routingWidget);
                slotsLayout->addWidget(slot);
            }
        }
        
        QPushButton* addBtn = new QPushButton("+ Add Routing");
        addBtn->setObjectName("ModAddBtn");
        addBtn->setFixedSize(140, 140); // Make it match the height of cards
        addBtn->setStyleSheet("background-color: transparent; border: 2px dashed #444; border-radius: 8px; color: #777; font-weight: bold;");
        connect(addBtn, &QPushButton::clicked, [this, rootId](){
            Node* n = m_pm->getNode(rootId);
            if (n && n->type == "SampleGroup") {
                SampleGroup* sg = static_cast<SampleGroup*>(n);
                auto copy = sg->routings;
                ModRouting r; r.source = "LFO1"; r.destination = "Pitch"; r.amount = 0.0;
                copy.append(r);
                
                QJsonArray oldArr, newArr;
                for (const auto& routing : sg->routings) oldArr.append(routing.toJson());
                for (const auto& routing : copy) newArr.append(routing.toJson());
                m_pm->getUndoStack()->push(new ModifyPropertyCommand(m_pm, rootId, "routings", oldArr, newArr));
            }
        });
        
        slotsLayout->addWidget(addBtn);
        slotsLayout->addStretch(); // Push everything to the left
        
        oscLayout->addLayout(slotsLayout);
        scrollLayout->addWidget(oscSection);
    }
    
    scrollLayout->addStretch();
    
    scrollArea->setWidget(container);
    m_layout->addWidget(scrollArea);
    
    m_isUpdating = false;
    
    // Restore scroll position after layout is recalculated
    QTimer::singleShot(0, this, [this]() {
        QScrollArea* scroll = findChild<QScrollArea*>();
        if (scroll && scroll->verticalScrollBar()) {
            scroll->verticalScrollBar()->setValue(m_savedScrollPos);
        }
    });
}
