#ifndef DPUSHBUTTON_H
#define DPUSHBUTTON_H

#include <QPushButton>

class DPushButton : public QPushButton
{
    Q_OBJECT
public:
    explicit DPushButton(const QString& text, QWidget *parent = 0);

    void setTextColor(const QString& colorStr);
    void setImages(const QString& normal, const QString& hover, const QString& pressed);
};

#endif // DPUSHBUTTON_H
