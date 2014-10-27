#include "dcombobox.h"

#include <QListView>

static QString s_QComboBoxStyle =
"QComboBox {"
    "color:#b4b4b4;"
    "font-size: 12px;"
    "color: grey;"
    "margin: 0px;"
    "padding-left: 8px;"
    "border: 1px solid rgb(0, 0, 0);"
    "border-radius: 4px;"
    "background: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,"
    "stop:0 rgba(48, 49, 50, 255),"
    "stop:0.1 rgb(32, 32, 32),"
    "stop:1 rgb(24, 24, 24));"
"}"
"QComboBox:editable {"
    "background: #232323;"
"}"
"QComboBox:!editable, QComboBox::drop-down:editable {"
"}"
"/* QComboBox gets the \"on\" state when the popup is open */"
"QComboBox:!editable:on, QComboBox::drop-down:editable:on {"
"}"
"QComboBox:on { /* shift the text when the popup opens */"
"}"
"QComboBox::drop-down {"
    "margin-right: 2px;"
    "subcontrol-origin: padding;"
    "subcontrol-position: top right;"
    "width: 20px;"
    "border-width: 2px;"
    "border-top-right-radius: 4px; /* same radius as the QComboBox */"
    "border-bottom-right-radius: 4px;"
"}"
"QComboBox::down-arrow {"
    "image: url(:/images/arrow_down_normal.png);"
"}"
"QComboBox::down-arrow:hover {"
    "image: url(:/images/arrow_down_hover.png);"
"}"
"QComboBox::down-arrow:on { /* shift the arrow when popup is open */"
    "top: 1px;"
    "left: 1px;"
"}";

static QString s_QListViewStyle =
"QListView {"
    "color:#b4b4b4;"
    "font-size: 12px;"
    "border: 1px solid #202020;;"
    "alternate-background-color: #232323;"
    "show-decoration-selected: 1;"
    "background: #232323;"
"}"
"QListView::item {"
    "padding-left: 6px;"
    "border: 0px solid gray;"
    "background: #232323;"
"}"
"QListView::item:alternate {"
    "background: #232323;"
"}"
"QListView::item:selected {"
    "background: #232323;"
"}"
"QListView::item:selected:!active {"
    "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                                "stop: 0 black, stop: 1 #232323);"
"}"
"QListView::item:selected:active {"
    "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                                "stop: 0 black, stop: 1 #232323);"
"}"
"QListView::item:hover {"
    "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                                "stop: 0 black, stop: 1 #232323);"
"}";

DComboBox::DComboBox(QWidget *parent) :
    QComboBox(parent)
{
    this->setStyleSheet(s_QComboBoxStyle);
    QListView * listView = new QListView(this);
    listView->setStyleSheet(s_QListViewStyle);
    this->setView(listView);
}
