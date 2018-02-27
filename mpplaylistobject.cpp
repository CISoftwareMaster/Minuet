#include "mpplaylistobject.h"

MPPlaylistObject::MPPlaylistObject(QObject *parent)
    : QObject(parent)
{
    _name = "New Playlist";
    _metadata = new QList<MPMetadata *>;
    _playlist = new QMediaPlaylist;
    _playlist->setCurrentIndex(0);

    // initialised flag (are all files loaded?)
    _initialised = false;
    _filenames = NULL;
}

bool MPPlaylistObject::load_files()
{
    if (_filenames == NULL) return true;

    // load songs, if this playlist is not initialised
    if (!_initialised)
    {
        for (int i = 0, l = _filenames->length(); i < l; ++i)
        {
            QString filename = _filenames->at(i);

            // load song
            _playlist->addMedia(QUrl::fromLocalFile(filename));

            // promise a metadata object value
            MPMetadata *metaData = new MPMetadata;
            metaData->set_replaceable(true);
            _metadata->append(metaData);
        }

        _initialised = true;
        return false;
    }
    return true;
}

QList<QString> *MPPlaylistObject::filenames()
{
    return _filenames;
}

QMediaPlaylist *MPPlaylistObject::playlist()
{
    return _playlist;
}

QList<MPMetadata *> *MPPlaylistObject::metadata()
{
    return _metadata;
}

QString MPPlaylistObject::name()
{
    return _name;
}

bool MPPlaylistObject::initialised()
{
    return _initialised;
}

void MPPlaylistObject::set_name(QString name)
{
    _name = name;
}

void MPPlaylistObject::set_filenames(QList<QString> *filenames)
{
    _filenames = filenames;
}

void MPPlaylistObject::set_initialised(bool initialised)
{
    _initialised = initialised;
}

MPPlaylistObject::~MPPlaylistObject()
{
    // deallocate metadata objects
    for (int i = 0, l = _metadata->length(); i < l; ++ i)
        delete _metadata->at(i);

    delete _metadata;
    delete _playlist;

    if (_filenames != NULL)
        delete _filenames;
}
