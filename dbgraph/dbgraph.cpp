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
	std::list<node_info_tmp> node_info_list;
	for (address_detail_map_b = b->address_map.begin(); address_detail_map_b != address_detail_map_e; ++address_detail_map_b)
	{

		char d[1024] = { 0, };
		unsigned long size_of_disasm = 0;
		unsigned long long next_address = 0;
		engine_->disasm(address_detail_map_b->first, d, sizeof(d), &size_of_disasm, &next_address);

		std::multimap<unsigned long long, unsigned long long>::iterator fi = ref_map.find(address_detail_map_b->first);
		if (fi != ref_map.end() && print.size())
		{
			node_info_tmp ni;
			ni.address = address_detail_map_b->first;
			ni.jmp_dest = 0;
			ni.next_address = next_address;
			ni.node = new code_graph::node;
			ni.node->setting(&G, &GA, print);
			ni.jmp = false;

			node_info_list.push_back(ni);

#ifdef USE_CMD_LOG_VIEWER
			printf("1. node : %I64x %I64x %I64x\n", ni.address, ni.next_address, ni.jmp_dest);
#endif
			print = "";
		}

		print += d;

		if ((address_detail_map_b->second->is_jmp_code && address_detail_map_b->second->instruction_id != X86_INS_JMP) && print.size())
		{
			node_info_tmp ni;

			ni.address = address_detail_map_b->first;
			ni.jmp_dest = address_detail_map_b->second->operands[0].value;
			ni.next_address = next_address;
			ni.node = new code_graph::node;
			ni.node->setting(&G, &GA, print);
			ni.jmp = false;

			node_info_list.push_back(ni);

#ifdef USE_CMD_LOG_VIEWER
			printf("2. node : %I64x %I64x %I64x %d\n", ni.address, ni.next_address, ni.jmp_dest, address_detail_map_b->second->instruction_id);
#endif
			print = "";
		}
		else if (address_detail_map_b->second->instruction_id == X86_INS_JMP && print.size())
		{
			node_info_tmp ni;
			ni.address = address_detail_map_b->first;
			ni.jmp_dest = address_detail_map_b->second->operands[0].value;
			ni.next_address = 0;
			ni.node = new code_graph::node;
			ni.node->setting(&G, &GA, print);
			ni.jmp = true;

			node_info_list.push_back(ni);
#ifdef USE_CMD_LOG_VIEWER
			printf("3. node : %I64x %I64x %I64x\n", ni.address, ni.next_address, ni.jmp_dest);
#endif
			print = "";
		}
	}

	if (print.size())
	{
		node_info_tmp ni;
		ni.address = 0;
		ni.jmp_dest = 0;
		ni.next_address = 0;
		ni.node = new code_graph::node;
		ni.node->setting(&G, &GA, print);

		node_info_list.push_back(ni);
	}

	std::list<node_info_tmp>::iterator node_it = node_info_list.begin();
	for (node_it = node_info_list.begin(); node_it != node_info_list.end(); ++node_it)
	{
#ifdef USE_CMD_LOG_VIEWER
		printf("node info:: %08x %08x %08x\n", (unsigned long)node_it->address, (unsigned long)node_it->jmp_dest, (unsigned long)node_it->next_address);
#endif
		node_info_tmp src = *node_it;
		node_info_tmp dest = find_node_info(node_info_list, node_it->jmp_dest);
		node_info_tmp next = find_node_info(node_info_list, node_it->next_address);

		if (node_it->jmp_dest)
		{
#ifdef USE_CMD_LOG_VIEWER
			printf("%I64x-%I64x %I64x\n", src.address, next.address, dest.address);
#endif
			if (dest.node)
			{
				if (!src.node->get_left())
				{
					src.node->add_left(dest.node);
				}
			}

			if (next.node && !node_it->jmp)
			{
				if (!src.node->get_right())
				{
					src.node->add_right(next.node);
				}

				if (node_it->jmp_dest > node_it->next_address)
				{
					std::list<node_info_tmp>::iterator jmp_it = node_it;
					++jmp_it, ++jmp_it;
					if (jmp_it != node_info_list.end() && jmp_it->address == dest.address)
					{
						if (!next.node->get_right())
						{
							next.node->add_right(dest.node);
						}
					}
				}
			}
		}
		else if(node_it->next_address)
		{
			if (next.node)
			{
				if (!src.node->get_right())
				{
					src.node->add_right(next.node);
				}
			}
		}
	}

	code_graph::node *root = (node_info_list.begin())->node;
	code_graph::tree t;
	t.set_root(root);

	graph_view_->draw(&G, &GA, &t);

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

