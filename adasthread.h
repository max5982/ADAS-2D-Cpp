#ifndef __ADAS__H__
#define __ADAS__H__

#include <QThread>
#include <QLabel>
#include <QtDebug>
#include "circular_buffer.h"

class AdasThread : public QThread
{
public:
    explicit AdasThread(QLabel *parent = NULL, circular_buffer<unsigned char *> *pcub1 = NULL,
                                              circular_buffer<unsigned char *> *pcub2 = NULL,
                                              circular_buffer<unsigned char *> *pcub3 = NULL,
                                              circular_buffer<unsigned char *> *pcub4 = NULL);

    // overriding the QThread's run() method
    void run();

private:
    QLabel			*pLabel;
    QImage			*pImage;			/* 	frame                */
    QPixmap pixmap;
    circular_buffer<unsigned char *> *pcub1, *pcub2, *pcub3, *pcub4;
};

#endif
