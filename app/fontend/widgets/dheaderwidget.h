#ifndef DHEADERWIDGET_H
#define DHEADERWIDGET_H

#include <QWidget>

class DCloseButton;

class DHeaderWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DHeaderWidget(QWidget *parent = 0);

    void enableClose(bool enable=true);
signals:
    void closeClicked();
public slots:

private:
    DCloseButton * m_CloseButton;
};

#endif // DHEADERWIDGET_H
