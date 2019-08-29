#include <iostream>
#include <opencv2/core/check.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <math.h>

//use cv::filter2D to implement kernel on each channel
void filter(double kernel_data[], cv::Mat &mat);
//implement part one
cv::Mat diff(cv::Mat &demosaic, cv::Mat &originalImage);
//implement part two
cv::Mat modified(cv::Mat &mat, cv::Mat &greenImage);

using namespace cv;
using namespace std;

int main(int argc, char** argv) {
    Mat mosaicImage;
    Mat blueImage;
    Mat greenImage;
    Mat redImage;
    Mat partOneResult;

    Mat modifiedRed;
    Mat modifiedBlue;
    Mat bfAlgorithmImage;
    Mat partTwoResult;

    Mat originalImage;
    Mat demosaicImage;
    Mat improvementImage;

    //load image
    mosaicImage = imread("image_set/crayons_mosaic.bmp", IMREAD_GRAYSCALE);
    originalImage = imread("image_set/crayons.jpg");

    if(!mosaicImage.data || !originalImage.data) {
        cout << "Pictures don't exist" << endl;
        return -1;
    }

    //kernel for green channel and blue channel
    double kernel_data_bg[9]={ 0.25, 0.5, 0.25,

                              0.5,   1.0,  0.5,

                              0.25, 0.5, 0.25};

    //kernel for red channel
    double kernel_data_r[9]={ 0,   0.25,     0,

                             0.25,    1.0,  0.25,

                             0,   0.25,     0};

    //split source opencv- B/G/R
    blueImage = Mat::zeros(mosaicImage.rows, mosaicImage.cols, 0);
    for (int x = 0; x < mosaicImage.rows; x++) {
        for (int y = 0; y < mosaicImage.cols; y++) {
            if (x % 2 == 0 && y % 2 == 0){
                blueImage.at<uchar>(x, y) = mosaicImage.at<uchar>(x, y);
            }
        }
    }

    greenImage = Mat::zeros(mosaicImage.rows, mosaicImage.cols, 0);
    for (int x = 0; x < mosaicImage.rows; x++) {
        for (int y = 0; y < mosaicImage.cols; y++) {
            if (x % 2 != 0 && y % 2 != 0){
                greenImage.at<uchar>(x, y) = mosaicImage.at<uchar>(x, y);
            }
        }
    }

    redImage = Mat::zeros(mosaicImage.rows, mosaicImage.cols, 0);
    for (int x = 0; x < mosaicImage.rows; x++) {
        for (int y = 0; y < mosaicImage.cols; y++) {
            if ((x % 2 == 0 && y % 2 != 0) || (x % 2 != 0 && y % 2 == 0)){
                redImage.at<uchar>(x, y) = mosaicImage.at<uchar>(x, y);
            }
        }
    }

    //use simple linear interpolation for each channel respectively
    filter(kernel_data_bg, blueImage);
    filter(kernel_data_bg, greenImage);
    filter(kernel_data_r, redImage);

    vector<Mat> bgrChannel_1;
    bgrChannel_1.push_back(blueImage);
    bgrChannel_1.push_back(greenImage);
    bgrChannel_1.push_back(redImage);

    merge(bgrChannel_1, demosaicImage);
    partOneResult = diff(demosaicImage, originalImage);

//    for(int i = 0; i < partOneResult.rows; i++) {
//        for(int j = 0; j < partOneResult.cols; j++) {
//            cout << int(partOneResult.at<uchar>(i, j)) << endl;
//        }
//    }

    //part 2: Bill Freeman propose
    blueImage = modified(blueImage, redImage);
    greenImage = modified(greenImage, redImage);

    vector<Mat> bgrChannel_2;
    bgrChannel_2.push_back(blueImage);
    bgrChannel_2.push_back(greenImage);
    bgrChannel_2.push_back(redImage);
    merge(bgrChannel_2, bfAlgorithmImage);
    partTwoResult = diff(bfAlgorithmImage, originalImage);

    improvementImage = diff(bfAlgorithmImage, demosaicImage);

    imwrite("demosaic.jpg", demosaicImage);
    imshow("demosaic", demosaicImage);
    imwrite("difference_1.jpg", partOneResult);
    imshow("partOneResult", partOneResult);
    imwrite("bfAlgorithm.jpg", bfAlgorithmImage);
    imshow("bfAlgorithm", bfAlgorithmImage);
    imwrite("difference_2.jpg", partTwoResult);
    imshow("partTwoResult", partTwoResult);
    imwrite("improvement.jpg", improvementImage);
    imshow("improvement", improvementImage);

    waitKey(0);
    return 0;
}

//use cv::filter2D to implement kernel on each channel
void filter(double kernel_data[], cv::Mat &mat) {
    Mat kernel = cv::Mat(3, 3, CV_64F, kernel_data);
    filter2D(mat, mat, mat.depth(), kernel);
}

// implement Bill Freeman propose on green and blue
Mat modified(cv::Mat &bgImage, cv::Mat &redImage) {
    Mat minusR = bgImage - redImage;
    Mat rMinus = redImage - bgImage;
    medianBlur(minusR, minusR, 3);
    medianBlur(rMinus, rMinus, 3);
    return  minusR + redImage - rMinus;
}

//implement difference
cv::Mat diff(cv::Mat &demosaicImage, cv::Mat &originalImage) {
    Mat diffImage = Mat::zeros(originalImage.rows, originalImage.cols, 0);
    int demosaicPointB, demosaicPointG, demosaicPointR, originalPointB, originalPointG, originalPointR;
    Mat diffImg = abs(demosaicImage - originalImage);
    for(int i = 0; i < originalImage.rows; i++) {
        for(int j = 0; j < originalImage.cols; j++) {
            int totalDiff = 0;
            demosaicPointB = demosaicImage.at<Vec3b>(i, j)[0];
            demosaicPointG = demosaicImage.at<Vec3b>(i, j)[1];
            demosaicPointR = demosaicImage.at<Vec3b>(i, j)[2];
            originalPointB = originalImage.at<Vec3b>(i, j)[0];
            originalPointG = originalImage.at<Vec3b>(i, j)[1];
            originalPointR = originalImage.at<Vec3b>(i, j)[2];
            totalDiff += pow((demosaicPointB - originalPointB), 2);
            totalDiff += pow((demosaicPointG - originalPointG), 2);
            totalDiff += pow((demosaicPointR - originalPointR), 2);
            diffImage.at<uchar>(i, j) = totalDiff;
        }
    }
    diffImage.convertTo(diffImage, CV_32FC1);
    sqrt(diffImage, diffImage);
    diffImage.convertTo(diffImage, CV_8UC1);
    return diffImage;
}
