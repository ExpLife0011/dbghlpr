#ifndef DBGRAPH_H
#define DBGRAPH_H

#include <QtWidgets/QMainWindow>
#include <qfile.h>
#include <qtextstream.h>
#include <qplaintextedit.h>
#include <qsplitter.h>
#include <qgridlayout.h>
#include <graph_view.h>
#include <syntax_highlighter.h>

#include <Windows.h>
#include <memory>
#include <list>
#include <map>
#include <set>

#include <interface.h>
#include <capstone.h>
#include <engine_linker.h>

#include <analyzer_wrapper.h>

#include "ui_dbgraph.h"

class dbgraph : public QMainWindow
{
	Q_OBJECT

public:
	dbgraph(QWidget *parent = 0);
	~dbgraph();

public:
	QPlainTextEdit *disassembly_text_browser_;
	SyntaxHighlighter *syntax_highlighter_;
	QGridLayout *mnemonic_tab_layout_;

	GraphView *graph_view_;
	QGridLayout *graph_tab_layout_;

	std::shared_ptr<engine::linker> engine_;

private:
	Ui::dbgraphClass ui;

private:
	bool setup();
	bool write();

public slots:
	void syntax_highlight();
};

#endif // DBGRAPH_H
