#ifndef MPPLAYLISTOBJECT_H
#define MPPLAYLISTOBJECT_H

#include <QObject>
#include <QMediaPlaylist>
#include <QList>
#include "mpmetadata.h"

class MPPlaylistObject : public QObject
{
    Q_OBJECT
public:
    explicit MPPlaylistObject(QObject *parent = nullptr);
    ~MPPlaylistObject();

    void set_name(QString name);
    void set_initialised(bool initialised);
    void set_filenames(QList<QString> *filenames);
    bool load_files();

    QMediaPlaylist *playlist();
    QList<MPMetadata *> *metadata();
    QList<QString> *filenames();
    QString name();
    bool initialised();

private:
    QMediaPlaylist *_playlist;
    QList<MPMetadata *> *_metadata;
    QList<QString> *_filenames;
    QString _name;
    bool _initialised = false;
};

#endif // MPPLAYLISTOBJECT_H
