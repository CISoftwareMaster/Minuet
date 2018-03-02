#include "mpmetadatawriter.h"

MPMetadataWriter::MPMetadataWriter(QObject *parent)
    : QObject(parent)
{
}

void MPMetadataWriter::write_metadata(QFile *target, MPMetadata *metadata)
{
    if (target != NULL)
    {
        QXmlStreamWriter writer(target);

        // write header
        writer.setAutoFormatting(true);
        writer.writeStartDocument("1.0");

        writer.writeStartElement("metadata");

        // write metadata
        QList<MPMetadataPair> pairs = metadata->get_metadata_pairs();

        for (int i = 0, l = pairs.length(); i < l; ++i)
        {
            MPMetadataPair pair = pairs.at(i);
            writer.writeTextElement(pair.key(), pair.value());
        }

        // end of writing
        writer.writeEndElement();
        writer.writeEndDocument();
    }
}
