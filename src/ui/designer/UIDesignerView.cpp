#include "UIDesignerView.h"
#include "../../core/ProjectManager.h"
#include "../canvas/InstrumentCanvasView.h"
#include "../../commands/AddNodeCommand.h"
#include "../../commands/RemoveNodeCommand.h"
#include "../../commands/MoveNodeCommand.h"
#include "../../commands/UiCommands.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QToolBar>
#include <QStackedWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QPushButton>

UIDesignerView::UIDesignerView(ProjectManager* pm, QWidget* parent) 
    : QWidget(parent), m_pm(pm) 
{
    QVBoxLayout* uiLayout = new QVBoxLayout(this);
    
    QHBoxLayout* modeLayout = new QHBoxLayout();
    QLabel* modeLabel = new QLabel("UI Target Engine:", this);
    QComboBox* modeCombo = new QComboBox(this);
    modeCombo->addItem("Decent Sampler (Full Graphical Design)");
    modeCombo->addItem("Sforzando / SFZ (Controls Mapping)");
    modeLayout->addWidget(modeLabel);
    modeLayout->addWidget(modeCombo);
    modeLayout->addStretch();
    uiLayout->addLayout(modeLayout);
    
    QToolBar* designToolbar = new QToolBar(this);
    
    QAction* actAlignLeft = designToolbar->addAction("Align Left");
    QAction* actAlignCenter = designToolbar->addAction("Align Center");
    QAction* actAlignRight = designToolbar->addAction("Align Right");
    designToolbar->addSeparator();
    QAction* actAlignTop = designToolbar->addAction("Align Top");
    QAction* actAlignBottom = designToolbar->addAction("Align Bottom");
    
    QAction* actSnap = designToolbar->addAction("Snap to Grid");
    actSnap->setCheckable(true);
    actSnap->setChecked(true);
    
    QAction* actZoomIn = designToolbar->addAction("🔍+");
    QAction* actZoomOut = designToolbar->addAction("🔍-");
    QAction* actZoomFit = designToolbar->addAction("Fit");
    QAction* actZoom100 = designToolbar->addAction("100%");
    
    uiLayout->addWidget(designToolbar);
    
    QStackedWidget* uiStack = new QStackedWidget(this);
    uiLayout->addWidget(uiStack);
    
    m_uiCanvas = new InstrumentCanvasView(pm, uiStack);
    uiStack->addWidget(m_uiCanvas);
    
    connect(actSnap, &QAction::toggled, [this](bool checked) { m_uiCanvas->setSnapToGrid(checked); });
    connect(actZoomIn, &QAction::triggered, [this]() { m_uiCanvas->scale(1.15, 1.15); });
    connect(actZoomOut, &QAction::triggered, [this]() { m_uiCanvas->scale(1.0/1.15, 1.0/1.15); });
    connect(actZoomFit, &QAction::triggered, [this, pm]() { 
        QRectF rect(0, 0, pm->getUiState()->getWidth(), pm->getUiState()->getHeight());
        m_uiCanvas->fitInView(rect, Qt::KeepAspectRatio); 
    });
    connect(actZoom100, &QAction::triggered, [this]() { 
        m_uiCanvas->resetTransform(); 
    });
    
    connect(m_uiCanvas, &InstrumentCanvasView::notePressed, this, &UIDesignerView::notePressed);
    connect(m_uiCanvas, &InstrumentCanvasView::noteReleased, this, &UIDesignerView::noteReleased);
    
    auto addComponent = [pm](const QString& type) {
        std::unique_ptr<UiComponent> c;
        if (type == "UiKnob") c = std::make_unique<UiKnob>();
        else if (type == "UiSlider") c = std::make_unique<UiSlider>();
        else if (type == "UiButton") c = std::make_unique<UiButton>();
        else if (type == "UiLabel") c = std::make_unique<UiLabel>();
        else if (type == "UiRect" || type == "UiEllipse") {
            auto s = std::make_unique<UiShape>();
            s->shapeType = (type == "UiRect") ? "Rectangle" : "Ellipse";
            s->width = 100; s->height = 100;
            c = std::move(s);
        } else if (type == "UiImage") {
            auto img = std::make_unique<UiImage>();
            img->width = 200; img->height = 150;
            c = std::move(img);
        } else if (type == "UiMenu") {
            c = std::make_unique<UiMenu>();
        } else if (type == "UiXYPad") {
            c = std::make_unique<UiXYPad>();
        } else if (type == "UiOscilloscope") {
            c = std::make_unique<UiOscilloscope>();
        } else if (type == "UiKeyboard") {
            c = std::make_unique<UiKeyboard>();
        } else if (type == "UiTab") {
            c = std::make_unique<UiTab>();
        }
        if (c) {
            c->x = 50; c->y = 50; 
            pm->getUndoStack()->push(new AddUiComponentCommand(pm, std::move(c)));
        }
    };
    
    m_componentLibraryWidget = new QWidget();
    QVBoxLayout* libLayout = new QVBoxLayout(m_componentLibraryWidget);
    libLayout->setContentsMargins(5, 5, 5, 5);
    libLayout->setSpacing(8);
    
    QLabel* libTitle = new QLabel("<b>Tools</b>");
    libTitle->setObjectName("UIDesignerLibTitle");
    libTitle->setAlignment(Qt::AlignCenter);
    libLayout->addWidget(libTitle);
    
    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->setSpacing(6);
    libLayout->addLayout(gridLayout);
    
    struct ToolDef { QString type; QString icon; QString name; };
    std::vector<ToolDef> tools = {
        {"UiKnob", "🔘", "Knob"},
        {"UiSlider", "⎚", "Slider"},
        {"UiButton", "🟩", "Button"},
        {"UiLabel", "🔤", "Label"},
        {"UiMenu", "📄", "Menu"},
        {"UiRect", "▭", "Rect"},
        {"UiEllipse", "⬭", "Ellipse"},
        {"UiImage", "🖼️", "Image"},
        {"UiKeyboard", "🎹", "Keys"},
        {"UiXYPad", "✛", "XY Pad"},
        {"UiOscilloscope", "〰", "Scope"},
        {"UiTab", "🗂", "Tabs"}
    };
    
    int row = 0, col = 0;
    for (const auto& t : tools) {
        QPushButton* btn = new QPushButton(t.icon + "\n" + t.name);
        btn->setFixedSize(60, 60);
        btn->setObjectName("UIDesignerComponentBtn");

        QString type = t.type;
        connect(btn, &QPushButton::clicked, [addComponent, type]() { addComponent(type); });
        gridLayout->addWidget(btn, row, col);
        
        col++;
        if (col > 1) { col = 0; row++; }
    }
    libLayout->addStretch();
    
    connect(actAlignLeft, &QAction::triggered, [this](){ m_uiCanvas->alignSelectedItems("left"); });
    connect(actAlignCenter, &QAction::triggered, [this](){ m_uiCanvas->alignSelectedItems("center"); });
    connect(actAlignRight, &QAction::triggered, [this](){ m_uiCanvas->alignSelectedItems("right"); });
    connect(actAlignTop, &QAction::triggered, [this](){ m_uiCanvas->alignSelectedItems("top"); });
    connect(actAlignBottom, &QAction::triggered, [this](){ m_uiCanvas->alignSelectedItems("bottom"); });
    
    QTableWidget* sfzTable = new QTableWidget(0, 2, uiStack);
    sfzTable->setHorizontalHeaderLabels({"Knob Name", "Assigned MIDI CC"});
    sfzTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    sfzTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    uiStack->addWidget(sfzTable);
    
    connect(modeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [uiStack, pm, sfzTable](int index) {
        uiStack->setCurrentIndex(index);
        if (index == 1) { 
            sfzTable->setRowCount(0);
            int cc = 70;
            for (const auto& pair : pm->getAllUiComponents()) {
                const auto& comp = pair.second;
                if (comp->type == "UiKnob") {
                    int row = sfzTable->rowCount();
                    sfzTable->insertRow(row);
                    sfzTable->setItem(row, 0, new QTableWidgetItem(comp->name.isEmpty() ? "Knob" : comp->name));
                    sfzTable->setItem(row, 1, new QTableWidgetItem(QString("CC %1").arg(cc++)));
                }
            }
        }
    });
    
    connect(pm, &ProjectManager::uiComponentAdded, m_uiCanvas, &InstrumentCanvasView::onComponentAdded);
    connect(pm, &ProjectManager::uiComponentRemoved, m_uiCanvas, &InstrumentCanvasView::onComponentRemoved);
    connect(pm, &ProjectManager::uiComponentModified, m_uiCanvas, &InstrumentCanvasView::onComponentModified);
    
    connect(m_uiCanvas, &InstrumentCanvasView::requestMoveComponent, [pm](const QUuid& id, const QPointF& oldPos, const QPointF& newPos){
        pm->getUndoStack()->push(new MoveUiComponentCommand(pm, id, oldPos, newPos));
    });
    
    connect(m_uiCanvas, &InstrumentCanvasView::requestAddComponent, [pm](const QString& type, const QPointF& pos){
        std::unique_ptr<UiComponent> c;
        if (type == "UiKnob") c = std::make_unique<UiKnob>();
        else if (type == "UiLabel") c = std::make_unique<UiLabel>();
        else if (type == "UiRect" || type == "UiEllipse") {
            auto s = std::make_unique<UiShape>();
            s->shapeType = (type == "UiRect") ? "Rectangle" : "Ellipse";
            s->width = 100; s->height = 100;
            c = std::move(s);
        } else if (type == "UiImage") {
            auto img = std::make_unique<UiImage>();
            img->width = 200; img->height = 150;
            c = std::move(img);
        }
        if (c) {
            c->x = pos.x(); c->y = pos.y();
            pm->getUndoStack()->push(new AddUiComponentCommand(pm, std::move(c)));
        }
    });
    
    connect(m_uiCanvas, &InstrumentCanvasView::requestRemoveComponent, [pm](const QUuid& id){
        pm->getUndoStack()->push(new RemoveUiComponentCommand(pm, id));
    });
}
