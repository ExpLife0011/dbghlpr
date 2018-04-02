#include "graph_view.h"

#include <ogdf/fileformats/GraphIO.h>
#include <ogdf/layered/SugiyamaLayout.h>
#include <ogdf/layered/OptimalRanking.h>
#include <ogdf/layered/MedianHeuristic.h>
#include <ogdf/layered/OptimalHierarchyLayout.h>

GraphView::GraphView(QWidget *parent) : QGraphicsView(parent)
{
	scene_ = new QGraphicsScene(this);
	scene_->setBackgroundBrush(QBrush(Qt::white));

	setScene(scene_);
}

void GraphView::wheelEvent(QWheelEvent *event)
{
	scaleView(pow((qreal)2, -event->delta() / 240.0));
}

#if 0
void GraphView::timerEvent(QTimerEvent *event)
{
	Q_UNUSED(event);

	QList<code_graph::node *> node_list;
	foreach (QGraphicsItem *item, scene()->items())
	{
		if (code_graph::node * n = qgraphicsitem_cast<code_graph::node *>(item))
		{
			node_list << n;
		}
	}

	foreach (code_graph::node * n, node_list)
	{
		
	}
}
#endif

void GraphView::scaleView(qreal factor)
{
	qreal f = transform().scale(factor, factor).mapRect(QRectF(0, 0, 1, 1)).width();
	if (f < 0.07 || f > 100)
		return;

	scale(factor, factor);
}

void GraphView::draw(ogdf::Graph *G, ogdf::GraphAttributes *GA, code_graph::tree *t)
{
	t_ = t;

	ogdf::node v;
	forall_nodes(v, *G)
	{
		auto n = t->find(v);
		if (n)
		{
			auto rect = n->boundingRect();
			GA->width(v) = rect.width();
			GA->height(v) = rect.height();
		}
	}

	ogdf::OptimalHierarchyLayout* OHL = new ogdf::OptimalHierarchyLayout;
	OHL->nodeDistance(10.0);
	OHL->layerDistance(10.0);
	OHL->weightBalancing(0.0);
	OHL->weightSegments(0.0);

	ogdf::SugiyamaLayout SL;
	SL.setRanking(new ogdf::OptimalRanking);
	SL.setCrossMin(new ogdf::MedianHeuristic);
	SL.alignSiblings(true);
	SL.setLayout(OHL);
	SL.call(*GA);

	forall_nodes(v, *G)
	{
		auto n = t->find(v);
		if (n)
		{
			auto rect = n->boundingRect();
			qreal x = GA->x(v) - (rect.width() / 2);
			qreal y = GA->y(v) - (rect.height() / 2);
			n->setGeometry(x, y, rect.width(), rect.height());
			//code_graph::node::addGraphNodeWidget(x, y, n, scene_);
			n->resize(rect.width()+1, rect.height()+1);
			//scene_->addWidget(n);
		}
	}

	ogdf::edge e;
	forall_edges(e, *G)
	{
		const auto bends = GA->bends(e);
		const auto source = e->source();
		const auto target = e->target();

		code_graph::node *src = t->find(source);
		code_graph::node *dest = t->find(target);

		if (!src || !dest)
		{
			continue;
		}

		QRectF sourceRect = src->geometry();
		sourceRect.adjust(10, 10, -10, -10);  // const 10 = shadow width in code node

		QRectF targetRect = dest->geometry();
		targetRect.adjust(10, 10, -10, -10);  // const 10 = shadow width in code node

		QPointF start(GA->x(source), GA->y(source));
		QPointF end(GA->x(target), GA->y(target));

		if (src->get_right() && src->get_left() == dest)
		{
			code_graph::edge* edge = new code_graph::edge(start, end, bends, sourceRect, targetRect, Qt::green);
			scene_->addItem(edge);
		}
		else if(src->get_left() && src->get_right() == dest)
		{
			code_graph::edge* edge = new code_graph::edge(start, end, bends, sourceRect, targetRect, Qt::red);
			scene_->addItem(edge);
		}
		else
		{
			code_graph::edge* edge = new code_graph::edge(start, end, bends, sourceRect, targetRect, Qt::blue);
			scene_->addItem(edge);
		}
	}

	forall_nodes(v, *G)
	{
		auto n = t->find(v);
		if (n)
		{
			auto rect = n->boundingRect();
			qreal x = GA->x(v) - (rect.width() / 2);
			qreal y = GA->y(v) - (rect.height() / 2);
			n->setGeometry(x, y, rect.width(), rect.height());
			//code_graph::node::addGraphNodeWidget(x, y, n, scene_);
			n->resize(rect.width() + 1, rect.height() + 1);
			scene_->addWidget(n);
		}
	}
}