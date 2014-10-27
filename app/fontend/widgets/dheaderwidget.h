#ifndef DHEADERWIDGET_H
#define DHEADERWIDGET_H

#include <QWidget>

class DHeaderWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DHeaderWidget(QWidget *parent = 0);

signals:
    void closeClicked();
public slots:

};

#endif // DHEADERWIDGET_H
