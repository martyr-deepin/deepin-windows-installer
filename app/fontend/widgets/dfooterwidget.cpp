#include "dfooterwidget.h"

#include <DPushButton>
#include <QHBoxLayout>

DFooterWidget::DFooterWidget(const QList<DPushButton*> &btlist, QWidget *parent) :
    QWidget(parent)
{
    int width = 100;
    int sapce = 10;
    if (btlist.size() > 2) {
        width = 240 / btlist.size();
        sapce = 5;
    }

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->setContentsMargins(0, 0, 0, 30);
    layout->setSpacing(sapce);
    layout->addStretch();
    foreach (DPushButton* item, btlist) {
        item->setFixedHeight (30);
        item->setMinimumWidth (width);
        layout->addWidget(item);
        layout->setAlignment (item, Qt::AlignCenter);
    }
    layout->addStretch();
    this->setLayout(layout);
    this->setFixedHeight(60);

}
