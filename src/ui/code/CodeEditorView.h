#pragma once
#include <QWidget>
#include <QPlainTextEdit>
#include <QComboBox>
#include "core/ProjectManager.h"
#include "SyntaxHighlighter.h"

class CodeEditorView : public QWidget {
    Q_OBJECT
public:
    explicit CodeEditorView(ProjectManager* pm, QWidget *parent = nullptr);
    
public slots:
    void updateCode();

private:
    ProjectManager* m_pm;
    QPlainTextEdit* m_textEdit;
    QComboBox* m_formatCombo;
    
    XmlHighlighter* m_xmlHighlighter;
    SfzHighlighter* m_sfzHighlighter;
};
