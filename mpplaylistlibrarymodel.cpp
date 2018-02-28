#include "mpplaylistlibrarymodel.h"

MPPlaylistLibraryModel::MPPlaylistLibraryModel(QObject *parent)
    : QAbstractListModel(parent)
{
    // initialise our playlist object list
    playlists = new QList<MPPlaylistObject *>;

    // create the "default" playlist
    create_playlist("Default", true);

    // initialise our database connection
    database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName("playlists.db");
    database.open();

    _db_con_error = !QFileInfo::exists("playlists.db");

    QSqlQuery query = database.exec("SELECT * FROM playlists");

    // fetch playlists from our database
    while (query.next())
    {
        MPPlaylistObject *new_playlist = create_playlist(
                    query.value("name").toString());
        QList<MPPlaylistObjectGroup> *items = new QList<MPPlaylistObjectGroup>;

        // populate playlist with URLs
        QSqlQuery url_query = database.exec("SELECT * FROM urls WHERE pid="
                                            + query.value("pid").toString());

        while (url_query.next())
        {
            MPPlaylistObjectGroup group(url_query.value("url").toString(),
                                        url_query.value("uid").toString());
            items->append(group);
        }

        // update playlist filenames
        new_playlist->set_pid(query.value("pid").toString());
        new_playlist->set_items(items);
        new_playlist->playlist()->setCurrentIndex(0);
    }

    // default index
    index = 0;
}

void MPPlaylistLibraryModel::remove_item(int index)
{
    if (valid_index(current_index()))
    {
        MPPlaylistObject *current = playlists->at(current_index());

        // delete
        if (database.transaction())
        {
            // delete the targetted index
            MPMetadata *target = current->metadata()->at(index);
            database.exec("DELETE FROM urls WHERE uid=" + target->iid());

            // save changes
            database.commit();
        }
    }
}

void MPPlaylistLibraryModel::insert_item(QString path)
{
    if (index > 0 && database.transaction())
    {
        // insert filename to database
        database.exec("INSERT INTO urls (pid,url) VALUES (" + QString(playlists->at(index)->pid())
                      + ", \"" + path + "\")");
        database.commit();
    }
}

bool MPPlaylistLibraryModel::valid_index(int index)
{
    return (index >= 0 && index < playlists->length());
}

bool MPPlaylistLibraryModel::database_is_opened()
{
    return !_db_con_error;
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

MPPlaylistObject *MPPlaylistLibraryModel::create_playlist(QString name, bool initialised)
{
    // allocate memory for a new playlist object
    MPPlaylistObject *playlist = new MPPlaylistObject;
    playlist->set_name(name);
    playlist->set_initialised(initialised);

    // insert it to our playlist list
    playlists->append(playlist);

    // update layout
    emit layoutChanged();

    return playlist;
}

MPPlaylistObject *MPPlaylistLibraryModel::current_playlist()
{
    playlists->at(index)->playlist()->setCurrentIndex(0);
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
    int row = index.row();

    if (role == Qt::DisplayRole)
        return playlists->at(row)->name().append((row == this->index ? " (current)" : ""));

    return QVariant();
}

MPPlaylistLibraryModel::~MPPlaylistLibraryModel()
{
    // free allocated memory
    for (int i = 0, l = playlists->length(); i < l; ++i)
        delete playlists->at(i);

    delete playlists;
}
