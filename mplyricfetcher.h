#ifndef MPLYRICFETCHTHREAD_H
#define MPLYRICFETCHTHREAD_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QFileInfo>
#include <QTextStream>
#include "mpmetadata.h"
#include "mpmetadatawriter.h"

class MPLyricFetcher : public QObject
{
    Q_OBJECT
public:
    MPLyricFetcher(QObject *parent = 0);
    ~MPLyricFetcher();

    void fetch(MPMetadata *metadata);
    bool downloading();
    void set_file_extensions(QStringList file_extensions);

private:
    QNetworkAccessManager manager;
    MPMetadata *_metadata;
    bool is_downloading;
    bool found_link;
    void download(QUrl url);

    // regular expressions
    QRegularExpression link_pattern;
    QRegularExpression href_pattern;
    QStringList file_extensions;

private slots:
    void download_finished(QNetworkReply *reply);

signals:
    void fetch_finished(QString);
};

#endif // MPLYRICFETCHTHREAD_H
