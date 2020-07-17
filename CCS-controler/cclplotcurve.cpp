#include "cclplotcurve.h"

cclPlotCurve::cclPlotCurve(QWidget *parent)
    : QOpenGLWidget(parent)
{
    pulleyAngle = 0.1;
    vehiclePosition = 0.2;
    controllerOutput = 0.1;

    cPlot = new QCustomPlot(this);

    cPlot->setMinimumHeight(400);
    cPlot->setMinimumWidth(800);

    cPlot->yAxis->setTickLabels(false);
    connect(cPlot->yAxis2, SIGNAL(rangeChanged(QCPRange)), cPlot->yAxis, SLOT(setRange(QCPRange)));

    cPlot->yAxis2->setVisible(true);
    cPlot->axisRect()->addAxis(QCPAxis::atRight);
    cPlot->axisRect()->axis(QCPAxis::atRight, 0)->setPadding(30);
    cPlot->axisRect()->axis(QCPAxis::atRight, 1)->setPadding(30);

    angleGraph = cPlot->addGraph(cPlot->xAxis, cPlot->axisRect()->axis(QCPAxis::atRight, 1));
    angleGraph->setPen(QPen(QColor(250, 0, 0)));
    positionGraph = cPlot->addGraph(cPlot->xAxis, cPlot->axisRect()->axis(QCPAxis::atRight, 0));
    positionGraph->setPen(QPen(QColor(0, 250, 0)));
    controlGraph = cPlot->addGraph(cPlot->xAxis, cPlot->axisRect()->axis(QCPAxis::atRight, 1));
    controlGraph->setPen(QPen(QColor(0, 0, 250)));

    angleTag = new AxisTag(angleGraph->valueAxis());
    angleTag->setPen(angleGraph->pen());
    positionTag = new AxisTag(positionGraph->valueAxis());
    positionTag->setPen(positionGraph->pen());
    controlTag = new AxisTag(angleGraph->valueAxis());
    controlTag->setPen(controlGraph->pen());

}

void cclPlotCurve::setPulleyAngle(float _pulleyAngle)
{
    pulleyAngle = _pulleyAngle;
}

void cclPlotCurve::setVehiclePosition(float _vehiclePosition)
{
    vehiclePosition = _vehiclePosition;
}

void cclPlotCurve::setControllerOutPut(float _controllerOutput)
{
    controllerOutput = _controllerOutput;
}

void cclPlotCurve::animate()
{
    update();
}

void cclPlotCurve::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    angleGraph->addData(angleGraph->dataCount(), pulleyAngle);
    positionGraph->addData(positionGraph->dataCount(), vehiclePosition);
    controlGraph->addData(controlGraph->dataCount(), controllerOutput);

    cPlot->xAxis->rescale();
    angleGraph->rescaleValueAxis(false, true);
    positionGraph->rescaleValueAxis(false, true);
    controlGraph->rescaleValueAxis(false, true);
    cPlot->xAxis->setRange(cPlot->xAxis->range().upper, 100, Qt::AlignRight);

    float oldpulleyAngle = angleGraph->dataMainValue(angleGraph->dataCount()-1);
    float oldvehiclePosition = positionGraph->dataMainValue(positionGraph->dataCount()-1);
    float oldcontrollerOutput = controlGraph->dataMainValue(controlGraph->dataCount()-1);

    angleTag->updatePosition(oldpulleyAngle);
    positionTag->updatePosition(oldvehiclePosition);
    controlTag->updatePosition(oldcontrollerOutput);

    angleTag->setText(QString::number(oldpulleyAngle, 'f', 4));
    positionTag->setText(QString::number(oldvehiclePosition, 'f', 4));
    controlTag->setText(QString::number(oldcontrollerOutput, 'f', 4));

    cPlot->replot();

}
