//
// Created by uni on 10/23/26.
//
#include <opencv2/opencv.hpp>

int main () {
    const cv::Size boardSize(9, 6);

    // =================================================================
    std::vector<cv::Point3f> objectPattern;
    for (int row = 0; row < boardSize.height; row++) {
        for (int col = 0; col < boardSize.width; col++) {
            objectPattern.emplace_back(
                    col,
                    row,
                    0.0f
            );
        }
    }

    // ================================================================
    std::vector<std::vector<cv::Point3f>> objectPoints;
    std::vector<std::vector<cv::Point2f>> imagePoints;

    std::vector<cv::String> imageFiles;
    cv::glob("./images/*", imageFiles);

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

    std::cout << "image size: " << imageSize << '\n';

    std::cout << "\nCamera Matrix K:\n"
          << cameraMatrix << '\n';

    std::cout << "\nDistortion Coefficients:\n"
              << distortionCoefficients << '\n';

    std::cout << "\nRMS Reprojection Error: "
              << rms << " pixels\n";

}