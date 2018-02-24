#ifndef MPIMAGEWELL_H
#define MPIMAGEWELL_H

#include <QWidget>
#include <QPainter>
#include <QImage>
#include <QImageReader>
#include <QPixmap>
#include <QMimeData>
#include <QFileInfo>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QAbstractScrollArea>
#include <QScrollBar>
#include <QDropEvent>

class MPImageWell : public QWidget
{
    Q_OBJECT
public:
    explicit MPImageWell(QWidget *parent = nullptr);

    void set_image(QPixmap pixmap);
    void set_mparent(QAbstractScrollArea *mparent);
    QPixmap image();

private:
    QPixmap _image;
    QList<QByteArray> image_formats;
    QAbstractScrollArea *mparent;

    bool _drag_target;

    void dragEnterEvent(QDragEnterEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);
    void paintEvent(QPaintEvent *event);
};

#endif // MPIMAGEWELL_H
