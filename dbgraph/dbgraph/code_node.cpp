#include "code_graph.h"
#include <strsafe.h>
//#define USE_PLAIN_TEXT_WIDGET

int g_shadow_width;

code_graph::node::node(QWidget *parent) : QWidget(parent), left_(nullptr), right_(nullptr)
{
	font_ = QFont("Consolas", 10);
	g_shadow_width = 10;

	setShadowWidth(g_shadow_width);
	setWindowFlags(Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground, true);
	setAttribute(Qt::WA_NoSystemBackground, true);
	setStyleSheet("background:#FFFFFF;");
	setAutoFillBackground(false);
}

code_graph::node::~node()
{
}

void code_graph::node::paintEvent(QPaintEvent* event)
{
	Q_UNUSED(event);

#ifndef USE_PLAIN_TEXT_WIDGET
	QPainter painter(this);
	QPen red_pen(Qt::red);

	painter.save();

	QRectF rect = boundingRect();

	//
	// draw shadow
	//
	if (!_cashe.isNull())
		painter.drawPixmap(QPoint(), _cashe);

	painter.setBrush(palette().background());
	painter.setPen(Qt::NoPen);
	painter.setRenderHint(QPainter::Antialiasing, true);
	auto m = QMargins(shadowWidth(), shadowWidth(), shadowWidth(), shadowWidth());
	painter.drawRoundedRect(rect.marginsRemoved(m), 2.0, 2.0);
	painter.setRenderHint(QPainter::Antialiasing, false);

	//
	// draw text
	//
	painter.setPen(Qt::black);
	painter.setFont(font_);

	QRect textRect = QRect(g_shadow_width, g_shadow_width, rect.width() - g_shadow_width, rect.height() - g_shadow_width);
	painter.drawText(textRect, data_);

	QPen pen(Qt::black);
	pen.setWidth(5);

	painter.setPen(pen);
	//painter.drawRect(rect);

	painter.restore();
#endif

}

QRectF code_graph::node::boundingRect() const
{
	return QRectF(0, 0, width_, height_);
}

void code_graph::node::syntax_highlight()
{
	QList<QTextEdit::ExtraSelection> extraSelections;
	QTextEdit::ExtraSelection selection;

	//QColor lineColor = QColor("#509CE4").light(160);

	//selection.format.setBackground(lineColor);
	selection.format.setProperty(QTextFormat::FullWidthSelection, true);
	selection.cursor = plain_text_edit_->textCursor();
	selection.cursor.clearSelection();
	extraSelections.append(selection);
	plain_text_edit_->setExtraSelections(extraSelections);
}

//
//
//
void code_graph::node::add_left(code_graph::node *left)
{
	ogdf::edge edge = graph_->newEdge(this->node_, left->get_ogdf_node());
	graph_attribute_->arrowType(edge) = ogdf::EdgeArrow::eaLast;

	left_ = left;
	left_->set_edge_color(Qt::red);
}

void code_graph::node::add_right(code_graph::node *right)
{
	ogdf::edge edge = graph_->newEdge(this->node_, right->get_ogdf_node());
	graph_attribute_->arrowType(edge) = ogdf::EdgeArrow::eaLast;

	right_ = right;
	right_->set_edge_color(Qt::green);
}

ogdf::node code_graph::node::get_ogdf_node()
{
	return node_;
}

code_graph::node * code_graph::node::get_left()
{
	return left_;
}

code_graph::node * code_graph::node::get_right()
{
	return right_;
}

QString code_graph::node::get_data()
{
	return data_;
}

QColor code_graph::node::get_edge_color()
{
	return edge_color_;
}

//
//
//
void code_graph::node::setting(ogdf::Graph *graph, ogdf::GraphAttributes *graph_attribute, QString data)
{
	//
	// init
	//
	left_ = nullptr;
	right_ = nullptr;

	graph_ = graph;
	graph_attribute_ = graph_attribute;
	data_ = data.toStdString().c_str();
	edge_color_ = Qt::black;

	print_string_ = (char *)malloc(strlen(data.toStdString().c_str()));
	if (print_string_)
	{
		StringCbCopyA(print_string_, strlen(data.toStdString().c_str()), data.toStdString().c_str());
	}

	//
	// set ogdf node
	//
	node_ = graph_->newNode();

	//
	//
	//
	set_node_size();

	//
	//
	//
#ifdef USE_PLAIN_TEXT_WIDGET
	plain_text_edit_ = new QPlainTextEdit(this);
	plain_text_edit_->setStyleSheet(
		"QPlainTextEdit { selection-background-color: transparent;}"
		"QPlainTextEdit { selection-color: red; }"
	);

	plain_text_edit_->setReadOnly(true);
	plain_text_edit_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	plain_text_edit_->setMinimumSize(width_, height_ + 15);
	plain_text_edit_->setFont(font_);

	plain_text_edit_->insertPlainText(data_);
	plain_text_edit_->moveCursor(QTextCursor::Start);

	plain_text_edit_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	plain_text_edit_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	QObject::connect(plain_text_edit_, SIGNAL(cursorPositionChanged()), this, SLOT(syntax_highlight()));
#endif
}

void code_graph::node::set_node_size()
{
	unsigned int w = 0;
	unsigned int lf = 0;
	unsigned int l = 0;
	char *str = print_string_;//(char *)data_.toStdString().c_str();

	for (int i = 0; i < strlen(str); ++i)
	{
		char *f = strstr(&str[i], "\n");
		if (f)
		{
			if (w <= f - &str[i])
			{
				w = f - &str[i];
			}

			i = f - str;
			lf = i;
			++l;
		}
	}
	++l;
	//printf("l=%d %d\n", l, data_.size());

	if (w <= strlen(&str[lf + 1]))
	{
		w = strlen(&str[lf + 1]);
	}

	QFontMetrics metrics(this->font_);

	//
	// set node width and height
	//
#if 0 // display size 200%
	width_ = (w * 15) + g_shadow_width * 2;
	height_ = metrics.height() + (g_shadow_width/3) * ((font_.pointSize()) * l);
#else // display size 100%
	width_ = ((w * 15) + g_shadow_width * 2) / 2;
	height_ = metrics.height()*l + g_shadow_width + (font_.pointSize() * 2);
#endif
}

void code_graph::node::set_edge_color(QColor edge_color)
{
	edge_color_ = edge_color;
}

void code_graph::node::addGraphNodeWidget(qreal x, qreal y, code_graph::node *n, QGraphicsScene *scene)
{
	QRectF rect = n->boundingRect();

	QGraphicsRectItem *rect_item = scene->addRect(x, y, n->width(), 20, QPen(Qt::black), QBrush(Qt::darkCyan)); // allow ver
	//QGraphicsRectItem *rect_item = scene->addRect(x, y, n->width(), 20, QPen(Qt::black), QBrush(n->get_edge_color()));

	rect_item->setFlag(QGraphicsItem::ItemIsSelectable, true);

	QGraphicsProxyWidget *const proxy = scene->addWidget(n);
	proxy->setPos(x, y + rect_item->rect().height());
	proxy->setParentItem(rect_item);
}

