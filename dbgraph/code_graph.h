#ifndef __DEFINE_GRAPH_NODE__
#define __DEFINE_GRAPH_NODE__

#include <qwidget.h>
#include <qlayout.h>
#include <qplaintextedit.h>
#include <qpainter.h>
#include <qgraphicseffect.h>
#include <qgraphicsscene.h>
#include <qgraphicsproxywidget.h>
#include <qscrollbar.h>

#include <ogdf/fileformats/GraphIO.h>
#include <shadow.h>

#pragma comment(lib, "ogdf.lib")
#pragma comment(lib, "coin.lib")

namespace code_graph
{
	//
	// graph node
	//
	class node : public QWidget
	{
		Q_OBJECT

		//
		// ogdf control
		//
		ogdf::Graph *graph_;
		ogdf::GraphAttributes *graph_attribute_;
		ogdf::node node_;
		
		//
		// node info
		//
		code_graph::node *left_;
		code_graph::node *right_;
		QString data_;
		QFont font_;

		unsigned long width_;
		unsigned long height_;

		QVBoxLayout *layout_;
		QPlainTextEdit *plain_text_edit_;
		
		//
		// edge color
		//
		QColor edge_color_;
		int shadow_width_;

	public:
		explicit code_graph::node(QWidget *parent = 0);
		~node();

		void paintEvent(QPaintEvent* event);
		QRectF boundingRect() const;

		//
		// set shadow effect
 		//
		void setShadowWidth(int w) 
		{
			shadow_width_ = w;
		}

		int shadowWidth() const 
		{
			return shadow_width_;
		}

		void ensureLoaded() 
		{
			if (_cashe.isNull()) 
			{
				_cashe = QPixmap::fromImage(Shadow::paint(this, rect(), 18.0, shadowWidth(), QColor(0, 0, 0, 128)));
			}
		}

		void resizeEvent(QResizeEvent *e) 
		{
			ensureLoaded();
			_cashe = _cashe.scaled(e->size());
			QWidget::resizeEvent(e);
		}

		QPixmap _cashe = QPixmap();

	public:
		void add_left(code_graph::node *left);
		void add_right(code_graph::node *right);

		ogdf::node get_ogdf_node();
		code_graph::node *get_left();
		code_graph::node *get_right();
		QString get_data();
		QColor get_edge_color();

	public:
		void setting(ogdf::Graph *graph, ogdf::GraphAttributes *graph_attribute, QString data);
		void set_node_size();
		void set_edge_color(QColor edge_color);

	public:
		static void addGraphNodeWidget(qreal x, qreal y, code_graph::node *n, QGraphicsScene* scene);


	public slots:
		void syntax_highlight();
	};

	//
	// graph edge
	//
	class edge : public QAbstractGraphicsShapeItem
	{
	private:
		QPainterPath line_;
		QPainterPath arrow_;

		QList<QPointF> line_points_;
		QList<QPointF> arrow_points_;

		QColor edge_color_;

	public:
		edge(QPointF start, QPointF end, ogdf::DPolyline bends, QRectF src, QRectF dest, QColor edge_color);

		void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
		QRectF boundingRect() const;

	private:
		qreal calc_distance(QPointF p1, QPointF p2);
		QPointF calc_nearest_intersect(QRectF rect, QPointF p1, QPointF p2);
		QList<QPointF> calc_line(QPointF start, QPointF end, ogdf::DPolyline bends, QRectF sourceRect, QRectF targetRect);
		QList<QPointF> calc_arrow(const QList<QPointF> & linePoints);
		void prepare_painter_paths();
	};

	//
	// graph tree
	//
	class tree
	{
	private:
		std::list<code_graph::node *> node_list_;
		std::map<ogdf::node, code_graph::node *> odgf_map_;

	public:
		void set_root(code_graph::node *n);
		code_graph::node *find(ogdf::node n);
		code_graph::node *find(QString str);
	};
}

class test_node : public QWidget
{
	Q_OBJECT

public:
	explicit test_node(QWidget *parent = 0);
	~test_node();

	static void add(QPoint pos, QGraphicsScene* scene);
};

#endif