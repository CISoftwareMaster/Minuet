#include "mpmetadata.h"

MPMetadata::MPMetadata(QObject *parent)
    : QObject(parent)
{
    // initialise our variables with
    _track_number = "";
    _duration = "";
    _title = "";
    _artist = "";
    _album = "";
    _album_artist = "";
    _year = "";
    _genre = "";
    _lyrics = "";

    // default cover
    _image = QPixmap::fromImage(QImage(":/icons/default_cover.png"));
}

QString MPMetadata::track_number()
{
    return _track_number;
}

QString MPMetadata::duration()
{
    return _duration;
}

QString MPMetadata::title()
{
    return _title;
}

QString MPMetadata::artist()
{
    return _artist;
}

QString MPMetadata::album()
{
    return _album;
}

QString MPMetadata::album_artist()
{
    return _album_artist;
}

QString MPMetadata::year()
{
    return _year;
}

QString MPMetadata::genre()
{
    return _genre;
}

QString MPMetadata::lyrics()
{
    return _lyrics;
}

QPixmap MPMetadata::image()
{
    return _image;
}

void MPMetadata::set_track_number(QString track_number)
{
    _track_number = track_number;
}

void MPMetadata::set_duration(QString duration)
{
    _duration = duration;
}

void MPMetadata::set_title(QString title)
{
    _title = title;
}

void MPMetadata::set_artist(QString artist)
{
    _artist = artist;
}

void MPMetadata::set_album(QString album)
{
    _album = album;
}

void MPMetadata::set_album_artist(QString album_artist)
{
    _album_artist = album_artist;
}

void MPMetadata::set_year(QString year)
{
    _year = year;
}

void MPMetadata::set_genre(QString genre)
{
    _genre = genre;
}

void MPMetadata::set_lyrics(QString lyrics)
{
    _lyrics = lyrics;
}

void MPMetadata::set_image(QPixmap image)
{
    _image = image;
}
