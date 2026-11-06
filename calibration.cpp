//
// Created by uni on 10/23/26.
//
#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>

int main() {
    /** A2.1 todo
     * save and reload the resulting calibration parameters;?? just store the numbers if they ask
     * evaluate whether the calibration result is reasonable; -> check rms
     */
    const int displayIndex = 2;

    // calibration ==========================================================
    const cv::Size boardSize(12, 7);

    // =================================================================
    std::vector<cv::Point3f> objectPattern;
    for (int row = 0; row < boardSize.height; row++) {
        for (int col = 0; col < boardSize.width; col++) {
            objectPattern.emplace_back(
                    col * 2.0, // Todo: do the real world scales belong here?
                    row * 2.0,
                    0.0f
            );
        }
    }

    // ================================ ================================
    std::vector<std::vector<cv::Point3f>> objectPoints;
    std::vector<std::vector<cv::Point2f>> imagePoints;

    std::vector<cv::String> imageFiles;
    cv::glob("./images/calibration/*", imageFiles);

    cv::Size imageSize;

    std::cout << "#imageFiles = " << imageFiles.size() << '\n';
    int debugging_counter = 0;
    for (const auto& filename : imageFiles) {
        cv::Mat image = cv::imread(filename);

        cv::Mat gray;
        cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
        imageSize = gray.size();

        std::vector<cv::Point2f> corners;
        bool success = cv::findChessboardCorners(gray, boardSize, corners);
        if (!success) {
            std::cout << "Could not find chessboard corners in image #" << debugging_counter << " " << filename << '\n';
        }

        // todo how to use this
        cv::cornerSubPix(
            gray,
            corners,
            cv::Size(11, 11),
            cv::Size(-1, -1),
            cv::TermCriteria(
                cv::TermCriteria::EPS |
                cv::TermCriteria::MAX_ITER,
                30,
                0.001
            )
        );

        imagePoints.push_back(corners);
        objectPoints.push_back(objectPattern);
        debugging_counter++;
    }

    cv::Mat cameraMatrix;
    cv::Mat distortionCoefficients;

    std::vector<cv::Mat> rotationVectors;
    std::vector<cv::Mat> translationVectors;

    double rms = cv::calibrateCamera(
        objectPoints,
        imagePoints,
        imageSize,
        cameraMatrix,
        distortionCoefficients,
        rotationVectors,
        translationVectors
    );

    std::cout << "rms = " << rms << std::endl;
    std::cout << "camera " << cameraMatrix << std::endl;
    std::cout << "distortion " << distortionCoefficients << std::endl;

    cv::Mat image = cv::imread(imageFiles[displayIndex]);
    const int frame_width = image.cols;
    const int frame_height = image.rows;

    std::vector<cv::Point3f> cubePoints = {
        {0, 0, 0},
        {1, 0, 0},
        {1, 1, 0},
        {0, 1, 0},
        {0, 0, -1},
        {1, 0, -1},
        {1, 1, -1},
        {0, 1, -1}
    };

    std::vector<cv::Point2f> projected;

    cv::projectPoints(
        cubePoints,
        rotationVectors[displayIndex],
        translationVectors[displayIndex],
        cameraMatrix,
        distortionCoefficients,
        projected
    );

    std::vector<std::vector<cv::Point>> lines = {
        // First square
        {projected[0], projected[1], projected[2], projected[3]},

        // Second square
        {projected[4], projected[5], projected[6], projected[7]},

        // Connecting edges
        {projected[0], projected[4]},
        {projected[1], projected[5]},
        {projected[2], projected[6]},
        {projected[3], projected[7]}
    };


    cv::polylines(
        image,
        lines,
        true,
        cv::Scalar(0, 255, 0),
        2
    );

    cv::imshow("Captured Frame", image);
    char key = cv::waitKey(1);
    while (key != 'q') {
        key = cv::waitKey(1);
    }

    cv::destroyAllWindows();

    return 0;
}
