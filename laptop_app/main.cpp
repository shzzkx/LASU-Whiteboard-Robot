#include <opencv2/opencv.hpp>
#include <opencv2/geometry/2d.hpp>
#include <iostream>
#include <vector>
#include <fstream>

//TEMP for debugging
#include <filesystem>
namespace fs = std::filesystem;

// To create header file in vex_robot folder:
std::ofstream output(
    "C:/Users/skhan/vscode_projects/WhiteboardRobot/vex_robot/drawing_data.h"
);

int main() {
    // B1: Load the input image.
    cv::Mat image = cv::imread("drawings/square_drawing.png");
    
    //TEMP for debugging
    std::cout << "CWD: " << std::filesystem::current_path() << '\n';
    std::string path = "drawings/square_drawing.png";
    std::cout << "Exists: " << fs::exists(path) << '\n';
    std::cout << "Absolute: " << fs::absolute(path) << '\n';

    if (image.empty()) {
        std::cerr << "Could not load square_drawing.png\n";
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
        cv::RETR_EXTERNAL, // Previously RETR_LIST, but that caused contour lines drawn around (insetad of on) outline 
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

    // Visually confirm contour tracing worked
    // by creating image of red contours overlayed on original image.
    // This may be jagged, but the actual (x, y) coordinates will not be.
    cv::Mat debug = image.clone();
    cv::drawContours(debug, contours, -1, cv::Scalar(0, 0, 255), 2);
    cv::imwrite("debug_contours.png", debug);

    return 0;
}

/* --- Sample output for square_drawing.png ---
Path:
    x=221.534, y=163.432
    x=77.521, y=163.15
    x=77.521, y=34.0366
    x=221.849, y=34.0366
*/