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
        painter.setRenderHints(QPainter::Antialiasing);

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

            case MPButtonTranslucent:
                mbackground = (active ? QColor(50, 125, 255) : QColor(240, 240, 240, 100));
                mforeground = (active ? QColor(255, 255, 255) : QColor(50, 50, 50));
            break;
        }

        if (underMouse() && !isDown())
            mbackground = mbackground.lighter(105);

        // draw button frame
        painter.setPen(mbackground.darker(25));
        painter.setBrush(QBrush(mbackground));
        painter.drawRect(QRect(0, 0, width(), height()));

        if (icon().isNull())
        {
            // draw button text
            painter.setPen(mforeground);
            painter.drawText(QRect(0, 0, width(), height()), Qt::AlignCenter, text());
        }
        else
        {
            QIcon sicon = this->icon();
            QPixmap icon_pix = sicon.pixmap(width(), height(),
                                            isDown() ? QIcon::Selected : QIcon::Normal);

            // draw button icon
            painter.drawPixmap(0, 0, width(), height(), icon_pix);
        }

        painter.end();
    }
}

MPButton::~MPButton()
{
}
