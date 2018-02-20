#ifndef MPMETADATAANALYSISTHREAD_H
#define MPMETADATAANALYSISTHREAD_H

#include <QObject>
#include <QThread>
#include <QFile>
#include <QImage>
#include <QImageReader>
#include <QPixmap>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QMediaMetaData>
#include "mpmetadata.h"

class MPMetadataAnalysisThread : public QThread
{
    Q_OBJECT
public:
    MPMetadataAnalysisThread(QObject *parent = 0);
    void dealloc();

    void begin(QMediaPlayer *player);

private:
    QList<MPMetadata *> *_metadata;
    QMediaPlayer *_player;
    QImage load_image(QString url);
    int current_index;
    int analysis_index;
    bool _initialised;
    bool _started;

    int _player_state;
    qint64 _player_position;

public slots:
    void metadata_update(bool);

protected:
    void run() override;

signals:
    void analysis_complete(QList<MPMetadata *> *);
};

#endif // MPMETADATAANALYSISTHREAD_H
