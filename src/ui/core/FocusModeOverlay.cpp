#include "FocusModeOverlay.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include "../../core/ThemePalette.h"
#include <QPushButton>
#include <QTabWidget>
#include <QEvent>
#include "../editor/SampleEditorContainer.h"

FocusModeOverlay::FocusModeOverlay(QWidget* parent, QTabWidget* editorTabs, SampleEditorContainer* sampleEditor)
    : QDialog(parent), m_editorTabs(editorTabs), m_sampleEditor(sampleEditor), m_contentContainer(nullptr) 
{
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    
    if (parent) {
        QPoint p = parent->mapToGlobal(QPoint(0, 0));
        setGeometry(p.x(), p.y(), parent->width(), parent->height());
        parent->installEventFilter(this);
    }
    
    m_contentContainer = new QWidget(this);
    m_contentContainer->setObjectName("FocusContainer");
    m_contentContainer->setStyleSheet("#FocusContainer { background-color: #1e1e1e; border: 1px solid #ff6600; border-radius: 8px; }");
    
    QVBoxLayout* layout = new QVBoxLayout(m_contentContainer);
    layout->setContentsMargins(10, 10, 10, 10);
    
    // Top bar for the close button
    QHBoxLayout* topBar = new QHBoxLayout();
    topBar->addStretch();
    QPushButton* closeBtn = new QPushButton("X Exit Focus Mode", m_contentContainer);
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setStyleSheet("background-color: transparent; color: #ff6600; font-weight: bold; padding: 5px; border: 1px solid #ff6600; border-radius: 4px;");
    topBar->addWidget(closeBtn);
    layout->addLayout(topBar);
    
    connect(closeBtn, &QPushButton::clicked, this, &FocusModeOverlay::closeOverlay);
    
    // Switch sample editor to focus mode (vertical layout)
    if (m_sampleEditor) {
        m_sampleEditor->setFocusMode(true);
    }
    
    // Reparent editor tabs to this overlay
    layout->addWidget(m_editorTabs);
    
    int margin = 40;
    m_contentContainer->setGeometry(margin, margin, width() - 2 * margin, height() - 2 * margin);
    
    show();
    raise();
}

FocusModeOverlay::~FocusModeOverlay() {
    // Revert focus mode
    if (m_sampleEditor) {
        m_sampleEditor->setFocusMode(false);
    }
}

void FocusModeOverlay::closeOverlay() {
    emit closed();
    deleteLater();
}

bool FocusModeOverlay::eventFilter(QObject *watched, QEvent *event) {
    if (watched == parentWidget() && (event->type() == QEvent::Resize || event->type() == QEvent::Move)) {
        QPoint p = parentWidget()->mapToGlobal(QPoint(0, 0));
        setGeometry(p.x(), p.y(), parentWidget()->width(), parentWidget()->height());
        int margin = 40;
        if (m_contentContainer) {
            m_contentContainer->setGeometry(margin, margin, width() - 2 * margin, height() - 2 * margin);
        }
        return false;
    }
    return QDialog::eventFilter(watched, event);
}

void FocusModeOverlay::resizeEvent(QResizeEvent *event) {
    QDialog::resizeEvent(event);
    if (!m_contentContainer) return;
    int margin = 40;
    m_contentContainer->setGeometry(margin, margin, width() - 2 * margin, height() - 2 * margin);
}

void FocusModeOverlay::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Just draw a dark semi-transparent background to avoid Qt native window grab bugs
    QColor bgColor = ThemePalette::color("qss_color_1", QColor(15, 15, 15));
    bgColor.setAlpha(220);
    painter.fillRect(rect(), bgColor);
}
