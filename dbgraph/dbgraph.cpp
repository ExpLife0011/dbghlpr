#include "dbgraph.h"

QColor lineColor;

dbgraph::dbgraph(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	char css_path[MAX_PATH] = { 0, };
	GetCurrentDirectoryA(MAX_PATH, css_path);
	StringCbCatA(css_path, MAX_PATH, "\\dbghlpr\\css\\style.qss");
	//printf("%s\n", css_path);
	QFile f(css_path);
	if (f.exists())
	{
		f.open(QFile::ReadOnly | QFile::Text);
		QTextStream ts(&f);
		qApp->setStyleSheet(ts.readAll());
		lineColor = QColor("#000000").light(160);
	}
	else
	{
		lineColor = QColor("#509CE4").light(160);
	}

	disassembly_text_browser_ = new QPlainTextEdit(ui.mnemonic_tab);
	disassembly_text_browser_->setStyleSheet(
		"QPlainTextEdit { selection-background-color: transparent;}"
		"QPlainTextEdit { selection-color: red; }"
	);
	disassembly_text_browser_->setFont(QFont("Consolas", 10));
	disassembly_text_browser_->setReadOnly(true);

	syntax_highlighter_ = new SyntaxHighlighter(disassembly_text_browser_->document());
	QObject::connect(disassembly_text_browser_, SIGNAL(cursorPositionChanged()), this, SLOT(syntax_highlight()));

	mnemonic_tab_layout_ = new QGridLayout(ui.mnemonic_tab);
	mnemonic_tab_layout_->setSpacing(6);
	mnemonic_tab_layout_->setContentsMargins(11, 11, 11, 11);
	mnemonic_tab_layout_->setObjectName(QStringLiteral("gridLayout"));
	mnemonic_tab_layout_->addWidget(disassembly_text_browser_);

	graph_view_ = new GraphView(ui.graph_tab);
	graph_tab_layout_ = new QGridLayout(ui.graph_tab);
	graph_tab_layout_->setSpacing(6);
	graph_tab_layout_->setContentsMargins(11, 11, 11, 11);
	graph_tab_layout_->setObjectName(QStringLiteral("gridLayout"));
	graph_tab_layout_->addWidget(graph_view_);

	if (setup())
	{
		if (write())
		{
			this->setWindowTitle(QCoreApplication::arguments().at(2));
			ui.dissassembly_tab_widget->setCurrentIndex(0);
		}
	}
}

dbgraph::~dbgraph()
{

}

bool dbgraph::setup()
{
	if (wcsstr(QCoreApplication::arguments().at(1).toStdWString().c_str(), L"-z"))
	{
		if (!engine::create<engine_linker>(engine_))
		{
			return false;
		}

		if (!engine_->open((char *)QCoreApplication::arguments().at(2).toStdString().c_str()))
		{
			return false;
		}

		return true;
	}

	return false;
}

//
//
//
using namespace ogdf;

tnode * dbgraph::create_node(Graph *g, GraphAttributes *ga, std::set<unsigned long long> &address_set, unsigned long long next_address, unsigned long long jump_address)
{
	tnode *n = new tnode();

	n->address_set.insert(address_set.begin(), address_set.end());
	n->next_address = next_address;
	n->jump_address = jump_address;

	n->node = new code_graph::node;
	QString print;
	std::set<unsigned long long>::iterator it = n->address_set.begin();
	for (it; it != n->address_set.end(); ++it)
	{
		char d[1024] = { 0, };
		unsigned long size_of_disasm = 0;
		unsigned long long next_address = 0;
		engine_->disasm(*it, d, sizeof(d), &size_of_disasm, &next_address);

		print += d;
	}
	n->node->setting(g, ga, print);

	address_set.clear();
	return n;
}

tnode *dbgraph::find(std::list<tnode *> tnode_list, unsigned long long address)
{
	tnode *f = nullptr;
	std::list<tnode *>::iterator it = tnode_list.begin();
	for (it; it != tnode_list.end(); ++it)
	{
		tnode *n = *it;
		std::set<unsigned long long>::iterator sit = n->address_set.find(address);
		if (sit != n->address_set.end())
		{
			f = n;
			break;
		}
	}

	return f;
}

bool dbgraph::write()
{
	if (!wcsstr(QCoreApplication::arguments().at(3).toStdWString().c_str(), L"-p"))
	{
		return false;
	}

	char *e = nullptr;
	unsigned long long ptr = strtoull(QCoreApplication::arguments().at(4).toStdString().c_str(), &e, 16);
	MEMORY_BASIC_INFORMATION64 mbi = { 0, };
	if (!engine_->query_virtual(ptr, &mbi))
	{
		return false;
	}
	unsigned long long base = mbi.BaseAddress;
	unsigned long long end = base + mbi.RegionSize;

	if (analyzer_wrapper::check(base, end, ptr - 0x1000))
	{
		base = ptr - 0x1000;
	}
	ptr = analyzer_wrapper::find_entry(ptr, base, end - base);
	//printf("%I64x %I64x-%I64x\n", ptr, base, end);

	if (ptr == 0)
	{
		return false;
	}

	analyzer an(base, end - base);
	unsigned char *memory_dump = an.alloc(engine_);
	if (!memory_dump)
	{
		return false;
	}
	std::shared_ptr<void> memory_dump_closer(memory_dump, free);

	std::set<unsigned long long> visited;
	analyzer::block *b = new analyzer::block;
	memset(b->tag, 0, sizeof(b->tag));

	an.trace(engine_, ptr, memory_dump, visited, *b);
	std::map<unsigned long long, analyzer::detail *>::iterator address_detail_map_b = b->address_map.begin();
	std::map<unsigned long long, analyzer::detail *>::iterator address_detail_map_e = b->address_map.end();
	std::map<unsigned long long, analyzer::detail *>::iterator address_detail_map_t = b->address_map.end();

	end = (--address_detail_map_t)->first + 0x16;

	std::multimap<unsigned long long, unsigned long long> ref_map;
	analyzer_wrapper::find_reference_value(ptr, end-ptr, ref_map);

	//printf("%I64x-%I64x %d\n", ptr, end, ref_map.size());

	QString print;
	for (address_detail_map_b; address_detail_map_b != address_detail_map_e; ++address_detail_map_b)
	{
		std::multimap<unsigned long long, unsigned long long>::iterator fi = ref_map.find(address_detail_map_b->first);
		if (fi != ref_map.end())
		{
			print += "\n";
		}

		char d[1024] = { 0, };
		unsigned long size_of_disasm = 0;
		unsigned long long next_address = 0;
		engine_->disasm(address_detail_map_b->first, d, sizeof(d), &size_of_disasm, &next_address);
		print += d;

		if (address_detail_map_b->second->is_jmp_code)
		{
			address_detail_map_t = address_detail_map_b;
			++address_detail_map_t;

			fi = ref_map.find(address_detail_map_t->first);

			if (fi == ref_map.end())
			{
				print += "\n";
			}
		}
	}

	disassembly_text_browser_->insertPlainText(print);
	disassembly_text_browser_->moveCursor(QTextCursor::Start);

	//
	//
	//
	Graph G;
	GraphAttributes GA(G, GraphAttributes::nodeGraphics |
		GraphAttributes::edgeGraphics |
		GraphAttributes::nodeLabel |
		GraphAttributes::nodeStyle |
		GraphAttributes::edgeType |
		GraphAttributes::edgeArrow |
		GraphAttributes::edgeStyle);

	print = "";
	std::list<tnode *> temp_node_list;
	std::set<unsigned long long> address_set;
	for (address_detail_map_b = b->address_map.begin(); address_detail_map_b != address_detail_map_e; ++address_detail_map_b) // create node
	{
		std::multimap<unsigned long long, unsigned long long>::iterator fi = ref_map.find(address_detail_map_b->first);
		if (fi != ref_map.end() && address_set.size())
		{
			//std::map<unsigned long long, analyzer::detail *>::iterator next = address_detail_map_b;
			tnode *n = create_node(&G, &GA, address_set, address_detail_map_b->first, 0);
			temp_node_list.push_back(n);
		}

		address_set.insert(address_detail_map_b->first);

		if ((address_detail_map_b->second->is_jmp_code && address_detail_map_b->second->instruction_id != X86_INS_JMP) && address_set.size())
		{
			std::map<unsigned long long, analyzer::detail *>::iterator next = address_detail_map_b;
			tnode *n = create_node(&G, &GA, address_set, (++next)->first, address_detail_map_b->second->operands[0].value);

			temp_node_list.push_back(n);
		}

		if (address_detail_map_b->second->instruction_id == X86_INS_JMP && address_set.size())
		{
			tnode *n = create_node(&G, &GA, address_set, 0, address_detail_map_b->second->operands[0].value);
			temp_node_list.push_back(n);
		}
	}

	if (address_set.size())
	{
		tnode *n = create_node(&G, &GA, address_set, 0, 0);
		temp_node_list.push_back(n);
	}

	//printf("node_count : %d\n", temp_node_list.size());
	std::list<tnode *>::iterator it = temp_node_list.begin();
	for (it; it != temp_node_list.end(); ++it)
	{
		tnode *src = *it;
#if 0
		std::set<unsigned long long>::iterator sit;
		for (sit = src->address_set.begin(); sit != src->address_set.end(); ++sit)
		{
			printf("%I64x\n", *sit);
		}
		printf("%d %d %I64x %I64x\n", src->node->get_left(), src->node->get_right(), src->next_address, src->jump_address);
#endif
		tnode *jump = nullptr;
		if (src->jump_address)
		{
			jump = find(temp_node_list, src->jump_address);
			//printf("jmp %I64x\n", *(jump->address_set.begin()));
		}

		tnode *next = nullptr;
		if (src->next_address)
		{
			next = find(temp_node_list, src->next_address);
			//printf("nxt %I64x\n", *(next->address_set.begin()));
		}
		//printf("\n");

		if (jump) // check jxx
		{
			if (!src->node->get_left()) // only jump
			{
				src->node->add_left(jump->node);
			}
		}
		
		if(next)
		{
			if (!src->node->get_right())
			{
				src->node->add_right(next->node);
			}
		}
	}

	tnode *rn = *(temp_node_list.begin());
	code_graph::node *root = rn->node;
	code_graph::tree t;
	t.set_root(root);

	graph_view_->draw(&G, &GA, &t);

#if 0
	std::list<node_info_tmp>::iterator node_it = node_info_list.begin();
	for (node_it = node_info_list.begin(); node_it != node_info_list.end(); ++node_it)
	{
#ifdef USE_CMD_LOG_VIEWER
		printf("node info:: %08x %08x %08x\n", (unsigned long)node_it->address, (unsigned long)node_it->jmp_dest, (unsigned long)node_it->next_address);
#endif
		node_info_tmp src = *node_it;
		node_info_tmp dest = { 0, };
		node_info_tmp next = { 0, };

		if (node_it->jmp_dest != 0)
		{
			dest = find_node_info(node_info_list, node_it->jmp_dest);
		}

		if (node_it->next_address != 0)
		{
			next = find_node_info(node_info_list, node_it->next_address);
		}

#ifdef USE_CMD_LOG_VIEWER
		printf("1. s=%I64x n=%I64x d=%I64x\n", src.address, next.address, dest.address);
		printf("1. %d %d %d\n", src.jmp, next.jmp, dest.jmp);
#endif

		if (dest.address)
		{
			if (dest.node)
			{
				if (!src.node->get_left())
				{
					printf("add l\n");
					src.node->add_left(dest.node);
				}
			}

			if (next.node && src.jmp == false)
			{
				if (!src.node->get_right())
				{
					src.node->add_right(next.node);
				}

				if (src.jmp_dest > src.next_address)
				{
					std::list<node_info_tmp>::iterator jmp_it = node_it;
					++jmp_it, ++jmp_it;

					if (jmp_it != node_info_list.end() && jmp_it->address == dest.address)
					{
						if (!next.node->get_right())
						{
							printf("add r\n");

							next.node->add_right(dest.node);
						}
					}
				}
			}
		}
		else if (next.address)
		{
			if (next.node)
			{
				if (!src.node->get_right())
				{
					printf("add r\n");

					src.node->add_right(next.node);
				}
			}
		}

		printf("\n");
	}

	code_graph::node *root = (node_info_list.begin())->node;
	code_graph::tree t;
	t.set_root(root);

	graph_view_->draw(&G, &GA, &t);
#endif
	return true;
}

//
// draw
//
node_info_tmp dbgraph::find_node_info(std::list<node_info_tmp> node_info_list, unsigned long long address)
{
	std::list<node_info_tmp>::iterator node_it = node_info_list.begin();
	char text[100];
	sprintf(text, "%x ", (unsigned long)address);

	for (node_it; node_it != node_info_list.end(); ++node_it)
	{
		//printf("%s %s\n", node_it->node->get_data().toStdString().c_str(), text);
		if (strstr(node_it->node->get_data().toStdString().c_str(), text))
		{
			return *node_it;
		}
	}

	return{ 0 };
}

void dbgraph::draw_graph()
{
}


//
//
//
void dbgraph::syntax_highlight()
{
	QList<QTextEdit::ExtraSelection> extraSelections;
	QTextEdit::ExtraSelection selection;

	selection.format.setBackground(lineColor);
	//selection.format.setForeground(Qt::white);
	selection.format.setProperty(QTextFormat::FullWidthSelection, true);
	selection.cursor = disassembly_text_browser_->textCursor();
	selection.cursor.clearSelection();
	extraSelections.append(selection);
	disassembly_text_browser_->setExtraSelections(extraSelections);

	syntax_highlighter_->setHighlightedString(disassembly_text_browser_->textCursor().selectedText());
}

