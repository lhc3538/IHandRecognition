#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtNetwork/QTcpSocket>
#include <QString>
#include <QMutex>
#include "cv.h"
#include "highgui.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/ml/ml.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include <QImage>
#include "IplImageToQImage.h"
#include <QTime>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
    
public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void on_help_clicked();
    void on_connect_clicked();
    void on_stop_clicked();
    void on_record_clicked();
    void on_stoprecord_clicked();
    void sendrequest();
    void read();
    void socketerror();
    
private:
    Ui::Widget *ui;
    QTcpSocket *client;
    QByteArray block1;
    QMutex mutex;

};

#endif // WIDGET_H
