#include "dheaderwidget.h"

#include "../../backend/backend.h"

#include <DCloseButton>

#include <QHBoxLayout>
#include <QLabel>

DHeaderWidget::DHeaderWidget(QWidget *parent) :
    QWidget(parent)
{
    QHBoxLayout *btlayout = new QHBoxLayout();
    DCloseButton *m_closeButton = new DCloseButton();
    connect(m_closeButton, SIGNAL(clicked()), this, SIGNAL(closeClicked()));
    btlayout->setMargin(0);
    btlayout->addWidget(m_closeButton);
    btlayout->setAlignment(m_closeButton, Qt::AlignRight);

    QHBoxLayout *logolayout = new QHBoxLayout();
    QLabel *logolabel = new QLabel();
    logolabel->setPixmap(QPixmap(":/fontend/images/logo.png"));
    logolayout->addStretch();
    logolayout->addWidget(logolabel);
    logolayout->setAlignment(logolabel, Qt::AlignBottom);
    QLabel *versionlabel = new QLabel(QString("<a style='color:white; font-size:11px;'>%1</a>").arg(DeepinInstaller::Version));
    versionlabel->setIndent(2);
    logolayout->addSpacing(2);
    logolayout->addWidget(versionlabel);
    logolayout->setAlignment(versionlabel, Qt::AlignBottom);
    logolayout->addStretch();

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addLayout(btlayout);
    layout->addLayout(logolayout);
    this->setLayout(layout);
    this->setFixedHeight(80);
}
