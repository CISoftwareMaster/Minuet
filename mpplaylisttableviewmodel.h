#ifndef MPPLAYLISTTABLEVIEWMODEL_H
#define MPPLAYLISTTABLEVIEWMODEL_H

#include <QObject>
#include <QPixmap>
#include <QAbstractTableModel>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include "mpmetadata.h"

class MPPlaylistTableVIewModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    MPPlaylistTableVIewModel(QObject *parent = 0);
    ~MPPlaylistTableVIewModel();

    QList<MPMetadata *> *metadata();

    void set_updating(bool updating);
    void set_playlist(QMediaPlaylist *playlist);
    void set_metadata(QList<MPMetadata *> *metadata);
    bool updating();
    void shift_items(int x, int y);
    void set_player_reference(QMediaPlayer *ref);

    QMediaPlaylist *playlist();

public slots:
    void metadata_update(QList<MPMetadata *> *);

private:
    QMediaPlayer *player_ref;
    QMediaPlaylist *_playlist;
    QList<MPMetadata *> *_metadata;
    bool _updating;

protected:
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
};

#endif // MPPLAYLISTTABLEVIEWMODEL_H
