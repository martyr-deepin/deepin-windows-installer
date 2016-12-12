#include <QWidget>

class QTimer;
class QImage;
class QLabel;

namespace DSI {
namespace Widget {

class WaterProgress : public QWidget
{
    Q_OBJECT
public:
    explicit WaterProgress(QWidget *parent = 0);

    virtual void paintEvent(QPaintEvent *);

signals:

public slots:
    void start();
    void stop();

private slots:
    void updateProcess();
    void setProgress(int);

private:
    void setText(int);

    int m_ForntXOffset1;
    int m_ForntXOffset2;
    int m_BackXOffset1;
    int m_BackXOffset2;
    int m_YOffset;
    int m_FrontWidth;
    int m_BackWidth;

    double m_Pop7YOffset;
    double m_Pop7XOffset;

    double m_Pop8YOffset;
    double m_Pop8XOffset;

    double m_Pop11YOffset;
    double m_Pop11XOffset;

    QImage *m_BackImage;
    QImage *m_FrontImage;
    QTimer *m_timer;
    QLabel *m_Text;
    int     m_Progresss;
};

}
}
