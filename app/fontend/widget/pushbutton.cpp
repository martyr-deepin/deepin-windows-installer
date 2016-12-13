#include "pushbutton.h"

#include "hint.h"

static QString sStyleTemplate = "PushButton{"
        "color: %1;"
        "font-size: 14px;"
        "border-image:url(:/images/transparent_button_normal.png) 3 6 3 6;"
        "border-top: 3px transparent;"
        "border-bottom: 3px transparent;"
        "border-right: 6px transparent;"
        "border-left: 6px transparent;"
    "}"
    "PushButton:hover{"
        "border-image:url(:/images/transparent_button_hover.png) 3 6 3 6;"
        "border-top: 3px transparent;"
        "border-bottom: 3px transparent;"
        "border-right: 6px transparent;"
        "border-left: 6px transparent;"
    "}"
    "PushButton:pressed{"
        "border-image:url(:/images/transparent_button_press.png) 3 6 3 6;"
        "border-top: 3px transparent;"
        "border-bottom: 3px transparent;"
        "border-right: 6px transparent;"
        "border-left: 6px transparent;"
    "}";

PushButton::PushButton(const QString& text, QWidget *parent) :
    QPushButton(text, parent)
{
    QString style = sStyleTemplate.arg("#b4b4b4");
    setFocusPolicy(Qt::StrongFocus);
    setStyleSheet(style);
}

void PushButton::setTextColor(const QString &colorStr){
    QString style = sStyleTemplate.arg(colorStr);
    setStyleSheet(style);
}

void PushButton::setImages(const QString &normal, const QString &hover, const QString &pressed){
    QString style = "PushButton{"
        "color: #b4b4b4;"
        "font-size: 14px;"
        "border-image:url(%1) 3 6 3 6;"
        "border-top: 3px transparent;"
        "border-bottom: 3px transparent;"
        "border-right: 6px transparent;"
        "border-left: 6px transparent;"
    "}"
    "PushButton:hover{"
        "border-image:url(%2) 3 6 3 6;"
        "border-top: 3px transparent;"
        "border-bottom: 3px transparent;"
        "border-right: 6px transparent;"
        "border-left: 6px transparent;"
    "}"
    "PushButton:pressed{"
        "border-image:url(%3) 3 6 3 6;"
        "border-top: 3px transparent;"
        "border-bottom: 3px transparent;"
        "border-right: 6px transparent;"
        "border-left: 6px transparent;"
    "}";
    style = style.arg(normal).arg(hover).arg(pressed);
    setStyleSheet(style);
}
