#ifndef CCLPLOTCURVE_H
#define CCLPLOTCURVE_H

#include "qcustomplot.h"
#include "axistag.h"

#include <QOpenGLWidget>

class cclPlotCurve : public QOpenGLWidget
{
    Q_OBJECT

public:
    cclPlotCurve(QWidget *parent);
    void setPulleyAngle(float _pulleyAngle);
    void setVehiclePosition(float _vehiclePosition);
    void setControllerOutPut(float _controllerOutput);
    void animate();

protected:
    void paintEvent(QPaintEvent *event);

private:
    QCustomPlot *cPlot;
    QPointer<QCPGraph> angleGraph;
    QPointer<QCPGraph> positionGraph;
    QPointer<QCPGraph> controlGraph;
    AxisTag *angleTag;
    AxisTag *positionTag;
    AxisTag *controlTag;
    float pulleyAngle;
    float vehiclePosition;
    float controllerOutput;
};

#endif // CCLPLOTCURVE_H
