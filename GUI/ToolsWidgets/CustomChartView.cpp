//
// Created by oleg on 29.12.17.
//

#include "CustomChartView.h"

#include <QToolTip>
#include <QTextStream>

using namespace QtCharts;

CustomChartView::CustomChartView(QWidget* parent)
        : QChartView(parent)
{
    setup();
}

CustomChartView::CustomChartView(QChart* chart, QWidget* parent)
        : QChartView(chart, parent)
{
    setup();
}

void CustomChartView::mouseMoveEvent(QMouseEvent* event)
{
    QString tooltip;
    QTextStream stream(&tooltip);
    stream.setRealNumberPrecision(3);
    auto coords = graph_coords(event->pos());
    stream << "(" << coords.x() << ", " << coords.y() << ")";
    QToolTip::showText(event->globalPos(), *stream.string(),
                       this, this->rect(), 10000);
    QChartView::mouseMoveEvent(event);
}

void CustomChartView::setup()
{
    setMouseTracking(true);
}

QPointF CustomChartView::graph_coords(const QPoint& widget_coords) const
{
    return chart()->mapToValue(
            chart()->mapFromScene(
                    mapToScene(widget_coords)));
}
