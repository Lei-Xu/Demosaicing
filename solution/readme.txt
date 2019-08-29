For part one:

1.Use imread to load the original picture and the mosaic picture.
    mosaicImage = imread("image_set/oldwell_mosaic.bmp", IMREAD_GRAYSCALE);
    originalImage = imread("image_set/oldwell.jpg");

2.Split the mosaic into 3 channels.
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

3.Use simple linear interpolation for each channel respectively
    filter(kernel_data_bg, blueImage);
    filter(kernel_data_bg, greenImage);
    filter(kernel_data_r, redImage);
	

4.Set different kernel to different channel, according to simple linear 
interpolation( averaging the 4 or 2 nearest neighbours).
    void filter(float kernel_data[], cv::Mat &mat) {
    	Mat kernel = cv::Mat(3, 3, CV_32F, kernel_data);
    	filter2D(mat, mat, mat.depth(), kernel);
    }

5.Then merge the 3 channels after filtering them with kernel by creating a new channel and pushing bgr back.
    vector<Mat> bgrChannel_1;
    bgrChannel_1.push_back(blueImage);
    bgrChannel_1.push_back(greenImage);
    bgrChannel_1.push_back(redImage);
    merge(bgrChannel_1, demosaicImage);

6.Visualize the difference by squared differences between the original and reconstructed values for each pixel, summed over the three color channels.
    partOneResult = diff(demosaicImage, originalImage);
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

7.The result of part one is "difference_1.jpg"

For part two:

1.Create the modified R and B channels by adding the respective difference images by compute the difference images R-G and B-g between the respective interpolated channel and apply median filtering on it
    Mat modified(cv::Mat &bgImage, cv::Mat &redImage) {
    	Mat minusR = bgImage - redImage;
    	Mat rMinus = redImage - bgImage;
    	medianBlur(minusR, minusR, 3);
    	medianBlur(rMinus, rMinus, 3);
    	return  minusR + redImage - rMinus;
    }


2.Merge the modified R and B channels and G channel.
    vector<Mat> bgrChannel_2;
    bgrChannel_2.push_back(blueImage);
    bgrChannel_2.push_back(greenImage);
    bgrChannel_2.push_back(redImage);
    merge(bgrChannel_2, bfAlgorithmImage);

3.The result of part two is "difference_2.jpg"

4.The improvement of part two compared with part one is "improvement.jpg"

5.We can see there is a visible improvement of part two compared with part one
