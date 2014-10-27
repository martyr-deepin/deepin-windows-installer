#ifndef DCOMBOBOX_H
#define DCOMBOBOX_H

#include <QComboBox>
#include "dwidget.h"

class DComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit DComboBox(QWidget *parent = 0);
};

#endif // DCOMBOBOX_H
