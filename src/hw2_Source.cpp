#include <iostream>
#include<opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <stack>
#include <ctime>
#include <list>
#include <map>

using namespace std;
using namespace cv;

void ConvertRGBToGray(Mat& src, Mat& dest);
void ConvertGrayToBinary(Mat& src, Mat& dest, int threshold);
void Seed_Filling_4C(const cv::Mat& binImg, cv::Mat& lableImg);
void Seed_Filling_8C(const cv::Mat& binImg, cv::Mat& lableImg);
void LabelColor(const cv::Mat& labelImg, cv::Mat& colorLabelImg);
Scalar GetRandomColor();

int main(int argc, char** argv)
{
    vector<string> FileStoragePath = { "../images/OriginalImage/","../images/BinaryImage/Binary_","../images/4CImage/4C_","../images/8CImage/8C_" };
    vector<string> OriginalImageString = { "1","2","3","4" };
    vector<int> Threshold = {104,236,90,230}; // 二值化門檻值
    string FilenameExtension = ".png";
    srand(time(NULL)); //讓顏色隨機
    for (int currentimage = 0; currentimage < OriginalImageString.size(); currentimage++) {
        Mat Original = imread(FileStoragePath.at(0) + OriginalImageString.at(currentimage) + FilenameExtension);
        //imshow("Original" + OriginalImageString.at(currentimage), Original);

        Mat Gray;
        ConvertRGBToGray(Original, Gray);

        Mat Binary;
        ConvertGrayToBinary(Gray, Binary, Threshold.at(currentimage));
        imshow("Binary" + OriginalImageString.at(currentimage), Binary);
        imwrite(FileStoragePath.at(1) + OriginalImageString.at(currentimage) + FilenameExtension, Binary);

        Mat labelImg;
        Seed_Filling_4C(Binary, labelImg);

        Mat colorLabelImg_4c;
        cout << "4C-Image" + OriginalImageString.at(currentimage) + " ";
        LabelColor(labelImg, colorLabelImg_4c);
        cv::imshow("4C_colorImg" + OriginalImageString.at(currentimage), colorLabelImg_4c);
        imwrite(FileStoragePath.at(2) + OriginalImageString.at(currentimage) + FilenameExtension, colorLabelImg_4c);

        Seed_Filling_8C(Binary, labelImg);

        Mat colorLabelImg_8c;
        cout << "8C-Image" + OriginalImageString.at(currentimage) + " ";
        LabelColor(labelImg, colorLabelImg_8c);
        cv::imshow("8C_colorImg" + OriginalImageString.at(currentimage), colorLabelImg_8c);
        imwrite(FileStoragePath.at(3) + OriginalImageString.at(currentimage) + FilenameExtension, colorLabelImg_8c);
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

void Seed_Filling_4C(const Mat& binImg, Mat& labelImg)
{
    labelImg.release();
    binImg.convertTo(labelImg, CV_32SC1);
    int label = 0;
    int rows = binImg.rows;
    int cols = binImg.cols;
    Mat mask(rows, cols, CV_8UC1);
    mask.setTo(0);
    for (int y = 0; y < rows; y++)
    {
        int* data = labelImg.ptr<int>(y);
        for (int x = 0; x < cols; x++)
        {
            if (data[x] == 0 && mask.at<uchar>(y, x) != 1)
            {
                mask.at<uchar>(y, x) = 1;
                std::stack<std::pair<int, int>> neighborPixels;
                neighborPixels.push(std::pair<int, int>(y, x)); 
                ++label;
                if (label == 255)
                {
                    ++label;
                }
                while (!neighborPixels.empty())
                {
                    std::pair<int, int> curPixel = neighborPixels.top();
                    int curY = curPixel.first;
                    int curX = curPixel.second;
                    labelImg.at<int>(curY, curX) = label;
                    neighborPixels.pop();
                    // 4-neighbors 
                    //  [(     ), (x,y-1) , (   )  ]
                    //  [(x-1,y), (x,y)   , (x+1,y)]    
                    //  [(     ), (x,y+1) , (   )  ]
                    // 0、(cols-1)、(rows-1)為index的界線
                    if (curX - 1 >= 0) // left pixel
                    {
                        if (labelImg.at<int>(curY, curX - 1) == 0 && mask.at<uchar>(curY, curX - 1) != 1)
                        {
                            neighborPixels.push(std::pair<int, int>(curY, curX - 1));
                            mask.at<uchar>(curY, curX - 1) = 1;
                        }
                    }
                    if (curX + 1 <= cols - 1) // right pixel
                    {
                        if (labelImg.at<int>(curY, curX + 1) == 0 && mask.at<uchar>(curY, curX + 1) != 1)
                        {
                            neighborPixels.push(std::pair<int, int>(curY, curX + 1));
                            mask.at<uchar>(curY, curX + 1) = 1;
                        }
                    }
                    if (curY - 1 >= 0) // up pixel
                    {
                        if (labelImg.at<int>(curY - 1, curX) == 0 && mask.at<uchar>(curY - 1, curX) != 1)
                        {
                            neighborPixels.push(std::pair<int, int>(curY - 1, curX));
                            mask.at<uchar>(curY - 1, curX) = 1;
                        }
                    }
                    if (curY + 1 <= rows - 1) // down pixel
                    {
                        if (labelImg.at<int>(curY + 1, curX) == 0 && mask.at<uchar>(curY + 1, curX) != 1)
                        {
                            neighborPixels.push(std::pair<int, int>(curY + 1, curX));
                            mask.at<uchar>(curY + 1, curX) = 1;
                        }
                    }
                }
            }
        }
    }
}

void Seed_Filling_8C(const Mat& binImg, Mat& labelImg)
{
    labelImg.release();
    binImg.convertTo(labelImg, CV_32SC1);
    int label = 0;
    int rows = binImg.rows;
    int cols = binImg.cols;
    Mat mask(rows, cols, CV_8UC1);
    mask.setTo(0);
    for (int y = 0; y < rows; y++)
    {
        int* data = labelImg.ptr<int>(y);
        for (int x = 0; x < cols; x++)
        {
            if (data[x] == 0 && mask.at<uchar>(y, x) != 1)
            {
                mask.at<uchar>(y, x) = 1;
                std::stack<std::pair<int, int>> neighborPixels;
                neighborPixels.push(std::pair<int, int>(y, x)); 
                ++label;    
                if (label == 255)
                {
                    ++label;
                }
                while (!neighborPixels.empty())
                {
                    std::pair<int, int> curPixel = neighborPixels.top();
                    int curY = curPixel.first;
                    int curX = curPixel.second;
                    labelImg.at<int>(curY, curX) = label;
                    neighborPixels.pop();
                    // 8-neighbors
                    //[(x-1,y-1), (x,y-1) , (x+1,y-1)]
                    //[(x-1,y)  , (x,y)   , (x+1,y)  ]    
                    //[(x-1,y+1), (x,y+1) , (x+1,y+1)]  
                    if ((curX - 1 >= 0) && (curY - 1 >= 0)) // upper left corner pixel
                    {
                        if (labelImg.at<int>(curY - 1, curX - 1) == 0 && mask.at<uchar>(curY - 1, curX - 1) != 1)
                        {
                            neighborPixels.push(std::pair<int, int>(curY - 1, curX - 1));
                            mask.at<uchar>(curY - 1, curX - 1) = 1;
                        }
                    }
                    if (curX - 1 >= 0) // left pixel
                    {
                        if (labelImg.at<int>(curY, curX - 1) == 0 && mask.at<uchar>(curY, curX - 1) != 1)
                        {
                            neighborPixels.push(std::pair<int, int>(curY, curX - 1));
                            mask.at<uchar>(curY, curX - 1) = 1;
                        }
                    }
                    
                    if ((curX - 1 >= 0) && (curY + 1 <= rows - 1)) //lower left corner pixel
                    {
                        if (labelImg.at<int>(curY + 1, curX - 1) == 0 && mask.at<uchar>(curY + 1, curX - 1) != 1)
                        {
                            neighborPixels.push(std::pair<int, int>(curY + 1, curX - 1));
                            mask.at<uchar>(curY + 1, curX - 1) = 1;
                        }
                    }
                    if ((curX + 1 <= cols - 1) && (curY - 1 >= 0)) // upper right corner pixel
                    {
                        if (labelImg.at<int>(curY - 1, curX + 1) == 0 && mask.at<uchar>(curY - 1, curX + 1) != 1)
                        {
                            neighborPixels.push(std::pair<int, int>(curY - 1, curX + 1));
                            mask.at<uchar>(curY - 1, curX + 1) = 1;
                        }
                    }
                    if (curX + 1 <= cols - 1) // right pixel
                    {
                        if (labelImg.at<int>(curY, curX + 1) == 0 && mask.at<uchar>(curY, curX + 1) != 1)
                        {
                            neighborPixels.push(std::pair<int, int>(curY, curX + 1));
                            mask.at<uchar>(curY, curX + 1) = 1;
                        }
                    }
                    if ((curX + 1 <= cols - 1) && (curY + 1 <= rows - 1)) // lower right corner pixel
                    {
                        if (labelImg.at<int>(curY + 1, curX + 1) == 0 && mask.at<uchar>(curY + 1, curX + 1) != 1)
                        {
                            neighborPixels.push(std::pair<int, int>(curY + 1, curX + 1));
                            mask.at<uchar>(curY + 1, curX + 1) = 1;
                        }
                    }
                    if (curY - 1 >= 0) // up pixel
                    {
                        if (labelImg.at<int>(curY - 1, curX) == 0 && mask.at<uchar>(curY - 1, curX) != 1)
                        {
                            neighborPixels.push(std::pair<int, int>(curY - 1, curX));
                            mask.at<uchar>(curY - 1, curX) = 1;
                        }
                    }
                    if (curY + 1 <= rows - 1) // down pixel
                    {
                        if (labelImg.at<int>(curY + 1, curX) == 0 && mask.at<uchar>(curY + 1, curX) != 1)
                        {
                            neighborPixels.push(std::pair<int, int>(curY + 1, curX));
                            mask.at<uchar>(curY + 1, curX) = 1;
                        }
                    }
                }
            }
        }
    }
}

cv::Scalar GetRandomColor()
{
    uchar r = 255 * (rand() / (1.0 + RAND_MAX));
    uchar g = 255 * (rand() / (1.0 + RAND_MAX));
    uchar b = 255 * (rand() / (1.0 + RAND_MAX));
    return cv::Scalar(b, g, r);
}

void LabelColor(const cv::Mat& labelImg, cv::Mat& colorLabelImg)
{
    int object_number = 0;
    std::map<int, cv::Scalar> colors;
    int rows = labelImg.rows;
    int cols = labelImg.cols;
    colorLabelImg.release();
    colorLabelImg.create(rows, cols, CV_8UC3);
    colorLabelImg.setTo(Scalar(0, 0, 0));
    for (int y = 0; y < rows; y++)
    {
        const int* data_src = (int*)labelImg.ptr<int>(y);
        uchar* data_dst = colorLabelImg.ptr<uchar>(y);
        for (int x = 0; x < cols; x++)
        {
            int pixelValue = data_src[x];
            if (pixelValue >= 1 && pixelValue != 255)
            {
                if (colors.count(pixelValue) == 0)
                {
                    colors[pixelValue] = GetRandomColor();
                    object_number++;
                }

                cv::Scalar color = colors[pixelValue];
                *data_dst++ = color[0];
                *data_dst++ = color[1];
                *data_dst++ = color[2];
            }
            else
            {
                data_dst += 3;
            }
            
        }
    }
    cout << "object number :" << object_number << endl;
}

