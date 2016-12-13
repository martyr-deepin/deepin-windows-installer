#ifndef FOOTWIDGET_H
#define FOOTWIDGET_H

#include <QFrame>
#include <QList>

class QPushButton;

class DFooterWidget : public QFrame
{
    Q_OBJECT
public:
    explicit DFooterWidget( QList<QPushButton*> btlist, QWidget *parent = 0);

signals:

public slots:

};

#endif // FOOTWIDGET_H
