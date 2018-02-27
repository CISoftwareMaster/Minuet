#ifndef MPPLAYLISTLIBRARYMODEL_H
#define MPPLAYLISTLIBRARYMODEL_H

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

    void set_index(int index);
    void create_playlist(QString name, bool initialised=false);

signals:
    void model_changed();

private:
    QList<MPPlaylistObject *> *playlists;
    int index;

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
};

#endif // MPPLAYLISTLIBRARYMODEL_H
