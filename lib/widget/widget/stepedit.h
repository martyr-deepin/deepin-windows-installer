#pragma once

#include <QLabel>

class QLineEdit;

namespace DSI {
namespace Widget {

class StepEdit : public QLabel
{
    Q_OBJECT
public:
    explicit StepEdit(QWidget *parent = 0);

    Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged USER true)

    int value();

signals:
    void valueChanged(int);

public slots:
    void setMin(int);
    void setMax(int);

    void setValue(int);

    void add();
    void sub();

private slots:
    void textValueChanged(QString text);
    void editfinished();

private:
    QLineEdit *m_LineEdit;
    int m_Value;
    int m_Max;
    int m_Min;
};

}
}
