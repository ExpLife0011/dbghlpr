#ifndef __DEFINE_DBGSUIT_SYNTAX_HIGHLIGHTER__
#define __DEFINE_DBGSUIT_SYNTAX_HIGHLIGHTER__

#include <QSyntaxHighlighter>

class SyntaxHighlighter : public QSyntaxHighlighter {

public:
	SyntaxHighlighter(QTextDocument* parent);
	void highlightBlock(const QString& text);
	void setHighlightedString(const QString& str);

private:
	QString m_highlightedString;

};

#endif