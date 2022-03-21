#include <cmath>
#include "image.h"
#include <cmath>
#include <iostream>


using namespace std;

// HW1 #1
// float x,y: inexact coordinates
// int c: channel
// returns the bilinearly interpolated pixel (x,y,c)
float Image::pixel_bilinear(float x, float y, int c) const {
    // Since you are inside class Image you can
    // use the member function pixel(a,b,c)
    float x1, x2, y1, y2;
    x1 = floor(x); y1 = floor(y);
    x2 = ceil(x); y2 = ceil(y);
    float v11, v12, v21, v22;
    v11 = clamped_pixel(x1,y1,c);
    v12 = clamped_pixel(x1,y2,c);
    v21 = clamped_pixel(x2,y1,c);
    v22 = clamped_pixel(x2,y2,c);
    float v_lin1 = (x-x1)*v21 + (x2-x)*v11;
    float v_lin2 = (x-x1)*v22 + (x2-x)*v12;
    float pixel = (y-y1)*v_lin2 + (y2-y)*v_lin1;
//    if (pixel > 1.0) {
//        std::cout << "error" << std::endl;
//    }

    return pixel;
}


// HW1 #1
// float x,y: inexact coordinates
// int c: channel
// returns the nearest neighbor to pixel (x,y,c)
float Image::pixel_nearest(float x, float y, int c) const {
    // Since you are inside class Image you can
    // use the member function pixel(a,b,c)
    int xn = (int) round(x);
    int yn = (int) round(y);
    float pixel = clamped_pixel(xn, yn, c);
    return pixel;
}
/*// HW1 #1
// int w,h: size of new image
// const Image& im: input image
// return new Image of size (w,h,im.c)*/
Image nearest_resize(const Image &im, int w, int h) {
    Image ret(w, h, im.c);

    for (int i = 0; i < w; ++i) {
        for (int j = 0; j < h; ++j) {
            // scaled coordinates
            float x = (i+0.5) * ((float)im.w / w) -0.5;
            float y = (j+0.5) * ((float)im.h / h) -0.5;
            for (int k = 0; k < im.c; ++k) {
                float ch_val = im.pixel_nearest(x, y, k);
                ret(i,j,k) = ch_val;
            }

        }
    }

    return ret;
}


// HW1 #1
// int w,h: size of new image
// const Image& im: input image
// return new Image of size (w,h,im.c)
Image bilinear_resize(const Image &im, int w, int h) {

    Image ret(w, h, im.c);

    for (int i = 0; i < w; ++i) {
        for (int j = 0; j < h; ++j) {
            // scaled coordinates
            float x = (i+0.5) * ((float)im.w / w) -0.5;
            float y = (j+0.5) * ((float)im.h / h) -0.5;
            for (int k = 0; k < im.c; ++k) {
                float ch_val = im.pixel_bilinear(x, y, k);
                ret(i,j,k) = ch_val;
            }

        }
    }


    return ret;
}


