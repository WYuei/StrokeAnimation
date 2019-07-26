#include "timebutton.h"
#include "keybutton.h"

TimeButton::TimeButton()
{
}

void TimeButton::CreateButton(QFrame *qframe)
{
    int i=0;
    while(i!=214)
    {
        newbutton[i]=new KeyButton(i);
        newbutton[i]->button=new QRadioButton(qframe);
        newbutton[i]->button->setGeometry(5+i*14,1,14,34);
        newbutton[i]->button->setCheckable(true);
        //newbutton[i]->button->setAutoRaise(true);
        newbutton[i]->button->setStyleSheet("border:1px solid rgb(172, 255, 127)");
        i++;
    }
}
