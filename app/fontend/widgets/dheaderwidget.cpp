#include "dheaderwidget.h"

#include "../../backend/backend.h"

#include <DCloseButton>

#include <QHBoxLayout>
#include <QLabel>

DHeaderWidget::DHeaderWidget(QWidget *parent) :
    QWidget(parent)
{
    QHBoxLayout *btlayout = new QHBoxLayout();
    m_CloseButton = new DCloseButton();
    connect(m_CloseButton, SIGNAL(clicked()), this, SIGNAL(closeClicked()));
    btlayout->setMargin(0);
    btlayout->addWidget(m_CloseButton);
    btlayout->setAlignment(m_CloseButton, Qt::AlignRight);

    QVBoxLayout *logolayout = new QVBoxLayout();
    QLabel *logolabel = new QLabel();
    logolabel->setPixmap(QPixmap(":/fontend/images/logo.png"));
    logolayout->addStretch();
    logolayout->addWidget(logolabel);
    logolayout->setAlignment(logolabel, Qt::AlignCenter);
    logolayout->addSpacing(10);
    QLabel *versionlabel = new QLabel(QString("<a style='color:white; font-size:12px;font-weight:bold;'>%1 </a><a style='color:white; font-size:8px;'>%2</a>").arg(DeepinInstaller::Title).arg(DeepinInstaller::Version));
    versionlabel->setIndent(2);
    logolayout->addSpacing(2);
    logolayout->addWidget(versionlabel);
    logolayout->setAlignment(versionlabel, Qt::AlignCenter);
    logolayout->addStretch();

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addLayout(btlayout);
    layout->addLayout(logolayout);
    this->setLayout(layout);
    this->setFixedHeight(80);
}


void DHeaderWidget::enableClose(bool enable) {
    m_CloseButton->enableClose(enable);
}
