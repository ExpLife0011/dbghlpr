#include "code_graph.h"

code_graph::edge::edge(QPointF start, QPointF end, ogdf::DPolyline bends, QRectF sourceRect, QRectF targetRect, QColor edge_color) : QAbstractGraphicsShapeItem()
{
	edge_color_ = edge_color;
	line_points_ = calc_line(start, end, bends, sourceRect, targetRect);
	arrow_points_ = calc_arrow(line_points_);

	prepare_painter_paths();
}

void code_graph::edge::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);

	//save painter
	painter->save();

	//set painter options
	painter->setRenderHints(QPainter::Antialiasing | QPainter::HighQualityAntialiasing);

	int lineSize = 3;

	//draw line
	painter->setPen(QPen(edge_color_, lineSize));
	painter->drawPath(line_);

	//draw arrow
	painter->setPen(QPen(edge_color_, lineSize));
	painter->drawPath(arrow_);

	//restore painter
	painter->restore();
}

QRectF code_graph::edge::boundingRect() const
{
	QList<QPointF> allPoints;
	allPoints << line_points_ << arrow_points_;

	QPointF topLeft = allPoints[0];
	QPointF bottomRight = topLeft;
	for (auto p : allPoints)
	{
		qreal x = p.x();
		qreal y = p.y();

		if (x < topLeft.x())
			topLeft.setX(x);
		if (y < topLeft.y())
			topLeft.setY(y);

		if (x > bottomRight.x())
			bottomRight.setX(x);
		if (y > bottomRight.y())
			bottomRight.setY(y);
	}

	return QRectF(topLeft, bottomRight);
}

//
//
//
qreal code_graph::edge::calc_distance(QPointF p1, QPointF p2)
{
	QPointF d = p2 - p1;
	return sqrt(d.x() * d.x() + d.y() * d.y());
}

QPointF code_graph::edge::calc_nearest_intersect(QRectF rect, QPointF p1, QPointF p2)
{
	qreal div = p1.x() - p2.x();
	if (div == 0)
	{
		//printf("a1\n");
		QPointF i1(p1.x(), rect.top());
		QPointF i2(p1.x(), rect.bottom());

		if (p2.y() < p1.y())
			return i1;
		else
			return i2;
	}
	else
	{
		//printf("a2\n");

		QPointF result;
		qreal bestDist = 10e99;

		qreal a = (p1.y() - p2.y()) / div;
		qreal b = p1.y() - a * p1.x();

		QPointF i1((b - rect.top()) / -a, rect.top());
		if (rect.contains(i1))
		{
			qreal dist = calc_distance(p2, i1);
			if (dist < bestDist)
			{
				bestDist = dist;
				result = i1;
			}
		}

		QPointF i2((b - rect.bottom()) / -a, rect.bottom());
		if (rect.contains(i2))
		{
			qreal dist = calc_distance(p2, i2);
			if (dist < bestDist)
			{
				bestDist = dist;
				result = i2;
			}
		}

		QPointF i3(rect.left(), a * rect.left() + b);
		if (rect.contains(i3))
		{
			qreal dist = calc_distance(p2, i3);
			if (dist < bestDist)
			{
				bestDist = dist;
				result = i3;
			}
		}

		QPointF i4(rect.right(), a * rect.right() + b);
		if (rect.contains(i4))
		{
			qreal dist = calc_distance(p2, i4);
			if (dist < bestDist)
			{
				bestDist = dist;
				result = i4;
			}
		}
		return result;
	}
}

QList<QPointF> code_graph::edge::calc_line(QPointF start, QPointF end, ogdf::DPolyline bends, QRectF sourceRect, QRectF targetRect)
{
	QList<QPointF> linePoints;
	linePoints << start;
	for (auto p : bends)
		linePoints << QPointF(p.m_x, p.m_y);
	linePoints << end;

	QPointF nearestI = calc_nearest_intersect(sourceRect, linePoints[0], linePoints[1]);
	linePoints[0] = nearestI;

	int len = linePoints.length();
	nearestI = calc_nearest_intersect(targetRect, linePoints[len - 1], linePoints[len - 2]);
	linePoints[len - 1] = nearestI;

	return linePoints;
}

QList<QPointF> code_graph::edge::calc_arrow(const QList<QPointF> & linePoints)
{
	//arrow
	int len = linePoints.length();
	QLineF perpLine = QLineF(linePoints[len - 1], linePoints[len - 2]).normalVector();

	qreal arrowLen = 6;

	QLineF a;
	a.setP1(linePoints[len - 1]);
	a.setAngle(perpLine.angle() - 45);
	a.setLength(arrowLen);

	QLineF b;
	b.setP1(linePoints[len - 1]);
	b.setAngle(perpLine.angle() - 135);
	b.setLength(arrowLen);

	QLineF c;
	c.setP1(a.p2());
	c.setP2(b.p2());

	QList<QPointF> arrowPoints;
	arrowPoints << a.p1() << a.p2() << b.p1() << b.p2() << c.p1() << c.p2();
	return arrowPoints;
}

void code_graph::edge::prepare_painter_paths()
{
	//edge line
	QPolygonF polyLine;
	for (auto p : line_points_)
		polyLine << p;
	line_.addPolygon(polyLine);

	//arrow
	QPolygonF polyArrow;
	for (auto p : arrow_points_)
		polyArrow << p;
	arrow_.addPolygon(polyArrow);
}
