#include "mpplaylisttableviewmodel.h"

MPPlaylistTableVIewModel::MPPlaylistTableVIewModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    // point to null by default
    this->_playlist = NULL;

    // point to NULL by default
    this->player_ref = NULL;
    this->_metadata = NULL;
    this->_updating = false;
}

void MPPlaylistTableVIewModel::set_metadata(QList<MPMetadata *> *metadata)
{
    _metadata = metadata;
}

void MPPlaylistTableVIewModel::set_playlist(QMediaPlaylist *playlist)
{
    // update playlist pointer
    _playlist = playlist;
}

void MPPlaylistTableVIewModel::set_player_reference(QMediaPlayer *ref)
{
    player_ref = ref;
}

void MPPlaylistTableVIewModel::shift_items(int x, int y)
{
    // only allow shifting, if there's a player connected to this model
    if (player_ref  == NULL) return;

    if (x >= 0 && y >= 0 && x < metadata()->length() && y < metadata()->length())
    {
        // cancel, if we're shifting the same row
        if (x != y)
        {
            // store current player state
            int player_position = player_ref->position();
            int playlist_index = _playlist->currentIndex();
            int player_state = player_ref->state();

            // perform shift
            _metadata->move(x, y);
            _playlist->moveMedia(x, y);

            // update playlist index
            if (playlist_index == x)
                _playlist->setCurrentIndex(y);
            else if (playlist_index == y)
                _playlist->setCurrentIndex(playlist_index + (x < y ? -1 : 1));
            else if ((playlist_index == _playlist->mediaCount()-1) &&
                     (y < playlist_index))
                _playlist->setCurrentIndex(_playlist->mediaCount()-1);
            else if((playlist_index < _playlist->mediaCount()) &&
                    (y < playlist_index))
                _playlist->setCurrentIndex(playlist_index+1);
            else
                _playlist->setCurrentIndex(playlist_index);

            // restore state
            if (player_state == QMediaPlayer::PlayingState)
            {
                player_ref->setPosition(player_position);
                player_ref->play();
            }

            // update layout
            emit layoutChanged();
        }
    }
}

bool MPPlaylistTableVIewModel::updating()
{
    return _updating;
}

void MPPlaylistTableVIewModel::set_updating(bool updating)
{
    _updating = updating;
}

QList<MPMetadata *> *MPPlaylistTableVIewModel::metadata()
{
    return this->_metadata;
}

void MPPlaylistTableVIewModel::metadata_update(QList<MPMetadata *> *metadata_ptr)
{
    if (metadata_ptr != NULL)
    {
        // finish update
        this->_updating = false;
        this->_metadata = metadata_ptr;
        emit layoutChanged();
    }
}

MPPlaylistTableVIewModel::~MPPlaylistTableVIewModel()
{
    // deallocate memory used by our playlist
    // delete this->_playlist;
}

int MPPlaylistTableVIewModel::rowCount(const QModelIndex &) const
{
    if (_playlist == NULL) return 0;
    return (_metadata != NULL ? _metadata->length() : _playlist->mediaCount());
}

int MPPlaylistTableVIewModel::columnCount(const QModelIndex &) const
{
    return 7;
}

QVariant MPPlaylistTableVIewModel::data(const QModelIndex &index, int role) const
{
    if (_playlist == NULL) return QVariant();
    if (!(index.row() >= 0 && index.row() < _metadata->length())) return QVariant();

    // get current media information
    MPMetadata *metadata = _metadata->at(index.row());

    if (role == Qt::DisplayRole)
    {
        if (_metadata != NULL)
        {
            switch (index.column())
            {
                case 0: return metadata->track_number(); break;
                case 1: return metadata->title(); break;
                case 2: return metadata->artist(); break;
                case 3: return metadata->album_artist(); break;
                case 4: return metadata->album(); break;
                case 5: return metadata->year(); break;
                case 6: return metadata->genre(); break;
            }
        }
    }

    return QVariant();
}

QVariant MPPlaylistTableVIewModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        switch (section)
        {
            case 0: return "Track #"; break;
            case 1: return "Title"; break;
            case 2: return "Artist"; break;
            case 3: return "Album Artist"; break;
            case 4: return "Album"; break;
            case 5: return "Year"; break;
            case 6: return "Genre"; break;
        }
    }

    return QVariant();
}

QMediaPlaylist *MPPlaylistTableVIewModel::playlist()
{
    return this->_playlist;
}
