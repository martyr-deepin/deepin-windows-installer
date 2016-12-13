#pragma once

#include <QLabel>

namespace DSI {
namespace Widget {

class Hint : public QLabel
{
    Q_OBJECT
public:
    explicit Hint(QWidget *parent);

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


}
}
