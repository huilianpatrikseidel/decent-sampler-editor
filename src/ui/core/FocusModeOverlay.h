#pragma once
#include <QDialog>
#include <QPixmap>

class QTabWidget;
class SampleEditorContainer;
class QSvgRenderer;
class QMouseEvent;

class FocusModeOverlay : public QDialog {
    Q_OBJECT
    
public:
    explicit FocusModeOverlay(QWidget* parent = nullptr, QTabWidget* editorTabs = nullptr, SampleEditorContainer* sampleEditor = nullptr);
    ~FocusModeOverlay() override;
    
    void closeOverlay();
    
signals:
    void closed();
    
protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;
    
private:
    QTabWidget* m_editorTabs;
    SampleEditorContainer* m_sampleEditor;
    QWidget* m_contentContainer;
};
