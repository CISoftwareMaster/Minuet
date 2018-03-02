#ifndef MPPLAYLISTLIBRARYMODEL_H
#define MPPLAYLISTLIBRARYMODEL_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDir>
#include <QObject>
#include <QAbstractListModel>
#include <QList>
#include "mpplaylistobject.h"
#include "mpplaylistobjectgroup.h"
#include "mpplaylistlibrarymodel.h"

class MPPlaylistLibraryModel : public QAbstractListModel
{
    Q_OBJECT
public:
    MPPlaylistLibraryModel(QObject *parent = nullptr);
    ~MPPlaylistLibraryModel();

    MPPlaylistObject *current_playlist();

    int current_index();
    bool valid_index(int index);
    bool database_is_opened();

    // playlist operation(s)
    void remove_item(int index);
    void insert_item(QString path);
    void insert_playlist(QString name);
    void change_playlist_name(int index, QString name);
    void remove_playlist(int index);

    void set_index(int index);
    MPPlaylistObject *playlist_at(int index);

signals:
    void model_changed();

private:
    MPPlaylistObject *create_playlist(QString name, bool initialised=false);
    QList<MPPlaylistObject *> *playlists;
    int index;
    bool _db_con_error;

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;

protected:
    QSqlDatabase database;
};

#endif // MPPLAYLISTLIBRARYMODEL_H
