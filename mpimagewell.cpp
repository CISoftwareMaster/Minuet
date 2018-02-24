#include "mpimagewell.h"
#include <QDebug>

MPImageWell::MPImageWell(QWidget *parent)
    : QWidget(parent)
{
    // allow file dropping
    setAcceptDrops(true);

    // set allowed file format
    image_formats = QImageReader::supportedImageFormats();

    // point to null by default
    this->mparent = NULL;

    // is this well being targetted?
    _drag_target = false;
}

void MPImageWell::dragEnterEvent(QDragEnterEvent *event)
{
    const QMimeData *data = event->mimeData();
    QFileInfo info(data->urls().first().toString());

    // check if the file being dragged is compatible
    if (image_formats.contains(info.suffix().toLocal8Bit()))
    {
        // allow file drop
        event->acceptProposedAction();

        // this is now being targetted
        _drag_target = true;
        repaint();
    }
}

void MPImageWell::dragLeaveEvent(QDragLeaveEvent *)
{
    // this is not being targetted anymore
    _drag_target = false;

    // update user interface
    repaint();
}

void MPImageWell::dropEvent(QDropEvent *event)
{
    // load the dropped image
    const QMimeData *data = event->mimeData();
    QString url = data->urls().first().toString().replace("file://", "");

    QImageReader reader(url);
    QImage image = reader.read();

    if (!image.isNull())
    {
        // update album art
        QPixmap new_pixmap = QPixmap::fromImage(image);
        this->set_image(new_pixmap);
    }

    // disable target indicator
    _drag_target = false;
}

void MPImageWell::paintEvent(QPaintEvent *)
{
    QPainter painter;

    if (painter.begin(this))
    {
        painter.setBrush(QBrush(QColor(240, 240, 240)));

        // draw an empty rectangle for its default background
        painter.drawRect(QRect(0, 0, width(), height()));

        // draw the album art
        painter.drawPixmap(QRect(0, 0, width(), height()), _image);

        // draw the "being targetted" indicator
        if (_drag_target)
        {
            painter.setBrush(QBrush(QColor(255, 255, 255, 200)));
            painter.drawRect(QRect(0, 0, width(), height()));

            // find centre
            if (mparent != NULL)
            {
                int x = mparent->horizontalScrollBar()->value();
                int y = mparent->verticalScrollBar()->value();
                int w = mparent->width();
                int h = mparent->height();

                painter.drawText(QRect(x, y, w, h),
                                 Qt::AlignCenter, "Update album cover art.");
            }
        }

        painter.end();
    }
}

void MPImageWell::set_mparent(QAbstractScrollArea *mparent)
{
    this->mparent = mparent;
}

QPixmap MPImageWell::image()
{
    return _image;
}

void MPImageWell::set_image(QPixmap pixmap)
{
    // update image
    _image = pixmap;

    // resize this widget
    setMinimumSize(pixmap.width(), pixmap.height());
    setMaximumSize(pixmap.width(), pixmap.height());

    // update user interface
    repaint();
}
