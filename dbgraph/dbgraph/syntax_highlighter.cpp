#include "syntax_highlighter.h"

SyntaxHighlighter::SyntaxHighlighter(QTextDocument* parent) : QSyntaxHighlighter(parent)
{
}

void SyntaxHighlighter::highlightBlock(const QString& text)
{
	if (m_highlightedString.isEmpty())
	{
		return;
	}

	QTextCharFormat fmt;
	QColor txt = QColor("#a3caff");
	fmt.setFont(QFont("Times", 10, QFont::Bold));
	//fmt.setBackground(txt);
	fmt.setForeground(QColor("red"));

	const int LEN = m_highlightedString.length();
	for (int index = text.indexOf(m_highlightedString); 0 <= index; index = text.indexOf(m_highlightedString, index + LEN))
	{
		setFormat(index, LEN, fmt);
	}
}

void SyntaxHighlighter::setHighlightedString(const QString& str)
{
	m_highlightedString = str;
	rehighlight();
}
