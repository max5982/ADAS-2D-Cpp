#include <QtGui>
#include <QTextCodec>
#include <QApplication>
#include <QImage>
#include "showvideo.h"
#include <getopt.h>             /* getopt_long() */
#include <thread>
#include <vector>
#include <csignal>

#include <opencv2/core.hpp>     // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/imgproc.hpp>  // Gaussian Blur
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>  // OpenCV window I/O


#include "adas_ui.h"
#include "adasthread.h"
#include "circular_buffer.h"

using namespace std;


ShowVideo* myShowVideo1;
ShowVideo* myShowVideo2;
ShowVideo* myShowVideo3;
ShowVideo* myShowVideo4;

void signalHandler( int signum ) {
   printf("signal handler = %d\n", signum);

   // cleanup and close up stuff here
   // terminate program
   delete myShowVideo1;
   delete myShowVideo2;
   delete myShowVideo3;
   delete myShowVideo4;

   exit(signum);
}

class AdasUI : public QWidget
{
public:
    AdasUI(QWidget *parent = 0);

    Ui_Form ui;
};

AdasUI::AdasUI(QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);

    // Remove title bar
    this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
}

int main(int argc, char *argv[])
{
    // register signal SIGINT and signal handler
    signal(SIGINT|SIGTERM, signalHandler);

    QApplication	app(argc, argv);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("gb 18030"));

    // GUI init
    AdasUI *adas_ui = new AdasUI;

    // Circular buffer
    int cam_num = 4;
    circular_buffer<unsigned char *> cbuf[cam_num];
    for (int i = 0 ; i < cam_num ; i++)
        cbuf->reset();
    
    uint32_t img_width = 1920, img_height = 1080;

    myShowVideo1 = new ShowVideo(adas_ui->ui.label_front_img,(char *)"/dev/video0", img_width, img_height, &cbuf[0]);
    myShowVideo2 = new ShowVideo(adas_ui->ui.label_rear_img,(char *)"/dev/video4", img_width, img_height, &cbuf[1]);
    myShowVideo3 = new ShowVideo(adas_ui->ui.label_left_img,(char *)"/dev/video8", img_width, img_height, &cbuf[2]);
    myShowVideo4 = new ShowVideo(adas_ui->ui.label_right_img,(char *)"/dev/video10", img_width, img_height, &cbuf[3]);

    // Thread
    AdasThread *adas = new AdasThread(&cbuf[0], &cbuf[1], &cbuf[2], &cbuf[3]);
    adas->start();

    // Set position
    adas_ui->setGeometry(0,0,3840,1080);

    // Show
    adas_ui->show();

    return	app.exec();
}
