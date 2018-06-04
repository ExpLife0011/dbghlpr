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
#include <strsafe.h>

#include "ui_dbgraph.h"

//#define USE_CMD_LOG_VIEWER

typedef struct _tag_node_info_tmp_
{
	unsigned long long address;
	unsigned long long jmp_dest;
	unsigned long long next_address;
	bool jmp;
	code_graph::node * node;
}node_info_tmp;

typedef struct _tag_tmp_node_
{
	std::set<unsigned long long> address_set;
	unsigned long long next_address;
	unsigned long long jump_address;

	code_graph::node * node;
}tnode;

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
	node_info_tmp find_node_info(std::list<node_info_tmp> node_info_list, unsigned long long address);
	void draw_graph();

	//
	// new 
	//
	tnode * create_node(ogdf::Graph *g, ogdf::GraphAttributes *ga, std::set<unsigned long long> &address_set, unsigned long long next_address, unsigned long long jump_address);
	tnode *find(std::list<tnode *> tnode_list, unsigned long long address);
public slots:
	void syntax_highlight();
};

#endif // DBGRAPH_H
