#include "stepedit.h"

#include "../../app/fontend/widget/pushbutton.h"

#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QSizePolicy>
#include <QDebug>

namespace DSI {
namespace Widget {

static QString s_DStepEditStyle =
"#UIStepEdit {"
"margin: 0px;"
"border: 1px solid rgba(0, 0, 0, 255);"
"border-bottom: 2px solid qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,"
"stop:0 rgba(0, 0, 0, 128),"
"stop:0.66 rgba(0, 0, 0, 128),"
"stop:0.67 rgba(48, 49, 50, 128*0.05),"
"stop:1 rgba(48, 49, 50, 128*0.05));"
"border-radius: 4px;"
"border-bottom-left-radius: 5px;"
"border-bottom-right-radius: 5px;"
"background: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,"
"stop:0 rgb(24, 24, 24),"
"stop:1 rgb(32, 32, 32));"
"}";

static QString s_QLineEditStyle =
"QLineEdit {"
"font-size: 12px;"
"margin: 0px;"
"color: grey;"
"border: 0px solid rgba(0, 0, 0, 255);"
"border-radius: 4px;"
"background: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,"
"stop:0 rgb(24, 24, 24),"
"stop:1 rgb(32, 32, 32));"
"selection-background-color: darkgray;"
"}";

static QString s_LabelStyle =
"QLabel {"
"font-size: 12px;"
"margin: 0px;"
"color: grey;"
"border: 0px solid rgba(0, 0, 0, 255);"
"border-radius: 0px;"
"background: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,"
"stop:0 rgb(24, 24, 24),"
"stop:1 rgb(32, 32, 32));"
"}";


StepEdit::StepEdit(QWidget *parent) :
    QLabel(parent)
{
    setObjectName("UIStepEdit");
    m_Value = 0;

    m_LineEdit = new QLineEdit;
    m_LineEdit->setStyleSheet(s_QLineEditStyle);
    m_LineEdit->setAlignment(Qt::AlignRight);
    m_LineEdit->setMaxLength(3);
    m_LineEdit->setEnabled(false);
   // m_LineEdit->setInputMask("999");
    m_LineEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    m_LineEdit->setFixedWidth(28);
    //connect(m_LineEdit, SIGNAL(textChanged(QString)), this, SLOT(textValueChanged(QString)));
    connect(m_LineEdit, SIGNAL(editingFinished()), this, SLOT(editfinished()));

    QLabel *m_Util = new QLabel;
    m_Util->setStyleSheet(s_LabelStyle);
    m_Util->setText("G");
    m_Util->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    PushButton *m_Add = new PushButton("");
    m_Add->setImages (":/images/stepedit/add-normal.png",
                      ":/images/stepedit/add-hover.png",
                      ":/images/stepedit/add-press.png");
    m_Add->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    m_Add->setFixedSize (41, 34);
//    m_Add->setText("+");
    connect(m_Add, SIGNAL(clicked()), this, SLOT(add()));

    PushButton *m_Sub = new PushButton("");
    m_Sub->setImages (":/images/stepedit/sub-normal.png",
                      ":/images/stepedit/sub-hover.png",
                      ":/images/stepedit/sub-press.png");
    m_Sub->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_Sub->setFixedSize (41, 34);
//    m_Sub->setText("-");
    connect(m_Sub, SIGNAL(clicked()), this, SLOT(sub()));

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);

    layout->addWidget(m_LineEdit);
    layout->addWidget(m_Util);
    layout->addWidget(m_Add);
    layout->setAlignment (m_Add, Qt::AlignVCenter);
    layout->addWidget(m_Sub);
    this->setLayout(layout);
    this->setStyleSheet(s_DStepEditStyle);
}

void StepEdit::add() {
    int step = (m_Max - m_Min) / 10;
    step = (step > 0) ? step:1;
    setValue(m_Value+step);
}

void StepEdit::sub() {
    int step = (m_Max - m_Min) / 10;
    step = (step > 0) ? step:1;
    setValue(m_Value-step);
}

int StepEdit::value() {
    return m_Value;
}

void StepEdit::setValue(int v){
    if (v > m_Max) {
        v = m_Max;
    }
    if (v < m_Min) {
        v = m_Min;
    }

    m_Value = v;
    m_LineEdit->setText(QString("%1").arg(m_Value));
    emit valueChanged(m_Value);
}

void StepEdit::textValueChanged(QString text) {
//    if (text.isEmpty()) {
//        setValue(m_Min);
//        return;
//    }
//    setValue(text.toInt());
}

void StepEdit::editfinished(){
    setValue(m_LineEdit->text().toInt());
}

void StepEdit::setMax(int max) {
    m_Max = max;
    setValue(m_Value);
}

void StepEdit::setMin(int min) {
    m_Min = min;
    setValue(m_Value);
}

}
}
