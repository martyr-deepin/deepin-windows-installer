#include "dwaterprogress.h"
#include "dconstant.h"

#include <QPainter>
#include <QTimer>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QtMath>

DWaterProgress::DWaterProgress(QWidget *parent) :
    QWidget(parent)
{
    setFixedSize(100,100);
    this->setMask(QRegion(0,0,100,100, QRegion::Ellipse));
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(updateProcess()));

    m_FrontImage = new QImage(":/images/water_front.png");
    m_BackImage = new QImage(":/images/water_back.png");
    m_FrontWidth = m_FrontImage->width() - 1;
    m_BackWidth = m_BackImage->width() - 1;
    m_ForntXOffset1 = 0;
    m_ForntXOffset2 = -m_FrontWidth;
    m_BackXOffset1 = m_BackWidth/2;
    m_BackXOffset2 = -m_BackWidth/2;

    m_Text = new QLabel;

    m_Text->setStyleSheet("QLabel{color:white;font-size:20px;}");
    m_Progresss = 0;

    setProgress(m_Progresss);

    m_Pop7YOffset = 100;
    m_Pop8YOffset = 100;
    m_Pop11YOffset = 100;
    QVBoxLayout *layout = new QVBoxLayout;
//    QPushButton *bt = new QPushButton("S");
//    connect(bt, SIGNAL(clicked()), this, SLOT(start()));
//    layout->addWidget(bt);
    layout->addWidget(m_Text);
    layout->setAlignment(m_Text, Qt::AlignCenter);
    this->setLayout(layout);
}

void DWaterProgress::setProgress(int p) {
    m_Progresss = abs(p) % 100;
    m_YOffset = 88 - p;
    setText(m_Progresss);
}

void DWaterProgress::setText(int p) {
    m_Text->setText(QString("%1%").arg(p));
}

void DWaterProgress::start(){
    m_timer->start(66);
}

void DWaterProgress::stop(){
    m_timer->stop();
}

void DWaterProgress::updateProcess() {
    m_ForntXOffset1 += 2;
    m_ForntXOffset2 += 2;
    if (m_ForntXOffset1 >= 100) {
        m_ForntXOffset1 = -m_FrontWidth;
    }
    if (m_ForntXOffset2 >= 100) {
        m_ForntXOffset2 = -m_FrontWidth;
    }

    m_BackXOffset1 -= 3;
    m_BackXOffset2 -= 3;
    if (m_BackXOffset1 < -m_BackWidth) {
        m_BackXOffset1 = 100;
    }
    if (m_BackXOffset2 < -m_BackWidth) {
        m_BackXOffset2 = 100;
    }

    if (m_YOffset < -12) {
        m_YOffset = 88;
    }

    m_Pop7YOffset-=0.8;
    if (m_Pop7YOffset < m_YOffset + 10) {
        m_Pop7YOffset = 100;
    }
    m_Pop7XOffset = 35 + qSin((100 - m_Pop7YOffset)*2*3.14/40) * 8;

    m_Pop8YOffset-=1.2;
    if (m_Pop8YOffset < m_YOffset + 10) {
        m_Pop8YOffset = 100;
    }
    m_Pop8XOffset = 45 + qCos((100 - m_Pop8YOffset)*2*3.14/40) * 9
            *(100 - m_Pop8XOffset) / 60;

    m_Pop11YOffset-=1.6;
    if (m_Pop11YOffset < m_YOffset + 10) {
        m_Pop11YOffset = 100;
    }
    m_Pop11XOffset = 55 + qSin((100 - m_Pop11YOffset)*2*3.14/50)
            * 11 * (100 - m_Pop11YOffset) / 60;

    this->update();
}

void DWaterProgress::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QPen white(QColor(96,96,96), 2);
    p.setPen(white);
    p.drawEllipse(2, 2, 96, 96);

    p.drawImage(m_BackXOffset1, m_YOffset, *m_BackImage);
    p.drawImage(m_BackXOffset2, m_YOffset, *m_BackImage);
    p.drawImage(m_ForntXOffset1, m_YOffset, *m_FrontImage);
    p.drawImage(m_ForntXOffset2, m_YOffset, *m_FrontImage);

    QPainterPath pop7;
    pop7.addEllipse (m_Pop7XOffset, m_Pop7YOffset, 7, 7);
    p.fillPath (pop7, QColor(77,208,255));

    QPainterPath pop8;
    pop8.addEllipse (m_Pop8XOffset, m_Pop8YOffset, 8, 8);
    p.fillPath (pop8, QColor(77,208,255));

    QPainterPath pop11;
    pop11.addEllipse (m_Pop11XOffset, m_Pop11YOffset, 11, 11);
    p.fillPath (pop11, QColor(77,208,255));

    QPen borderPen(DeepinWidget::BackgroundBottonColor, 4);
    p.setPen(borderPen);
    p.drawEllipse(0, 0, 100, 100);

}
