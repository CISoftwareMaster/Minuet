#ifndef MPPLAYLISTTABLEVIEW_H
#define MPPLAYLISTTABLEVIEW_H

#include <QObject>
#include <QPointF>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QWheelEvent>
#include <QTableView>
#include <QHeaderView>
#include <QPaintEvent>
#include <QPainter>
#include <QWidget>
#include <QScrollBar>
#include "mpplaylisttableviewmodel.h"

class MPPlaylistTableView : public QTableView
{
    Q_OBJECT
public:
    MPPlaylistTableView(QWidget *parent = NULL);
    void setModel(MPPlaylistTableVIewModel *model);
    ~MPPlaylistTableView();

private:
    bool _dragging = false;
    QPoint cursor_point;

    void mousePressEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void paintEvent(QPaintEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;
    void wheelEvent(QWheelEvent *ev) override;
    float find_row_y(int index);

    QModelIndex drag_body_index;
    QModelIndex drag_target_index;

    int scroll_position;

    MPPlaylistTableVIewModel *model_ref;
};

#endif // MPPLAYLISTTABLEVIEW_H
