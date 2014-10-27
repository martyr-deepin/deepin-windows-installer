#include "dclosebutton.h"

QString EnableStyle = "DCloseButton{"
    "background:url(:/images/window_close_normal.png);"
    "border:0px;"
"}"
"DCloseButton:hover{"
    "background:url(:/images/window_close_hover.png);"
    "border:0px;"
"}"
"DCloseButton:pressed{"
    "background:url(:/images/window_close_press.png);"
    "border:0px;"
"}";

QString DisableStyle = "DCloseButton{"
    "background:url();"
    "border:0px;"
"}"
"DCloseButton:hover{"
    "background:url();"
    "border:0px;"
"}"
"DCloseButton:pressed{"
    "background:url();"
    "border:0px;"
"}";

DCloseButton::DCloseButton(QWidget *parent) :
    QPushButton(parent)
{
    QPixmap pixmap (":/images/window_close_normal.png");
    this->setFixedSize(pixmap.size());

    this->setStyleSheet(EnableStyle);

    setFocusPolicy(Qt::NoFocus);
}

void DCloseButton::enableClose(bool enbale) {
    if (enbale) {
        this->setStyleSheet(EnableStyle);
    } else {
        this->setStyleSheet(DisableStyle);
    }
}
