#include "Vst3EditorWidget.h"
#include <QResizeEvent>
#include <QVBoxLayout>
#include <QTimer>
#include <QExposeEvent>
#include <QSlider>
#include <QLabel>

Vst3ContainerWindow::Vst3ContainerWindow(std::shared_ptr<Vst3Host> host, QWindow* parent)
    : QWindow(parent), m_host(host)
{
    setFlags(Qt::FramelessWindowHint);
    create(); // Ensure the native HWND is created
}

Vst3ContainerWindow::~Vst3ContainerWindow() {
    if (m_host && m_uiOpened) {
        m_host->closeUI();
    }
}

void Vst3ContainerWindow::exposeEvent(QExposeEvent* event) {
    QWindow::exposeEvent(event);
    if (!m_uiOpened && m_host && isExposed()) {
        void* hwnd = reinterpret_cast<void*>(this->winId());
        QTimer::singleShot(0, this, [this, hwnd]() {
            if (m_host) m_host->openUI(hwnd);
        });
        m_uiOpened = true;
    }
}

void Vst3ContainerWindow::resizeEvent(QResizeEvent* event) {
    QWindow::resizeEvent(event);
    if (m_host && m_uiOpened) {
        m_host->resizeUI(event->size().width(), event->size().height());
    }
}

Vst3EditorWidget::Vst3EditorWidget(std::shared_ptr<Vst3Host> host, QWidget* parent)
    : QWidget(parent), m_host(host)
{
    m_window = new Vst3ContainerWindow(host);
    QWidget* container = QWidget::createWindowContainer(m_window, this);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(container);
    
    // Add generic slider for Parameter 0 to fulfill Task 12
    if (m_host) {
        float val = m_host->getParameter(0); // try to read getParameter
        QSlider* paramSlider = new QSlider(Qt::Horizontal);
        paramSlider->setRange(0, 100);
        paramSlider->setValue(static_cast<int>(val * 100.0f));
        
        QLabel* paramLabel = new QLabel("Param 0 (Generic)");
        layout->addWidget(paramLabel);
        layout->addWidget(paramSlider);
        
        connect(paramSlider, &QSlider::valueChanged, this, [this](int value) {
            m_host->setParameter(0, value / 100.0f);
        });
    }
    
    setFixedSize(320, 290);
}

Vst3EditorWidget::~Vst3EditorWidget() {
}
