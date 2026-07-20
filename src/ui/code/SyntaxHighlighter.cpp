#include "SyntaxHighlighter.h"
#include "../../core/ThemePalette.h"

// --- XmlHighlighter ---
XmlHighlighter::XmlHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    attributeFormat.setForeground(ThemePalette::color("code_attr", QColor("#9CDCFE")));
    rule.pattern = QRegularExpression(QStringLiteral("\\b[a-zA-Z\\-0-9_]+(?=\\=)"));
    rule.format = attributeFormat;
    highlightingRules.append(rule);

    attributeValueFormat.setForeground(ThemePalette::color("code_val", QColor("#CE9178")));
    rule.pattern = QRegularExpression(QStringLiteral("\"[^\"]*\"|'[^']*'"));
    rule.format = attributeValueFormat;
    highlightingRules.append(rule);

    tagFormat.setForeground(ThemePalette::color("code_tag", QColor("#569CD6")));
    rule.pattern = QRegularExpression(QStringLiteral("<\\/?[a-zA-Z\\-0-9_]+"));
    rule.format = tagFormat;
    highlightingRules.append(rule);
    
    rule.pattern = QRegularExpression(QStringLiteral(">?/?>"));
    rule.format = tagFormat;
    highlightingRules.append(rule);

    commentFormat.setForeground(ThemePalette::color("code_comment", QColor("#6A9955")));
    commentStartExpression = QRegularExpression(QStringLiteral("<!--"));
    commentEndExpression = QRegularExpression(QStringLiteral("-->"));
}

void XmlHighlighter::highlightBlock(const QString &text)
{
    for (const HighlightingRule &rule : highlightingRules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    setCurrentBlockState(0);
    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(commentStartExpression);

    while (startIndex >= 0) {
        QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int commentLength = 0;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex + match.capturedLength();
        }
        setFormat(startIndex, commentLength, commentFormat);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }
}

// --- SfzHighlighter ---
SfzHighlighter::SfzHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    headerFormat.setForeground(ThemePalette::color("code_header", QColor("#C586C0")));
    headerFormat.setFontWeight(QFont::Bold);
    rule.pattern = QRegularExpression(QStringLiteral("<[a-zA-Z_]+>"));
    rule.format = headerFormat;
    highlightingRules.append(rule);

    opcodeFormat.setForeground(ThemePalette::color("code_attr", QColor("#9CDCFE")));
    rule.pattern = QRegularExpression(QStringLiteral("\\b[a-zA-Z0-9_]+(?=\\=)"));
    rule.format = opcodeFormat;
    highlightingRules.append(rule);

    valueFormat.setForeground(ThemePalette::color("code_val", QColor("#CE9178")));
    rule.pattern = QRegularExpression(QStringLiteral("(?<=\\=)[^\\s]+"));
    rule.format = valueFormat;
    highlightingRules.append(rule);

    commentFormat.setForeground(ThemePalette::color("code_comment", QColor("#6A9955")));
    rule.pattern = QRegularExpression(QStringLiteral("//[^\n]*"));
    rule.format = commentFormat;
    highlightingRules.append(rule);
}

void SfzHighlighter::highlightBlock(const QString &text)
{
    for (const HighlightingRule &rule : highlightingRules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}
