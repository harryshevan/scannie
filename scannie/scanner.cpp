//
//  scanner.cpp
//  scannie
//
//  Created by harrysh on 02.01.2025.
//

#include <opencv2/opencv.hpp>

#include <stdio.h>
#include <math.h>
#include <iostream>

#include <filesystem>

namespace fs = std::filesystem;

using namespace cv;
using namespace std;


// extern "C" {
//     vector<uchar> processImage(const uchar *inputData, size_t inputSize);
// }

bool compareContourAreas(std::vector<cv::Point> contour1, std::vector<cv::Point> contour2)
{
    double i = fabs(contourArea(cv::Mat(contour1)));
    double j = fabs(contourArea(cv::Mat(contour2)));
    return (i > j);
}

bool compareXCords(Point p1, Point p2)
{
    return (p1.x < p2.x);
}

bool compareYCords(Point p1, Point p2)
{
    return (p1.y < p2.y);
}

bool compareDistance(pair<Point, Point> p1, pair<Point, Point> p2)
{
    return (norm(p1.first - p1.second) < norm(p2.first - p2.second));
}

double _distance(Point p1, Point p2)
{
    return sqrt(((p1.x - p2.x) * (p1.x - p2.x)) +
                ((p1.y - p2.y) * (p1.y - p2.y)));
}

void resizeToHeight(Mat src, Mat &dst, int height)
{
    Size s = Size(src.cols * (height / double(src.rows)), height);
    resize(src, dst, s, INTER_AREA);
}

void orderPoints(vector<Point> inpts, vector<Point> &ordered)
{
    sort(inpts.begin(), inpts.end(), compareXCords);
    vector<Point> lm(inpts.begin(), inpts.begin() + 2);
    vector<Point> rm(inpts.end() - 2, inpts.end());

    sort(lm.begin(), lm.end(), compareYCords);
    Point tl(lm[0]);
    Point bl(lm[1]);
    vector<pair<Point, Point>> tmp;
    for (size_t i = 0; i < rm.size(); i++)
    {
        tmp.push_back(make_pair(tl, rm[i]));
    }

    sort(tmp.begin(), tmp.end(), compareDistance);
    Point tr(tmp[0].second);
    Point br(tmp[1].second);

    ordered.push_back(tl);
    ordered.push_back(tr);
    ordered.push_back(br);
    ordered.push_back(bl);
}

void fourPointTransform(Mat src, Mat &dst, vector<Point> pts)
{
    vector<Point> ordered_pts;
    orderPoints(pts, ordered_pts);

    double wa = _distance(ordered_pts[2], ordered_pts[3]);
    double wb = _distance(ordered_pts[1], ordered_pts[0]);
    double mw = max(wa, wb);

    double ha = _distance(ordered_pts[1], ordered_pts[2]);
    double hb = _distance(ordered_pts[0], ordered_pts[3]);
    double mh = max(ha, hb);

    Point2f src_[] =
        {
            Point2f(ordered_pts[0].x, ordered_pts[0].y),
            Point2f(ordered_pts[1].x, ordered_pts[1].y),
            Point2f(ordered_pts[2].x, ordered_pts[2].y),
            Point2f(ordered_pts[3].x, ordered_pts[3].y),
        };
    Point2f dst_[] =
        {
            Point2f(0, 0),
            Point2f(mw - 1, 0),
            Point2f(mw - 1, mh - 1),
            Point2f(0, mh - 1)};
    Mat m = getPerspectiveTransform(src_, dst_);
    warpPerspective(src, dst, m, Size(mw, mh));
}

void preProcess(Mat src, Mat &dst)
{
    cv::Mat imageGrayed;
    cv::Mat imageOpen, imageClosed, imageBlurred;

    cvtColor(src, imageGrayed, COLOR_BGR2GRAY);

    cv::Mat structuringElmt = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(4, 4));
    morphologyEx(imageGrayed, imageOpen, cv::MORPH_OPEN, structuringElmt);
    morphologyEx(imageOpen, imageClosed, cv::MORPH_CLOSE, structuringElmt);

    GaussianBlur(imageClosed, imageBlurred, Size(151, 151), 0);
    adaptiveThreshold(imageBlurred, dst, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 31, 11);
}

string getOutputFileName(string path, string name)
{
    fs::path filePath = path;
    return filePath.stem().string() + "_" + name + filePath.extension().string();
}

bool isRectangle(const vector<Point>& contour) {
  double peri = arcLength(contour, true);
  vector<Point> approx;
  approxPolyDP(contour, approx, 0.02 * peri, true);
  return approx.size() == 4;  // It's approximately a rectangle if it has 4 vertices
}

void processImage(string image_name, Mat image, Mat &warped)
{
    double ratio = image.rows / 500.0;
//    Mat orig = image.clone();
    warped = image.clone();
    // Further processing on the warped image
    cvtColor(warped, warped, COLOR_BGR2GRAY, 1);
    // GaussianBlur(warped, warped, Size(9, 9), 0);
//    adaptiveThreshold(warped, warped, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 15, 11);


}

extern "C" {
    unsigned char* MagicProcessImage(const unsigned char* image_data, size_t data_length, int* out_width, int* out_height, int* out_sz) {
        if (!image_data || data_length <= 0) {
            *out_width = 0;
            *out_height = 0;
            *out_sz = 0;
            return nullptr;
        }

        // Decode the image data into a cv::Mat
        std::vector<unsigned char> buffer(image_data, image_data + data_length);
        cv::Mat image = cv::imdecode(buffer, cv::IMREAD_COLOR);
        cv::Mat warped;

        // Check if image decoding was successful
        if (image.empty()) {
            *out_width = 0;
            *out_height = 0;
            *out_sz = 0;
            return nullptr;
        }

        // Perform some image processing
        // Ensure this function works correctly and doesn't leave 'warped' empty
        processImage("example", image, warped);

        // Verify that processing did something
        if (warped.empty()) {
            *out_width = 0;
            *out_height = 0;
            *out_sz = 0;
            return nullptr;
        }

        // Encode the processed image back to a byte array
        std::vector<unsigned char> outputBuffer;
        if (!cv::imencode(".png", warped, outputBuffer)) {
            *out_width = 0;
            *out_height = 0;
            *out_sz = 0;
            return nullptr;
        }

        // Output image dimensions
        *out_width = warped.cols;
        *out_height = warped.rows;
        *out_sz = static_cast<int>(outputBuffer.size());


        // Create a copy of the data to return (caller must free this memory)
        unsigned char* outputData = static_cast<unsigned char*>(malloc(outputBuffer.size()));
        if (outputData) {
            std::copy(outputBuffer.begin(), outputBuffer.end(), outputData);
        } else {
            *out_sz = 0;
        }

        return outputData;
    }
}
