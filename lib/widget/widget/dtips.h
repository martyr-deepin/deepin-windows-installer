#ifndef DTIPS_H
#define DTIPS_H

#include <QLabel>

class DTips : public QLabel
{
    Q_OBJECT
public:
    explicit DTips(QWidget *parent);

    void paintEvent(QPaintEvent *);

signals:
    void poped();

public slots:
    void pop();
    void doPop();
    void pack();

protected slots:
    void popDone();
    void updateGeometry();


protected:
    bool    m_drawing;

    QTimer  *m_delayDrawTimer;
    QTimer  *m_posUpdateTimer;

    QWidget *m_parentDWidget;
};



#endif // DTIPS_H
