#ifndef	__SHOWVIDEO__H__
#define	__SHOWVIDEO__H__

#include <QtGui>
#include <QLabel>
#include <QImage>
#include <QTimer>
#include <QPixmap>

#include "videodevice.h"
#include "circular_buffer.h"

class ShowVideo : public QLabel
{
    Q_OBJECT

private:
    VideoDevice		*myVideoDevice;		/*	videodevice			 */
    QLabel			*pLabel;
    QImage			*pImage;			/* 	frame                */
    QTimer			*timer;
    unsigned char	*pChangedVideoData;	/* 	converted frame data */
    unsigned char	*pVideo0Data;		/* 	raw data			 */
    unsigned char	*pVideo1Data;		/* 	raw data			 */
    int				uDataLength;		/* 	frame size in bytes	 */

    unsigned char	**pCirBufData;	    /* 	converted frame data */
    int             bufCnt;

    QPixmap pixmap;
    circular_buffer<unsigned char *> *pcub;

public:
    ShowVideo(QLabel *parent = NULL, char* dev_name = NULL,
        uint32_t width = 1280, uint32_t height = 720, circular_buffer<unsigned char *> *pcub = NULL);
    ~ShowVideo();

    int convert_yuv_to_rgb_pixel(int y, int u, int v);
    int convert_yuyv_to_rgb_buffer(unsigned char *yuv, unsigned char *rgb, unsigned int width, unsigned int height);
    int convert_uyvy_to_rgb_buffer(unsigned char *yuv, unsigned char *rgb, unsigned int width, unsigned int height);
    int convert_nv12_to_rgb_buffer(unsigned char *yuv, unsigned char *rgb, unsigned int width, unsigned int height);

    uint32_t frame_cnt;
    uint32_t width;
    uint32_t height;
    uint32_t frame4showsize;
    uint32_t framesize;
    char* dev_name;

private slots:
    void paintEvent(QPaintEvent	*);		/*  callback when update */
};

#endif
