#include <iostream>
#include<opencv2/opencv.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>

using namespace std;
using namespace cv;

void ConvertRGBToGray(Mat& src, Mat& dest);
void ConvertGrayToBinary(Mat& src, Mat& dest, int threshold);
void Quadtree(Mat& binaryImage, int **image, int width, int layer, int* x, int* y);
int ScanningImage(Mat& binaryImage, int x, int y, int width);
void GenerateSplittingImage(Mat& layerImage, int** image, int width);

int main(int argc, char** argv)
{
    vector<string> FileStoragePath = { "../images/OriginalImage/","../images/LayerImage/Layer_" };
    vector<string> OriginalImageString = { "1","2","3","4"};
    vector<string> LayerString = { "0","1","2","3","4","5","6","7","8","9" };
    vector<int> Threshold = {135,245,155,254}; // 二值化門檻值
    string FilenameExtension = ".png";

    for (int currentimage = 0; currentimage < OriginalImageString.size(); currentimage++) {
        Mat Original = imread(FileStoragePath.at(0) + OriginalImageString.at(currentimage) + FilenameExtension);

        Mat Gray;
        ConvertRGBToGray(Original, Gray);

        Mat Binary;
        ConvertGrayToBinary(Gray, Binary, Threshold.at(currentimage));

        int width = Binary.rows;
        int layer = log2(width);
        for (int i = 1; i <= layer; i++) {
            Mat Layer;
            int x_index = 0, y_index = 0;
            int** image = new int* [width]; 

            for (int j = 0; j < width; j++) {
                image[j] = new int[width]; 
            }

            for (int j = 0; j < width; j++) {
                for (int k = 0; k < width; k++) {
                    image[j][k] =  0;
                }
            }

            Quadtree(Binary, image, width, i, &x_index, &y_index);
            GenerateSplittingImage(Layer, image, width);
            imwrite(FileStoragePath.at(1) + OriginalImageString.at(currentimage) + "_" + LayerString.at(i) + FilenameExtension, Layer);

            for (int j = 0; j < width; j++) {
                delete[] image[j]; 
            }
            delete[] image;
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

void ConvertGrayToBinary(Mat& src, Mat& dest, int threshold) {
    dest.create(src.rows, src.cols, CV_8UC1);

    for (int y = 0; y < src.rows; y++) {
        uchar* cp = src.ptr<uchar>(y);
        uchar* gp = dest.ptr<uchar>(y);
        for (int x = 0; x < src.cols; x++) {
            uchar pixel = *cp++;
            if (pixel > threshold) {
                *gp = 255;
            }
            else {
                *gp = 0;
            }
            gp++;
        }
    }
}

// 需要知道現在是左上右上左下右下哪邊，然後點座標一律在左上角。
void Quadtree(Mat& binaryImage, int **image, int width, int layer, int* x, int* y)
{
    int current_x = *x;
    int current_y = *y;
    // 判斷是全255還是全0，還是要128。
    int scanningreturn = ScanningImage(binaryImage, current_x, current_y, width);
    if (scanningreturn == 255)
    {
        for (int i = 0; i < width; i++)
        {
            if (i > 0)
            {
                *x += 1;
            }
            for (int j = 0; j < width; j++)
            {
                if (j > 0)
                {
                    *y += 1;
                }
                image[*x][*y] = 255; // white
            }
            *y = current_y;
        }
        *x = current_x;
    }
    else if (scanningreturn == 0)
    {
        for (int i = 0; i < width; i++)
        {
            if (i > 0)
            {
                *x += 1;
            }
            for (int j = 0; j < width; j++)
            {
                if (j > 0)
                {
                    *y += 1;
                }
                image[*x][*y] = 0; // black
            }
            *y = current_y;
        }
        *x = current_x;
    }
    else if (scanningreturn == 128)
    {
        for (int i = 0; i < width; i++)
        {
            if (i > 0)
            {
                *x += 1;
            }
            for (int j = 0; j < width; j++)
            {
                if (j > 0)
                {
                    *y += 1;
                }
                image[*x][*y] = 128;
            }
            *y = current_y;
        }
        *x = current_x;
        if (layer > 0) {
            layer--;
            for (int i = 0; i < 4; i++)
            {
                if (i == 0)
                {
                    current_x = *x;
                    current_y = *y;
                }
                if (i == 1)
                {
                    current_x = *x;
                    current_y = *y + (width / 2);
                }
                if (i == 2)
                {
                    current_x = *x + (width / 2);
                    current_y = *y;
                }
                if (i == 3)
                {
                    current_x = *x + (width / 2);
                    current_y = *y + (width / 2);
                }
                Quadtree(binaryImage, image, width / 2, layer, &current_x, &current_y);
            }
        }
    }
}

int ScanningImage(Mat& binaryImage, int x, int y, int width)
{
    uchar* indexp = binaryImage.ptr<uchar>(x) + y;
    uchar firstcolor = *indexp;

    for (int i = x; i < x + width; i++) {
        uchar* p = binaryImage.ptr<uchar>(i) + y;
        for (int j = y; j < y + width; j++) {
            uchar pixel = *p++;
            if (pixel != firstcolor)
            {
                return 128;
            }
        }
    }
    if (firstcolor == 255)
    {
        return 255;
    }
    else if (firstcolor == 0)
    {
        return 0;
    }
}

void GenerateSplittingImage(Mat& layerImage, int** image, int width)
{
    layerImage.create(width, width, CV_8UC1);
    for (int y = 0; y < width; y++) {
        uchar* sp = layerImage.ptr<uchar>(y);
        for (int x = 0; x < width; x++) {
            *sp = (uchar)image[y][x];
            sp++;
        }
    }
}
