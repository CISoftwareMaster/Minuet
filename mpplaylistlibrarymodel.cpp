#include "mpplaylistlibrarymodel.h"

MPPlaylistLibraryModel::MPPlaylistLibraryModel(QObject *parent)
    : QAbstractListModel(parent)
{
    // initialise our playlist object list
    playlists = new QList<MPPlaylistObject *>;

    // create the "default" playlist
    create_playlist("Temporary", true);

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
        new_playlist->set_initialised(items->length() == 0);
        new_playlist->playlist()->setCurrentIndex(0);
    }

    // default index
    index = 0;
}

void MPPlaylistLibraryModel::change_playlist_name(int index, QString name)
{
    if (valid_index(index))
    {
        MPPlaylistObject *target = playlists->at(index);

        // update playlist name
        if (database.transaction())
        {
            QSqlQuery query;
            query.prepare("UPDATE playlists SET name=:name WHERE pid=:pid");
            query.bindValue(":pid", target->pid());
            query.bindValue(":name", name);
            query.exec();

            database.commit();
        }
    }
}

MPPlaylistObject *MPPlaylistLibraryModel::playlist_at(int index)
{
    return playlists->at(index);
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
            QSqlQuery query;
            query.prepare("DELETE FROM urls WHERE uid=:uid");
            query.bindValue(":uid", target->iid());
            query.exec();

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
        QSqlQuery query;
        query.prepare("INSERT INTO urls (pid, url) VALUES (:pid, :url)");
        query.bindValue(":pid", playlists->at(index)->pid());
        query.bindValue(":url", path);
        query.exec();

        database.commit();
    }
}

void MPPlaylistLibraryModel::insert_playlist(QString name)
{
    if (database.transaction())
    {
        QSqlQuery query;
        query.prepare("INSERT INTO playlists (name) VALUES (:name)");
        query.bindValue(":name", name);
        query.exec();

        database.commit();

        // fetch playlist information
        QSqlQuery nquery;
        nquery.prepare("SELECT * FROM playlists ORDER BY pid DESC LIMIT 1");
        nquery.exec();

        if (nquery.first())
        {
            MPPlaylistObject *new_playlist = create_playlist(name, true);
            new_playlist->set_pid(nquery.value("pid").toString());
            new_playlist->playlist()->setCurrentIndex(0);
        }
    }
}

void MPPlaylistLibraryModel::remove_playlist(int index)
{
    if (valid_index(index))
    {
        if (database.transaction())
        {
            MPPlaylistObject *current = playlists->at(index);
            QSqlQuery query;
            query.prepare("DELETE FROM playlists WHERE pid=:pid");
            query.bindValue(":pid", current->pid());
            query.exec();

            database.commit();

            // remove from model
            playlists->removeAt(index);
            emit layoutChanged();
        }
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
        return playlists->at(row)->name().prepend((row == this->index ? " >" : ""));

    return QVariant();
}

MPPlaylistLibraryModel::~MPPlaylistLibraryModel()
{
    // free allocated memory
    for (int i = 0, l = playlists->length(); i < l; ++i)
        delete playlists->at(i);

    delete playlists;
}
