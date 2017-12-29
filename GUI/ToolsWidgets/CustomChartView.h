//
// Created by oleg on 29.12.17.
//

#ifndef EVOLUTION_CUSTOMCHARTVIEW_H
#define EVOLUTION_CUSTOMCHARTVIEW_H

#include <QChartView>

class CustomChartView : public QtCharts::QChartView
{
public:
    explicit CustomChartView(QWidget* parent = nullptr);
    explicit CustomChartView(QtCharts::QChart* chart, QWidget* parent = nullptr);

protected:
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private:
    void setup();
    QPointF graph_coords(const QPoint& widget_coords) const;
};


#endif //EVOLUTION_CUSTOMCHARTVIEW_H
