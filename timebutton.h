#ifndef TIMEBUTTON_H
#define TIMEBUTTON_H

#include <QObject>
#include <QFrame>
#include <QMouseEvent>

#include "keybutton.h"

class TimeButton
{
public:
    TimeButton();

    void CreateButton(QFrame *qframe);

    KeyButton *newbutton[220];

};

#endif // TIMEBUTTON_H
