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

#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <memory>
#include <random>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <utility>
#include <vector>

#include <gflags/gflags.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <openvino/openvino.hpp>

#include <models/detection_model.h>
#include <models/detection_model_centernet.h>
#include <models/detection_model_faceboxes.h>
#include <models/detection_model_retinaface.h>
#include <models/detection_model_retinaface_pt.h>
#include <models/detection_model_ssd.h>
#include <models/detection_model_yolo.h>
#include <models/detection_model_yolov3_onnx.h>
#include <models/detection_model_yolox.h>
#include <models/input_data.h>
#include <models/model_base.h>
#include <models/results.h>
#include <monitors/presenter.h>
#include <pipelines/async_pipeline.h>
#include <pipelines/metadata.h>
#include <utils/args_helper.hpp>
#include <utils/common.hpp>
#include <utils/config_factory.h>
#include <utils/default_flags.hpp>
#include <utils/images_capture.h>
#include <utils/ocv_common.hpp>
#include <utils/performance_metrics.hpp>
#include <utils/slog.hpp>


using namespace std;
using namespace cv;

enum nCamera{
    FRONT_CAM,
    RIGHT_CAM,
    REAR_CAM,
    LEFT_CAM,
    NUM_CAM
};

class ColorPalette {
private:
    std::vector<cv::Scalar> palette;

    static double getRandom(double a = 0.0, double b = 1.0) {
        static std::default_random_engine e;
        std::uniform_real_distribution<> dis(a, std::nextafter(b, std::numeric_limits<double>::max()));
        return dis(e);
    }

    static double distance(const cv::Scalar& c1, const cv::Scalar& c2) {
        auto dh = std::fmin(std::fabs(c1[0] - c2[0]), 1 - fabs(c1[0] - c2[0])) * 2;
        auto ds = std::fabs(c1[1] - c2[1]);
        auto dv = std::fabs(c1[2] - c2[2]);

        return dh * dh + ds * ds + dv * dv;
    }

    static cv::Scalar maxMinDistance(const std::vector<cv::Scalar>& colorSet,
                                     const std::vector<cv::Scalar>& colorCandidates) {
        std::vector<double> distances;
        distances.reserve(colorCandidates.size());
        for (auto& c1 : colorCandidates) {
            auto min =
                *std::min_element(colorSet.begin(), colorSet.end(), [&c1](const cv::Scalar& a, const cv::Scalar& b) {
                    return distance(c1, a) < distance(c1, b);
                });
            distances.push_back(distance(c1, min));
        }
        auto max = std::max_element(distances.begin(), distances.end());
        return colorCandidates[std::distance(distances.begin(), max)];
    }

    static cv::Scalar hsv2rgb(const cv::Scalar& hsvColor) {
        cv::Mat rgb;
        cv::Mat hsv(1, 1, CV_8UC3, hsvColor);
        cv::cvtColor(hsv, rgb, cv::COLOR_HSV2RGB);
        return cv::Scalar(rgb.data[0], rgb.data[1], rgb.data[2]);
    }

public:
    explicit ColorPalette(size_t n) {
        palette.reserve(n);
        std::vector<cv::Scalar> hsvColors(1, {1., 1., 1.});
        std::vector<cv::Scalar> colorCandidates;
        size_t numCandidates = 100;

        hsvColors.reserve(n);
        colorCandidates.resize(numCandidates);
        for (size_t i = 1; i < n; ++i) {
            std::generate(colorCandidates.begin(), colorCandidates.end(), []() {
                return cv::Scalar{getRandom(), getRandom(0.8, 1.0), getRandom(0.5, 1.0)};
            });
            hsvColors.push_back(maxMinDistance(hsvColors, colorCandidates));
        }

        for (auto& hsv : hsvColors) {
            // Convert to OpenCV HSV format
            hsv[0] *= 179;
            hsv[1] *= 255;
            hsv[2] *= 255;

            palette.push_back(hsv2rgb(hsv));
        }
    }

    const cv::Scalar& operator[](size_t index) const {
        return palette[index % palette.size()];
    }
};

// Input image is stored inside metadata, as we put it there during submission stage
cv::Mat renderDetectionData(DetectionResult& result, const ColorPalette& palette, OutputTransform& outputTransform) {
    if (!result.metaData) {
        throw std::invalid_argument("Renderer: metadata is null");
    }

    auto outputImg = result.metaData->asRef<ImageMetaData>().img;

    if (outputImg.empty()) {
        throw std::invalid_argument("Renderer: image provided in metadata is empty");
    }
    outputTransform.resize(outputImg);

    for (auto& obj : result.objects) {
        outputTransform.scaleRect(obj);
        std::ostringstream conf;
        conf << ":" << std::fixed << std::setprecision(1) << obj.confidence * 100 << '%';
        const auto& color = palette[obj.labelID];
        putHighlightedText(outputImg,
                           obj.label + conf.str(),
                           cv::Point2f(obj.x, obj.y - 5),
                           cv::FONT_HERSHEY_COMPLEX_SMALL,
                           1,
                           color,
                           2);
        cv::rectangle(outputImg, obj, color, 2);
    }

    try {
        for (auto& lmark : result.asRef<RetinaFaceDetectionResult>().landmarks) {
            outputTransform.scaleCoord(lmark);
            cv::circle(outputImg, lmark, 2, cv::Scalar(0, 255, 255), -1);
        }
    } catch (const std::bad_cast&) {}

    return outputImg;
}


AdasThread::AdasThread(circular_buffer<unsigned char *> *pcub1,
                       circular_buffer<unsigned char *> *pcub2,
                       circular_buffer<unsigned char *> *pcub3,
                       circular_buffer<unsigned char *> *pcub4)
{
    this->pcub1 = pcub1;
    this->pcub2 = pcub2;
    this->pcub3 = pcub3;
    this->pcub4 = pcub4;
}

#define INPUT_S_H 1080
#define INPUT_S_W 1920

void AdasThread::run()
{
    int loop_sleep_ms = 10; // ms

    /* Required image loading */
    Mat bg = imread("./imgs/bg_black.jpg", IMREAD_COLOR);

    Mat left_mirror = imread("./imgs/left_side_mirror_blue.png", IMREAD_COLOR);
    Mat left_mirror_in_mask = imread("./imgs/left_side_mirror_mask.png", IMREAD_COLOR);
    Mat left_mirror_out_mask = imread("./imgs/left_side_mirror_total.png", IMREAD_COLOR);
    bitwise_not(left_mirror_out_mask, left_mirror_out_mask);

    Mat right_mirror = imread("./imgs/right_side_mirror_blue.png", IMREAD_COLOR);
    Mat right_mirror_in_mask = imread("./imgs/right_side_mirror_mask.png", IMREAD_COLOR);
    Mat right_mirror_out_mask = imread("./imgs/right_side_mirror_total.png", IMREAD_COLOR);
    bitwise_not(right_mirror_out_mask, right_mirror_out_mask);

    Mat room_mirror = imread("./imgs/room_mirror_blue.png", IMREAD_COLOR);
    Mat room_mirror_in_mask = imread("./imgs/room_mirror_mask.png", IMREAD_COLOR);
    Mat room_mirror_out_mask = imread("./imgs/room_mirror_total.png", IMREAD_COLOR);
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

    /* OpenVINO Init */
    std::vector<std::string> labels = {
        std::string("none"),
        std::string("vehicle"),
        std::string("pedestrian")
    };
    ColorPalette palette(5);
    std::unique_ptr<ModelBase> model;
    model.reset(new ModelSSD("pedestrian-and-vehicle-detector-adas-0001.xml",
			    0.6, 0, labels, ""));
    model->setInputsPreprocessing(0, "", "");
    ov::Core core;
    AsyncPipeline pipeline(std::move(model),
                  ConfigFactory::getUserConfig("CPU", 0, "", 0), core);
    int64_t frameNum = -1;
    std::unique_ptr<ResultBase> result;
    uint32_t framesProcessed = 0;

    Mat curr_frame;
    Mat detected_frame;

    cv::Size outputResolution;
    OutputTransform outputTransform = OutputTransform();

    while(true)
    {
        static bool changed[NUM_CAM];

        if(this->pcub1 != NULL && this->pcub1->empty() == false &&
           pipeline.isReadyToProcess()) {
            
            frames[FRONT_CAM].data = (uchar*)this->pcub1->get();
            changed[FRONT_CAM] = true;
            //qDebug() << "FRONT_CAM" << QTime::currentTime().msec();

            auto startTime = std::chrono::steady_clock::now();
            curr_frame = frames[FRONT_CAM];
            frameNum = pipeline.submitData(ImageInputData(curr_frame),
                                           std::make_shared<ImageMetaData>(curr_frame, startTime));
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

        if (frameNum == 0) {
            outputResolution = curr_frame.size();
            outputTransform = OutputTransform(curr_frame.size(), outputResolution);
            outputResolution = outputTransform.computeResolution();
        }

	pipeline.waitForData();

        while (result = pipeline.getResult()) {
            Mat outFrame = renderDetectionData(result->asRef<DetectionResult>(), palette, outputTransform);
            outFrame.copyTo(detected_frame);
            framesProcessed++;
        }

        // Image processing for ADAS
        if (changed[FRONT_CAM] && changed[RIGHT_CAM] && changed[REAR_CAM] && changed[LEFT_CAM]) {

            //Mat out = frames[FRONT_CAM];
            Mat out = detected_frame;
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

            // Display
            //QImage *pQimg = new QImage((uchar*) out.data, out.cols, out.rows, QImage::Format_RGB888);
            //this->pLabel->setPixmap(QPixmap::fromImage(*pQimg));

            cvtColor(out, out, COLOR_RGB2BGR);

            namedWindow("ADAS", WND_PROP_FULLSCREEN);
            setWindowProperty("ADAS", WND_PROP_FULLSCREEN, WINDOW_FULLSCREEN);
            moveWindow("ADAS", 1920, 1080);

            imshow("ADAS", out);
            //waitKey(1);

            changed[FRONT_CAM] = changed[RIGHT_CAM] = changed[REAR_CAM] = changed[LEFT_CAM] = false;
            //qDebug() << "Changed All!!!" << QTime::currentTime().msec();
        }

        msleep(loop_sleep_ms);
    }
}
