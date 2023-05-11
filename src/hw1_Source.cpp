#include <iostream>
#include<opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void ConvertRGBToGray(Mat& src, Mat& dest);
void ConvertGrayToBinary(Mat& src, Mat& dest);
void CreateColorMap(Mat& src);
void ConvertToIndexColorImage(Mat& src, Mat& dest);
void ResizeWithoutInterpolation(Mat& src, Mat& dest, float multiple);
void ResizeWithInterpolation(Mat& src, Mat& dest, float multiple);


int main(int argc, char** argv)
{   
    vector<string> FileStoragePath = { "../images/OriginalImage/","../images/GrayscaleImage/Gray_","../images/BinaryImage/Binary_","../images/Index-colorImage/IndexColor_","../images/ResizingImage/","../images/ResizingImage(interpolation)/" };
    vector<string> OriginalImageString = { "House256","House512","JellyBeans","Lena","Mandrill","Peppers" };
    string FilenameExtension = ".png";

    for (int currentimage = 0; currentimage < OriginalImageString.size(); currentimage++) {
        Mat Original = imread(FileStoragePath.at(0) + OriginalImageString.at(currentimage) + FilenameExtension);
        imshow("Original" + OriginalImageString.at(currentimage), Original);

        Mat Gray;
        ConvertRGBToGray(Original, Gray);
        imshow("Gray" + OriginalImageString.at(currentimage), Gray);
        imwrite(FileStoragePath.at(1) + OriginalImageString.at(currentimage) + FilenameExtension, Gray);

        Mat Binary;
        ConvertGrayToBinary(Gray, Binary);
        imshow("Binary" + OriginalImageString.at(currentimage), Binary);
        imwrite(FileStoragePath.at(2) + OriginalImageString.at(currentimage) + FilenameExtension, Binary);

        Mat ColorMap;
        CreateColorMap(ColorMap);
        imshow("ColorMap", ColorMap);
        imwrite(FileStoragePath.at(3) + "ColorMap" + FilenameExtension, ColorMap);

        Mat IndexColor;
        ConvertToIndexColorImage(Original, IndexColor);
        imshow("IndexColor" + OriginalImageString.at(currentimage), IndexColor);
        imwrite(FileStoragePath.at(3) + OriginalImageString.at(currentimage) + FilenameExtension, IndexColor);

        Mat ResizingHalfTime;
        ResizeWithoutInterpolation(Original, ResizingHalfTime, 0.5);
        imshow("ResizingHalfTime" + OriginalImageString.at(currentimage), ResizingHalfTime);
        imwrite(FileStoragePath.at(4) + "ResizingHalfTime_" + OriginalImageString.at(currentimage) + FilenameExtension, ResizingHalfTime);

        Mat Resizing2Time;
        ResizeWithoutInterpolation(Original, Resizing2Time, 2);
        imshow("Resizing2Time" + OriginalImageString.at(currentimage), Resizing2Time);
        imwrite(FileStoragePath.at(4) + "Resizing2Time_" + OriginalImageString.at(currentimage) + FilenameExtension, Resizing2Time);

        Mat ResizingHalfTimeInterpolation;
        ResizeWithInterpolation(Original, ResizingHalfTimeInterpolation, 0.5);
        imshow("ResizingHalfTimeInterpolation" + OriginalImageString.at(currentimage), ResizingHalfTimeInterpolation);
        imwrite(FileStoragePath.at(5) + "ResizingHalfTimeInterpolation_" + OriginalImageString.at(currentimage) + FilenameExtension, ResizingHalfTimeInterpolation);

        Mat Resizing2TimeInterpolation;
        ResizeWithInterpolation(Original, Resizing2TimeInterpolation, 2);
        imshow("Resizing2TimeInterpolation" + OriginalImageString.at(currentimage), Resizing2TimeInterpolation);
        imwrite(FileStoragePath.at(5) + "Resizing2TimeInterpolation_" + OriginalImageString.at(currentimage) + FilenameExtension, Resizing2TimeInterpolation);
    }
    waitKey(0);
    return 0;
}

// Q1
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

void ConvertGrayToBinary(Mat& src, Mat& dest) {
    dest.create(src.rows, src.cols, CV_8UC1);
    
    for (int y = 0; y < src.rows; y++) {
        uchar* cp = src.ptr<uchar>(y);
        uchar* gp = dest.ptr<uchar>(y);
        for (int x = 0; x < src.cols; x++) {
            uchar pixel = *cp++;
            if (pixel > 128 && pixel < 255) {
                *gp = 255;
            }
            else {
                *gp = 0;
            }
            gp++;
        }
    }
}

void CreateColorMap(Mat& src) {
    src.create(16, 16, CV_8UC3);
    uchar* p = src.ptr<uchar>(0);
    int index = 0;
    for (int i = 0; i < 256; i += 42) {
        for (int j = 0; j < 256; j += 51) {
            for (int k = 0; k < 256; k += 51) {
                //cout << index << ": " << i << ", " << j << ", " << k << endl;
                *p++ = i;
                *p++ = j;
                *p++ = k;
                index++;
            }
        }
    }
    for (int l = 0; l < 4; l++) {
        int i = 252 + (l * 1);
        int j = 255 - (35 + (l * 35));
        int k = 255 - (45 + (l * 45));
        //cout << index << ": " << i << ", " << j << ", " << k << endl;
        *p++ = i;
        *p++ = j;
        *p++ = k;
        index++;
    }
}

void ConvertToIndexColorImage(Mat& src, Mat& dest) {
    dest.create(src.rows, src.cols, CV_8UC3);
    for (int y = 0; y < src.rows; y++) {
        for (int x = 0; x < src.cols; x++) {
            // R
            if (src.at<Vec3b>(y, x)[2] < 42) {
                dest.at<Vec3b>(y, x)[2] = 0;
            }
            else if (src.at<Vec3b>(y, x)[2] >= 42 && src.at<Vec3b>(y, x)[2] < 84) {
                dest.at<Vec3b>(y, x)[2] = 42;
            }
            else if (src.at<Vec3b>(y, x)[2] >= 84 && src.at<Vec3b>(y, x)[2] < 126) {
                dest.at<Vec3b>(y, x)[2] = 84;
            }
            else if (src.at<Vec3b>(y, x)[2] >= 126 && src.at<Vec3b>(y, x)[2] < 168) {
                dest.at<Vec3b>(y, x)[2] = 126;
            }
            else if (src.at<Vec3b>(y, x)[2] >= 168 && src.at<Vec3b>(y, x)[2] < 210) {
                dest.at<Vec3b>(y, x)[2] = 168;
            }
            else if (src.at<Vec3b>(y, x)[2] >= 210 && src.at<Vec3b>(y, x)[2] < 252) {
                dest.at<Vec3b>(y, x)[2] = 210;
            }
            else if (src.at<Vec3b>(y, x)[2] == 252) {
                dest.at<Vec3b>(y, x)[2] = 252;
            }
            else if (src.at<Vec3b>(y, x)[2] == 253) {
                dest.at<Vec3b>(y, x)[2] = 253;
            }
            else if (src.at<Vec3b>(y, x)[2] == 254) {
                dest.at<Vec3b>(y, x)[2] = 254;
            }
            else if (src.at<Vec3b>(y, x)[2] == 255) {
                dest.at<Vec3b>(y, x)[2] = 255;
            }
            // G
            if (src.at<Vec3b>(y, x)[1] == 75) {
                dest.at<Vec3b>(y, x)[1] = 75;
            }
            else if (src.at<Vec3b>(y, x)[1] == 120) {
                dest.at<Vec3b>(y, x)[1] = 120;
            }
            else if (src.at<Vec3b>(y, x)[1] == 165) {
                dest.at<Vec3b>(y, x)[1] = 165;
            }
            else if (src.at<Vec3b>(y, x)[1] == 210) {
                dest.at<Vec3b>(y, x)[1] = 210;
            }
            else if (src.at<Vec3b>(y, x)[1] < 51) {
                dest.at<Vec3b>(y, x)[1] = 0;
            }
            else if (src.at<Vec3b>(y, x)[1] >= 51 && src.at<Vec3b>(y, x)[1] < 102) {
                dest.at<Vec3b>(y, x)[1] = 51;
            }
            else if (src.at<Vec3b>(y, x)[1] >= 102 && src.at<Vec3b>(y, x)[1] < 153) {
                dest.at<Vec3b>(y, x)[1] = 102;
            }
            else if (src.at<Vec3b>(y, x)[1] >= 153 && src.at<Vec3b>(y, x)[1] < 204) {
                dest.at<Vec3b>(y, x)[1] = 153;
            }
            else if (src.at<Vec3b>(y, x)[1] >= 204 && src.at<Vec3b>(y, x)[1] < 255) {
                dest.at<Vec3b>(y, x)[1] = 204;
            }
            else if (src.at<Vec3b>(y, x)[1] == 255) {
                dest.at<Vec3b>(y, x)[1] = 255;
            }
            // B
            if (src.at<Vec3b>(y, x)[0] == 115) {
                dest.at<Vec3b>(y, x)[0] = 115;
            }
            else if (src.at<Vec3b>(y, x)[0] == 150) {
                dest.at<Vec3b>(y, x)[0] = 150;
            }
            else if (src.at<Vec3b>(y, x)[0] == 185) {
                dest.at<Vec3b>(y, x)[0] = 185;
            }
            else if (src.at<Vec3b>(y, x)[0] == 220) {
                dest.at<Vec3b>(y, x)[0] = 220;
            }
            else if (src.at<Vec3b>(y, x)[0] < 51) {
                dest.at<Vec3b>(y, x)[0] = 0;
            }
            else if (src.at<Vec3b>(y, x)[0] >= 51 && src.at<Vec3b>(y, x)[0] < 102) {
                dest.at<Vec3b>(y, x)[0] = 51;
            }
            else if (src.at<Vec3b>(y, x)[0] >= 102 && src.at<Vec3b>(y, x)[0] < 153) {
                dest.at<Vec3b>(y, x)[0] = 102;
            }
            else if (src.at<Vec3b>(y, x)[0] >= 153 && src.at<Vec3b>(y, x)[0] < 204) {
                dest.at<Vec3b>(y, x)[0] = 153;
            }
            else if (src.at<Vec3b>(y, x)[0] >= 204 && src.at<Vec3b>(y, x)[0] < 255) {
                dest.at<Vec3b>(y, x)[0] = 204;
            }
            else if (src.at<Vec3b>(y, x)[0] == 255) {
                dest.at<Vec3b>(y, x)[0] = 255;
            }
        }
    }
}

// Q2
void ResizeWithoutInterpolation(Mat& src, Mat& dest,float multiple) {
    dest.create(src.rows * multiple, src.cols * multiple, CV_8UC3);
    if (multiple >= 1) {
        for (int y = 0; y < src.rows; y++) {
            for (int x = 0; x < src.cols; x++) {
                int yIndex = (y * multiple);
                int xIndex = (x * multiple);
                for (int i = 0; i < 3; i++) {
                    dest.at<Vec3b>(yIndex, xIndex)[i] = src.at<Vec3b>(y, x)[i];
                    dest.at<Vec3b>(yIndex, xIndex + 1)[i] = src.at<Vec3b>(y, x)[i];
                    dest.at<Vec3b>(yIndex + 1, xIndex)[i] = src.at<Vec3b>(y, x)[i];
                    dest.at<Vec3b>(yIndex + 1, xIndex + 1)[i] = src.at<Vec3b>(y, x)[i];
                }
            }
        }
    }
    else if (multiple < 1) {
        for (int y = 0; y < dest.rows; y++) {
            for (int x = 0; x < dest.cols; x++) {
                int yIndex = (y * (1 / multiple));
                int xIndex = (x * (1 / multiple));
                for (int i = 0; i < 3; i++) {
                    dest.at<Vec3b>(y, x)[i] = src.at<Vec3b>(yIndex, xIndex)[i];
                }
            }
        }
    }

}

void ResizeWithInterpolation(Mat& src, Mat& dest, float multiple) {
    dest.create(src.rows * multiple, src.cols * multiple, CV_8UC3);

    int v00, v01, v10, v11; // 00 01 10 11
    int x1, x2, y1, y2;
    int des_x, des_y, src_x, src_y;
    int R1, R2, P;
    //float weightX, weightY;
    for (int y = 0; y < dest.rows; y++) {
        for (int x = 0; x < dest.cols; x++) {   
          /*src_x = (int)((float)x / multiple); src_y = (int)((float)y / multiple);
            weightX = ((float)x) / multiple - src_x; weightY = ((float)y) / multiple - src_y;*/
            src_y = y * (src.rows * 1.0 / dest.rows);
            src_x = x * (src.cols * 1.0 / dest.cols);
            y1 = src_y;
            y2 = src_y + 1 > src.rows - 1 ? src_y - 1 : src_y + 1;
            x1 = src_x;
            x2 = src_x + 1 > src.cols - 1 ? src_x - 1 : src_x + 1;
            for (int i = 0; i < 3; i++) {
                v00 = src.at<Vec3b>(y1, x1)[i];
                v01 = src.at<Vec3b>(y1, x2)[i];
                v10 = src.at<Vec3b>(y2, x1)[i];
                v11 = src.at<Vec3b>(y2, x2)[i];

                R1 = (y2 - src_y) / (y2 - y1) * v00 + (src_y - y1) / (y2 - y1) * v10;
                R2 = (y2 - src_y) / (y2 - y1) * v01 + (src_y - y1) / (y2 - y1) * v11;
                P = (x2 - src_x) / (x2 - x1) * R1 + (src_x - x1) / (x2 - x1) * R2;
                dest.at<Vec3b>(y, x)[i] = P;
            }
            /*for (int i = 0; i < 3; i++) {
                v00 = src.at<Vec3b>(y1, x1)[i];
                v01 = src.at<Vec3b>(y1, x2)[i];
                v10 = src.at<Vec3b>(y2, x1)[i];
                v11 = src.at<Vec3b>(y2, x2)[i];
                dest.at<Vec3b>(y, x)[i] = (1 - weightX) * (1 - weightY) * v00 
                + (1 - weightX) * weightY * v01 
                + weightX * (1 - weightY) * v10 
                + weightX * weightY * v11;
            }*/
        }   
    }
}

