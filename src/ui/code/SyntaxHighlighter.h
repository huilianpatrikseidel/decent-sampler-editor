#pragma once
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>

class XmlHighlighter : public QSyntaxHighlighter {
    Q_OBJECT
public:
    explicit XmlHighlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightingRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QTextCharFormat tagFormat;
    QTextCharFormat attributeFormat;
    QTextCharFormat attributeValueFormat;
    QTextCharFormat commentFormat;
    
    QRegularExpression commentStartExpression;
    QRegularExpression commentEndExpression;
};

class SfzHighlighter : public QSyntaxHighlighter {
    Q_OBJECT
public:
    explicit SfzHighlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightingRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QTextCharFormat headerFormat;
    QTextCharFormat opcodeFormat;
    QTextCharFormat valueFormat;
    QTextCharFormat commentFormat;
};
