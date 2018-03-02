#include "mpsonginfoeditor.h"
#include "ui_mpsonginfoeditor.h"

MPSongInfoEditor::MPSongInfoEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MPSongInfoEditor)
{
    ui->setupUi(this);

    // handle connection(s)
    connect(ui->doneBtn, SIGNAL(clicked(bool)),
            this, SLOT(finish_editing()));

    // point to null by default
    this->_metadata = NULL;
}

void MPSongInfoEditor::edit(MPMetadata *metadata)
{
    // start editing
    _metadata = metadata;

    // populate our fields
    ui->songTitle->setText(metadata->title());
    ui->artistName->setText(metadata->artist());
    ui->album->setText(metadata->album());
    ui->genre->setText(metadata->genre());
    ui->albumArtist->setText(metadata->album_artist());
    ui->year->setText(metadata->year());
    ui->albumArt->set_image(metadata->image());
    ui->lyrics->setText(metadata->lyrics());

    this->show();
}

void MPSongInfoEditor::finish_editing()
{
    // update metadata
    _metadata->set_title(ui->songTitle->text());
    _metadata->set_artist(ui->artistName->text());
    _metadata->set_album(ui->album->text());
    _metadata->set_genre(ui->genre->text());
    _metadata->set_year(ui->year->text());
    _metadata->set_album_artist(ui->albumArtist->text());
    _metadata->set_lyrics(ui->lyrics->toPlainText());
    _metadata->set_image(ui->albumArt->image());

    // write metadata
    QFile metadata_file(_metadata->filename().append(".metadata"));

    if (metadata_file.open(QIODevice::WriteOnly))
    {
        MPMetadataWriter writer;
        writer.write_metadata(&metadata_file, _metadata);
        metadata_file.close();
    }

    // send editing finished signal
    emit editing_finished();

    // close this window
    this->hide();
}

MPSongInfoEditor::~MPSongInfoEditor()
{
    delete ui;
}
