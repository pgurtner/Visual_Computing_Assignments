//
// Created by uni on 10/29/26.
//
#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>


int main () {
    auto cam = cv::VideoCapture(0);
    // Check if the webcam was opened successfully
    if (!cam.isOpened()) {
        std::cout << "Error: Could not access the webcam." << std::endl;
        return -1;
    } else {
        std::cout << "Webcam accessed successfully!" << std::endl;
    }

    // SIFT setup ============================================================
    cv::Mat ref = cv::imread("images/target.png", cv::IMREAD_GRAYSCALE);
    int reference_height = ref.rows;
    int reference_width = ref.cols;

    // ---- Detect & describe (SIFT) -----------------------------------------
    auto sift = cv::SIFT::create( 2000);
    std::vector<cv::KeyPoint> kps1, kps2; cv::Mat desc1, desc2;
    sift->detectAndCompute(ref, cv::noArray(), kps1, desc1);

    cv::Mat frame;
    cv::Mat grayFrame;
    cv::Mat combined;

    cv::Ptr<cv::DescriptorMatcher> matcher = cv::BFMatcher::create(cv::NORM_L2);

    while (true) {
        auto ret = cam.read(frame);
        if (!ret) {
            std::cout << "Error: Could not capture frame." << std::endl;
        }
        cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);

        cv::Mat paddedFrame(
            reference_height,
            grayFrame.cols,
            grayFrame.type(),
            cv::Scalar(0, 0, 0)
            );
        int cam_offset = (reference_height - grayFrame.rows) / 2;
        grayFrame.copyTo(paddedFrame(cv::Rect(
            0, cam_offset, grayFrame.cols, grayFrame.rows
        )));

        // features ==========================================================================
        sift->detectAndCompute(grayFrame, cv::noArray(), kps2, desc2);

        std::vector<std::vector<cv::DMatch>> matches;

        // todo what is the exact meaning of k?
        matcher->knnMatch(desc1, desc2, matches, 2);

        std::vector<cv::Point2f> pts1, pts2;
        std::vector<cv::DMatch> goodMatches;
        std::vector<bool> usedWebcamFeatures(desc2.rows, false);

        for (const auto& match : matches) {
            if (match.size() < 2) continue;

            if (match[0].distance < 0.8 * match[1].distance) {
                int webcamIdx = match[0].trainIdx;

                // enforce a 1:1 feature mapping
                // it seemed like far too many features got mapped from reference to webcam
                // no idea if this is a proper solution
                if (!usedWebcamFeatures[webcamIdx]) {
                    goodMatches.push_back(match[0]);
                    pts1.push_back(kps1[match[0].queryIdx].pt);
                    pts2.push_back(kps2[match[0].trainIdx].pt);

                    usedWebcamFeatures[webcamIdx] = true;
                }
            }
        }

        // merge reference and camera ========================================================================
        cv::hconcat(ref, paddedFrame, combined);

        // draw lines =======================================================================
        std::vector<std::vector<cv::Point>> lines(pts1.size());
        for (int i = 0; i < pts1.size(); ++i) {
            pts2[i].x += reference_width;
            pts2[i].y += cam_offset;
            lines[i] = {pts1[i], pts2[i]};
        }

        cv::polylines(
            combined,
            lines,
            false,
            cv::Scalar(0, 255, 0),
            1
        );

        cv::imshow("Captured Frame", combined);

        char key = cv::waitKey(1);
        if (key == 'q') {
            break;
        }
    }

    cam.release();
    cv::destroyAllWindows();

    return 0;
}