#include "dbgraph.h"

dbgraph::dbgraph(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	ui.setupUi(this);

	QFile f("css\\style.qss");
	if (f.exists())
	{
		f.open(QFile::ReadOnly | QFile::Text);
		QTextStream ts(&f);
		qApp->setStyleSheet(ts.readAll());
	}

	disassembly_text_browser_ = new QPlainTextEdit(ui.mnemonic_tab);
	disassembly_text_browser_->setStyleSheet(
		"QPlainTextEdit { selection-background-color: transparent;}"
		"QPlainTextEdit { selection-color: red; }"
	);
	//disassembly_text_browser_->setFont(QFont("Times", 10));
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
		write();
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

bool dbgraph::write()
{
	if (wcsstr(QCoreApplication::arguments().at(3).toStdWString().c_str(), L"-p"))
	{
		char *e = nullptr;
		unsigned long long ptr = strtoull(QCoreApplication::arguments().at(4).toStdString().c_str(), &e, 16);
		unsigned long long base = 0;
		unsigned long long end = 0;

		analyzer_wrapper::calc_exe_segment(ptr, &base, &end);
		if (base == 0 || end == 0)
		{
			return false;
		}

		if (analyzer_wrapper::check(base, end, ptr - 0x1000))
		{
			base = ptr - 0x1000;
		}

		ptr = analyzer_wrapper::find_entry(ptr, base, end - base);
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

		QString print;
		std::multiset<unsigned long long> dest;
		for (address_detail_map_b; address_detail_map_b != address_detail_map_e; ++address_detail_map_b)
		{
			unsigned long long address = address_detail_map_b->first;
			//std::set<unsigned long long>::iterator f_it = dest.find(address);
			if (dest.count(address) == 1)
			{
				print += "\n";
			}

			unsigned long offset = address - base;
			cs_insn *insn = (cs_insn *)engine_->disasm(address, &memory_dump[offset]);
			char d[1024];
			sprintf(d, "   0x%x		%s %s\n", (unsigned long)insn->address, insn->mnemonic, insn->op_str);

			print += d;

			if (address_detail_map_b->second->is_jmp_code)
			{
				dest.insert(address_detail_map_b->second->operands[0].value);
				print += "\n";
			}
		}

		disassembly_text_browser_->insertPlainText(print);
		disassembly_text_browser_->moveCursor(QTextCursor::Start);
	}

	return true;
}

//
//
//
void dbgraph::syntax_highlight()
{
	QList<QTextEdit::ExtraSelection> extraSelections;
	QTextEdit::ExtraSelection selection;

	QColor lineColor = QColor("#509CE4").light(160);

	selection.format.setBackground(lineColor);
	//selection.format.setForeground(Qt::white);
	selection.format.setProperty(QTextFormat::FullWidthSelection, true);
	selection.cursor = disassembly_text_browser_->textCursor();
	selection.cursor.clearSelection();
	extraSelections.append(selection);
	disassembly_text_browser_->setExtraSelections(extraSelections);

	syntax_highlighter_->setHighlightedString(disassembly_text_browser_->textCursor().selectedText());
}

