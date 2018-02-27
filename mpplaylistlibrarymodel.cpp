#include "mpplaylistlibrarymodel.h"

MPPlaylistLibraryModel::MPPlaylistLibraryModel(QObject *parent)
    : QAbstractListModel(parent)
{
    // initialise our playlist object list
    playlists = new QList<MPPlaylistObject *>;

    // create the "default" playlist
    create_playlist("Default", true);

    // default index
    index = 0;
}

bool MPPlaylistLibraryModel::valid_index(int index)
{
    return (index >= 0 && index < playlists->length());
}

void MPPlaylistLibraryModel::set_index(int index)
{
    if (valid_index(index))
    {
        // update index
        this->index = index;

        // send model change signal
        emit model_changed();
        emit layoutChanged();
    }
}

void MPPlaylistLibraryModel::create_playlist(QString name, bool initialised)
{
    // allocate memory for a new playlist object
    MPPlaylistObject *playlist = new MPPlaylistObject;
    playlist->set_name(name);
    playlist->set_initialised(initialised);

    // insert it to our playlist list
    playlists->append(playlist);

    // update layout
    emit layoutChanged();
}

MPPlaylistObject *MPPlaylistLibraryModel::current_playlist()
{
    return playlists->at(index);
}

int MPPlaylistLibraryModel::current_index()
{
    return index;
}

int MPPlaylistLibraryModel::rowCount(const QModelIndex &) const
{
    return playlists->length();
}

QVariant MPPlaylistLibraryModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
        return playlists->at(index.row())->name().append(
                    (index.row() == this->index ? " (current)" : ""));

    return QVariant();
}

MPPlaylistLibraryModel::~MPPlaylistLibraryModel()
{
    // free allocated memory
    for (int i = 0, l = playlists->length(); i < l; ++i)
        delete playlists->at(i);

    delete playlists;
}
