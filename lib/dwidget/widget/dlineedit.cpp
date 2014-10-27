#include "dlineedit.h"

#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QSizePolicy>

static QString s_LabelStyle =
"QLabel {"
"margin: 0px;"
"border: 1px solid rgba(0, 0, 0, 255);"
"border-right: 0px;"
"border-bottom: 2px solid qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,"
    "stop:0 rgba(0, 0, 0, 128),"
    "stop:0.66 rgba(0, 0, 0, 128),"
    "stop:0.67 rgba(48, 49, 50, 128*0.05),"
    "stop:1 rgba(48, 49, 50, 128*0.05));"
"border-radius: 4px;"
"border-top-right-radius: 0px;"
"border-bottom-right-radius: 0px;"
"border-bottom-left-radius: 5px;"
"background: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,"
"stop:0 rgb(24, 24, 24),"
"stop:0.89 rgb(32, 32, 32),"
"stop:1 rgb(0, 0, 0));"
"selection-background-color: darkgray;"
"}";

static QString s_LineEditStyle =
"QLineEdit {"
"font-size: 12px;"
"color: grey;"
"margin: 0px;"
"border: 1px solid rgba(0, 0, 0, 255);"
"border-left: 0px;"
"border-bottom: 2px solid qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,"
    "stop:0 rgba(0, 0, 0, 128),"
    "stop:0.66 rgba(0, 0, 0, 128),"
    "stop:0.67 rgba(48, 49, 50, 128*0.05),"
    "stop:1 rgba(48, 49, 50, 128*0.05));"
"border-radius: 4px;"
"border-top-left-radius: 0px;"
"border-bottom-left-radius: 0px;"
"border-bottom-right-radius: 5px;"
"background: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,"
"stop:0 rgb(24, 24, 24),"
"stop:0.93 rgb(32, 32, 32),"
"stop:1 rgb(0, 0, 0));"
"selection-background-color: darkgray;"
"}";


DLineEdit::DLineEdit(const QString& qurl, const QString& defaultText, QWidget *parent) :
    DWidget(parent)
{
    m_Label = new QLabel;
    m_Label->setMargin(2);
    m_Label->setPixmap(QPixmap(qurl).scaled(12, 12));
    m_Label->setFixedWidth(20);
    m_Label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    m_Label->setStyleSheet(s_LabelStyle);

    m_LineEdit = new QLineEdit;
    m_LineEdit->setStyleSheet(s_LineEditStyle);
    m_LineEdit->setPlaceholderText(defaultText);
    m_LineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);

    layout->addWidget(m_Label);
    layout->addWidget(m_LineEdit);
    this->setLayout(layout);
    connect(m_LineEdit, SIGNAL(textChanged(QString)), this, SIGNAL(textChanged(QString)));
}

QString DLineEdit::text() const {
    return m_LineEdit->text();
}

void DLineEdit::setText(const QString &text){
    m_LineEdit->setText(text);
}

QLineEdit::EchoMode DLineEdit::echoMode() const {
    return m_LineEdit->echoMode();
}

void DLineEdit::setEchoMode(QLineEdit::EchoMode mode) {
    return m_LineEdit->setEchoMode(mode);
}
