#include "FxRackWidget.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QPushButton>
#include <QMenu>
#include <QAction>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include "../../core/ProjectManager.h"
#include "../../core/factories/NodeFactory.h"
#include "../../commands/ModifyPropertyCommand.h"
#include "EffectEditorDialog.h"
#include "FxSlotWidget.h"
#include "../../core/ApplicationController.h"

FxRackWidget::FxRackWidget(ApplicationController* appCtrl, const QUuid& sgId, QWidget* parent)
    : QWidget(parent), m_appCtrl(appCtrl), m_pm(appCtrl->getProjectManager()), m_sgId(sgId)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(2);

    QLabel* fxLabel = new QLabel("FX RACK");
    fxLabel->setAlignment(Qt::AlignCenter);
    fxLabel->setStyleSheet("color: #888; font-size: 9px; font-weight: bold; background: transparent; border: none;");
    layout->addWidget(fxLabel);

    m_fxScrollArea = new QScrollArea();
    m_fxScrollArea->setWidgetResizable(true);
    m_fxScrollArea->setFixedHeight(140);
    m_fxScrollArea->setStyleSheet("QScrollArea { border: 1px solid #111; background: #1c1c1c; border-radius: 2px; } QScrollBar { width: 6px; }");
    
    m_fxContainer = new QWidget();
    m_fxContainer->setStyleSheet("background: transparent;");
    m_fxContainerLayout = new QVBoxLayout(m_fxContainer);
    m_fxContainerLayout->setContentsMargins(2, 2, 2, 2);
    m_fxContainerLayout->setSpacing(2);
    m_fxContainerLayout->setAlignment(Qt::AlignTop);
    m_fxScrollArea->setWidget(m_fxContainer);
    layout->addWidget(m_fxScrollArea);

    m_btnAddFx = new QPushButton("+");
    m_btnAddFx->setFixedHeight(20);
    m_btnAddFx->setStyleSheet("QPushButton { background-color: #2a2a2a; color: #aaa; border: 1px solid #111; border-radius: 2px; font-weight: bold; } QPushButton:hover { background-color: #3a3a3a; color: white; }");
    layout->addWidget(m_btnAddFx);

    connect(m_btnAddFx, &QPushButton::clicked, this, [this]() {
        QMenu menu(this);
        menu.addAction("Gain");
        menu.addAction("Filter");
        menu.addAction("Equalizer");
        menu.addAction("Chorus");
        menu.addAction("Phaser");
        menu.addAction("PitchShifter");
        menu.addAction("WaveFolder");
        menu.addAction("WaveShaper");
        menu.addAction("StereoSimulator");
        menu.addAction("BitCrusher");
        menu.addAction("Delay");
        menu.addAction("Reverb");
        
        QAction* selected = menu.exec(QCursor::pos());
        if (selected) {
            QString type = selected->text();
            QJsonObject fxObj;
            fxObj["type"] = type;
            fxObj["id"] = QUuid::createUuid().toString();
            fxObj["name"] = type;
            
            auto newFx = NodeFactory::fromJson(fxObj);
            if (newFx) {
                QUuid newId = newFx->id;
                m_pm->addNode(std::move(newFx));
                
                if (m_sgId.isNull()) {
                    auto copy = m_pm->getAudioState()->getMasterEffects();
                    copy.append(newId);
                    m_pm->getAudioState()->setMasterEffects(copy); 
                } else {
                    Node* n = m_pm->getNode(m_sgId);
                    if (n->type == "SampleGroup") {
                        auto sg = static_cast<SampleGroup*>(n);
                        sg->insertEffects.append(newId);
                    } else if (n->type == "Bus") {
                        auto b = static_cast<BusNode*>(n);
                        b->insertEffects.append(newId);
                    }
                }
                
                m_pm->setDirty(true);
                updateFromNode();
            }
        }
    });

    m_fxContainer->setAcceptDrops(true);
    m_fxContainer->installEventFilter(this);
}

void FxRackWidget::updateFromNode() {
    QLayoutItem* item;
    while ((item = m_fxContainerLayout->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }

    QVector<QUuid> effectsList;
    if (m_sgId.isNull()) {
        effectsList = m_pm->getAudioState()->getMasterEffects();
    } else {
        Node* n = m_pm->getNode(m_sgId);
        if (n) {
            if (n->type == "SampleGroup") {
                effectsList = static_cast<SampleGroup*>(n)->insertEffects;
            } else if (n->type == "Bus") {
                effectsList = static_cast<BusNode*>(n)->insertEffects;
            }
        }
    }

    for (const QUuid& fxId : effectsList) {
        Node* fxNode = m_pm->getNode(fxId);
        if (fxNode) {
            FxSlotWidget* slot = new FxSlotWidget(fxId, m_sgId, fxNode->type, fxNode->bypassed);
            connect(slot, &FxSlotWidget::requestEdit, this, [this](QUuid id) {
                EffectEditorDialog* dlg = new EffectEditorDialog(m_appCtrl, id, this);
                dlg->show();
            });
            connect(slot, &FxSlotWidget::requestBypassToggle, this, [this](QUuid id, bool bypassed) {
                Node* node = m_pm->getNode(id);
                if (node && node->bypassed != bypassed) {
                    m_pm->getUndoStack()->push(new ModifyPropertyCommand(m_pm, id, "bypassed", node->bypassed, bypassed));
                }
            });
            connect(slot, &FxSlotWidget::requestReorder, this, [this](QUuid srcFxId, QUuid srcChId, QUuid tgtFxId) {
                m_pm->reorderEffect(srcFxId, srcChId, tgtFxId, m_sgId);
            });
            m_fxContainerLayout->addWidget(slot);
        }
    }
}

bool FxRackWidget::eventFilter(QObject* obj, QEvent* event) {
    if (obj == m_fxContainer) {
        if (event->type() == QEvent::DragEnter) {
            QDragEnterEvent* de = static_cast<QDragEnterEvent*>(event);
            if (de->mimeData()->hasFormat("application/x-sampler-fx")) {
                de->acceptProposedAction();
                return true;
            }
        } else if (event->type() == QEvent::Drop) {
            QDropEvent* de = static_cast<QDropEvent*>(event);
            if (de->mimeData()->hasFormat("application/x-sampler-fx")) {
                QByteArray encodedData = de->mimeData()->data("application/x-sampler-fx");
                QList<QByteArray> parts = encodedData.split('|');
                if (parts.size() == 2) {
                    QUuid sourceFxId(QString::fromUtf8(parts[0]));
                    QUuid sourceChannelId(QString::fromUtf8(parts[1]));
                    m_pm->reorderEffect(sourceFxId, sourceChannelId, QUuid(), m_sgId);
                }
                de->acceptProposedAction();
                return true;
            }
        }
    }
    return QWidget::eventFilter(obj, event);
}
