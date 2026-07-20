#include "CodeEditorView.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QApplication>
#include <QClipboard>
#include <QFontDatabase>
#include "transpilers/DecentSamplerTranspiler.h"
#include "transpilers/SfzTranspiler.h"

CodeEditorView::CodeEditorView(ProjectManager* pm, QWidget *parent) 
    : QWidget(parent), m_pm(pm) 
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // Toolbar
    QWidget* toolbar = new QWidget(this);
    toolbar->setFixedHeight(40);
    toolbar->setObjectName("CodeEditorToolbar");
    QHBoxLayout* toolbarLayout = new QHBoxLayout(toolbar);
    toolbarLayout->setContentsMargins(10, 0, 10, 0);
    
    m_formatCombo = new QComboBox(toolbar);
    m_formatCombo->addItem("Decent Sampler (XML)");
    m_formatCombo->addItem("SFZ");
    m_formatCombo->setFixedWidth(200);
    m_formatCombo->setObjectName("CodeEditorCombo");
    
    QPushButton* btnRefresh = new QPushButton("Refresh", toolbar);
    btnRefresh->setObjectName("CodeEditorBtn");
    
    QPushButton* btnCopy = new QPushButton("Copy to Clipboard", toolbar);
    btnCopy->setObjectName("CodeEditorCopyBtn");
    
    toolbarLayout->addWidget(m_formatCombo);
    toolbarLayout->addStretch();
    toolbarLayout->addWidget(btnRefresh);
    toolbarLayout->addWidget(btnCopy);
    
    mainLayout->addWidget(toolbar);
    
    // Editor
    m_textEdit = new QPlainTextEdit(this);
    m_textEdit->setReadOnly(true);
    m_textEdit->setObjectName("CodeEditorTextEdit");
    
    const QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    m_textEdit->setFont(fixedFont);
    
    mainLayout->addWidget(m_textEdit);
    
    // Highlighters
    m_xmlHighlighter = new XmlHighlighter(m_textEdit->document());
    m_sfzHighlighter = new SfzHighlighter(nullptr); // Only attach one at a time
    
    // Connections
    connect(m_formatCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
        if (index == 0) {
            m_sfzHighlighter->setDocument(nullptr);
            m_xmlHighlighter->setDocument(m_textEdit->document());
        } else {
            m_xmlHighlighter->setDocument(nullptr);
            m_sfzHighlighter->setDocument(m_textEdit->document());
        }
        updateCode();
    });
    
    connect(btnRefresh, &QPushButton::clicked, this, &CodeEditorView::updateCode);
    
    connect(btnCopy, &QPushButton::clicked, this, [this]() {
        QApplication::clipboard()->setText(m_textEdit->toPlainText());
    });
}

void CodeEditorView::updateCode() {
    if (!m_pm) return;
    
    if (m_formatCombo->currentIndex() == 0) {
        DecentSamplerTranspiler transpiler;
        QString code = transpiler.generate(m_pm);
        m_textEdit->setPlainText(code);
    } else {
        SfzTranspiler transpiler;
        QString code = transpiler.generate(m_pm);
        m_textEdit->setPlainText(code);
    }
}
