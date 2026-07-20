#include "CanvasInspectorStrategy.h"
#include "../PropertiesInspector.h"
#include "../../../core/models/ProjectStates.h"
#include <QLabel>
#include <QSpinBox>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QPixmap>

CanvasInspectorStrategy::CanvasInspectorStrategy(UiStateModel* uiState)
    : m_uiState(uiState) {}

void CanvasInspectorStrategy::buildForm(QFormLayout* layout, PropertiesInspector* inspector) {
    if (!m_uiState) return;

    QLabel* title = new QLabel("<b>Canvas Properties</b>");
    layout->addRow(title);
    
    QSpinBox* wSpin = new QSpinBox();
    wSpin->setRange(100, 4000);
    wSpin->setValue(m_uiState->getWidth());
    
    QSpinBox* hSpin = new QSpinBox();
    hSpin->setRange(100, 4000);
    hSpin->setValue(m_uiState->getHeight());
    
    QLabel* lblW = new QLabel("Width");
    lblW->setMinimumWidth(85);
    layout->addRow(lblW, wSpin);
    
    QLabel* lblH = new QLabel("Height");
    lblH->setMinimumWidth(85);
    layout->addRow(lblH, hSpin);
    
    QObject::connect(wSpin, &QSpinBox::editingFinished, inspector, [inspector, wSpin, hSpin]() {
        if (!inspector->isUpdatingUI()) emit inspector->canvasSizePropertyChanged(wSpin->value(), hSpin->value());
    });
    QObject::connect(hSpin, &QSpinBox::editingFinished, inspector, [inspector, wSpin, hSpin]() {
        if (!inspector->isUpdatingUI()) emit inspector->canvasSizePropertyChanged(wSpin->value(), hSpin->value());
    });
    
    QLineEdit* bgImageEdit = new QLineEdit(m_uiState->getBgImage());
    bgImageEdit->setPlaceholderText("Background Image Path");
    QLabel* lblBg = new QLabel("Background");
    lblBg->setMinimumWidth(85);
    layout->addRow(lblBg, bgImageEdit);
    
    QObject::connect(bgImageEdit, &QLineEdit::editingFinished, inspector, [inspector, this, bgImageEdit]() {
        if (!inspector->isUpdatingUI()) {
            emit inspector->canvasBgPropertyChanged(bgImageEdit->text(), static_cast<int>(m_uiState->getBgMode()));
        }
    });
    
    QComboBox* bgModeCombo = new QComboBox();
    bgModeCombo->addItem("Stretch", static_cast<int>(BgMode::Stretch));
    bgModeCombo->addItem("Center", static_cast<int>(BgMode::Center));
    bgModeCombo->addItem("Absolute", static_cast<int>(BgMode::Absolute));
    bgModeCombo->setCurrentIndex(bgModeCombo->findData(static_cast<int>(m_uiState->getBgMode())));
    
    QObject::connect(bgModeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), inspector, [inspector, this, bgModeCombo]() {
        if (!inspector->isUpdatingUI()) {
            emit inspector->canvasBgPropertyChanged(m_uiState->getBgImage(), bgModeCombo->currentData().toInt());
        }
    });
    layout->addRow("Bg Mode", bgModeCombo);
    
    QPushButton* btnResize = new QPushButton("Resize Canvas to Image");
    QObject::connect(btnResize, &QPushButton::clicked, inspector, [inspector, this]() {
        QString imgPath = m_uiState->getBgImage();
        if (!imgPath.isEmpty()) {
            QPixmap pix(imgPath);
            if (!pix.isNull()) {
                emit inspector->canvasSizePropertyChanged(pix.width(), pix.height());
            }
        }
    });
    layout->addRow("", btnResize);
}
