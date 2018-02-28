#ifndef MPPLAYLISTOBJECT_H
#define MPPLAYLISTOBJECT_H

#include <QObject>
#include <QMediaPlaylist>
#include <QList>
#include "mpmetadata.h"
#include "mpplaylistobjectgroup.h"

class MPPlaylistObject : public QObject
{
    Q_OBJECT
public:
    explicit MPPlaylistObject(QObject *parent = nullptr);
    ~MPPlaylistObject();

    void set_pid(QString pid);
    void set_name(QString name);
    void set_initialised(bool initialised);
    void set_items(QList<MPPlaylistObjectGroup> *items);
    bool load_files();

    QString pid();
    QMediaPlaylist *playlist();
    QList<MPMetadata *> *metadata();
    QList<MPPlaylistObjectGroup> *items();
    QString name();
    bool initialised();

private:
    QString _pid;
    QMediaPlaylist *_playlist;
    QList<MPMetadata *> *_metadata;
    QList<MPPlaylistObjectGroup> *_items;
    QString _name;
    bool _initialised = false;
};

#endif // MPPLAYLISTOBJECT_H
