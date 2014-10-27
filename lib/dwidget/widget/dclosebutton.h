#ifndef DCLOSEBUTTON_H
#define DCLOSEBUTTON_H

#include <QPushButton>

class DCloseButton : public QPushButton
{
    Q_OBJECT
public:
    explicit DCloseButton(QWidget *parent = 0);

    void enableClose(bool enbale=true);
signals:

public slots:

};

#endif // DCLOSEBUTTON_H
