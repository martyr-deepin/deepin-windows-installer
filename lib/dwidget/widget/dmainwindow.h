#pragma once

#include <QMainWindow>

namespace DeepinWidget {

class DMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit DMainWindow(QWidget *parent = 0);
    ~DMainWindow();

    virtual void paintEvent(QPaintEvent *);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);

private:
    int     m_ShaddowMargin;
    int     m_Radius;
    bool    m_MousePressed;
    QPoint  m_LastMousePos;

    static DMainWindow *s_window;
};

}
