#ifndef MPBUTTON_H
#define MPBUTTON_H

#include <QObject>
#include <QWidget>
#include <QPushButton>
#include <QMouseEvent>
#include <QPainter>

enum MPButtonType
{
    MPButtonNormal,
    MPButtonDefault,
    MPButtonTranslucent
};

class MPButton : public QPushButton
{
    Q_OBJECT
public:
    MPButton(QWidget *parent = nullptr);
    ~MPButton();
    MPButtonType btn_type;

    void paintEvent(QPaintEvent *) override;
};

// other button types
class MPDefaultButton : public MPButton
{
    Q_OBJECT
public:
    MPDefaultButton(QWidget *parent) : MPButton(parent) { btn_type = MPButtonDefault; }
};

class MPTranslucentButton : public MPButton
{
    Q_OBJECT
public:
    MPTranslucentButton(QWidget *parent) : MPButton(parent) { btn_type = MPButtonTranslucent; }
};

#endif // MPBUTTON_H
