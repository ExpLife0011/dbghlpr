#ifndef __DEFINE_DBGSUIT_CODE_GRAPH_VIEWER__
#define __DEFINE_DBGSUIT_CODE_GRAPH_VIEWER__

#include <qwidget>
#include <qgraphicsview.h>
#include <qgraphicsscene.h>
#include "code_graph.h"

class GraphView : public QGraphicsView
{
	Q_OBJECT

private:
	code_graph::tree *t_;
	QGraphicsScene *scene_;

public:
	GraphView(QWidget *parent = 0);
	void draw(ogdf::Graph *G, ogdf::GraphAttributes *GA, code_graph::tree *t);

protected:
	//void timerEvent(QTimerEvent *event) override;
	void wheelEvent(QWheelEvent *event) override;
	void scaleView(qreal factor);
};

#endif