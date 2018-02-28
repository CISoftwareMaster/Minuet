#include "mpplaylistobjectgroup.h"

MPPlaylistObjectGroup::MPPlaylistObjectGroup(QString filename, QString iid)
{
    _filename = filename;
    _iid = iid;
}

QString MPPlaylistObjectGroup::filename()
{
    return _filename;
}

QString MPPlaylistObjectGroup::iid()
{
    return _iid;
}

