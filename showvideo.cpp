#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "showvideo.h"
#include "videodevice.h"

#include <QHBoxLayout>
#include <QImage>
#include <QPixmap>

ShowVideo::ShowVideo(QLabel *parent,char* dev_name, uint32_t img_width, uint32_t img_height, circular_buffer<unsigned char *> *pcub)
    :QLabel(parent),dev_name(dev_name)
{
    this->myVideoDevice = new VideoDevice;
    this->pcub = pcub;

    int iRet = this->myVideoDevice->open_device(dev_name);	/*	open video device		*/
    if(-1 == iRet)
    {
        this->myVideoDevice->close_device();		/*	close video device				*/
    }

    iRet = this->myVideoDevice->init_V4L2(img_width, img_height);	/* int V4L2：init device and init mmap */
    if(-1 == iRet)
    {
        this->myVideoDevice->close_device();
    }

    iRet = this->myVideoDevice->start_capturing();
    if(-1 == iRet)
    {
        this->myVideoDevice->stop_capturing();
        this->myVideoDevice->close_device();
    }

    v4l2_format fmt=this->myVideoDevice->getfmt();
    width=fmt.fmt.pix.width;
    height=fmt.fmt.pix.height;
    printf("w = %d, h = %d\n", width, height);

    switch (fmt.fmt.pix.pixelformat)
    {
    case V4L2_PIX_FMT_GREY:
        frame4showsize=width*height;
        framesize=frame4showsize;
        break;
    case V4L2_PIX_FMT_YUYV:
    case V4L2_PIX_FMT_UYVY:
    case V4L2_PIX_FMT_NV12:
        frame4showsize=width*height*3;
        framesize=width*height*2;
        break;
    default:
        break;
    }

    this->pChangedVideoData = (unsigned char *)malloc(frame4showsize * sizeof(char));
    this->pVideo0Data = (unsigned char *)malloc(framesize * sizeof(char));
    if(NULL == this->pChangedVideoData || NULL == this->pVideo0Data)
    {
        perror("malloc error");
        exit(-1);
    }

    pLabel = parent;

    /* Buffer init for circular buffer */
    this->pCirBufData = (unsigned char **)malloc(TElemCount * sizeof(unsigned char*));
    for (int i=0; i<TElemCount; i++) {
        this->pCirBufData[i] = (unsigned char *)malloc(frame4showsize * sizeof(char));
    }
    this->pcub = pcub;
}

void ShowVideo::paintEvent(QPaintEvent *)
{
    /*	get frame	*/
    this->myVideoDevice->get_frame((void *)((this->pVideo0Data)), &uDataLength);
    /*	qbuffer when frame processed done		*/
    this->myVideoDevice->unget_frame();
    v4l2_format fmt=this->myVideoDevice->getfmt();
    switch (fmt.fmt.pix.pixelformat)
    {
    case V4L2_PIX_FMT_GREY:
        this->pImage = new QImage(this->pVideo0Data, width, height, QImage::Format_Grayscale8);
        if(NULL == this->pImage)
        {
            perror("malloc pImage error");
            exit(-1);
        }
        // // /*	load image from buffer	*/
        // this->pImage->loadFromData(this->pChangedVideoData, frame4showsize * sizeof(char));
        break;

    case V4L2_PIX_FMT_YUYV:
        this->pImage = new QImage(this->pChangedVideoData, width, height, QImage::Format_RGB888);
        if(NULL == this->pImage)
        {
            perror("malloc pImage error");
            exit(-1);
        }
        /*	yuyv to rgb	*/
        convert_yuyv_to_rgb_buffer(this->pVideo0Data, this->pChangedVideoData, width, height);

        // // /*	load image from buffer	*/
        // this->pImage->loadFromData(this->pChangedVideoData, frame4showsize * sizeof(char));
        break;
    case V4L2_PIX_FMT_UYVY:
        this->pImage = new QImage(this->pChangedVideoData, width, height, QImage::Format_RGB888);
        if(NULL == this->pImage)
        {
            perror("malloc pImage error");
            exit(-1);
        }
        /*	yuyv to rgb	*/
        convert_uyvy_to_rgb_buffer(this->pVideo0Data, this->pChangedVideoData, width, height);

        // // /*	load image from buffer	*/
        // this->pImage->loadFromData(this->pChangedVideoData, frame4showsize * sizeof(char));
        break;
    case V4L2_PIX_FMT_NV12:
        this->pImage = new QImage(this->pChangedVideoData, width, height, QImage::Format_RGB888);
        if(NULL == this->pImage)
        {
            perror("malloc pImage error");
            exit(-1);
        }
        /*	nv12 to rgb	*/
        convert_nv12_to_rgb_buffer(this->pVideo0Data,this->pChangedVideoData, width, height);
        // // /*	load image from buffer	*/
        // this->pImage->loadFromData(this->pChangedVideoData, frame4showsize * sizeof(char));
        break;
    default:
        break;
    }

    //QImage resize = this->pImage->scaled(950,500,Qt::KeepAspectRatio);
    QImage resize = this->pImage->scaled(950,500);
    /*	display	*/
    //this->pLabel->setPixmap(QPixmap::fromImage(*pImage));
    this->pLabel->setPixmap(QPixmap::fromImage(resize));

    // Copy image data to the circular buffer
    if (this->pcub->full() == false) {
        int idx = this->pcub->get_head();
        memcpy(this->pCirBufData[idx], this->pChangedVideoData, frame4showsize);
        this->pcub->put(this->pCirBufData[idx]);
        //printf("buffer update - %d \n", this->pcub->get_head());
    }
    //else
    //    printf("buffer full - %d \n", this->pcub->get_head());

    //qDebug() << "Paint event" << QTime::currentTime().msec();

    delete(this->pImage);
}

/* yuv to rgb */
int ShowVideo::convert_yuv_to_rgb_pixel(int y, int u, int v)
{
    unsigned	int		pixel32 = 0;
    unsigned	char 	*pixel = (unsigned char *)&pixel32;
    int 				r, g, b;

    r = (int)(y + (1.370705 * (v-128)));
    g = (int)(y - (0.698001 * (v-128)) - (0.337633 * (u-128)));
    b = (int)(y + (1.732446 * (u-128)));

    if(r > 255) r = 255;
    if(g > 255) g = 255;
    if(b > 255) b = 255;
    if(r < 0) r = 0;
    if(g < 0) g = 0;
    if(b < 0) b = 0;

    pixel[0] = r * 220 / 256;
    pixel[1] = g * 220 / 256;
    pixel[2] = b * 220 / 256;

    return pixel32;
}

static unsigned int frame_number=1;
int ShowVideo::convert_yuyv_to_rgb_buffer(unsigned char *yuv, unsigned char *rgb, unsigned int width, unsigned int height)
{
    unsigned int 	in, out = 0;
    unsigned int 	pixel_16;
    unsigned char 	pixel_24[3];
    unsigned int 	pixel32;
    int 			y0, u, y1, v;

    for(in = 0; in < width * height * 2; in += 4)
    {
        pixel_16 = yuv[in + 3] << 24 | yuv[in + 2] << 16 | yuv[in + 1] <<  8 | yuv[in + 0];
        y0 = (pixel_16 & 0x000000ff);
        u  = (pixel_16 & 0x0000ff00) >>  8;
        y1 = (pixel_16 & 0x00ff0000) >> 16;
        v  = (pixel_16 & 0xff000000) >> 24;
        pixel32 = convert_yuv_to_rgb_pixel(y0, u, v);
        pixel_24[0] = (pixel32 & 0x000000ff);
        pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
        pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;

        rgb[out++] = pixel_24[0];
        rgb[out++] = pixel_24[1];
        rgb[out++] = pixel_24[2];

        pixel32 = convert_yuv_to_rgb_pixel(y1, u, v);
        pixel_24[0] = (pixel32 & 0x000000ff);
        pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
        pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;

        rgb[out++] = pixel_24[0];
        rgb[out++] = pixel_24[1];
        rgb[out++] = pixel_24[2];
    }
    return 0;
}

int ShowVideo::convert_uyvy_to_rgb_buffer(unsigned char *yuv, unsigned char *rgb, unsigned int width, unsigned int height)
{
    unsigned int 	in, out = 0;
    unsigned int 	pixel_16;
    unsigned char 	pixel_24[3];
    unsigned int 	pixel32;
    int 			y0, u, y1, v;

    for(in = 0; in < width * height * 2; in += 4)
    {
        pixel_16 = yuv[in + 3] << 24 | yuv[in + 2] << 16 | yuv[in + 1] <<  8 | yuv[in + 0];
        u = (pixel_16 & 0x000000ff);
        y0  = (pixel_16 & 0x0000ff00) >>  8;
        v = (pixel_16 & 0x00ff0000) >> 16;
        y1  = (pixel_16 & 0xff000000) >> 24;
        pixel32 = convert_yuv_to_rgb_pixel(y0, u, v);
        pixel_24[0] = (pixel32 & 0x000000ff);
        pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
        pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;

        rgb[out++] = pixel_24[0];
        rgb[out++] = pixel_24[1];
        rgb[out++] = pixel_24[2];

        pixel32 = convert_yuv_to_rgb_pixel(y1, u, v);
        pixel_24[0] = (pixel32 & 0x000000ff);
        pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
        pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;

        rgb[out++] = pixel_24[0];
        rgb[out++] = pixel_24[1];
        rgb[out++] = pixel_24[2];
    }
    return 0;
}
int ShowVideo::convert_nv12_to_rgb_buffer(unsigned char *yuv, unsigned char *rgb, unsigned int width, unsigned int height)
{
    unsigned int 	in, out = 0;
    unsigned char 	pixel_y[4];
    unsigned char 	pixel_uv[2];
    unsigned char 	pixel_24[3];
    unsigned int 	pixel32;
    int 			y0, u, y1, v;
    uint32_t uvoffset=width*height;
    for(in = 0; in < width * height; in += 4)
    {
        u  = yuv[in/2+uvoffset];
        v  = yuv[in/2+1+uvoffset];
        for (int i=0;i<4;i+=2) {
            y0=yuv[in+i];
            y1=yuv[in+i+1];
            pixel32 = convert_yuv_to_rgb_pixel(y0, u, v);
            pixel_24[0] = (pixel32 & 0x000000ff);
            pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
            pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;

            rgb[out++] = pixel_24[0];
            rgb[out++] = pixel_24[1];
            rgb[out++] = pixel_24[2];

            pixel32 = convert_yuv_to_rgb_pixel(y1, u, v);
            pixel_24[0] = (pixel32 & 0x000000ff);
            pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
            pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;

            rgb[out++] = pixel_24[0];
            rgb[out++] = pixel_24[1];
            rgb[out++] = pixel_24[2];
        }
    }
    return 0;
}

ShowVideo::~ShowVideo()
{
    printf("working on showvideo deconstruct...\n");
    this->myVideoDevice->stop_capturing();
    this->myVideoDevice->close_device();
    if(this->pChangedVideoData)
	    free(this->pChangedVideoData);
	    free(this->pVideo0Data);
}
