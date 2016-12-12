#pragma once

#include <QFrame>
#include <QLineEdit>

class QLabel;

namespace DSI {
namespace Widget {

class SimpleLineEdit:  public QLineEdit {
    Q_OBJECT
public:
    explicit SimpleLineEdit(QWidget *parent = 0):QLineEdit(parent) {}

    virtual void focusInEvent(QFocusEvent *e);

signals:
    void focusIn(QFocusEvent *);
};

class LineEdit : public QFrame
{
    Q_OBJECT
public:
    explicit LineEdit(const QString& qurl, const QString& defaultText, QWidget *parent = 0);

    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged USER true)
    Q_PROPERTY(QLineEdit::EchoMode echoMode READ echoMode WRITE setEchoMode)

    QString text() const;
    QLineEdit::EchoMode echoMode() const;

    bool hasFocus () const;
    void setMaxLength(int);

signals:
    void textChanged(const QString &);
    void textEdited(const QString &);
    void cursorPositionChanged(int, int);
    void returnPressed();
    void editingFinished();
    void editingBegin(const QString &);

    void entered();
    void leaved();
    void moved();

public slots:
    void setText(const QString& text);
    void overwriteText(const QString& text);
    void setEchoMode(QLineEdit::EchoMode);

private slots:
    void editFocusIn(QFocusEvent *e);

protected:
    virtual void enterEvent( QEvent* e );
    virtual void leaveEvent(QEvent * e);

private:
    SimpleLineEdit     *m_LineEdit;
    QLabel              *m_Label;
};

}
}
