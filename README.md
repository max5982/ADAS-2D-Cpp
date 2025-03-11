# v4l2_preview

use QT5 do preview, need to work with soc-hkr-host-driver/pcie driver.

precondition:
QT5 installed
```
sudo apt-get install build-essential cmake
sudo apt-get install qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools
sudo apt-get install qtcreator
sudo apt-get install qt5*
```

V4L2 installed:
```
sudo apt-get install libv4l-dev
```

OpenVINO inatlled:
```
curl -L https://storage.openvinotoolkit.org/repositories/openvino/packages/2022.3/linux/l_openvino_toolkit_ubuntu20_2022.3.0.9052.9752fafe8eb_x86_64.tgz --output openvino_2022.3.0.tgz
tar -xf openvino_2022.3.0.tgz
sudo mv l_openvino_toolkit_ubuntu20_2022.3.0.9052.9752fafe8eb_x86_64 /opt/intel/openvino_2022.3.0
cd /opt/intel/openvino_2022.3.0/
sudo -E ./install_dependencies/install_openvino_dependencies.sh
sudo ln -s openvino_2022.3.0 openvino_2022
```

Build packages:
```
sudo apt-get install build-essential cmake pkg-config libjpeg-dev libtiff5-dev libpng-dev ffmpeg libavcodec-dev libavformat-dev libswscale-dev libxvidcore-dev libx264-dev libxine2-dev libv4l-dev v4l-utils libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev mesa-utils libgl1-mesa-dri
```

Build OpenCV:
```
sudo apt-get remove --purge libopencv* python-opencv
sudo rm /usr/local/{bin,lib}/*opencv*
sudo apt-get autoremove
sudo apt-get update && sudo apt-get upgrade -y
sudo apt-get install libgstreamer1.0-dev gstreamer1.0-plugins-base libgstreamer-plugins-base1.0-dev
sudo apt-get install -y build-essential cmake git unzip pkg-config make
sudo apt-get install -y python3.10-dev python3-numpy libtbb2 libtbb-dev
sudo apt-get install -y  libjpeg-dev libpng-dev libtiff-dev libgtk2.0-dev libavcodec-dev libavformat-dev libswscale-dev libeigen3-dev libtheora-dev libvorbis-dev libxvidcore-dev libx264-dev sphinx-common libtbb-dev yasm libfaac-dev libopencore-amrnb-dev libopencore-amrwb-dev libopenexr-dev libgstreamer-plugins-base1.0-dev libavutil-dev libavfilter-dev
```
```
mkdir .opencv && cd .opencv
wget -O opencv.zip https://github.com/opencv/opencv/archive/4.10.0.zip
unzip opencv.zip
wget -O opencv_contrib.zip https://github.com/opencv/opencv_contrib/archive/4.10.0.zip
unzip opencv_contrib.zip
cd opencv-4.10.0 && mkdir build && cd build
export CMAKE_INSTALL_PREFIX=/usr/local
cmake \
-D CMAKE_BUILD_TYPE=RELEASE \
-D CMAKE_INSTALL_PREFIX=/usr/local \
-D WITH_TBB=OFF \
-D BUILD_ZLIB=ON \
-D WITH_IPP=ON \
-D WITH_1394=OFF \
-D HIGHGUI_PLUGIN_LIST=all \
-D OPENCV_IPP_GAUSSIAN_BLUR=ON \
-D WITH_JPEG=ON \
-D BUILD_JPEG=ON \
-D WITH_PNG=ON \
-D BUILD_WITH_DEBUG_INFO=OFF \
-D BUILD_DOCS=OFF \
-D INSTALL_C_EXAMPLES=OFF \
-D BUILD_EXAMPLES=OFF \
-D BUILD_PACKAGE=OFF \
-D BUILD_TESTS=OFF \
-D BUILD_PERF_TESTS=OFF \
-D WITH_QT=ON \
-D WITH_GTK=ON \
-D WITH_OPENGL=ON \
-D WITH_LIBREALSENSE=ON \
-D WITH_VA=ON \
-D WITH_VA_INTEL=ON \
-D WITH_MFX=ON \
-D OPENCV_EXTRA_MODULES_PATH=../../opencv_contrib-4.10.0/modules \
-D WITH_V4L=ON  \
-D WITH_FFMPEG=ON \
-D WITH_GSTREAMER=ON \
-D WITH_XINE=ON \
-D OPENCV_ENABLE_NONFREE=ON \
-D BUILD_NEW_PYTHON_SUPPORT=ON \
-D BUILD_opencv_python3=ON \
-D PYTHON_DEFAULT_AVAILABLE=ON \
-D HAVE_opencv_python3=ON \
-D BUILD_opencv_python2=OFF \
-D BUILD_opencv_apps=ON \
-D OPENCV_SKIP_PYTHON_LOADER=ON \
-D OPENCV_GENERATE_PKGCONFIG=ON \
-D INSTALL_PYTHON_EXAMPLES=OFF ../

make -j$(nproc)
sudo make install
```
Make sure `cat /etc/ld.so.conf.d/*` includes `/usr/local/lib`. If not, please run the below command.
```shell
sudo sh -c 'echo '/usr/local/lib' > /etc/ld.so.conf.d/opencv.conf'
sudo ldconfig
python3 -c "import cv2; print(cv2.getBuildInformation())"
```

* For OpenGL support
```
sudo apt-get update
sudo apt-get install mesa-common-dev libgl1-mesa-dev libglu1-mesa-dev freeglut3-dev
sudo apt-get install libqt5opengl5-dev libva-dev
cmake -D CMAKE_BUILD_TYPE=RELEASE \
      -D CMAKE_INSTALL_PREFIX=/usr/local \
      -D WITH_TBB=OFF \
      -D BUILD_ZLIB=ON \
      -D WITH_IPP=ON \
      -D WITH_1394=OFF \
      -D HIGHGUI_PLUGIN_LIST=all \
      -D OPENCV_IPP_GAUSSIAN_BLUR=ON \
      -D WITH_JPEG=ON \
      -D BUILD_JPEG=ON \
      -D WITH_PNG=ON \
      -D BUILD_WITH_DEBUG_INFO=OFF \
      -D BUILD_DOCS=OFF \
      -D INSTALL_C_EXAMPLES=OFF \
      -D BUILD_EXAMPLES=OFF \
      -D BUILD_PACKAGE=OFF \
      -D BUILD_TESTS=OFF \
      -D BUILD_PERF_TESTS=OFF \
      -D WITH_QT=ON \
      -D WITH_GTK=OFF \
      -D WITH_OPENGL=ON \
      -D WITH_LIBREALSENSE=ON \
      -D WITH_VA=ON \
      -D WITH_VA_INTEL=ON \
      -D WITH_MFX=ON \
      -D OPENCV_EXTRA_MODULES_PATH=../../opencv_contrib-4.10.0/modules \
      -D WITH_V4L=ON  \
      -D WITH_FFMPEG=ON \
      -D WITH_GSTREAMER=ON \
      -D WITH_XINE=ON \
      -D OPENCV_ENABLE_NONFREE=ON \
      -D BUILD_NEW_PYTHON_SUPPORT=ON \
      -D BUILD_opencv_python3=ON \
      -D PYTHON_DEFAULT_AVAILABLE=ON \
      -D HAVE_opencv_python3=ON \
      -D BUILD_opencv_python2=OFF \
      -D BUILD_opencv_apps=ON \
      -D OPENCV_SKIP_PYTHON_LOADER=ON \
      -D OPENCV_GENERATE_PKGCONFIG=ON \
      -D INSTALL_PYTHON_EXAMPLES=OFF \
      ../
```


```
cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local -D WITH_TBB=OFF -D BUILD_ZLIB=ON -D WITH_IPP=ON -D WITH_1394=OFF -D HIGHGUI_PLUGIN_LIST=all -D OPENCV_IPP_GAUSSIAN_BLUR=ON -D WITH_JPEG=ON -D WITH_PNG=ON -D BUILD_WITH_DEBUG_INFO=OFF -D BUILD_DOCS=ON -D INSTALL_C_EXAMPLES=ON -D BUILD_EXAMPLES=OFF -D BUILD_PACKAGE=OFF -D BUILD_TESTS=OFF -D BUILD_PERF_TESTS=OFF -D WITH_QT=ON -D WITH_GTK=OFF -D WITH_OPENGL=ON -D WITH_LIBREALSENSE=ON -D WITH_VA=ON -D WITH_VA_INTEL=ON -D WITH_MFX=ON -D OPENCV_EXTRA_MODULES_PATH=../../opencv_contrib-4.8.1/modules -D WITH_V4L=ON  -D WITH_FFMPEG=ON -D WITH_GSTREAMER=ON -D WITH_XINE=ON -D OPENCV_ENABLE_NONFREE=ON -D BUILD_NEW_PYTHON_SUPPORT=ON -D BUILD_opencv_python3=ON -D BUILD_opencv_python2=OFF -D BUILD_opencv_apps=ON -D OPENCV_SKIP_PYTHON_LOADER=ON -D OPENCV_GENERATE_PKGCONFIG=ON -D INSTALL_PYTHON_EXAMPLES=ON -D WITH_CUDA=ON -D WITH_OPENCL=ON ../
```

```
cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local -D WITH_TBB=OFF -D BUILD_ZLIB=ON -D WITH_IPP=ON -D WITH_1394=OFF -D HIGHGUI_PLUGIN_LIST=all -D OPENCV_IPP_GAUSSIAN_BLUR=ON -D WITH_JPEG=ON -D BUILD_JPEG=ON -D WITH_PNG=ON -D BUILD_WITH_DEBUG_INFO=OFF -D BUILD_DOCS=ON -D INSTALL_C_EXAMPLES=ON -D BUILD_EXAMPLES=OFF -D BUILD_PACKAGE=OFF -D BUILD_TESTS=OFF -D BUILD_PERF_TESTS=OFF -D WITH_QT=ON -D WITH_GTK=ON -D WITH_OPENGL=ON -D WITH_LIBREALSENSE=ON -D WITH_VA=ON -D WITH_VA_INTEL=ON -D WITH_MFX=ON -D OPENCV_EXTRA_MODULES_PATH=../../opencv_contrib-4.10.0/modules -D WITH_V4L=ON  -D WITH_FFMPEG=ON -D WITH_GSTREAMER=ON -D WITH_XINE=ON -D OPENCV_ENABLE_NONFREE=ON -D BUILD_NEW_PYTHON_SUPPORT=ON -D BUILD_opencv_python3=ON -D PYTHON_DEFAULT_AVAILABLE=ON -D HAVE_opencv_python3=ON -D BUILD_opencv_python2=OFF -D BUILD_opencv_apps=ON -D OPENCV_SKIP_PYTHON_LOADER=ON -D OPENCV_GENERATE_PKGCONFIG=ON -D INSTALL_PYTHON_EXAMPLES=ON -D BUILD_opencv_cvv=OFF -D Qt6_DIR=/usr/lib/qt6  -D BUILD_opencv_rgbd=ON -D OpenGL_GL_PREFERENCE=GLVND  ../
```

OpenCV python path (optional):
```
export PYTHONPATH=$PYTHONPATH:/path/to/your/opencv/build/lib/python3/
```



Optional:
sudo apt install v4l-utils

usage:
source /opt/intel/openvino_2022/setupvars.sh


# Build
- mkdir build && cd build
- cmake ..
- make

# OpenCV higui frameless
```
# opencv-4.10.0/modules/highgui/src/window_QT.cpp
void GuiReceiver::createWindow(QString name, int flags)
{
    if (!qApp)
        CV_Error(cv::Error::StsNullPtr, "NULL session handler" );

    // Check the name in the storage
    if (icvFindWindowByName(name.toLatin1().data()))
    {
        return;
    }

    nb_windows++;

-    new CvWindow(name, flags);
+    CvWindow* window = new CvWindow(name, flags);

+    // Frameless (타이틀바 및 메뉴 제거)
+    window->setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
+
+    window->show(); // 반드시 호출하여 창 표시

    cvWaitKey(1);
}
```
