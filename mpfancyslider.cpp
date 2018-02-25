#include "mpfancyslider.h"
#include <QDebug>

MPFancySlider::MPFancySlider(QWidget *parent)
    : QSlider(parent)
{
    _dragging = false;

    // set slider type
    slider_type = MPFancyNormalSlider;

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
        double position = minimum() + ((double)event->x()/width()) * (maximum()-minimum());
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
        double position = minimum() + ((double)event->x()/width()) * (maximum()-minimum());
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

        QColor fill_color = QColor(50, 125, 255);
        double position = (((double)value()-minimum())/
                           ((double)maximum()-minimum())) * width();

        if (slider_type == MPFancyNormalSlider)
        {
            // draw track
            painter.setPen(QColor(150, 150, 150));
            painter.setBrush(QBrush(QColor(150, 150, 150)));
            painter.drawRect(QRect(0, (height()/2)-1, width(), 3));

            // draw fill
            painter.setPen(fill_color.darker(110));
            painter.setBrush(QBrush(fill_color));
            painter.drawRect(QRect(0, (height()/2)-1, fabs(position), 3));

            // draw handle
            painter.drawEllipse(fabs(position)-8, 3, height()-6, height()-6);
        }
        else if (slider_type == MPFancySeekSlider)
        {
            // draw track
            painter.setBrush(QBrush(QColor(200, 200, 200)));
            painter.drawRect(QRect(0, 0, width(), height()));

            // draw fill
            if (value() != 0 && maximum() != 0)
            {
                painter.setPen(fill_color.darker(105));
                painter.setBrush(QBrush(fill_color));
                painter.drawRect(QRect(0, 0, fabs(position), height()));

                // draw handle
                if (_dragging)
                {
                    painter.setBrush(QBrush(fill_color.lighter(130)));
                    painter.drawEllipse(fabs(position)-8, 0, height(), height());
                }
            }
        }

        painter.end();
    }
}
