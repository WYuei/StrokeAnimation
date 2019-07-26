#include "scalemark.h"
#include "iostream"
using namespace std;

ScaleMark::ScaleMark()
{

}

void ScaleMark::ShowText(QTextEdit *scaletext)
{
    int i=0;
    scaletext->setPlainText("|    ");
    while (i!=213) {
        scaletext->insertPlainText("|    ");
        i++;
    }

    i=0;
    scaletext->moveCursor(QTextCursor::End);
    scaletext->append(" 1                                                 ");
    while(i!=7)
    {
        QString num =QString::number((i+1)*12,10);
        scaletext->insertPlainText(num+"                                                    ");
        i++;
    }
    while(i>=7&&i!=8)
    {
        QString num =QString::number((i+1)*12,10);
        scaletext->insertPlainText(num+"                                                   ");
        i++;
    }
    while(i>=8&&i!=17)
    {
        QString num =QString::number((i+1)*12,10);
        scaletext->insertPlainText(num+"                                                  ");
        i++;
    }
}
