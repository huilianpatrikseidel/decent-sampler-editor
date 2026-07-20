#include "AboutDialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QSvgRenderer>
#include <QApplication>
#include <QPainter>
#include <QMouseEvent>
#include <QResizeEvent>

#include "../core/DatabaseManager.h"
#include "../core/ThemePalette.h"

AboutDialog::AboutDialog(DatabaseManager* dbManager, QWidget *parent) : QWidget(parent), m_contentContainer(nullptr) {
    // Hide immediately so we don't accidentally appear in the parent's pixmap grab
    hide();
    
    if (parent) {
        setGeometry(0, 0, parent->width(), parent->height());
        // Ensure we intercept parent resize events so we always cover the window perfectly
        parent->installEventFilter(this);
    } else {
        setFixedSize(600, 300);
    }
    
    m_renderer = new QSvgRenderer(QString(":/ui/about.svg"), this);
    
    m_contentContainer = new QWidget(this);
    m_contentContainer->setStyleSheet("background-color: rgba(0,0,0,0); border: none;");
    m_contentContainer->setAttribute(Qt::WA_TranslucentBackground);
    
    QVBoxLayout *contentLayout = new QVBoxLayout(m_contentContainer);
    // Restore the top margin to 85 so it stays safely below the orange bar
    contentLayout->setContentsMargins(30, 85, 30, 10);
    contentLayout->setSpacing(6); // A balanced spacing
    
    auto setupLabel = [](QLabel* label) {
        label->setAlignment(Qt::AlignCenter);
        label->setAttribute(Qt::WA_TranslucentBackground);
        label->setStyleSheet("background-color: rgba(0,0,0,0); border: none; color: #cccccc;");
    };
    
    QLabel *versionLabel = new QLabel(QString("Version %1").arg(QApplication::applicationVersion()), m_contentContainer);
    setupLabel(versionLabel);
    contentLayout->addWidget(versionLabel);
    
    QLabel *copyrightLabel = new QLabel("Copyright © Huilian Patrik Seidel<br><a href=\"http://huilianmusic.com\" style=\"color: #ff6600;\">huilianmusic.com</a>", m_contentContainer);
    setupLabel(copyrightLabel);
    copyrightLabel->setOpenExternalLinks(true);
    contentLayout->addWidget(copyrightLabel);
    
    QLabel *licenseLabel = new QLabel(m_contentContainer);
    // Restore <br><br> for proper visual paragraphs
    licenseLabel->setText("<b>GNU General Public License v2 (GPLv2)</b><br><br>"
                          "This program is free software; you can redistribute it and/or modify "
                          "it under the terms of the GNU General Public License as published by "
                          "the Free Software Foundation; either version 2 of the License, or "
                          "(at your option) any later version.<br><br>"
                          "This program is distributed in the hope that it will be useful, "
                          "but WITHOUT ANY WARRANTY; without even the implied warranty of "
                          "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.");
    licenseLabel->setWordWrap(true);
    licenseLabel->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    licenseLabel->setAttribute(Qt::WA_TranslucentBackground);
    // Removed the 5px padding to save 10px of vertical height!
    licenseLabel->setStyleSheet("background-color: rgba(0,0,0,0); border: none; color: #969696; font-size: 8pt;");
    contentLayout->addWidget(licenseLabel);
    
    if (dbManager) {
        QLabel* dbPathLabel = new QLabel(QString("Database Path: %1").arg(dbManager->getDatabasePath()), m_contentContainer);
        setupLabel(dbPathLabel);
        dbPathLabel->setStyleSheet("background-color: rgba(0,0,0,0); border: none; color: #777777; font-size: 8pt;");
        contentLayout->addWidget(dbPathLabel);
    }
    
    contentLayout->addStretch();
    
    int w = 600;
    int h = 300;
    int x = (width() - w) / 2;
    int y = (height() - h) / 2;
    m_contentContainer->setGeometry(x, y, w, h);
    
    show();
    raise();
}

bool AboutDialog::eventFilter(QObject *watched, QEvent *event) {
    if (watched == parentWidget() && event->type() == QEvent::Resize) {
        // Stay perfectly locked to the parent's size
        setGeometry(0, 0, parentWidget()->width(), parentWidget()->height());
        return false;
    }
    return QWidget::eventFilter(watched, event);
}

void AboutDialog::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    if (!m_contentContainer) return;
    
    int w = 600;
    int h = 300;
    int x = (width() - w) / 2;
    int y = (height() - h) / 2;
    m_contentContainer->setGeometry(x, y, w, h);
}

void AboutDialog::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);
}

void AboutDialog::mousePressEvent(QMouseEvent *event) {
    Q_UNUSED(event);
    deleteLater(); // We are an overlay widget, clicking deletes us!
}

void AboutDialog::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    painter.fillRect(rect(), ThemePalette::color("qss_color_30", QColor(15, 15, 18, 220)));
    
    if (m_renderer->isValid()) {
        int w = 600;
        int h = 300;
        int x = (width() - w) / 2;
        int y = (height() - h) / 2;
        m_renderer->render(&painter, QRectF(x, y, w, h));
    }
}
