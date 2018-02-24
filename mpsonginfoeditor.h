#ifndef MPSONGINFOEDITOR_H
#define MPSONGINFOEDITOR_H

#include <QWidget>
#include "mpmetadata.h"
#include "mpimagewell.h"

namespace Ui {
class MPSongInfoEditor;
class MPImageWell;
}

class MPSongInfoEditor : public QWidget
{
    Q_OBJECT

public:
    explicit MPSongInfoEditor(QWidget *parent = 0);
    ~MPSongInfoEditor();

    void edit(MPMetadata *metadata);

public slots:
    void finish_editing();

private:
    Ui::MPSongInfoEditor *ui;
    MPMetadata *_metadata;

signals:
    void editing_finished();
};

#endif // MPSONGINFOEDITOR_H
