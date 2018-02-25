#include "mpplaylisttableview.h"

MPPlaylistTableView::MPPlaylistTableView(QWidget *parent)
    : QTableView(parent)
{
    // set up drag and drop variables
    _dragging = false;
    cursor_point = QPoint(0, 0);
    drag_body_index = QModelIndex();
    drag_target_index = QModelIndex();

    // point to null by default
    model_ref = NULL;
    scroll_position = 0;

    // update palette
    QPalette npalette = palette();
    npalette.setColor(QPalette::AlternateBase, QColor(230, 230, 230));
    npalette.setColor(QPalette::Highlight, QColor(50, 125, 255));
    setPalette(npalette);

    // allow mouse tracking
    setMouseTracking(true);
}

void MPPlaylistTableView::setModel(MPPlaylistTableVIewModel *model)
{
    // handle default behaviour
    QTableView::setModel(model);

    // update model reference
    model_ref = model;
}

void MPPlaylistTableView::mousePressEvent(QMouseEvent *event)
{
    if (model_ref != NULL)
    {
        if (model_ref->metadata() == NULL) return;

        if (event->buttons() == Qt::LeftButton && model_ref->metadata()->length() > 0)
        {
            _dragging = true;

            // update mouse coordinate
            cursor_point = QPoint(event->x(), event->y());

            // set drag body
            drag_body_index = indexAt(cursor_point);
            drag_target_index = drag_body_index;

            viewport()->repaint();
        }
    }

    // handle default behaviour
    QTableView::mousePressEvent(event);
}

void MPPlaylistTableView::mouseMoveEvent(QMouseEvent *event)
{
    // only update drag properties, if we are in drag mode
    if (_dragging && model_ref != NULL)
    {
        if (model_ref->metadata() != NULL)
        {
            // update mouse coordinate
            cursor_point = QPoint(event->x(), event->y());

            // set drag target
            drag_target_index = indexAt(cursor_point);
        }

        viewport()->repaint();
    }

    QTableView::mouseMoveEvent(event);
}

void MPPlaylistTableView::mouseReleaseEvent(QMouseEvent *event)
{
    if (_dragging)
    {
        // end of drag operation
        _dragging = false;

        // swap
        model_ref->shift_items(drag_body_index.row(), drag_target_index.row());

        viewport()->repaint();
    }

    QTableView::mouseReleaseEvent(event);
}

void MPPlaylistTableView::wheelEvent(QWheelEvent *ev)
{
    QTableView::wheelEvent(ev);

    // get scroll position
    scroll_position = verticalScrollBar()->value();
    viewport()->repaint();
}

float MPPlaylistTableView::find_row_y(int index)
{
    float y = 0;

    if (model_ref != NULL)
    {
        if (model_ref->metadata() == NULL) return 0;

        if (index >= 0 && index < model_ref->metadata()->length())
        {
            int row_height = rowHeight(0);

            // calculate y
            int target_index = index;
            y = ((float)target_index * row_height) -  (float)scroll_position;

            // hide the item when we reach the edge
            if (y < 0)
                y = -100;
        }
    }

    return y;
}

void MPPlaylistTableView::resizeEvent(QResizeEvent *e)
{
    QTableView::resizeEvent(e);
    find_row_y(1);
}

void MPPlaylistTableView::paintEvent(QPaintEvent *e)
{
    // handle default painting behaviour
    QTableView::paintEvent(e);
    QPainter painter;

    if (painter.begin(this->viewport()))
    {
        if (_dragging && (drag_body_index != drag_target_index))
        {
            if (drag_target_index.row() == -1 || drag_body_index.row() == -1) return;

            float position_body = find_row_y(drag_body_index.row());
            float position_target = find_row_y(drag_target_index.row());

            // highlight the drag body object
            painter.fillRect(QRect(-4, position_body-4, width(), rowHeight(0)+8),
                             QColor(0, 0, 0, 50));

            painter.fillRect(QRect(0, position_body, width(), rowHeight(0)),
                             QColor(255, 255, 255, 150));

            // highlight the drag target object
            painter.fillRect(QRect(0, position_target, width(), rowHeight(0)),
                             QColor(80, 125, 85, 200));
        }

        painter.end();
    }
}

MPPlaylistTableView::~MPPlaylistTableView()
{
}
