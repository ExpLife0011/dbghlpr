#include "graph_view.h"

#include <ogdf/fileformats/GraphIO.h>
#include <ogdf/layered/SugiyamaLayout.h>
#include <ogdf/layered/OptimalRanking.h>
#include <ogdf/layered/MedianHeuristic.h>
#include <ogdf/layered/OptimalHierarchyLayout.h>

GraphView::GraphView(QWidget *parent) : QGraphicsView(parent)
{
	scene_ = new QGraphicsScene(this);
	scene_->setBackgroundBrush(QBrush(Qt::darkGray));

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
	printf("11111111\n");

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
	OHL->nodeDistance(170);
	OHL->layerDistance(170);
	OHL->weightBalancing(100);
	OHL->weightSegments(0);

	ogdf::SugiyamaLayout SL;
	SL.setRanking(new ogdf::OptimalRanking);
	SL.setCrossMin(new ogdf::MedianHeuristic);
	SL.alignSiblings(false);
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
			code_graph::node::addGraphNodeWidget(x, y, n, scene_);
		}
	}

	printf("xxxxxxxxxxxxxxxxxx\n");

	ogdf::edge e;
	forall_edges(e, *G)
	{
		printf("1\n");

		const auto bends = GA->bends(e);
		const auto source = e->source();
		const auto target = e->target();

		printf("2\n");

		code_graph::node *src = t->find(source);
		code_graph::node *dest = t->find(target);

		if (!src || !dest)
			continue;

		printf("3\n");

		QRectF sourceRect = src->geometry();

		printf("3.1\n");

		sourceRect.adjust(-4, -4, 4, 4);

		printf("3.2\n");

		QRectF targetRect = dest->geometry();

		printf("3.3\n");

		targetRect.adjust(-4, -4, 4, 4);

		printf("5\n");

		QPointF start(GA->x(source), GA->y(source));
		QPointF end(GA->x(target), GA->y(target));

		printf("6\n");

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

		printf("7\n\n");
	}
}