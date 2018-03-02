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
#include <QXmlStreamReader>
#include <QTextStream>
#include <QFileInfo>
#include "mpmetadata.h"
#include "mpmetadatawriter.h"

class MPMetadataAnalysisThread : public QThread
{
    Q_OBJECT
public:
    MPMetadataAnalysisThread(QObject *parent = 0);
    void dealloc();

    void begin(QMediaPlayer *player, bool full_analysis=false);
    void set_metadata_list(QList<MPMetadata *> *metadata);

private:
    QList<MPMetadata *> *_metadata;
    QMediaPlaylist *playlist;
    QMediaPlayer *_player;
    QImage load_image(QString url);
    int analysis_index;
    bool _initialised;
    bool _started;
    bool full_analysis;

public slots:
    void metadata_update(bool);

protected:
    void run() override;

signals:
    void analysis_complete(QList<MPMetadata *> *);
};

#endif // MPMETADATAANALYSISTHREAD_H
