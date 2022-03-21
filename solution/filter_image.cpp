#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <algorithm>
#include "image.h"

#define M_PI 3.14159265358979323846

// HW1 #2.1
// Image& im: image to L1-normalize
void l1_normalize(Image &im) {

    // Normalize each channel
    for (int k = 0; k < im.c; ++k) {
        float sum = 0;
        for (int i = 0; i < im.w; ++i) {
            for (int j = 0; j < im.h; ++j) {
                sum += im(i, j, k);
            }

        }
        for (int i = 0; i < im.w; ++i) {
            for (int j = 0; j < im.h; ++j) {
                im(i, j, k) /= sum;
            }
        }
    }

}

// HW1 #2.1
// int w: size of filter
// returns the filter Image of size WxW
Image make_box_filter(int w) {
    assert(w % 2); // w needs to be odd

    Image filter = Image(w, w, 1);
    float normalizer = 1.0 / (w * w);
    for (int i = 0; i < w; ++i) {
        for (int j = 0; j < w; ++j) {
            filter(i, j, 0) = normalizer;
        }

    }

    return filter;
}

// HW1 #2.2
// const Image&im: input image
// const Image& filter: filter to convolve with
// bool preserve: whether to preserve number of channels
// returns the convolved image
Image convolve_image(const Image &im, const Image &filter, bool preserve) {
    assert(filter.c == 1);
    int filter_offset = filter.w / 2;
    Image ret;
    if (preserve) {
        ret = Image(im.w, im.h, im.c);
    } else {
        ret = Image(im.w, im.h, 1);
    }
    // This is the case when we need to use the function clamped_pixel(x,y,c).
    // Otherwise you'll have to manually check whether the filter goes out of bounds

    // for each pixel in im
    for (int i = 0; i < im.w; ++i) {
        for (int j = 0; j < im.h; ++j) {
            if (preserve) {
                for (int k = 0; k < im.c; ++k) {
                    float sum = 0;
                    for (int l = -filter_offset; l <= filter_offset; ++l) {
                        for (int m = -filter_offset; m <= filter_offset; ++m) {
                            float a = im.clamped_pixel(i - l, j - m, k);
                            float b = filter(filter_offset - l,
                                             filter_offset - m);
                            sum += a * b;
                        }
                    } //end for k
                    ret(i, j, k) = sum;
                }
            } else {

                float sum = 0;
                for (int l = -filter_offset; l <= filter_offset; ++l) {
                    for (int m = -filter_offset; m <= filter_offset; ++m) {
                        for (int k = 0; k < im.c; ++k) {
                            float a = im.clamped_pixel(i - l, j - m, k);
                            float b = filter(filter_offset - l,
                                             filter_offset - m);
                            sum += a * b;
                        }
                    }
                }
                ret(i, j, 0) = sum;

            }
        }
    }

    return ret;
}

// HW1 #2.3
// returns basic 3x3 high-pass filter
Image make_highpass_filter() {

    Image filter = Image(3, 3, 1);
    float filter_caps[3][3] = {{0,  -1, 0},
                               {-1, 4,  -1},
                               {0,  -1, 0}};
    float normalizer = 1.0;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            filter(i, j, 0) = filter_caps[i][j] * normalizer;
        }

    }

    return filter;

}

// HW1 #2.3
// returns basic 3x3 sharpen filter
Image make_sharpen_filter() {
    Image filter = Image(3, 3, 1);
    float filter_caps[3][3] = {{0,  -1, 0},
                               {-1, 5,  -1},
                               {0,  -1, 0}};
    float normalizer = 1.0;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            filter(i, j, 0) = filter_caps[i][j] * normalizer;
        }

    }

    return filter;

}

// HW1 #2.3
// returns basic 3x3 emboss filter
Image make_emboss_filter() {
    Image filter = Image(3, 3, 1);
    float filter_caps[3][3] = {{-2, -1, 0},
                               {-1, 1,  +1},
                               {0,  +1, +2}};
    float normalizer = 1.0;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            filter(i, j, 0) = filter_caps[i][j] * normalizer;
        }

    }

    return filter;

}

// HW1 #2.4
// float sigma: sigma for the gaussian filter
// returns basic gaussian filter
float compute_2d_gaussian(float x, float y, float sigma) {
    double sigma2 = sigma * sigma;
    return 1.0 / (2 * M_PI * sigma2) * exp(-(x * x + y * y) / (2 * sigma2));
}

Image make_gaussian_filter(float sigma) {
    // kernel size 6x sigma
    int range = round(sigma * 3);
    int size = range * 2 + 1;

    // fill the kernel
    Image filter(size, size, 1);
    for (int l = 0; l < size; ++l) {
        for (int m = 0; m < size; ++m) {
            filter(l, m, 0) = compute_2d_gaussian(l - range, m - range, sigma);
        }

    }
    l1_normalize(filter);
    return filter;

}


// HW1 #3
// const Image& a: input image
// const Image& b: input image
// returns their sum
Image add_image(const Image &a, const Image &b) {
    assert(a.w == b.w && a.h == b.h &&
           a.c == b.c); // assure images are the same size

    Image result(a.w, a.h, a.c);
    for (int i = 0; i < a.w; ++i) {
        for (int j = 0; j < a.h; ++j) {
            for (int k = 0; k < a.c; ++k) {
                result(i, j, k) = a(i, j, k) + b(i, j, k);
            }
        }
    }

    return result;

}

// HW1 #3
// const Image& a: input image
// const Image& b: input image
// returns their difference res=a-b
Image sub_image(const Image &a, const Image &b) {
    assert(a.w == b.w && a.h == b.h &&
           a.c == b.c); // assure images are the same size

    Image result(a.w, a.h, a.c);
    for (int i = 0; i < a.w; ++i) {
        for (int j = 0; j < a.h; ++j) {
            for (int k = 0; k < a.c; ++k) {
                result(i, j, k) = a(i, j, k) - b(i, j, k);
            }
        }
    }

    return result;

}

// HW1 #4.1
// returns basic GX filter
Image make_gx_filter() {
    Image filter = Image(3, 3, 1);
    float filter_caps[3][3] = {{-1, 0, 1},
                               {-2, 0, 2},
                               {-1, 0, 1}};
    float normalizer = 1.0/8;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            filter(i, j, 0) = filter_caps[j][i] * normalizer;
        }

    }
    return filter;
}

// HW1 #4.1
// returns basic GY filter
Image make_gy_filter() {
    Image filter = Image(3, 3, 1);
    float filter_caps[3][3] = {{-1, -2, -1},
                               {0,  0,  0},
                               {1,  2,  1}};
    float normalizer = 1.0/8;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            filter(i, j, 0) = filter_caps[j][i] * normalizer;
        }

    }

    return filter;
}

// HW1 #4.2
// Image& im: input image
void feature_normalize(Image &im) {
    assert(im.c == 1); // assure single channel image
    assert(im.w * im.h); // assure we have non-empty image
    float *start_it = im.data;
    float *end_it = start_it + im.w * im.h;
    pair<float *, float *> minmax = std::minmax_element(start_it, end_it);
    float min_val = *minmax.first;
    float max_val = *minmax.second;
    float range = max_val - min_val;

    if (!range) return; // the image is already empty

    for (int i = 0; i < im.w; ++i) {
        for (int j = 0; j < im.h; ++j) {
            im(i, j) = (im(i, j) - min_val) / range;
        }
    }
}


// Normalizes features across all channels
void feature_normalize_total(Image &im) {
    assert(im.w * im.h * im.c); // assure we have non-empty image

    int nc = im.c;
    im.c = 1;
    im.w *= nc;

    feature_normalize(im);

    im.w /= nc;
    im.c = nc;

}


// HW1 #4.3
// Image& im: input image
// return a pair of images of the same size
pair<Image, Image> sobel_image(const Image &im) {

    Image fx = make_gx_filter();
    Image fy = make_gy_filter();
    Image Gx = convolve_image(im, fx, false);
    Image Gy = convolve_image(im, fy, false);

    Image mod(im.w, im.h, 1);
    Image theta(im.w, im.h, 1);
    for (int i = 0; i < im.w; ++i) {
        for (int j = 0; j < im.h; ++j) {
            double gx = Gx(i, j, 0);
            double gy = Gy(i, j, 0);
            mod(i, j, 0) = sqrtf(pow(gx, 2) + pow(gy, 2));
            theta(i, j, 0) = atan2f(gy, gx);
        }
    }
    return {mod, theta};
}


// HW1 #4.4
// const Image& im: input image
// returns the colorized Sobel image of the same size
Image colorize_sobel(const Image &im) {

    // TODO: Your code here
    Image f = make_gaussian_filter(4);
    Image blur = convolve_image(im, f, true);
    blur.clamp();

    pair<Image, Image> sobel = sobel_image(blur);

    Image mag = sobel.first;  // ampiezza
    Image theta = sobel.second; // direzione gradiente


    feature_normalize(mag);

    for (int y = 0; y < im.h; y++) {
        for (int x = 0; x < im.w; x++) {
            theta(x, y, 0) = theta(x, y, 0) / (2 * M_PI) + 0.5;
        }
    }


    Image hsv(im.w, im.h, 3);

    for (int y = 0; y < im.h; y++) {
        for (int x = 0; x < im.w; x++) {
            hsv(x, y, 0) = theta(x, y, 0);
            hsv(x, y, 1) = mag(x, y, 0);
            hsv(x, y, 2) = mag(x, y, 0);
        }
    }

    hsv_to_rgb(hsv);

    return hsv;
}

Image
make_bilateral_filter(const Image &im, const Image &sgf, int cx, int cy, int cc,
                      float sigma) {


    // Color gaussian filter
    Image cgf(sgf.w, sgf.h, 1);

    for (int y = 0; y < sgf.w; y++) {
        for (int x = 0; x < sgf.w; x++) {
            int ax = cx - sgf.w / 2 + x;
            int ay = cy - sgf.w / 2 + y;

            float diff =
                    im.clamped_pixel(ax, ay, cc) - im.clamped_pixel(cx, cy, cc);

            float var = powf(sigma, 2);
            float c = 2 * M_PI * var;
            float p = -powf(diff, 2) / (2 * var);
            float e = expf(p);
            float val = e / c;

            cgf(x, y, 0) = val;

        }
    }


    Image bf(sgf.w, sgf.h, 1);

    // Multiply space gaussian by color gaussian
    for (int y = 0; y < bf.h; y++) {
        for (int x = 0; x < bf.w; x++) {
            bf(x, y, 0) = sgf(x, y, 0) * cgf(x, y, 0);
        }
    }


    l1_normalize(bf);


    return bf;
}

// HW1 #4.5
// const Image& im: input image
// float sigma1,sigma2: the two sigmas for bilateral filter
// returns the result of applying bilateral filtering to im
Image bilateral_filter(const Image &im, float sigma1, float sigma2) {

    // TODO: Your bilateral code
    Image gf = make_gaussian_filter(sigma1);

    Image res(im.w, im.h, im.c);


    for (int c = 0; c < res.c; c++) {
        for (int y = 0; y < im.h; y++) {
            for (int x = 0; x < im.w; x++) {

                // Get bilateral filter
                Image bf = make_bilateral_filter(im, gf, x, y, c, sigma2);

                float sum = 0.0;
                // Convolve for pixel x,y,c
                for (int fy = 0; fy < gf.w; fy++) {
                    for (int fx = 0; fx < gf.w; fx++) {

                        int ax = x - bf.w / 2 + fx;
                        int ay = y - bf.w / 2 + fy;

                        sum += bf(fx, fy, 0) * im.clamped_pixel(ax, ay, c);
                    }
                }

                res(x, y, c) = sum;

            }
        }
    }

    return res;
}

// HM #5
//
float *compute_histogram(const Image &im, int ch, int num_bins) {
    float *hist = (float *) malloc(sizeof(float) * num_bins);
    for (int i = 0; i < num_bins; ++i) {
        hist[i] = 0;
    }
    int bin_val;
    int N = im.w * im.h;
    float eps = 1.0 / (num_bins * 1000);
    for (int x = 0; x < im.w; ++x) {
        for (int y = 0; y < im.h; ++y) {
            bin_val = (im(x, y, ch) - eps) * (float) num_bins;
            hist[bin_val]++;
        }
    }
    for (int i = 0; i < num_bins; ++i) {
        hist[i] /= N;
    }
    return hist;
}

float *compute_CDF(float *hist, int num_bins) {
    float *cdf = (float *) malloc(sizeof(float) * num_bins);

    cdf[0] = hist[0];
    for (int i = 1; i < num_bins; ++i) {
        cdf[i] = cdf[i - 1] + hist[i];
    }
    return cdf;
}

Image histogram_equalization_hsv(const Image &im, int num_bins) {
    Image new_im(im);
    float eps = 1.0 / (num_bins * 1000);

    // convert to hsv
    rgb_to_hsv(new_im);
    // compute histograms for the luminance channel
    float *hist = compute_histogram(new_im, 2, num_bins);
    // compute cdf
    float *cdf = compute_CDF(hist, num_bins);
    // equalization
    for (int x = 0; x < new_im.w; ++x) {
        for (int y = 0; y < new_im.h; ++y) {
            // convert from [0,1] to the required range and back
            unsigned int val = (unsigned int) ((new_im(x, y, 2) - eps) *
                                               num_bins);
            new_im(x, y, 2) = cdf[val];
        }
    }
    // convert back to rgb
    hsv_to_rgb(new_im);

    // delete the allocated memory!
    delete hist;
    delete cdf;

    return new_im;
}

Image histogram_equalization_rgb(const Image &im, int num_bins) {
    Image new_im(im);
    float eps = 1.0 / (num_bins * 1000);

    // compute histograms for each color channel
    for (int c = 0; c < im.c; ++c) {
        float *hist = compute_histogram(new_im, c, num_bins);
        // compute cdf
        float *cdf = compute_CDF(hist, num_bins);
        // equalization
        for (int x = 0; x < new_im.w; ++x) {
            for (int y = 0; y < new_im.h; ++y) {
                // convert from [0,1] to the required range and back
                unsigned int val = (unsigned int) ((new_im(x, y, c) - eps) *
                                                   num_bins);
                new_im(x, y, 2) = cdf[val];
            }
        }
        // delete the allocated memory!
        delete hist;
        delete cdf;
    }

    return new_im;
}

// HELPER MEMBER FXNS

void Image::feature_normalize(void) { ::feature_normalize(*this); }

void Image::feature_normalize_total(void) { ::feature_normalize_total(*this); }

void Image::l1_normalize(void) { ::l1_normalize(*this); }

Image operator-(const Image &a, const Image &b) { return sub_image(a, b); }

Image operator+(const Image &a, const Image &b) { return add_image(a, b); }
