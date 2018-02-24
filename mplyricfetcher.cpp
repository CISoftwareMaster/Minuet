#include "mplyricfetcher.h"

MPLyricFetcher::MPLyricFetcher(QObject *parent)
    : QObject(parent)
{
    is_downloading = false;
    found_link = false;

    // compile regular expression pattern(s)
    link_pattern = QRegularExpression("\\<a.*href.*.com/.*/.*/.*\\.html.*\\>");
    href_pattern = QRegularExpression("\".*//.*\\.com.*\"");

    // handle downloads
    connect(&manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(download_finished(QNetworkReply*)));
}

void MPLyricFetcher::fetch(MPMetadata *metadata)
{
    // only allow one download at a time
    if (!this->is_downloading)
    {
        // set target metadata
        this->_metadata = metadata;

        // prepare arguments
        QString title = metadata->title();
        title.remove(QRegularExpression("[^A-Za-z0-9\\ ]"));

        // remove file extensions from the title
        foreach (QString extension, file_extensions)
            title.remove(extension);

        QString artist = metadata->artist();
        artist.remove(QRegularExpression("[^A-Za-z0-9\\ ]"));

        // prepare whitespace(s)
        title = title.replace(" ", "+");
        artist = artist.replace(" ", "+");

        QString url_body = "https://search.azlyrics.com/search.php?q="
                + title + "+" + artist;

        // download URL
        QUrl target_url(url_body);

        // set downloading flag
        this->is_downloading = true;

        // send download request
        this->download(target_url);
    }
}

void MPLyricFetcher::set_file_extensions(QStringList file_extensions)
{
    this->file_extensions = file_extensions;
}

bool MPLyricFetcher::downloading()
{
    return is_downloading;
}

void MPLyricFetcher::download_finished(QNetworkReply *reply)
{
    if (reply == NULL)
    {
        // reset everything
        found_link = false;
        is_downloading = false;
        emit fetch_finished("Lyric fetching error!");
    }

    if (reply->error())
    {
        // reset found link
        found_link = false;
        is_downloading = false;
        emit fetch_finished("Lyric fetching error!\n" + reply->errorString());
    }
    else
    {
        if (!found_link)
        {
            // set the found link flag to true
            found_link = true;

            // find the first link
            QString document = QString(reply->readAll());
            QRegularExpressionMatch match = link_pattern.match(document);

            // delete the reply later
            reply->deleteLater();

            if (match.capturedTexts().length() > 0)
            {
                QString link = match.capturedTexts().first();

                // find its reference (href)
                QRegularExpressionMatch lmatch = href_pattern.match(link);

                if (lmatch.capturedTexts().length() > 0)
                {
                    QStringList href = lmatch.capturedTexts().first().split("\"");

                    if (href.length() > 1)
                    {
                        // download the lyric document
                        QUrl lyric_url = QUrl(href.at(1));
                        download(lyric_url);
                        return;
                    }
                }
            }

            // fetch error
            is_downloading = false;
            found_link = false;
            emit fetch_finished("Lyric fetching error!");
        }
        else
        {
            // get div elements
            QStringList dividers = QString(reply->readAll()).split("<div>");
            bool found_lyricbody = false;

            foreach (QString str, dividers)
            {
                if (str.contains(". Sorry about that. -->"))
                {
                    int terminate_at = str.indexOf("<br><br>");
                    QString lyrics = str.left(terminate_at);

                    if (!lyrics.isEmpty())
                    {
                        _metadata->set_lyrics(lyrics);
                        found_lyricbody = true;

                        // send finished signal
                        emit fetch_finished(lyrics);

                        // end of download
                        is_downloading = false;
                        break;
                    }
                }
            }

            // lyric fetch error
            if (!found_lyricbody)
            {
                emit fetch_finished("Lyric fetching error");
                is_downloading = false;
            }

            // reset found link
            found_link = false;
        }
    }

    // delete the request later
    reply->deleteLater();
}

void MPLyricFetcher::download(QUrl url)
{
    QNetworkRequest request(url);

    // set user agent
    request.setRawHeader("User-Agent", "Minuet Lyric Fetcher 1.0");

    // execute our request
    manager.get(request);
}

MPLyricFetcher::~MPLyricFetcher()
{
}
