#include <iostream>
#include <vector>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <stdlib.h>

using namespace std;

/** * Category constants relative to the downsampled image resolution
 */
const float R1 = 50.0f;
const float R2 = 100.0f;
const float R3 = 150.0f;
const float R4 = 200.0f;

// --- Detection mechanisms ---

int detectSmallCraters(cv::Mat gray, cv::Mat &img, ofstream &resultsTable, string imageName)
{
    vector<cv::Vec3f> circles;
    cv::HoughCircles(gray, circles, cv::HOUGH_GRADIENT, 1.7, 100, 70, 50, 20, 80);
    for (size_t i = 0; i < circles.size(); i++)
    {
        float x = circles[i][0];
        float y = circles[i][1];
        float r = circles[i][2];

        // Visual representation via rendering on the downsampled image substrate
        cv::circle(img, cv::Point(cvRound(x), cvRound(y)), (int)r, cv::Scalar(0, 255, 255), 2);

        // Populating the data tables with coordinates rescaled to the original dimensions (* 8)
        // Image, Category, X_orig, Y_orig, Radius_orig, Diameter_orig
        resultsTable << imageName << ",Small," << (x * 8) << "," << (y * 8) << "," << (r * 8) << "," << (r * 16) << "\n";
    }
    return circles.size();
}

int detectMediumCraters(cv::Mat gray, cv::Mat &img, ofstream &resultsTable, string imageName)
{
    vector<cv::Vec3f> circles;
    cv::HoughCircles(gray, circles, cv::HOUGH_GRADIENT, 1.5, 120, 65, 60, 80, 150);
    for (size_t i = 0; i < circles.size(); i++)
    {
        float x = circles[i][0];
        float y = circles[i][1];
        float r = circles[i][2];

        cv::circle(img, cv::Point(cvRound(x), cvRound(y)), (int)r, cv::Scalar(0, 255, 0), 2);
        resultsTable << imageName << ",Medium," << (x * 8) << "," << (y * 8) << "," << (r * 8) << "," << (r * 16) << "\n";
    }
    return circles.size();
}

int detectLargeCraters(cv::Mat gray, cv::Mat &img, ofstream &resultsTable, string imageName)
{
    vector<cv::Vec3f> circles;
    cv::HoughCircles(gray, circles, cv::HOUGH_GRADIENT, 1.5, 200, 70, 60, 150, 200);
    for (size_t i = 0; i < circles.size(); i++)
    {
        float x = circles[i][0];
        float y = circles[i][1];
        float r = circles[i][2];

        cv::circle(img, cv::Point(cvRound(x), cvRound(y)), (int)r, cv::Scalar(255, 0, 255), 2);
        resultsTable << imageName << ",Large," << (x * 8) << "," << (y * 8) << "," << (r * 8) << "," << (r * 16) << "\n";
    }
    return circles.size();
}

int detectExtraLargeCraters(cv::Mat gray, cv::Mat &img, ofstream &resultsTable, string imageName)
{
    vector<cv::Vec3f> circles;
    cv::HoughCircles(gray, circles, cv::HOUGH_GRADIENT, 1.5, 400, 90, 68, 200, 500);
    for (size_t i = 0; i < circles.size(); i++)
    {
        float x = circles[i][0];
        float y = circles[i][1];
        float r = circles[i][2];

        cv::circle(img, cv::Point(cvRound(x), cvRound(y)), (int)r, cv::Scalar(0, 0, 255), 2);
        resultsTable << imageName << ",Extra-Large," << (x * 8) << "," << (y * 8) << "," << (r * 8) << "," << (r * 16) << "\n";
    }
    return circles.size();
}

// --- MAIN EXECUTION ---

int main()
{
    // Increase limit for high-resolution images
    setenv("OPENCV_IO_MAX_IMAGE_PIXELS", "2147483648", 1);

    cv::String path("moon_images/*.TIF");
    vector<cv::String> fileNames;
    cv::glob(path, fileNames, false);

    if (fileNames.empty())
    {
        cout << "No images found! Verify if the folder 'moon_images' contains .TIF files." << endl;
        return -1;
    }

    // Results table header with rescaled dimension columns
    ofstream resultsTable("crater_detection_results.csv");
    resultsTable << "Image,Category,X_Original,Y_Original,Radius_Original,Diameter_Original\n";

    cout << "Located " << fileNames.size() << " images. Commencing processing..." << endl;

    for (size_t i = 0; i < fileNames.size(); i++)
    {
        cout << "\n[" << i + 1 << "/" << fileNames.size() << "] " << fileNames[i] << endl;

        try
        {
            // Loading at 1/8 scale to optimize memory consumption
            cv::Mat img = cv::imread(fileNames[i], cv::IMREAD_REDUCED_COLOR_8);

            if (img.empty())
            {
                cout << "Skipping entry: Image could not be initialized." << endl;
                continue;
            }

            cout << "Processed resolution (1/8 scale): " << img.cols << "x" << img.rows << endl;

            cv::Mat gray;
            cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
            cv::normalize(gray, gray, 0, 255, cv::NORM_MINMAX);
            cv::GaussianBlur(gray, gray, cv::Size(9, 9), 2);

            // Execute detection routines (automated rescale by factor of 8 performed internally)
            detectSmallCraters(gray, img, resultsTable, fileNames[i]);
            detectMediumCraters(gray, img, resultsTable, fileNames[i]);
            detectLargeCraters(gray, img, resultsTable, fileNames[i]);
            detectExtraLargeCraters(gray, img, resultsTable, fileNames[i]);

            // Save visualization output
            string outputName = "detected_" + to_string(i) + ".jpg";
            cv::imwrite(outputName, img);

            img.release();
            gray.release();
        }
        catch (const cv::Exception &e)
        {
            cerr << "ERROR encountered at image: " << fileNames[i] << " -> " << e.what() << endl;
            cerr << "Proceeding to next entry..." << endl;
            continue;
        }
    }

    resultsTable.close();
    cout << "\nProcessing complete! Native-scale data exported to crater_detection_results.csv" << endl;
    return 0;
}