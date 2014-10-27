#ifndef FOOTWIDGET_H
#define FOOTWIDGET_H

#include <QWidget>
#include <QList>

class DPushButton;

class DFooterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DFooterWidget(const QList<DPushButton*> &btlist, QWidget *parent = 0);

signals:

public slots:

};

#endif // FOOTWIDGET_H
