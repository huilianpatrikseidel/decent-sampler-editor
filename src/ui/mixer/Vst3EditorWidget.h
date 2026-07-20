#pragma once

#include <QWidget>
#include <memory>
#include "../../../src/audio/Vst3Host.h"

#include <QWindow>


#include "ui/mixer/MixerLibExport.h"
class MIXERLIB_EXPORT Vst3ContainerWindow : public QWindow {
    Q_OBJECT
public:
    explicit Vst3ContainerWindow(std::shared_ptr<Vst3Host> host, QWindow* parent = nullptr);
    ~Vst3ContainerWindow() override;

protected:
    void exposeEvent(QExposeEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    std::shared_ptr<Vst3Host> m_host;
    bool m_uiOpened = false;
};

class MIXERLIB_EXPORT Vst3EditorWidget : public QWidget {
    Q_OBJECT
public:
    explicit Vst3EditorWidget(std::shared_ptr<Vst3Host> host, QWidget* parent = nullptr);
    ~Vst3EditorWidget() override;

private:
    Vst3ContainerWindow* m_window;
    std::shared_ptr<Vst3Host> m_host;
};
