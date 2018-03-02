#include "mpmainwindow.h"
#include "ui_mpmainwindow.h"

MPMainWindow::MPMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MPMainWindow)
{
    ui->setupUi(this);

    // set slider type
    ui->seekSlider->slider_type = MPFancySeekSlider;

    // supported extensions for file drop
    drop_file_extensions = QStringList({"ogg", "mp3", "m4a", "webm", "flac"});

    // set file extensions for the lyric fetcher
    lyric_fetcher.set_file_extensions(drop_file_extensions);

    // hide the current song information box
    ui->currentSongInfo->setMaximumHeight(0);

    // hide the loading indicator
    ui->loadingIndicator->setMaximumHeight(0);

    // hide the lyrics frame
    ui->lyricsFrame->setMaximumWidth(0);

    // hide the vertical scrollbar in the lyric view widget
    ui->lyricsView->verticalScrollBar()->hide();

    // hide the lyric fetch label indicator
    ui->lyricsDownloadLabelFrame->setMaximumHeight(0);

    // set playlist scrollbar width
    ui->playlistView->verticalScrollBar()->setMaximumWidth(4);

    // initialise our model then assign it to our playlist view
    model = new MPPlaylistTableVIewModel;
    ui->playlistView->setModel(model);

    // resize table view headers
    ui->playlistView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->playlistView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->playlistView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    ui->playlistView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    ui->playlistView->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Fixed);
    ui->playlistView->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch);

    // initialise the song info editor
    info_editor = new MPSongInfoEditor;

    // initialise our playlist library widget
    playlist_library = new MPPlaylistLibraryView(this);
    playlist_library->set_effect_view(ui->controlFrame);

    // point to the playlist library model
    library_model = playlist_library->model();

    // initial model change
    model_changed();

    // initialise our player
    player = new QMediaPlayer;
    player->setAudioRole(QAudio::MusicRole);
    player->setPlaylist(model->playlist());

    // connect this player to our model
    model->set_player_reference(player);

    // initialise our file dialog
    add_dialog.setAcceptMode(QFileDialog::AcceptOpen);
    add_dialog.setFileMode(QFileDialog::ExistingFiles);
    add_dialog.setNameFilter(tr("Music Files (*.ogg *.mp3 *.m4a *.webm *.flac)"));

    // previous position of the seek slider
    _seek_previous = 0;

    // update window size
    this->setMinimumWidth(640);
    this->setMinimumHeight(480);
    this->resize(820, 480);

    // update our volume slider value
    ui->volumeSlider->setValue(player->volume());

    // establish connections
    connect(ui->actionAdd, SIGNAL(triggered(bool)),
            &add_dialog, SLOT(show()));

    connect(model->playlist(), SIGNAL(loaded()),
            this, SLOT(playlistUpdated()));

    connect(model->playlist(), SIGNAL(loadFailed()),
            this, SLOT(playlistLoadError()));

    connect(&thread, SIGNAL(analysis_complete(QList<MPMetadata*>*)),
            model, SLOT(metadata_update(QList<MPMetadata*>*)));

    connect(&thread, SIGNAL(analysis_complete(QList<MPMetadata*>*)),
            this, SLOT(playlistUpdated()));

    connect(ui->actionAddToPlaylist, SIGNAL(triggered(bool)),
            &add_dialog, SLOT(show()));

    connect(ui->actionPausePlay, SIGNAL(triggered(bool)),
            this, SLOT(pausePlayTriggered()));

    connect(ui->actionRemove, SIGNAL(triggered(bool)),
            this, SLOT(deletePlaylistItem()));

    connect(&add_dialog, SIGNAL(filesSelected(QStringList)),
            this, SLOT(addToPlaylist(QStringList)));

    connect(ui->seekSlider, SIGNAL(valueChanged(int)),
            this, SLOT(seekSliderChanged(int)));

    connect(player, SIGNAL(stateChanged(QMediaPlayer::State)),
            this, SLOT(updatePausePlayIcon(QMediaPlayer::State)));

    connect(player, SIGNAL(durationChanged(qint64)),
            this, SLOT(durationChanged(qint64)));

    connect(player, SIGNAL(currentMediaChanged(QMediaContent)),
            this, SLOT(mediaChanged(QMediaContent)));

    connect(player, SIGNAL(positionChanged(qint64)),
            this, SLOT(positionChanged(qint64)));

    connect(ui->actionNext, SIGNAL(triggered(bool)),
            this, SLOT(nextSong()));

    connect(ui->actionPrevious, SIGNAL(triggered(bool)),
            this, SLOT(previousSong()));

    connect(ui->actionStop, SIGNAL(triggered(bool)),
            player, SLOT(stop()));

    connect(ui->actionLyrics, SIGNAL(triggered(bool)),
            this, SLOT(toggle_lyric_frame(bool)));

    connect(ui->playlistView, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(updatePlaylistIndex(QModelIndex)));

    connect(ui->volumeSlider, SIGNAL(valueChanged(int)),
            this, SLOT(volumeUpdate(int)));

    connect(&lyric_fetcher, SIGNAL(fetch_finished(QString)),
            this, SLOT(lyrics_downloaded(QString)));

    connect(ui->lyricsDownloadButton, SIGNAL(clicked(bool)),
            this, SLOT(download_lyrics()));

    connect(ui->actionEditInfo, SIGNAL(triggered(bool)),
            this, SLOT(edit_song()));

    connect(info_editor, SIGNAL(editing_finished()),
            this, SLOT(edit_finished()));

    connect(ui->actionPlaylists, SIGNAL(toggled(bool)),
            playlist_library, SLOT(set_open(bool)));

    connect(library_model, SIGNAL(model_changed()),
            this, SLOT(model_changed()));

    connect(ui->actionRefresh, SIGNAL(triggered(bool)),
            this, SLOT(reanalyse()));

    // initialise animations
    song_info_animation = new QPropertyAnimation(ui->currentSongInfo, "maximumHeight");
    loading_animation = new QPropertyAnimation(ui->loadingIndicator, "maximumHeight");
    lyric_frame_animation = new QPropertyAnimation(ui->lyricsFrame, "maximumWidth");
    lyric_frame_animation2 = new QPropertyAnimation(ui->lyricsFrame, "minimumWidth");
    lyric_fetch_animation = new QPropertyAnimation(ui->lyricsDownloadLabelFrame, "maximumHeight");

    song_info_animation->setDuration(ANIMATION_SPEED + 150);
    loading_animation->setDuration(ANIMATION_SPEED);
    lyric_frame_animation->setDuration(ANIMATION_SPEED);
    lyric_frame_animation2->setDuration(ANIMATION_SPEED);
    lyric_fetch_animation->setDuration(ANIMATION_SPEED);

    song_info_animation->setEasingCurve(QEasingCurve::OutBounce);
    lyric_frame_animation->setEasingCurve(QEasingCurve::InOutExpo);
    lyric_frame_animation2->setEasingCurve(QEasingCurve::InOutExpo);
    loading_animation->setEasingCurve(QEasingCurve::InOutCubic);
    lyric_fetch_animation->setEasingCurve(QEasingCurve::InOutCubic);

    // show an error message, if our database connection failed
    if (!library_model->database_is_opened())
        QMessageBox::warning(this, tr("Warning!"),
                             tr("Your playlist library database file is missing!"));
}

void MPMainWindow::reanalyse()
{
    if (model->metadata()->length() > 0)
    {
        // mark everything as replaceable
        for (int i = 0, l = library_model->current_playlist()->metadata()->length(); i < l; ++i)
            library_model->current_playlist()->metadata()->at(i)->set_replaceable(true);

        // start analyser
        thread.begin(this->player, true);
        set_loading_visibility(true);
    }
}

void MPMainWindow::previousSong()
{
    if (player->playlist() != NULL)
        player->playlist()->previous();
}

void MPMainWindow::nextSong()
{
    if (player->playlist() != NULL)
        player->playlist()->next();
}

void MPMainWindow::model_changed()
{
    MPPlaylistObject *object = library_model->current_playlist();

    // reset seek slider
    ui->seekSlider->setValue(0);

    // update pointers
    thread.set_metadata_list(object->metadata());
    model->set_metadata(object->metadata());
    model->set_playlist(object->playlist());

    // update player
    if (player != NULL)
    {
        player->setPlaylist(object->playlist());
        player->stop();
        emit player->mediaChanged(player->currentMedia());
    }

    // run the metadata analyser
    if (!object->initialised())
    {
        object->load_files();
        thread.begin(this->player, true);
        set_loading_visibility(true);
    }

    // update player
    emit model->layoutChanged();
}

void MPMainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    // get mime data
    const QMimeData *mime = event->mimeData();

    // do the dropped file(s) have a valid extension?
    bool has_valid = false;

    // try to find at least one supported file
    foreach (QUrl url, mime->urls())
    {
        QFileInfo info(url.toString());

        if (drop_file_extensions.contains(info.suffix()))
        {
            has_valid = true;
            break;
        }
    }

    // check if this file is supported
    if (has_valid)
    {
        // allow this file drop
        event->acceptProposedAction();
    }
}

void MPMainWindow::dropEvent(QDropEvent *event)
{
    int files_loaded = 0;

    // iterate over the URls
    foreach (QUrl url, event->mimeData()->urls())
    {
        QFileInfo info(url.toString());
        QString url_s = url.toString().replace("file://", "");

        // only load accepted files into the media player
        if (drop_file_extensions.contains(info.suffix()))
        {
            // load this file into the player
            player->playlist()->addMedia(QUrl::fromLocalFile(url_s));

            // update database
            library_model->insert_item(url_s);

            // increment the files loaded counter
            files_loaded ++;
        }
    }

    if (files_loaded > 0)
    {
        // show loading indicator
        set_loading_visibility(true);

        // start metadata analysis
        model->set_updating(true);
        thread.begin(this->player);
    }
}

QString MPMainWindow::time(int secs)
{
    // get minutes and seconds from "secs"
    int minutes = secs / 60;
    int seconds = secs % 60;

    QString min_str = QString("%1").arg(minutes);
    QString sec_str = QString("%1").arg(seconds);

    // apply padding
    if (minutes < 10) min_str = "0" + min_str;
    if (seconds < 10) sec_str = "0" + sec_str;

    return min_str + ":" + sec_str;
}

void MPMainWindow::mediaChanged(QMediaContent media)
{
    if (!media.isNull() && !model->updating())
    {
        int index = player->playlist()->currentIndex();

        // check if the index is within range
        if (index >= 0 && index < player->playlist()->mediaCount())
        {
            // select the currently playing item
            ui->playlistView->selectRow(index);

            // update the current song info box
            MPMetadata *metadata = model->metadata()->at(index);
            ui->songTitle->setText(metadata->title());
            ui->songArtist->setText(metadata->artist());
            ui->songAlbum->setText(metadata->album());
            ui->songGenre->setText(metadata->genre());
            ui->songYear->setText(metadata->year());
            ui->songArt->setPixmap(metadata->image());

            // update lyrics
            ui->lyricsView->setText((metadata->lyrics().isEmpty() ?
                                         "No Lyrics" : metadata->lyrics()));

            // update title
            setWindowTitle("Minuet - " + metadata->title() + " - " + metadata->artist());

            // show the current info box
            if (player->state() != QMediaPlayer::StoppedState)
                set_song_info_visibility(true);
        }
    }
    else
        setWindowTitle("Minuet");
}

void MPMainWindow::set_song_info_visibility(bool visible)
{
    if ((!visible && ui->currentSongInfo->maximumHeight() == 0) ||
        (visible && ui->currentSongInfo->maximumHeight() == 135))
        return;

    song_info_animation->stop();
    song_info_animation->setStartValue(visible ? 0 : 135);
    song_info_animation->setEndValue(visible ? 135 : 0);
    song_info_animation->start();
}

void MPMainWindow::set_lyric_frame_visibility(bool visible)
{
    if ((!visible && ui->lyricsFrame->maximumWidth() == 0) ||
        (visible && ui->lyricsFrame->maximumWidth() == 240))
        return;

    lyric_frame_animation->stop();
    lyric_frame_animation->setStartValue(visible ? 0 : 240);
    lyric_frame_animation->setEndValue(visible ? 240 : 0);
    lyric_frame_animation->start();

    lyric_frame_animation2->stop();
    lyric_frame_animation2->setStartValue(visible ? 0 : 240);
    lyric_frame_animation2->setEndValue(visible ? 240 : 0);
    lyric_frame_animation2->start();
}


void MPMainWindow::set_loading_visibility(bool visible)
{
    loading_animation->stop();
    loading_animation->setStartValue(visible ? 0 : 40);
    loading_animation->setEndValue(visible ? 40 : 0);
    loading_animation->start();
}

void MPMainWindow::set_lyric_download_visibility(bool visible)
{
    lyric_fetch_animation->stop();
    lyric_fetch_animation->setStartValue(visible ? 0 : 40);
    lyric_fetch_animation->setEndValue(visible ? 40 : 0);
    lyric_fetch_animation->start();
}

void MPMainWindow::durationChanged(qint64 duration)
{
    if (!model->updating())
    {
        // calculate total time
        double seconds = duration / 1000.0f;

        // update duration
        ui->seekSlider->setMaximum(duration);

        // update seek duration label
        ui->seekDuration->setText(time(seconds));
    }
}

void MPMainWindow::positionChanged(qint64 position)
{
    // update seek slider position
    _seek_previous = position;
    ui->seekSlider->setValue(position);
}

void MPMainWindow::seekSliderChanged(int seek)
{
    // update player position
    if (seek != _seek_previous)
    {
        player->setPosition(seek);
    }

    // find out position (in time)
    double seconds = (ui->seekSlider->maximum()
                     * ((double)seek / ui->seekSlider->maximum())) / 1000.0f;

    // update seek label
    ui->seekLabel->setText(time(seconds));

    // update previous seek position
    _seek_previous = seek;

    // if we're autoscrolling, update the lyric view
    if (ui->lyricAutoScrollButton->isChecked())
    {
        float position = ((float)seek/ui->seekSlider->maximum());
        position = position * ui->lyricsView->verticalScrollBar()->maximum();

        ui->lyricsView->verticalScrollBar()->setValue(
                    ui->lyricsDelaySlider->value() + (position-300));
    }
}

void MPMainWindow::volumeUpdate(int x)
{
    // update player volume
    player->setVolume(x);

    // update volume label
    ui->volumeLabel->setText(QString("%1\%").arg(x));
}

void MPMainWindow::pausePlayTriggered()
{
    if (player->state() == QMediaPlayer::PlayingState)
    {
        set_song_info_visibility(false);
        player->pause();
    }
    else
    {
        set_song_info_visibility(true);
        player->play();
    }
}

void MPMainWindow::updatePausePlayIcon(QMediaPlayer::State state)
{
    if (state == QMediaPlayer::PlayingState)
    {
        ui->actionPausePlay->setIcon(QIcon(":/icons/pause.png"));
        ui->actionPausePlay->setText(tr("Pause"));
    }
    else
    {
        ui->actionPausePlay->setIcon(QIcon(":/icons/play.png"));
        ui->actionPausePlay->setText(tr("Play"));
    }

    // hide the current information box
    if (state == QMediaPlayer::StoppedState)
            set_song_info_visibility(false);
}

void MPMainWindow::updatePlaylistIndex(QModelIndex index)
{
    player->play();

    // update playlist index
    player->playlist()->setCurrentIndex(index.row());
}

void MPMainWindow::playlistUpdated()
{
    // hide loading indicator
    set_loading_visibility(false);

    // update user interface
    emit model->layoutChanged();
    ui->playlistView->viewport()->repaint();
}

void MPMainWindow::playlistLoadError()
{
    QMessageBox::warning(this, tr("Add to Playlist (Error)"),
                     "There was a problem adding a selected file into the playlist.");
}

void MPMainWindow::addToPlaylist(QStringList filenames)
{
    // show the loading indicator
    set_loading_visibility(true);

    foreach (QString filename, filenames)
    {
        // load files into our media player
        model->playlist()->addMedia(QUrl::fromLocalFile(filename));

        // update database
        library_model->insert_item(filename);
    }

    // start metadata analyser
    model->set_updating(true);
    thread.begin(this->player);
}

void MPMainWindow::deletePlaylistItem()
{
    QModelIndex index = ui->playlistView->currentIndex();

    if (index.row() >= 0 && index.row() < model->metadata()->length())
    {
        qint64 current_position = ui->seekSlider->value();
        int former_state = player->state();
        int playlist_index = player->playlist()->currentIndex();

        // delete playlist item
        player->playlist()->removeMedia(index.row());

        // delete item from database
        if (library_model->current_index() > 0)
            library_model->remove_item(index.row());

        // delete the target row
        delete model->metadata()->at(index.row());
        model->metadata()->removeAt(index.row());

        if (index.row() < playlist_index)
        {
            // update position
            player->playlist()->setCurrentIndex(playlist_index - 1);
            player->setPosition(current_position);

            if (former_state == QMediaPlayer::PlayingState)
                player->play();
        }

        // hide song info box, if we deleted the last row
        if (model->metadata()->length() == 0)
            set_song_info_visibility(false);

        // update layout
        emit model->layoutChanged();
    }
}

void MPMainWindow::toggle_lyric_frame(bool active)
{
    set_lyric_frame_visibility(active);
}

void MPMainWindow::keyPressEvent(QKeyEvent *event)
{
    // handle default behaviour(s)
    QMainWindow::keyPressEvent(event);

    // handle "return" key for playback
    if (event->key() == Qt::Key_Return)
    {
        // get selected index
        QModelIndex index = ui->playlistView->currentIndex();

        if (index.row() >= 0 && index.row() < model->metadata()->length())
        {
            player->playlist()->setCurrentIndex(index.row());
            player->play();
        }
    }
}

void MPMainWindow::download_lyrics()
{
    if (player->currentMedia() != NULL && !lyric_fetcher.downloading())
    {
        // get metadata index
        int index = player->playlist()->currentIndex();

        if (index >= 0 && model->metadata()->length())
        {
            MPMetadata *metadata = model->metadata()->at(index);

            // show the lyric fetch indicator
            set_lyric_download_visibility(true);

            // start the lyric downloader
            lyric_fetcher.fetch(metadata);
        }
    }
}

void MPMainWindow::lyrics_downloaded(QString lyrics)
{
    // update lyrics view
    if (lyrics.isEmpty())
        ui->lyricsView->setPlainText("No Lyrics");
    else
        ui->lyricsView->setText(lyrics);

    // hide the lyric fetch indicator
    set_lyric_download_visibility(false);
}

void MPMainWindow::edit_song()
{
    int index = ui->playlistView->currentIndex().row();

    // check if the currently selected song is within the bounds of our metadata list
    if (index >= 0 && index < model->metadata()->length())
    {
        MPMetadata *metadata = model->metadata()->at(index);

        //  move the song info editor closer to our main window
        float position = round((x() + width())/2.0f);
        info_editor->move(QPoint(position, y()));

        // show the song info editor
        info_editor->edit(metadata);
    }
}

void MPMainWindow::edit_finished()
{
    // send update signal
    emit model->layoutChanged();

    // update media
    mediaChanged(player->currentMedia());
}

void MPMainWindow::resizeEvent(QResizeEvent *ev)
{
    // update playlist library view height
    playlist_library->update_height(height());

    // handle default behaviour
    QMainWindow::resizeEvent(ev);
}

MPMainWindow::~MPMainWindow()
{
    // stop player
    player->stop();

    // free allocated memory
    thread.dealloc();
    thread.quit();

    delete song_info_animation;
    delete loading_animation;
    delete lyric_frame_animation;
    delete lyric_frame_animation2;
    delete lyric_fetch_animation;

    delete info_editor;
    delete playlist_library;
    delete player;
    delete ui;
}
