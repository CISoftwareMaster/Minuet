#ifndef MPMETADATAWRITER_H
#define MPMETADATAWRITER_H

#include <QObject>
#include <QFile>
#include "mpmetadata.h"
#include <QXmlStreamWriter>

class MPMetadataWriter : public QObject
{
    Q_OBJECT
public:
    explicit MPMetadataWriter(QObject *parent = nullptr);
    void write_metadata(QFile *target, MPMetadata *metadata);
};

#endif // MPMETADATAWRITER_H
