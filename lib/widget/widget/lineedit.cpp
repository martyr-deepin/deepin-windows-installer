#include "lineedit.h"

#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QSizePolicy>
#include <QDebug>

namespace DSI {
namespace Widget {

void SimpleLineEdit::focusInEvent(QFocusEvent *e) {
   emit focusIn (e);
   QLineEdit::focusInEvent (e);
}

static QString s_LabelStyle =
"QLabel {"
"font-size: 12px;"
"color: grey;"
"margin: 0px;"
"border: 0px solid rgba(0, 0, 0, 255);"
"border-radius: 4px;"
"background: transparent;"
"}";

static QString s_QLineEditStyle =
"QLineEdit {"
"font-size: 12px;"
"color: grey;"
"margin: 0px;"
"border: 0px solid rgba(0, 0, 0, 255);"
"border-radius: 4px;"
"background: transparent;"
"selection-background-color: darkgray;"
"}";

QString s_LineEditStyle =
        "#UILineEdit {"
        "border-radius: 4px;"
        "border: 1px solid rgba(0, 0, 0, 255);"
        "background: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,"
            "stop:0 rgb(24, 24, 24),"
            "stop:0.97 rgb(32, 32, 32),"
            "stop:1 rgb(0, 0, 0));"
        "}";

LineEdit::LineEdit(const QString& qurl, const QString& defaultText, QWidget *parent) :
    QFrame(parent)
{
    setObjectName("UILineEdit");
    m_Label = new QLabel;
    m_Label->setMargin(2);
    m_Label->setPixmap(QPixmap(qurl).scaled(12, 12));
    m_Label->setFixedWidth(20);
    m_Label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    m_Label->setStyleSheet(s_LabelStyle);

    m_LineEdit = new SimpleLineEdit;
    m_LineEdit->setStyleSheet(s_QLineEditStyle);
    m_LineEdit->setPlaceholderText(defaultText);
    m_LineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);

    layout->addWidget(m_Label);
    layout->addWidget(m_LineEdit);
    this->setLayout(layout);

    setStyleSheet(s_LineEditStyle);
    connect(m_LineEdit, SIGNAL(textChanged(QString)),
            this, SIGNAL(textChanged(QString)));
    connect(m_LineEdit, SIGNAL(textEdited(QString)),
            this, SIGNAL(textEdited(QString)));
    connect(m_LineEdit, SIGNAL(editingFinished()),
            this, SIGNAL(editingFinished()));
    connect(m_LineEdit, SIGNAL(focusIn(QFocusEvent *)),
            this, SLOT(editFocusIn(QFocusEvent *)));
}

QString LineEdit::text() const {
    return m_LineEdit->text();
}

void LineEdit::setText(const QString &text){
    m_LineEdit->setText(text);
}

void LineEdit::overwriteText(const QString &text){
    m_LineEdit->blockSignals(true);
    m_LineEdit->setText(text);
    m_LineEdit->blockSignals(false);
}


QLineEdit::EchoMode LineEdit::echoMode() const {
    return m_LineEdit->echoMode();
}

void LineEdit::setEchoMode(QLineEdit::EchoMode mode) {
    return m_LineEdit->setEchoMode(mode);
}

bool LineEdit::hasFocus () const {
    return m_LineEdit->hasFocus ();
}

void LineEdit::setMaxLength(int len) {
    m_LineEdit->setMaxLength(len);
}

void LineEdit::editFocusIn(QFocusEvent *) {
    emit editingBegin (m_LineEdit->text ());
}

void LineEdit::enterEvent( QEvent* e ) {
    emit entered();
    QWidget::enterEvent(e);
}

void LineEdit::leaveEvent(QEvent *e){
    emit leaved();
    QWidget::leaveEvent(e);
}

}
}
