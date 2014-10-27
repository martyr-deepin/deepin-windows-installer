#ifndef DWIDGET_H
#define DWIDGET_H

#include <QWidget>

class DWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DWidget(QWidget *parent = 0);

signals:
    void entered();
    void leaved();
    void moved();

public slots:

protected:
    virtual void enterEvent( QEvent* e );
    virtual void leaveEvent(QEvent * e);

//    virtual void moveEvent(QMoveEvent *);
//    virtual void keyPressEvent(QKeyEvent *);
//    virtual void keyReleaseEvent(QKeyEvent *);
//    virtual void focusInEvent(QFocusEvent *);
//    virtual void focusOutEvent(QFocusEvent *);
//    virtual void paintEvent(QPaintEvent *);
//    virtual void resizeEvent(QResizeEvent *);
//    virtual void closeEvent(QCloseEvent *);
//    virtual void dragMoveEvent(QDragMoveEvent *);

};

#endif // DWIDGET_H
