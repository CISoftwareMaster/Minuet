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

    void set_index(int index);
    MPPlaylistObject *create_playlist(QString name, bool initialised=false);

signals:
    void model_changed();

private:
    QList<MPPlaylistObject *> *playlists;
    int index;
    bool _db_con_error;

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QSqlDatabase database;
};

#endif // MPPLAYLISTLIBRARYMODEL_H
