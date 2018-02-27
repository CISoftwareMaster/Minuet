#ifndef MPPLAYLISTLIBRARYVIEW_H
#define MPPLAYLISTLIBRARYVIEW_H

#include <QObject>
#include <QListView>
#include <QWidget>
#include <QPainter>
#include <QGraphicsBlurEffect>
#include <QPropertyAnimation>
#include <QListView>
#include <QTimer>
#include <QPixmap>
#include <QLabel>
#include <QFrame>
#include <QScrollBar>
#include "mpplaylistlibrarymodel.h"
#include "mpplaylistobject.h"

#define Y_GAP 40
#define BORDER_WIDTH 2
#define LIBRARY_SIDEBAR_WIDTH 300

namespace Ui
{
class MPPlaylistLibraryView;
};

class MPPlaylistLibraryView : public QWidget
{
    Q_OBJECT

public:
    explicit MPPlaylistLibraryView(QWidget *parent = 0);
    ~MPPlaylistLibraryView();

    void set_effect_view(QFrame *effect_view);
    void update_height(int h);

    MPPlaylistLibraryModel *model();

public slots:
    void set_open(bool active);

private:
    Ui::MPPlaylistLibraryView *ui;
    QFrame *effect_view;
    QPixmap background;
    QLabel *blur_label;
    QTimer *effect_update_timer;
    QGraphicsBlurEffect *blur_effect;
    MPPlaylistLibraryModel *_model;

    QPropertyAnimation *open_animation;

    void paintEvent(QPaintEvent *ev);

private slots:
    void selection_changed(QModelIndex);
    void update_effect();
};

#endif // MPPLAYLISTLIBRARYVIEW_H
