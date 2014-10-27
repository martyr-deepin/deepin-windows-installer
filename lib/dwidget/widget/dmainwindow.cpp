#include "dmainwindow.h"

#include "dconstant.h"

#include <QDebug>
#include <QPainter>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>

namespace DeepinWidget {

DMainWindow::DMainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_ShowWithoutActivating, true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    setFocusPolicy (Qt::NoFocus);
    m_ShaddowMargin = 10;
    m_Radius = 4;

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setOffset(0,0);
    shadow->setBlurRadius(m_ShaddowMargin);
    shadow->setColor(ShadowColor);
    this->setGraphicsEffect(shadow);

}

DMainWindow::~DMainWindow(){
}

void DMainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect rect = this->rect().marginsRemoved(QMargins(m_ShaddowMargin, m_ShaddowMargin, m_ShaddowMargin, m_ShaddowMargin));
    QPoint topLeft(rect.x(), rect.y());
    QPoint bottomRight(rect.x() + rect.width(), rect.y() + rect.height());
    QPainterPath border;
    border.addRoundedRect(rect, m_Radius, m_Radius);

    QLinearGradient linearGradient(topLeft, QPoint(topLeft.x(), bottomRight.y()));
    linearGradient.setColorAt(0.0, BackgroundTopColor);
    linearGradient.setColorAt(0.2, BackgroundBottonColor);
    linearGradient.setColorAt(1.0, BackgroundBottonColor);

    QPen borderPen(BorderColor);
    painter.setBrush(QBrush(linearGradient));
    painter.strokePath(border, borderPen);
    painter.fillPath(border, QBrush(linearGradient));
}

void DMainWindow::mousePressEvent(QMouseEvent *event){
    if (event->button() == Qt::LeftButton) {
        m_MousePressed = true;
        m_LastMousePos = event->globalPos() - this->pos();
    }
}
void DMainWindow::mouseMoveEvent(QMouseEvent *event){
    if (m_MousePressed) {
        move(event->globalPos() - m_LastMousePos);
    }
}

void DMainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    m_MousePressed = false;
    event->accept();
}


}
