#include "dheaderwidget.h"

#include "../../backend/backend.h"


#include <QHBoxLayout>
#include <QLabel>

DHeaderWidget::DHeaderWidget(QWidget *parent) :
    QWidget(parent)
{
    setFixedHeight(150);
    auto headLayout = new QVBoxLayout(this);

    QLabel *logolabel = new QLabel();
    logolabel->setPixmap(QPixmap(":/fontend/images/logo.png"));

    QLabel *title = new QLabel(tr("Install deepin"));
    title->setStyleSheet("QLabel{font-size:24px;color:grey;}");

    QLabel *hits = new QLabel;
    hits->setFixedWidth(400);
    hits->setWordWrap(true);
    hits->setAlignment(Qt::AlignCenter);
    hits->setText("<p style='color:grey; font-size:10px;'>" +
                  tr("This operation will not affect any of your data. Please use it freely.") +
                  "</p>");
  headLayout->addWidget(logolabel, 0, Qt::AlignCenter);
  headLayout->addStretch();
  headLayout->addWidget(title, 0, Qt::AlignCenter);
  headLayout->addStretch();
  headLayout->addWidget(hits, 0, Qt::AlignCenter);
  headLayout->addStretch();

}


void DHeaderWidget::enableClose(bool enable)
{
//    m_CloseButton->enableClose(enable);
}
