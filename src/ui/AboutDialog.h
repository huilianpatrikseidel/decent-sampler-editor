#pragma once
#include <QWidget>
#include <QPixmap>

class QSvgRenderer;
class QMouseEvent;

class DatabaseManager;

class AboutDialog : public QWidget {
    Q_OBJECT
    
public:
    explicit AboutDialog(DatabaseManager* dbManager, QWidget *parent = nullptr);
    
protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;
    
private:
    QSvgRenderer* m_renderer;
    QWidget* m_contentContainer;
};
