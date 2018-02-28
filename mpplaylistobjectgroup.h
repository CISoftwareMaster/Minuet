#ifndef MPPLAYLISTOBJECTGROUP_H
#define MPPLAYLISTOBJECTGROUP_H

#include <QObject>

class MPPlaylistObjectGroup
{
public:
    MPPlaylistObjectGroup(QString filename="", QString iid="");

    QString filename();
    QString iid();

private:
    QString _filename;
    QString _iid;
};

#endif // MPPLAYLISTOBJECTGROUP_H
