#include "mpmetadataanalysisthread.h"

MPMetadataAnalysisThread::MPMetadataAnalysisThread(QObject *parent)
    : QThread(parent)
{
    // is this thread initialised?
    _initialised = false;
    _started = false;

    // index for analysis
    analysis_index = 0;

    // point to null by default
    _metadata = NULL;

    playlist = new QMediaPlaylist;
    _player = new QMediaPlayer;
}

void MPMetadataAnalysisThread::set_metadata_list(QList<MPMetadata *> *metadata)
{
    _metadata = metadata;
}

void MPMetadataAnalysisThread::dealloc()
{
    this->quit();

    // deallocate temp player
    delete _player;
    delete playlist;
}

void MPMetadataAnalysisThread::begin(QMediaPlayer *player)
{
    // initialise
    if (!_initialised)
    {
        _player->setVolume(0);
        _player->setAudioRole(QAudio::AccessibilityRole);
        _player->setPlaylist(playlist);

        // handle analytics
        connect(_player, SIGNAL(metaDataAvailableChanged(bool)),
                this, SLOT(metadata_update(bool)));

        _initialised = true;
    }

    // clear playlist
    playlist->clear();

    // copy playlist
    for (int i = 0, l = player->playlist()->mediaCount(); i < l; ++i)
        playlist->addMedia(player->playlist()->media(i));

    playlist->setCurrentIndex(0);

    // stop this thread (if it's already running)
    if (this->isRunning())
        this->quit();

    // start this thread
    this->start(QThread::HighPriority);
    _started = true;
}

QImage MPMetadataAnalysisThread::load_image(QString url)
{
    if (!url.isEmpty())
    {
        QImageReader reader(url);
        reader.setAutoTransform(true);

        return QImage(reader.read());
    }

    return QImage();
}

void MPMetadataAnalysisThread::metadata_update(bool available)
{
    if (available && _started && (analysis_index == _metadata->length() + 1))
    {
        QFileInfo info(_player->currentMedia().canonicalUrl().toString());
        // get the base URL
        QString base_url = info.path().replace("file://", "");

        // get metadata
        MPMetadata *item = new MPMetadata;
        item->set_track_number(_player->metaData(QMediaMetaData::TrackNumber).toString());
        item->set_title(_player->metaData(QMediaMetaData::Title).toString());
        item->set_artist(_player->metaData(QMediaMetaData::ContributingArtist).toString());
        item->set_album(_player->metaData(QMediaMetaData::AlbumTitle).toString());
        item->set_album_artist(_player->metaData(QMediaMetaData::AlbumArtist).toString());
        item->set_year(_player->metaData(QMediaMetaData::Year).toString());
        item->set_genre(_player->metaData(QMediaMetaData::Genre).toString());
        item->set_filename(base_url + "/" + info.baseName());

        // try to load its lyrics
        QString lyrics = _player->metaData(QMediaMetaData::Lyrics).toString();

        if (lyrics.isEmpty())
        {
            // load its lyric file (if its available)
            QString title = item->filename() + ".lyrics";
            QFile lyric_file(title);

            if (lyric_file.open(QIODevice::ReadOnly))
            {
                // read its content
                QTextStream stream(&lyric_file);
                item->set_lyrics(stream.readAll());

                // end of reading
                lyric_file.close();
            }
        }

        // if the title string is empty, use its filename instead
        if (item->title().isEmpty())
            item->set_title(info.baseName());

        // try to load the cover art
        QPixmap cover_art = QPixmap(_player->metaData(QMediaMetaData::CoverArtImage).toByteArray());

        if (!cover_art.isNull())
        {
            item->set_image(cover_art);
        }
        else
        {
            QUrl c_art_url1 = _player->metaData(QMediaMetaData::CoverArtUrlLarge).toUrl();
            QUrl c_art_url2 = _player->metaData(QMediaMetaData::CoverArtUrlSmall).toUrl();

            // try the first URL
            QImage cover_art_image = load_image(c_art_url1.toString());

            // try the other URL
            if (cover_art_image.isNull())
                cover_art_image = load_image(c_art_url2.toString());

            if (cover_art_image.isNull())
            {
                // try to find "folder" or "cover" images

                QStringList c_art_names = QStringList({"cover.jpg", "cover.jpeg",
                        "cover.png", "album.jpg", "album.jpeg", "album.png",
                        "folder.jpg", "folder.jpeg", "folder.png", "Cover.jpg",
                        "Cover.jpeg", "Cover.png", "Album.jpg", "Album.jpeg", "Album.png",
                        "Folder.jpg", "Folder.jpeg", "Folder.png"
                });

                foreach (QString name, c_art_names)
                {
                    cover_art_image = load_image(base_url + "/" + name);
                    if (!cover_art_image.isNull())
                        break;
                }
            }

            // if either URL gave a valid image, assign it
            if (!cover_art_image.isNull())
                item->set_image(QPixmap::fromImage(cover_art_image));
        }

        _metadata->append(item);

        // move to the next item
        _player->playlist()->setCurrentIndex(analysis_index);
        _player->play();

        if (analysis_index == _player->playlist()->mediaCount())
        {
            // send analysis data
            emit analysis_complete(_metadata);

            _player->stop();
            _started = false;

            // stop thread
            this->quit();
        }

        // increment analysis index
        analysis_index ++;
    }
}

void MPMetadataAnalysisThread::run()
{
    // update index
    analysis_index = _metadata->length() + 1;
    _player->playlist()->setCurrentIndex(analysis_index - 1);

    // begin analysis
    _player->play();
}
