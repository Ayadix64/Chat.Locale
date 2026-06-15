
#include "screenCapturing.hpp"
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include "utils.hpp"
#ifdef __linux__

#include <X11/Xlib.h>
#include <X11/Xutil.h>


cv::Mat	getScreenCapture(){
	Display* dis = XOpenDisplay(nullptr);
	Screen* src = XDefaultScreenOfDisplay(dis);
	Drawable drawable = XDefaultRootWindow(dis);
	XImage *image = XGetImage(dis, drawable, 0, 0, src->width, src->height, AllPlanes, ZPixmap);
	
	cv::Mat img =  cv::Mat(image->height , image->width ,CV_8SC4, image->data );
	cv::Mat ret ;
	cv::cvtColor(img, ret, cv::COLOR_BGRA2BGR);

	return ret.clone();
} 


#elifdef __FreeBSD__

#include <X11/Xlib.h>
#include <X11/Xutil.h>


cv::Mat	getScreenCapture(){
	Display* dis = XOpenDisplay(nullptr);
	Screen* src = XDefaultScreenOfDisplay(dis);
	Drawable drawable = XDefaultRootWindow(dis);
	XImage *image = XGetImage(dis, drawable, 0, 0, src->width, src->height, AllPlanes, ZPixmap);
	
	cv::Mat img =  cv::Mat(image->height , image->width ,CV_8SC4, image->data );
	cv::Mat ret ;
	cv::cvtColor(img, ret, cv::COLOR_BGRA2BGR);

	return ret.clone();
} 


#elifdef  _WIN32

cv::Mat	getScreenCapture(){
	logMsgsErr("the app dosnt sepurt this OS screen capturing yet.");
	cv::Mat ret ;

	return ret.clone();
} 




#elifdef __APPLE__

cv::Mat	getScreenCapture(){
	logMsgsErr("the app dosnt sepurt this OS screen capturing yet.");
	cv::Mat ret ;

	return ret.clone();
} 




#endif
