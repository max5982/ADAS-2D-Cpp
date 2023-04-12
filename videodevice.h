#ifndef	__VIDEODEVICE__H__
#define	__VIDEODEVICE__H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>

#include <linux/videodev2.h>
#include <stdint.h>
#include <asm/types.h>

#define	DEVICE_NAME		"/dev/video1"
#define NUM_PLANES 2

class	VideoDevice
{
private:
    typedef struct VideoBuffer
    {
	void *start;
	uint32_t length;
    }VideoBuffer;

    VideoBuffer		*pBuffers;
    int				fd;
    unsigned int 	uCount = 4;
    int				uIndex;
    struct v4l2_format fmt;	

public:
    VideoDevice();
    ~VideoDevice();

    int open_device(char* dev_name);
    int close_device();

    int init_device(uint32_t img_width, uint32_t img_height);
    int init_mmap();
    int init_V4L2(uint32_t img_width, uint32_t img_height);

    int getbytesperline(uint32_t  fourcc);
    int getsizeimg(uint32_t  fourcc);
    struct v4l2_format getfmt();

    int start_capturing();
    int stop_capturing();

    int get_frame(void *, int *);
    int get_frame();
    int unget_frame();
    int numplanes=0;
    static int numBufs;
};

#endif
