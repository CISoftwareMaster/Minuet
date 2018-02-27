#include "mpplaylistlibraryview.h"
#include "ui_mpplaylistlibraryview.h"

MPPlaylistLibraryView::MPPlaylistLibraryView(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::MPPlaylistLibraryView)
{
    ui->setupUi(this);

    // set up playlist view
    QPalette mpalette = ui->playlistView->palette();
    mpalette.setColor(QPalette::Highlight, QColor(150, 150, 150, 25));
    mpalette.setColor(QPalette::HighlightedText, QColor(50, 50, 50));
    ui->playlistView->setPalette(mpalette);
    ui->playlistView->setStyleSheet("background: rgba(255, 255, 255, 0.2)");

    // initialise model
    _model = new MPPlaylistLibraryModel;
    ui->playlistView->setModel(_model);

    // allocate memory for this widget's background effect
    background = QPixmap();
    blur_label = new QLabel;
    blur_effect = new QGraphicsBlurEffect;
    effect_update_timer = new QTimer(this);
    blur_label->setScaledContents(false);

    // set up effect update timer
    effect_update_timer->setInterval(50);
    effect_update_timer->setSingleShot(false);
    effect_update_timer->start();

    // apply blur effect
    blur_effect->setBlurHints(QGraphicsBlurEffect::PerformanceHint);
    blur_effect->setBlurRadius(48);
    blur_label->setGraphicsEffect(blur_effect);

    // hide this by default
    setMaximumWidth(0);

    // initialise animation
    open_animation = new QPropertyAnimation(this, "maximumWidth");
    open_animation->setEasingCurve(QEasingCurve::InOutQuart);
    open_animation->setDuration(500);

    // points to NULL by default
    effect_view = NULL;

    // move this sidebar's y coordinate by "Y_GAP"
    move(x(), Y_GAP);

    // handle connections
    connect(effect_update_timer, SIGNAL(timeout()), this, SLOT(update_effect()));
    connect(ui->playlistView, SIGNAL(activated(QModelIndex)),
            this, SLOT(selection_changed(QModelIndex)));
}

void MPPlaylistLibraryView::selection_changed(QModelIndex index)
{
    // do nothing if the user reselects the current index
    if (index.row() == _model->current_index()) return;
    _model->set_index(index.row());
}

void MPPlaylistLibraryView::update_effect()
{
    // only update the effect when this widget is visible
    if (maximumWidth() > 0 && effect_view != NULL)
    {
        // update width
        if (width() != maximumWidth())
            resize(maximumWidth(), height());

        // capture background
        int mx = x() - effect_view->x();
        background = effect_view->grab(QRect(mx, 0, width(), effect_view->height()));

        // apply pixmap then capture the blurred version
        blur_label->setPixmap(background);

        // resize pixmap
        blur_label->resize(width(), effect_view->height());
        background = blur_label->grab().scaled(
                    width(), height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

        // update user interface
        repaint();
    }
}

void MPPlaylistLibraryView::update_height(int h)
{
    resize(width(), h-Y_GAP);
    repaint();
}

void MPPlaylistLibraryView::set_effect_view(QFrame *effect_view)
{
    this->effect_view = effect_view;
}

void MPPlaylistLibraryView::set_open(bool active)
{
    open_animation->stop();
    open_animation->setStartValue(active ? 0 : LIBRARY_SIDEBAR_WIDTH);
    open_animation->setEndValue(active ? LIBRARY_SIDEBAR_WIDTH : 0);
    open_animation->start();
}

void MPPlaylistLibraryView::paintEvent(QPaintEvent *ev)
{
    // only update the background when this widget is visible
    if (maximumWidth() > 0)
    {
        QPainter painter;

        if (painter.begin(this))
        {
            // enable antialiased rendering
            painter.setRenderHints(QPainter::Antialiasing);
            painter.setPen(Qt::NoPen);

            // draw background
            painter.setBrush(QBrush(QColor(255, 255, 255)));
            painter.drawRect(0, 0, width(), height());

            // draw blurred background
            if (!background.isNull())
                painter.drawPixmap(0, 50 - Y_GAP, background.width(), background.height(), background);

            // draw white rectangle
            painter.setBrush(QBrush(QColor(255, 255, 255, 150)));
            painter.drawRect(0, 0, width(), height());

            // draw side border
            painter.setBrush(QBrush(QColor(200, 200, 200, 150)));
            painter.drawRect(width()-BORDER_WIDTH, 0, BORDER_WIDTH, height());

            painter.end();
        }

        // handle default paint behaviour
        QWidget::paintEvent(ev);
    }
}

MPPlaylistLibraryModel *MPPlaylistLibraryView::model()
{
    return _model;
}

MPPlaylistLibraryView::~MPPlaylistLibraryView()
{
    // free allocated memory
    delete ui;
    delete _model;
    delete blur_effect;
    delete blur_label;
    delete effect_update_timer;
    delete open_animation;
}
