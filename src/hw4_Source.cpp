#include <iostream>
#include<opencv2/opencv.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <math.h>

using namespace std;
using namespace cv;

# define MY_PI 3.14159265358979323846

void ConvertRGBToGray(Mat& src, Mat& dest);
Mat mean_filter(Mat img, int kernel_size);
Mat median_filter(Mat img, int kernel_size);
Mat gaussian_filter5X5(Mat img, double sigma);

int main(int argc, char** argv)
{
    vector<string> FileStoragePath = { "../images/OriginalImage/","../images/filteringImage/mean3x3_","../images/filteringImage/mean7x7_"
        ,"../images/filteringImage/median3x3_","../images/filteringImage/median7x7_","../images/filteringImage/gaussian5x5_" };
    vector<string> OriginalImageString = { "House256_noise","Lena_gray","Mandrill_gray","Peppers_noise"};
    vector<string> LayerString = { "0","1","2","3","4","5","6","7","8","9" };
    string FilenameExtension = ".png";

    for (int currentimage = 0; currentimage < OriginalImageString.size(); currentimage++) {
        Mat Original = imread(FileStoragePath.at(0) + OriginalImageString.at(currentimage) + FilenameExtension);

        Mat mean3x3;
        ConvertRGBToGray(Original, mean3x3);

        Mat mean7x7;
        ConvertRGBToGray(Original, mean7x7);

        Mat median3x3;
        ConvertRGBToGray(Original, median3x3);

        Mat median7x7;
        ConvertRGBToGray(Original, median7x7);

        Mat gaussian;
        ConvertRGBToGray(Original, gaussian);
        //imshow("Gray" + OriginalImageString.at(currentimage), Gray);
        for (int i = 0; i < 7; i++)
        {
            int index = i + 1;
            mean3x3 = mean_filter(mean3x3, 3);
            //imshow("mean3x3" + OriginalImageString.at(currentimage) + "_" + LayerString.at(index), mean3x3);
            imwrite(FileStoragePath.at(1) + OriginalImageString.at(currentimage) + "_" + LayerString.at(index) + FilenameExtension, mean3x3);

            mean7x7 = mean_filter(mean7x7, 7);
            //imshow("mean7x7" + OriginalImageString.at(currentimage) + "_" + LayerString.at(index), mean7x7);
            imwrite(FileStoragePath.at(2) + OriginalImageString.at(currentimage) + "_" + LayerString.at(index) + FilenameExtension, mean7x7);

            median3x3 = median_filter(median3x3, 3);
            //imshow("median3x3" + OriginalImageString.at(currentimage) + "_" + LayerString.at(index), median3x3);
            imwrite(FileStoragePath.at(3) + OriginalImageString.at(currentimage) + "_" + LayerString.at(index) + FilenameExtension, median3x3);

            median7x7 = median_filter(median7x7, 7);
            //imshow("median7x7" + OriginalImageString.at(currentimage) + "_" + LayerString.at(index), median7x7);
            imwrite(FileStoragePath.at(4) + OriginalImageString.at(currentimage) + "_" + LayerString.at(index) + FilenameExtension, median7x7);

            gaussian = gaussian_filter5X5(gaussian, 1.3);
            //imshow("gaussian5x5" + OriginalImageString.at(currentimage) +"_"+ LayerString.at(index), gaussian);
            imwrite(FileStoragePath.at(5) + OriginalImageString.at(currentimage) + "_" + LayerString.at(index) + FilenameExtension, gaussian);

        }

    }
        
    waitKey(0);
    return 0;
}

void ConvertRGBToGray(Mat& src, Mat& dest) {
    dest.create(src.rows, src.cols, CV_8UC1);
    for (int y = 0; y < src.rows; y++) {
        uchar* cp = src.ptr<uchar>(y);
        uchar* gp = dest.ptr<uchar>(y);
        for (int x = 0; x < src.cols; x++) {
            *gp = (15 * cp[0] + 75 * cp[1] + 38 * cp[2]) >> 7;
            cp += 3;
            gp++;
        }
    }
}

Mat mean_filter(Mat img, int kernel_size) {
    int height = img.rows;
    int width = img.cols;
    Mat out;
    out.create(height, width, CV_8UC1);

    int pad = floor(kernel_size / 2);
    int kernel_num = kernel_size * kernel_size;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double v = 0;
            for (int k = -pad; k < pad + 1; k++) {
                for (int l = -pad; l < pad + 1; l++) {
                    int xx = x + l;
                    int yy = y + k;
                    if (0 <= xx && xx < width && 0 <= yy && yy < height)
                    {
                        v += (int)img.at<uchar>(yy, xx);
                    }
                }
            }
                
            v /= kernel_num;
            out.at<uchar>(y, x) = (uchar)v;
        }
    }
    return out;
}

Mat median_filter(Mat img, int kernel_size) {
    int height = img.rows;
    int width = img.cols;

    Mat out;
    out.create(height, width, CV_8UC1);

    int pad = floor(kernel_size / 2);
    int vs[49];
    int count = 0;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            count = 0;

            for (int i = 0; i < 49; i++) {
                vs[i] = 777;
            }

            for (int k = -pad; k < pad + 1; k++) {
                for (int l = -pad; l < pad + 1; l++) {
                    int xx = x + l;
                    int yy = y + k;
                    if (0 <= xx && xx < width && 0 <= yy && yy < height)
                    {
                        vs[count++] = (int)img.at<uchar>(yy, xx);
                    }
                }
            }

            int last = kernel_size * kernel_size;
            std::sort(vs, vs + last);
            out.at<uchar>(y, x) = (uchar)vs[int(floor(count / 2)) + 1];
        }
    }
    return out;
}

Mat gaussian_filter5X5(Mat img, double sigma) {
    int height = img.rows;
    int width = img.cols;
    int kernel_size = 5;
    Mat out;
    out.create(height, width, CV_8UC1);
    int pad = floor(kernel_size / 2);
    int _x = 0, _y = 0;
    double kernel_sum = 0;
    float kernel[5][5];
    for (int y = 0; y < kernel_size; y++) {
        for (int x = 0; x < kernel_size; x++) {
            _y = y - pad;
            _x = x - pad;
            kernel[y][x] = 1 / (2 * MY_PI * sigma * sigma) * exp(-(_x * _x + _y * _y) / (2 * sigma * sigma));
            kernel_sum += kernel[y][x];
        }
    }
    for (int y = 0; y < kernel_size; y++) {
        for (int x = 0; x < kernel_size; x++) {
            kernel[y][x] /= kernel_sum;
        }
    }
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
                double v = 0;
                for (int k = -pad; k < pad + 1; k++) {
                    for (int l = -pad; l < pad+1; l++) {
                        int xx = x + l;
                        int yy = y + k;
                        if (0 <= xx && xx < width && 0 <= yy && yy < height)
                        {
                            v += (double)img.at<uchar>(yy, xx) * kernel[k + pad][l + pad];
                        }
                    }
                }
                out.at<uchar>(y, x) = v;
        }
    }
    return out;
}