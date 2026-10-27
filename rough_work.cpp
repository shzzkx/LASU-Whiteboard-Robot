#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

int main() {
    // B1: Load the input image.
    cv::Mat image = cv::imread("drawing.png");

    if (image.empty()) {
        std::cerr << "Could not load drawing.png\n";
        return 1;
    }

    // B2: Convert color to grayscale.
    cv::Mat gray;
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

    // B3: Convert to a binary image.
    // Dark lines become white; light background becomes black.
    cv::Mat binary;
    cv::threshold(
        gray, binary, 180, 255, cv::THRESH_BINARY_INV
    );

    // B4: Extract contour point sequences.
    std::vector<std::vector<cv::Point>> contours;

    cv::findContours(
        binary,
        contours,
        cv::RETR_LIST,
        cv::CHAIN_APPROX_SIMPLE
    );

    // Example drawing area: 300 mm wide by 200 mm tall.
    const double boardWidthMm = 300.0;
    const double boardHeightMm = 200.0;

    const double scaleX = boardWidthMm / image.cols;
    const double scaleY = boardHeightMm / image.rows;

    // Tune this to simplify more or less aggressively.
    const double epsilon = 2.0;

    for (const auto& contour : contours) {
        // Ignore tiny contours, often noise or specks.
        if (cv::contourArea(contour) < 10.0) {
            continue;
        }

        // B5a: Simplify the contour.
        std::vector<cv::Point> simplified;
        cv::approxPolyDP(
            contour, simplified, epsilon, true
        );

        std::cout << "Path:\n";

        // B5b: Convert pixel coordinates to millimetres.
        for (const auto& p : simplified) {
            double xMm = p.x * scaleX;
            double yMm = (image.rows - 1 - p.y) * scaleY;

            std::cout << "  x=" << xMm
                      << ", y=" << yMm << '\n';
        }
    }

    return 0;
}