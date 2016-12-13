#pragma once

#include <QPushButton>

class PushButton : public QPushButton
{
    Q_OBJECT
public:
    explicit PushButton(const QString& text, QWidget *parent = 0);

    void setTextColor(const QString& colorStr);
    void setImages(const QString& normal, const QString& hover, const QString& pressed);
};
