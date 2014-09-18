#pragma once

#include <QMainWindow>

namespace DWidget {

class DMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit DMainWindow(QWidget *parent = 0);
    ~DMainWindow();

    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    int     m_ShaddowMargin;
    int     m_Radius;
    bool    m_MousePressed;
    QPoint  m_LastMousePos;
};

}
