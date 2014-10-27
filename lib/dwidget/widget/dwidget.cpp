#include "dwidget.h"
#include <QDebug>

DWidget::DWidget(QWidget *parent) :
    QWidget(parent)
{
}

void DWidget::enterEvent( QEvent* e ) {
    emit entered();
    QWidget::enterEvent(e);
}

void DWidget::leaveEvent(QEvent *e){
    emit leaved();
    QWidget::leaveEvent(e);
}

//void DWidget::moveEvent(QMoveEvent *e){
//    emit moved();
//    QWidget::moveEvent(e);
//}

//void DWidget::focusInEvent(QFocusEvent *e) {
//    qDebug()<<"focusInEvent";
//    QWidget::focusInEvent(e);
//}

//void DWidget::focusOutEvent(QFocusEvent *e){
//    qDebug()<<"focusOutEvent";
//    QWidget::focusOutEvent(e);
//}

//void DWidget::resizeEvent(QResizeEvent *e){
//    qDebug()<<"resizeEvent";
//    QWidget::resizeEvent(e);
//}

//void DWidget::closeEvent(QCloseEvent *e){
//    qDebug()<<"closeEvent";
//    QWidget::closeEvent(e);
//}


//void DWidget::dragMoveEvent(QDragMoveEvent *e) {
//    qDebug()<<"dragMoveEvent";
//    QWidget::dragMoveEvent(e);
//}
