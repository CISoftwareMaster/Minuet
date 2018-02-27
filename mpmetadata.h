#ifndef MPMETADATA_H
#define MPMETADATA_H

#include <QObject>
#include <QImage>
#include <QPixmap>

class MPMetadata : public QObject
{
    Q_OBJECT
public:
    explicit MPMetadata(QObject *parent = nullptr);

    QString track_number();
    QString duration();
    QString title();
    QString artist();
    QString album();
    QString album_artist();
    QString year();
    QString genre();
    QString lyrics();
    QString filename();
    QPixmap image();

    void set_track_number(QString track_number);
    void set_duration(QString duration);
    void set_title(QString title);
    void set_artist(QString artist);
    void set_album(QString album);
    void set_album_artist(QString album_artist);
    void set_year(QString year);
    void set_genre(QString genre);
    void set_lyrics(QString lyrics);
    void set_filename(QString filename);
    void set_image(QPixmap image);
private:
    QString _duration;
    QString _track_number;
    QString _title;
    QString _artist;
    QString _album;
    QString _album_artist;
    QString _year;
    QString _genre;
    QString _lyrics;
    QString _filename;
    QPixmap _image;
};

#endif // MPMETADATA_H
