#ifndef MPMAINWINDOW_H
#define MPMAINWINDOW_H

#include <math.h>
#include <QKeyEvent>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QDropEvent>
#include <QMainWindow>
#include <QTextBrowser>
#include <QMediaPlayer>
#include <QMessageBox>
#include <QFileDialog>
#include <QPropertyAnimation>
#include "mpfancyslider.h"
#include "mpplaylisttableviewmodel.h"
#include "mpmetadataanalysisthread.h"
#include "mplyricfetcher.h"
#include "mpsonginfoeditor.h"

namespace Ui {
class MPMainWindow;
}

class MPMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MPMainWindow(QWidget *parent = 0);
    ~MPMainWindow();

    void set_song_info_visibility(bool visible);
    void set_loading_visibility(bool visible);
    void set_lyric_frame_visibility(bool visible);
    void set_lyric_download_visibility(bool visible);

private slots:
    void volumeUpdate(int);
    void addToPlaylist(QStringList filenames);
    void playlistLoadError();
    void playlistUpdated();
    void pausePlayTriggered();
    void updatePausePlayIcon(QMediaPlayer::State state);
    void updatePlaylistIndex(QModelIndex);
    void durationChanged(qint64);
    void positionChanged(qint64);
    void seekSliderChanged(int);
    void mediaChanged(QMediaContent);
    void deletePlaylistItem();
    void toggle_lyric_frame(bool);
    void download_lyrics();
    void lyrics_downloaded(QString lyrics);
    void edit_song();
    void edit_finished();

private:
    QModelIndex selected_item;
    QString time(int seconds);

    QPropertyAnimation *song_info_animation;
    QPropertyAnimation *lyric_frame_animation;
    QPropertyAnimation *lyric_frame_animation2;
    QPropertyAnimation *loading_animation;
    QPropertyAnimation *lyric_fetch_animation;

    Ui::MPMainWindow *ui;
    QMediaPlayer *player;
    MPPlaylistTableVIewModel *model;
    QFileDialog add_dialog;
    qint64 _seek_previous;

    MPMetadataAnalysisThread thread;
    MPLyricFetcher lyric_fetcher;

    MPSongInfoEditor *info_editor;

    void keyPressEvent(QKeyEvent *event) override;

    // support for file drop
    QStringList drop_file_extensions;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
};

#endif // MPMAINWINDOW_H
