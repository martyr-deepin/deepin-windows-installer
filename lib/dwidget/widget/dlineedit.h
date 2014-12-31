#ifndef DLINEEDIT_H
#define DLINEEDIT_H

#include "dwidget.h"

#include <QLineEdit>

class QLabel;

class DSimpleLineEdit:  public QLineEdit {
    Q_OBJECT
public:
    explicit DSimpleLineEdit(QWidget *parent = 0):QLineEdit(parent) {}

    virtual void focusInEvent(QFocusEvent *e);

signals:
    void focusIn(QFocusEvent *);
};

class DLineEdit : public DWidget
{
    Q_OBJECT
public:
    explicit DLineEdit(const QString& qurl, const QString& defaultText, QWidget *parent = 0);

//    Q_PROPERTY(QString inputMask READ inputMask WRITE setInputMask)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged USER true)
//    Q_PROPERTY(int maxLength READ maxLength WRITE setMaxLength)
//    Q_PROPERTY(bool frame READ hasFrame WRITE setFrame)
    Q_PROPERTY(QLineEdit::EchoMode echoMode READ echoMode WRITE setEchoMode)
//    Q_PROPERTY(QString displayText READ displayText)
//    Q_PROPERTY(int cursorPosition READ cursorPosition WRITE setCursorPosition)
//    Q_PROPERTY(Qt::Alignment alignment READ alignment WRITE setAlignment)
//    Q_PROPERTY(bool modified READ isModified WRITE setModified DESIGNABLE false)
//    Q_PROPERTY(bool hasSelectedText READ hasSelectedText)
//    Q_PROPERTY(QString selectedText READ selectedText)
//    Q_PROPERTY(bool dragEnabled READ dragEnabled WRITE setDragEnabled)
//    Q_PROPERTY(bool readOnly READ isReadOnly WRITE setReadOnly)
//    Q_PROPERTY(bool undoAvailable READ isUndoAvailable)
//    Q_PROPERTY(bool redoAvailable READ isRedoAvailable)
//    Q_PROPERTY(bool acceptableInput READ hasAcceptableInput)
//    Q_PROPERTY(QString placeholderText READ placeholderText WRITE setPlaceholderText)
//    Q_PROPERTY(Qt::CursorMoveStyle cursorMoveStyle READ cursorMoveStyle WRITE setCursorMoveStyle)
//    Q_PROPERTY(bool clearButtonEnabled READ isClearButtonEnabled WRITE setClearButtonEnabled)

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

public slots:
    void setText(const QString& text);
    void overwriteText(const QString& text);
    void setEchoMode(QLineEdit::EchoMode);

private slots:
    void editFocusIn(QFocusEvent *e);

private:
    DSimpleLineEdit   *m_LineEdit;
    QLabel      *m_Label;

};


#endif // DLINEEDIT_H
