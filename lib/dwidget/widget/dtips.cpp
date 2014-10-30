#include "dtips.h"
#include <QPropertyAnimation>
#include <QSizePolicy>
#include <QPainter>
#include <QDebug>
#include <QTimer>

#include "dconstant.h"
#include "dwidget.h"

DTips::DTips(DWidget *parent):
    QLabel(parent)
{
    m_delayDrawTimer = new QTimer(this);
    connect(m_delayDrawTimer, SIGNAL(timeout()),
            this, SLOT(doPop()));

    m_posUpdateTimer = new QTimer(this);
    connect(m_posUpdateTimer, SIGNAL(timeout()),
        this, SLOT(updateGeometry()));
    m_posUpdateTimer->start(15);

    m_parentDWidget = parent;
    m_drawing = false;
    QString qss =
        "DTips { "
        "color:#ebab4c;"
        "font-size: 10px;"
        "margin-top: 8px;"
        "margin-left: 8px;"
        "margin-right: 8px;"
        "}";
    this->setStyleSheet(qss);
    this->setFixedHeight(8 + 24);
    this->setMinimumWidth(100);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    this->setAttribute(Qt::WA_ShowWithoutActivating, true);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    this->setFocusPolicy(Qt::NoFocus);
    this->setFrameStyle(Qt::FramelessWindowHint);
    this->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
}

QPainterPath DrawTipsPath(const QRect& rect, int radius) {
    int triHeight = 8;
    int triWidth = 12;

    QPoint topLeft(rect.x(), rect.y()+ triHeight);
    QPoint topRight(rect.x() + rect.width(), rect.y()+ triHeight);
    QPoint bottomRight(rect.x() + rect.width(), rect.y() + rect.height());
    QPoint bottomLeft(rect.x(), rect.y() + rect.height());
    QPoint cornerPoint(rect.x() + 24, rect.y());
    QPainterPath border;
    border.moveTo(topLeft.x() + radius, topLeft.y());
    border.lineTo(cornerPoint.x() - triWidth / 2, topRight.y());
    border.lineTo(cornerPoint);
    border.lineTo(cornerPoint.x() + triWidth / 2, topRight.y());
    border.lineTo(topRight.x() - radius, topRight.y());
    border.arcTo(topRight.x() - 2 * radius, topRight.y(), 2 * radius, 2 * radius, 90, -90);
    border.lineTo(bottomRight.x(), bottomRight.y() - radius);
    border.arcTo(bottomRight.x() - 2 * radius, bottomRight.y() - 2 * radius, 2 * radius, 2 * radius, 0, -90);
    border.lineTo(bottomLeft.x() + radius, bottomLeft.y());
    border.arcTo(bottomLeft.x(), bottomLeft.y() - 2 * radius, 2 * radius, 2 * radius, -90, -90);
    border.lineTo(topLeft.x(), topLeft.y() + radius);
    border.arcTo(topLeft.x(), topLeft.y(), 2 * radius, 2 * radius, 180, -90);
    return border;
}

void DTips::paintEvent(QPaintEvent *e) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    this->adjustSize();
    QRect rect = this->rect();

    QPainterPath border = DrawTipsPath(rect, 4);
    QRect shadowRect = QRect(rect.x() + 1, rect.y()+1, rect.width() - 2, rect.height() - 1);
    QPainterPath shadowBorder = DrawTipsPath(shadowRect, 4);

    QPen borderPen(DeepinWidget::TipsBorderColor);
    painter.strokePath(border, borderPen);

    painter.fillPath(shadowBorder, QBrush(DeepinWidget::TipsBackground));

    QLabel::paintEvent(e);
}


void DTips::pop() {
    if (!m_drawing)
        m_delayDrawTimer->start(50);
}

void DTips::pack() {
    m_delayDrawTimer->stop();
    this->hide();
}

void DTips::doPop() {
    m_delayDrawTimer->stop();
    if (m_drawing){
        return;
    }
    m_drawing = true;
    this->show();
    QPropertyAnimation *movie = new QPropertyAnimation(this, "geometry");
    movie->setDuration(200);
    movie->setEasingCurve(QEasingCurve::InOutCubic);
    QPoint pos = m_parentDWidget->mapToGlobal(m_parentDWidget->pos()) - m_parentDWidget->pos();
    QSize szLabel = m_parentDWidget->size();

    this->adjustSize();
    QSize sz = this->size();
    movie->setStartValue(QRect(pos.x(),
                               pos.y() + szLabel.height()/2 - 6,
                               sz.width(),
                               sz.height()));
    movie->setEndValue(QRect(pos.x(),
                             pos.y() + szLabel.height() - 6,
                             sz.width(),
                             sz.height()));
    movie->start();
    connect(movie, SIGNAL(finished()), this, SLOT(popDone()));
}

void DTips::popDone(){
    m_drawing = false;
    emit poped();
}

void DTips::updateGeometry(){
    if (!this->isVisible() || m_drawing) {
        return;
    }

    QPoint pos = m_parentDWidget->mapToGlobal(m_parentDWidget->pos()) - m_parentDWidget->pos();
    QSize szLabel = m_parentDWidget->size();
    this->adjustSize();
    QSize sz = this->size();
    this->setGeometry(QRect(pos.x(),
                            pos.y() + szLabel.height() - 6,
                            sz.width(),
                            sz.height()));
}
