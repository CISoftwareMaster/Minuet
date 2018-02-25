#include "mpbutton.h"
#include <QDebug>

MPButton::MPButton(QWidget *parent)
    : QPushButton(parent)
{
    btn_type = MPButtonNormal;
}

void MPButton::paintEvent(QPaintEvent *)
{
    QPainter painter;

    if (painter.begin(this))
    {
        bool active = (isDown() || isChecked());
        QColor mbackground;
        QColor mforeground;

        switch (btn_type)
        {
            case MPButtonNormal:
                mbackground = (active ? QColor(50, 125, 255) : QColor(240, 240, 240));
                mforeground = (active ? QColor(255, 255, 255) : QColor(50, 50, 50));
            break;

            case MPButtonDefault:
                mbackground = (active ? QColor(100, 150, 255) : QColor(50, 125, 255));
                mforeground = QColor(255, 255, 255);
            break;
        }

        if (underMouse() && !isDown())
            mbackground = mbackground.lighter(105);

        // draw button frame
        painter.setPen(mforeground.darker(25));
        painter.setBrush(QBrush(mbackground));
        painter.drawRect(QRect(0, 0, width(), height()));

        // draw button text
        painter.setPen(mforeground);
        painter.drawText(QRect(0, 0, width(), height()), Qt::AlignCenter, text());

        painter.end();
    }
}

MPButton::~MPButton()
{
}
