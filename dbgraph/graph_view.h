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
	QCursor current_curosr_;
	QPoint point_;
	bool is_move_;

public:
	GraphView(QWidget *parent = 0);
	void draw(ogdf::Graph *G, ogdf::GraphAttributes *GA, code_graph::tree *t);

protected:
	virtual void wheelEvent(QWheelEvent *event) override;
	virtual void mousePressEvent(QMouseEvent *event) override;
	virtual void mouseMoveEvent(QMouseEvent *event) override;
	virtual void mouseReleaseEvent(QMouseEvent *event) override;
	void scaleView(qreal factor);
};

#endif