#include "videodevice.h"

#include <QDebug>

int VideoDevice::numBufs=0;
VideoDevice::VideoDevice()
{
    this->pBuffers = NULL;			/***pointer to memory map***/
    this->fd = -1;					/***device label***/
    this->uCount = 4;				/***num of buffer request***/
    this->uIndex = -1;				/***index of frame buffer***/
}

VideoDevice::~VideoDevice()
{
    for (int i=0;i<VideoDevice::numBufs;i++) {
        free((this->pBuffers+i)->start);
    }
    free(this->pBuffers);
}

/**********************************************************************************
 * V4L2 steps：
1. open video device， int fd = open("/dev/video0", O_RDWR);

2. query capability, to verify if support video in/out/capture，
   struct capabilty cap; capabilities eg: V4L2_CAP_VIDEO_CAPTURE
   ioctl:VIDIOC_QUERYCAP

3. set frame format（PAL,NTSC）PAL:asia video standard
    NTSC:europ video standard；including frame width/height/format
    configure: VIDIOC_QUERYSTD, VIDIOC_S_STD, VIDIOC_S_FMT, struct v4l2_std_id,
    struct v4l2_format

4. mempry allocation， usually <5，2~5 is ok, struct v4l2_requestbuffers

5. map kernel memory to user space, mmap()

6. queue requested buffer into queue，
   ioctl：VIDIOC_QBUF, struct v4l2_buffer

7. start capturing
   ioctl：VIDIOC_STREAMON

8. dequeue buffer to get frame
   ioctl：VIDIOC_DQBUF

9. queue frame buffer to the end of queue after processed, in circle
   ioctl：VIDIOC_QBUF

10. stop capturing
    ioctl：VIDIOC_STREAMOFF

11. close devoce	close(fd);
***********************************************************************************/

int VideoDevice::open_device(char* dev_name)
{
    //this->fd = open(dev_name, O_RDWR  | O_NONBLOCK, 0);
    this->fd = open(dev_name, O_RDWR, 0);
    if(-1 == this->fd)
    {
        perror("open error");
        return	-1;
    }
    //qDebug()<<"Open device success"<<endl;

    return	0;
}

int VideoDevice::close_device()
{
    printf("working on close videodevice...\n");
    if(-1 ==close(fd))
    {
        return	-1;
    }

    return	0;
}


int VideoDevice::init_device(uint32_t img_width, uint32_t img_height)
{
    /***query caps***/
    struct v4l2_capability cap;		/***V4L2 driver caps**/
    struct v4l2_cropcap cropcap;
    struct v4l2_crop crop;
    memset(&cap, 0, sizeof(cap));
    memset(&cropcap, 0, sizeof(cropcap));
    
    if(-1 == ioctl(this->fd, VIDIOC_QUERYCAP, &cap))	/***fill in cap element***/
    {
        perror("ioctl VIDIOC_QUERYCAP error");
        return	-1;
    }

    if(! (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
    {
        return	-1;				/***capture support***/
    }

    if(! (cap.capabilities & V4L2_CAP_STREAMING))
    {
        return	-1;				/***streaming support***/
    }


    cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (ioctl(this->fd, VIDIOC_CROPCAP, &cropcap) == 0) {
            crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            crop.c = cropcap.defrect; /* reset to default */

            if (-1 == ioctl(this->fd, VIDIOC_S_CROP, &crop)) {
                    switch (errno) {
                    case EINVAL:
                            /* Cropping not supported. */
                            break;
                    default:
                            /* Errors ignored. */
                            break;
                   }
            }
    } else {
                /* Errors ignored. */
    }

    /***pring V4L2 basic info***/
    printf("Capability infrmations:\n");
    printf("diver : %s \n", cap.driver);
    printf("card : %s \n", cap.card);
    printf("bus_info : %s \n", cap.bus_info);
    printf("version : %08x \n", cap.version);
    printf("capabilities : %08x\n", cap.capabilities);

    /***set frame width/height/foramt(GPEG,YUYV,RGB)***/
    memset(&fmt, 0, sizeof(fmt));

    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;			/***video stream type***/
    if(-1 == ioctl(this->fd, VIDIOC_G_FMT, &fmt))	/***get current frame configure***/
    {
        perror("ioctl VIDIOC_G_FMT error");
        return	-1;
    }
    numplanes=fmt.fmt.pix_mp.num_planes;
    for(int i=0;i<numplanes;i++) {
        printf("Plane %d: with:%d height:%d perline:%d Sizeimage: %d\n ", i, fmt.fmt.pix_mp.width,
                fmt.fmt.pix_mp.height, fmt.fmt.pix_mp.plane_fmt[i].bytesperline,fmt.fmt.pix_mp.plane_fmt[i].sizeimage);
    }

    /* Set width & height */
    fmt.fmt.pix.width = img_width;
    fmt.fmt.pix.height = img_height;
    if(-1 == ioctl(this->fd, VIDIOC_S_FMT, &fmt))	/***set current frame configure***/
    {
        perror("ioctl VIDIOC_S_FMT error");
        return	-1;
    }

    /* Buggy driver paranoia. */

    // int  min = fmt.fmt.pix.width * 2;
    // if (fmt.fmt.pix.bytesperline < min)
    //             fmt.fmt.pix.bytesperline = min;
    // min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
    // if (fmt.fmt.pix.sizeimage < min)
    //             fmt.fmt.pix.sizeimage = min;
    //qDebug()<<"Init device success\n"<<endl;

    return	0;
}

/***********************************************************
 * 	
v4l2 allocate memory in kernel space, but user can't access kernel space directly, 
so memory map form kernel space to user space is needed. 
Request kernal space memory:
v4l2_requestbuffer: uCount(frame buffer number needed)
ioctl: VIDIOC_REQBUFS，request kernel memory as v4l2_requestbuff param.

Map kernel space to user space
wo need to request the same number and same size memory in user space first,
then map kernel space to user space by mmap.
struct v4l2_buffer

Notes:
uCount: requrested buffer number in kernel space driver asked to allocate
numBufs: allocated buffer number in kernel space driver actually allocate.

uCount may != numBUfs
************************************************************/

int VideoDevice::init_mmap()
{
    
    struct v4l2_requestbuffers reqbuf;	
    memset(&reqbuf, 0, sizeof(reqbuf));

    reqbuf.count = this->uCount;				
    reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;	
    reqbuf.memory = V4L2_MEMORY_MMAP;			/*	memory map	*/

    
    if(-1 == ioctl(this->fd, VIDIOC_REQBUFS, &reqbuf))
    {
        perror("ioctl VIDIOC_REQBUFS error");
        return	-1;
    }
    this->pBuffers = (VideoBuffer *)calloc(reqbuf.count, sizeof(VideoBuffer));
    if(NULL == this->pBuffers)
    {
        perror("calloc error");
        return	-1;
    }

    for(VideoDevice::numBufs = 0; VideoDevice::numBufs < reqbuf.count; VideoDevice::numBufs++)	
    {
	struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;		
        buf.memory = V4L2_MEMORY_MMAP;				
        buf.index = VideoDevice::numBufs;						/*	frame id  */

        if(-1 == ioctl(this->fd, VIDIOC_QUERYBUF, &buf))	/*	get buffer info	*/
        {
            perror("ioctl VIDIOC_QUERYBUF error");
            return	-1;
        }
	(this->pBuffers+VideoDevice::numBufs)->length = buf.length;
	(this->pBuffers+VideoDevice::numBufs)->start = 
                        mmap(NULL /* start anywhere */,
                          buf.length,
                          PROT_READ | PROT_WRITE /* required */,
                          MAP_SHARED /* recommended */,
                          fd, buf.m.offset);
        if((this->pBuffers+VideoDevice::numBufs)->start == MAP_FAILED)
        {
            perror("mmap error");
            return	-1;
        }
    }
    //qDebug()<<"Init mmap success."<<endl;

    return	0;
}

int VideoDevice::init_V4L2(uint32_t img_width, uint32_t img_height)	/*	int V4L2  */
{
    if(-1 == init_device(img_width, img_height))			
    {
        return	-1;
    }

    if(-1 == init_mmap())			/*	int memory  */
    {
        return	-1;
    }
    //qDebug()<<"Init V4L2 success"<<endl;

    return	0;
}

int VideoDevice::start_capturing()		
{
    unsigned int i = 0;
    //printf("numbufs: %d\n",VideoDevice::numBufs);
    for(i = 0; i < VideoDevice::numBufs; i++)
    {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        /*	queue buffer into queue	*/
        if(-1 == ioctl(fd, VIDIOC_QBUF, &buf))
        {
            perror("ioctl VIDIOC_QBUF error");
            return	-1;
        }
    }

    /*
        v4l2_buf_type: v4l2_BUF_TYPE_CAPTURE for capture
        VIDEIOC_STREAMON: stream on as v4l2_buf_type
    */

    enum v4l2_buf_type type;							
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if(-1 == ioctl(this->fd, VIDIOC_STREAMON, &type))	
    {
        perror("ioctl VIDIOC_STREAMON error");
        return	-1;
    }

    return	0;
}

int VideoDevice::stop_capturing()
{
    printf("working on stop capture...\n");
    enum v4l2_buf_type	type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    
    if(-1 == ioctl(this->fd, VIDIOC_STREAMOFF, &type))
    {
        perror("ioctl VIDIOC_STREAMON error");
        return	-1;
    }
    printf("working on unmmap...\n");
    for (int i=0;i<VideoDevice::numBufs;i++) {
            if(MAP_FAILED!=((this->pBuffers+i)->start)){
                if(-1==munmap(((this->pBuffers+i))->start,(this->pBuffers+i)->length)) {
                    printf("munmap error\n");
                }
            }
    }
    return	0;
}



int VideoDevice::get_frame() {
    printf("start get frame\n");
    struct v4l2_buffer buf;
    struct v4l2_plane *mplanes;
    memset(&buf, 0, sizeof(buf));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    /*  dequeue buffer from buffer queue        */
    if(-1 == ioctl(this->fd, VIDIOC_DQBUF, &buf))
    {
        perror("ioctl VIDIOC_DQBUF error");
        return  -1;
    }

    this->uIndex = buf.index;

    //qDebug()<<"get frame: "<<this->uIndex<<endl;
    free(mplanes);
    return 0;
}

int VideoDevice::get_frame(void *plane0buf, int *len)
{
    struct v4l2_buffer buf;
    memset(&buf, 0, sizeof(buf));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    /*	dequeue buffer from buffer queue	*/
    if(-1 == ioctl(this->fd, VIDIOC_DQBUF, &buf))
    {
        perror("ioctl VIDIOC_DQBUF error");
        return	-1;
    }	
    *len=(this->pBuffers+buf.index)->length;
    memcpy(plane0buf, (this->pBuffers+buf.index)->start, *len);
    
    this->uIndex = buf.index;

    //qDebug()<<"get frame: "<<this->uIndex<<endl;
    return	0;
}

int VideoDevice::unget_frame()
{
    if(-1 != this->uIndex)
    {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = this->uIndex;

        if(-1 == ioctl(this->fd, VIDIOC_QBUF, &buf))
        {
	    printf("unget frame failed.\n");
            return	-1;		/*	queue buffer to buffer queue after processed	*/
        }
	//printf("unget frame %d\n", this->uIndex);
        return	0;
    }

    return	-1;
}

struct v4l2_format VideoDevice::getfmt() {
    return this->fmt;
}
