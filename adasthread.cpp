#include <optional>
#include <QImage>
#include <QTime>
#include "adasthread.h"

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/core/hal/interface.h>
#include <opencv2/calib3d.hpp>


#include <iostream>
#include <math.h>
#include <stdio.h>

using namespace std;
using namespace cv;

enum nCamera{
    FRONT_CAM,
    RIGHT_CAM,
    REAR_CAM,
    LEFT_CAM,
    NUM_CAM
};

AdasThread::AdasThread(QLabel *parent, circular_buffer<unsigned char *> *pcub1,
                                     circular_buffer<unsigned char *> *pcub2,
                                     circular_buffer<unsigned char *> *pcub3,
                                     circular_buffer<unsigned char *> *pcub4
) :
    QThread(parent)
{
    this->pcub1 = pcub1;
    this->pcub2 = pcub2;
    this->pcub3 = pcub3;
    this->pcub4 = pcub4;
    pLabel = parent;
}

#define INPUT_S_H 1080
#define INPUT_S_W 1920

void AdasThread::run()
{
    int loop_sleep_ms = 30; // ms

    /* Required image loading */
    Mat bg = imread("/home/intel/git/ADAS/imgs/bg_black.jpg", IMREAD_COLOR);

    Mat left_mirror = imread("/home/intel/git/ADAS/imgs/left_side_mirror_blue.png", IMREAD_COLOR);
    Mat left_mirror_in_mask = imread("/home/intel/git/ADAS/imgs/left_side_mirror_mask.png", IMREAD_COLOR);
    Mat left_mirror_out_mask = imread("/home/intel/git/ADAS/imgs/left_side_mirror_total.png", IMREAD_COLOR);
    bitwise_not(left_mirror_out_mask, left_mirror_out_mask);

    Mat right_mirror = imread("/home/intel/git/ADAS/imgs/right_side_mirror_blue.png", IMREAD_COLOR);
    Mat right_mirror_in_mask = imread("/home/intel/git/ADAS/imgs/right_side_mirror_mask.png", IMREAD_COLOR);
    Mat right_mirror_out_mask = imread("/home/intel/git/ADAS/imgs/right_side_mirror_total.png", IMREAD_COLOR);
    bitwise_not(right_mirror_out_mask, right_mirror_out_mask);

    Mat room_mirror = imread("/home/intel/git/ADAS/imgs/room_mirror_blue.png", IMREAD_COLOR);
    Mat room_mirror_in_mask = imread("/home/intel/git/ADAS/imgs/room_mirror_mask.png", IMREAD_COLOR);
    Mat room_mirror_out_mask = imread("/home/intel/git/ADAS/imgs/room_mirror_total.png", IMREAD_COLOR);
    resize(room_mirror, room_mirror, Size(800, 340));
    resize(room_mirror_in_mask, room_mirror_in_mask, Size(800, 340));
    resize(room_mirror_out_mask, room_mirror_out_mask, Size(800, 340));
    bitwise_not(room_mirror_out_mask, room_mirror_out_mask);

    /* Define image buffer */
    vector<Mat> frames = {
        Mat::zeros(INPUT_S_H, INPUT_S_W, CV_8UC3),
        Mat::zeros(INPUT_S_H, INPUT_S_W, CV_8UC3),
        Mat::zeros(INPUT_S_H, INPUT_S_W, CV_8UC3),
        Mat::zeros(INPUT_S_H, INPUT_S_W, CV_8UC3)
    };

#if 0
    qDebug() << "bg size = " << bg.cols << ", " << bg.rows;
    resize(bg, bg, Size(1280, 1880));
    qDebug() << "bg size = " << bg.cols << ", " << bg.rows;
    Mat mask_tr = imread("/home/intel/git/ADAS/imgs/mask_tr1.png", IMREAD_COLOR);
    Mat mask_bl = imread("/home/intel/git/ADAS/imgs/mask_bl1.png", IMREAD_COLOR);
    Mat mask_br = imread("/home/intel/git/ADAS/imgs/mask_br1.png", IMREAD_COLOR);
    Mat mask_tl = imread("/home/intel/git/ADAS/imgs/mask_tl1.png", IMREAD_COLOR);
    resize(mask_tr, mask_tr, Size(500,100)); mask_tr.convertTo(mask_tr, CV_32F, 1.0 / 255.0);
    resize(mask_bl, mask_bl, Size(500,100)); mask_bl.convertTo(mask_bl, CV_32F, 1.0 / 255.0);
    resize(mask_br, mask_br, Size(500,100)); mask_br.convertTo(mask_br, CV_32F, 1.0 / 255.0);
    resize(mask_tl, mask_tl, Size(500,100)); mask_tl.convertTo(mask_tl, CV_32F, 1.0 / 255.0);

    Mat car = imread("/home/intel/git/ADAS/imgs/suzuki_jimny.png", IMREAD_COLOR);
    Mat car_mask = imread("/home/intel/git/ADAS/imgs/suzuki_jimny_mask.png", IMREAD_COLOR);
    Mat car_mask_inv;
    resize(car, car, Size(680, 1320));
    cvtColor(car, car, CV_BGR2RGB);
    resize(car_mask, car_mask, Size(680, 1320));
    bitwise_not(car_mask, car_mask_inv);

    /* Define image buffer */
    vector<Mat> frames = {
        Mat::zeros(INPUT_S_H, INPUT_S_W, CV_8UC3),
        Mat::zeros(INPUT_S_H, INPUT_S_W, CV_8UC3),
        Mat::zeros(INPUT_S_H, INPUT_S_W, CV_8UC3),
        Mat::zeros(INPUT_S_H, INPUT_S_W, CV_8UC3)
    };
    vector<Mat> resizes = {
        Mat::zeros(INPUT_RE_H, INPUT_RE_W, CV_8UC3),
        Mat::zeros(INPUT_RE_H, INPUT_RE_W, CV_8UC3),
        Mat::zeros(INPUT_RE_H, INPUT_RE_W, CV_8UC3),
        Mat::zeros(INPUT_RE_H, INPUT_RE_W, CV_8UC3)
    };
    Mat dsts[NUM_CAM];
    Mat birdviews[NUM_CAM];

  #endif
    while(true)
    {
        static bool changed[NUM_CAM];

        if(this->pcub1 != NULL && this->pcub1->empty() == false) {
            frames[FRONT_CAM].data = (uchar*)this->pcub1->get();
            changed[FRONT_CAM] = true;
            //qDebug() << "FRONT_CAM" << QTime::currentTime().msec();
        }
        if(this->pcub2 != NULL && this->pcub2->empty() == false) {
            frames[RIGHT_CAM].data = (uchar*)this->pcub2->get();
            changed[RIGHT_CAM]  = true;
            //qDebug() << "RIGHT_CAM" << QTime::currentTime().msec();
        }
        if(this->pcub3 != NULL && this->pcub3->empty() == false) {
            frames[REAR_CAM].data = (uchar*)this->pcub3->get();
            changed[REAR_CAM] = true;
            //qDebug() << "REAR_CAM" << QTime::currentTime().msec();
        }
        if(this->pcub4 != NULL && this->pcub4->empty() == false) {
            frames[LEFT_CAM].data = (uchar*)this->pcub4->get();
            changed[LEFT_CAM] = true;
            //qDebug() << "LEFT_CAM" << QTime::currentTime().msec();
        }

        // Image processing for ADAS
        if (changed[FRONT_CAM] && changed[RIGHT_CAM] && changed[REAR_CAM] && changed[LEFT_CAM]) {
#if 0
            // Resize
            for (int i = 0 ; i < NUM_CAM ; i++)
                resize(frames[i], resizes[i], Size(INPUT_RE_W, INPUT_RE_H));

            // Undistortion
            //for (int i = 0 ; i < NUM_CAM ; i++)
            //    remap(resizes[i], dsts[i], map1, map2, INTER_LINEAR, BORDER_CONSTANT);

            // Bird-eye view
            for (int i = 0 ; i < NUM_CAM ; i++) {
                Mat M = getPerspectiveTransform(corners, WarpCorners);
                //warpPerspective(dsts[i], birdviews[i], M, Size(bird_w, bird_h));
                warpPerspective(resizes[i], birdviews[i], M, Size(bird_w, bird_h));
            }

            Mat src1 = birdviews[FRONT_CAM](Range(100,500), Range(0,1280));
            Mat src2 = birdviews[RIGHT_CAM](Range(0,500), Range(0,1280));
            Mat src3 = birdviews[REAR_CAM](Range(100,500), Range(0,1280));
            Mat src4 = birdviews[LEFT_CAM](Range(0,500), Range(0,1280));
            rotate(src2, src2, ROTATE_90_CLOCKWISE);
            rotate(src3, src3, ROTATE_180);
            rotate(src4, src4, ROTATE_90_COUNTERCLOCKWISE);

            src1.copyTo(bg(Rect(0, 0, 1280, 400)));
            src3.copyTo(bg(Range(1480,1880), Range(0,1280)));
            multiply(mask_tr, src1(Range(300,400), Range(780,1280)), src1(Range(300,400), Range(780,1280)));
            multiply(Scalar::all(1.0) - mask_tr, src2(Range(0,100), Range(0, 500)), src2(Range(0,100), Range(0, 500)));
            add(src1(Range(300,400), Range(780,1280)), src2(Range(0,100), Range(0, 500)), bg(Range(300,400), Range(780,1280)));

            src2(Range(100,1180), Range(0,500)).copyTo(bg(Range(400,1480), Range(780,1280)));
            multiply(mask_br, src3(Range(0,100), Range(780,1280)), src3(Range(0,100), Range(780,1280)));
            multiply(Scalar::all(1.0) - mask_br, src2(Range(1180,1280), Range(0, 500)), src2(Range(1180,1280), Range(0, 500)));
            add(src3(Range(0,100), Range(780,1280)), src2(Range(1180,1280), Range(0, 500)), bg(Range(1480,1580), Range(780,1280)));

            multiply(mask_bl, src3(Range(0,100), Range(0,500)), src3(Range(0,100), Range(0,500)));
            multiply(Scalar::all(1.0) - mask_bl, src4(Range(1180,1280), Range(0, 500)), src4(Range(1180,1280), Range(0, 500)));
            add(src3(Range(0,100), Range(0,500)), src4(Range(1180,1280), Range(0, 500)), bg(Range(1480,1580), Range(0,500)));

            src4(Range(100,1180), Range(0,500)).copyTo(bg(Range(400,1480), Range(0,500)));
            multiply(mask_tl, src1(Range(300,400), Range(0,500)), src1(Range(300,400), Range(0,500)));
            multiply(Scalar::all(1.0) - mask_tl, src4(Range(0,100), Range(0, 500)), src4(Range(0,100), Range(0, 500)));
            add(src1(Range(300,400), Range(0,500)), src4(Range(0,100), Range(0, 500)), bg(Range(300,400), Range(0,500)));

            // Car
            Mat roi = bg(Range(290, 290+1320), Range(300, 300+680));

            //bitwise_and(roi, roi, roi, car_mask_inv);
            bitwise_and(roi, car_mask_inv, roi);
            bitwise_and(car, car_mask, car);
            bitwise_or(car, roi, bg(Range(290, 290+1320), Range(300, 300+680)));
#endif
            // Resize
            Mat out = frames[FRONT_CAM];
            Mat left = frames[LEFT_CAM]; resize(left, left, Size(700, 440));
            Mat right = frames[RIGHT_CAM]; resize(right, right, Size(700, 440));
            Mat room = frames[REAR_CAM](Range(150, 930), Range(0, 1920)); resize(room, room, Size(800, 340));

            Mat left_mirror_bg = out(Range(640, 1080), Range(0, 700));
            Mat right_mirror_bg = out(Range(640, 1080), Range(1220, 1920));
            Mat room_mirror_bg = out(Range(0, 340), Range(560, 560+800));

            Mat roi_l;
            bitwise_and(left_mirror_out_mask, left_mirror_bg, roi_l);
            bitwise_and(left, left_mirror_in_mask, left);
            bitwise_or(left, roi_l, left_mirror_bg);

            Mat roi_r;
            bitwise_and(right_mirror_out_mask, right_mirror_bg, roi_r);
            bitwise_and(right, right_mirror_in_mask, right);
            bitwise_or(right, roi_r, right_mirror_bg);

            Mat roi_room;
            bitwise_and(room_mirror_out_mask, room_mirror_bg, roi_room);
            bitwise_and(room, room_mirror_in_mask, room);
            bitwise_or(room, roi_room, room_mirror_bg);

            //bitwise_or(car, roi, out);
            QImage *pQimg = new QImage((uchar*) out.data, out.cols, out.rows, QImage::Format_RGB888);
            this->pLabel->setPixmap(QPixmap::fromImage(*pQimg));

            changed[FRONT_CAM] = changed[RIGHT_CAM] = changed[REAR_CAM] = changed[LEFT_CAM] = false;
            //qDebug() << "Changed All!!!" << QTime::currentTime().msec();
        }

        msleep(loop_sleep_ms);
    }

}
