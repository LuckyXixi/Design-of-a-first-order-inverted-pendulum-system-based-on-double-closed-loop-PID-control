#include "axistag.h"

AxisTag::AxisTag(QCPAxis *parentAxis) :
  QObject(parentAxis),
  mAxis(parentAxis)
{
  mDummyTracer = new QCPItemTracer(mAxis->parentPlot());
  mDummyTracer->setVisible(false);
  mDummyTracer->position->setTypeX(QCPItemPosition::ptAxisRectRatio);
  mDummyTracer->position->setTypeY(QCPItemPosition::ptPlotCoords);
  mDummyTracer->position->setAxisRect(mAxis->axisRect());
  mDummyTracer->position->setAxes(0, mAxis);
  mDummyTracer->position->setCoords(1, 0);

  mArrow = new QCPItemLine(mAxis->parentPlot());
  mArrow->setLayer("overlay");
  mArrow->setClipToAxisRect(false);
  mArrow->setHead(QCPLineEnding::esSpikeArrow);
  mArrow->end->setParentAnchor(mDummyTracer->position);
  mArrow->start->setParentAnchor(mArrow->end);
  mArrow->start->setCoords(15, 0);

  mLabel = new QCPItemText(mAxis->parentPlot());
  mLabel->setLayer("overlay");
  mLabel->setClipToAxisRect(false);
  mLabel->setPadding(QMargins(3, 0, 3, 0));
  mLabel->setBrush(QBrush(Qt::white));
  mLabel->setPen(QPen(Qt::blue));
  mLabel->setPositionAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  mLabel->position->setParentAnchor(mArrow->start);
}

AxisTag::~AxisTag()
{
  if (mDummyTracer)
    mDummyTracer->parentPlot()->removeItem(mDummyTracer);
  if (mArrow)
    mArrow->parentPlot()->removeItem(mArrow);
  if (mLabel)
    mLabel->parentPlot()->removeItem(mLabel);
}

void AxisTag::setPen(const QPen &pen)
{
  mArrow->setPen(pen);
  mLabel->setPen(pen);
}

void AxisTag::setBrush(const QBrush &brush)
{
  mLabel->setBrush(brush);
}

void AxisTag::setText(const QString &text)
{
  mLabel->setText(text);
}

void AxisTag::updatePosition(double value)
{
  mDummyTracer->position->setCoords(1, value);

  mArrow->end->setCoords(mAxis->offset(), 0);
}
