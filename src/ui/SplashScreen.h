#pragma once
#include <QWidget>
#include <QSvgRenderer>

class SplashScreen : public QWidget {
    Q_OBJECT
public:
    explicit SplashScreen(QWidget *parent = nullptr);

public slots:
    void setProgress(int percent, const QString& message);

signals:
    void cancelRequested();

protected:
    void paintEvent(QPaintEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    QSvgRenderer m_renderer;
    int m_percent = 0;
    QString m_message = "Starting...";
};
