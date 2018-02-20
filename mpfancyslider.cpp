#include "mpfancyslider.h"
#include <QDebug>

MPFancySlider::MPFancySlider(QWidget *parent)
    : QSlider(parent)
{
    _dragging = false;
    half_height = height()/2;

    // enable mouse tracking
    setMouseTracking(true);
}

void MPFancySlider::mousePressEvent(QMouseEvent *event)
{
    // handle default mouse events
    QSlider::mousePressEvent(event);

    if (event->buttons() == Qt::LeftButton)
    {
        _dragging = true;

        // move value to the selected point
        double position = (maximum() * ((double)event->x() / (double)width()));
        setValue(fabs(position));

        // update user interface
        repaint();
    }
}

void MPFancySlider::mouseReleaseEvent(QMouseEvent *event)
{
    QSlider::mouseReleaseEvent(event);

    _dragging = false;
    repaint();
}

void MPFancySlider::mouseMoveEvent(QMouseEvent *event)
{
    QSlider::mouseMoveEvent(event);

    // set value boundaries
    if (event->x() < 0 || event->x() > width()) return;

    if (event->buttons() == Qt::LeftButton)
    {
        // update value
        double position = (maximum() * ((double)event->x() / (double)width()));
        setValue(fabs(position));

        repaint();
    }
}

void MPFancySlider::paintEvent(QPaintEvent *)
{
    // initialise painter
    QPainter painter;

    if (painter.begin(this))
    {
        // enable antialiased rendering
        painter.setRenderHint(QPainter::Antialiasing);

        // draw track
        painter.setBrush(QBrush(QColor(150, 150, 150)));
        painter.drawRect(QRect(0, 0, width(), height()));

        // draw fill
        if (value() != 0 && maximum() != 0)
        {
            double position = width() * ((double)value()/(double)maximum());
            painter.setBrush(QBrush(QColor(255, 255, 255)));
            painter.drawRect(QRect(0, 0, fabs(position), height()));

            // draw handle
            if (_dragging)
                painter.drawEllipse(fabs(position)-8, 0, half_height, half_height);
        }

        painter.end();
    }
}
