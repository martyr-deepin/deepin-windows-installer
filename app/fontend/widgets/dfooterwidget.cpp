#include "dfooterwidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>

DFooterWidget::DFooterWidget(QList<QPushButton *> btlist, QWidget *parent) :
    QFrame(parent)
{
    int width = 100;
    int sapce = 10;
    if (btlist.size() > 2) {
        width = 240 / btlist.size();
        sapce = 5;
    }

    auto layout = new QVBoxLayout(this);
    QHBoxLayout *btlayout = new QHBoxLayout;
    btlayout->setMargin(0);
    btlayout->setContentsMargins(0, 0, 0, 30);
    btlayout->setSpacing(sapce);
    btlayout->addStretch();
    for (auto item: btlist) {
        item->setFixedHeight (30);
        item->setMinimumWidth (width);
        btlayout->addWidget(item,0, Qt::AlignCenter);
    }
    btlayout->addStretch();
    this->setFixedHeight(120);
//    this->setStyleSheet("QFrame{border: 1px solid red;}");

    layout->addSpacing(50);
    layout->addLayout(btlayout);

    layout->addStretch();
}
