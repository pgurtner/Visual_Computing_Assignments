//
// Created by uni on 10/29/26.
//
#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>


int main () {
    // rms was only around 0.72, but ChatGPT said that's still acceptable
    cv::Mat cameraMatrix = (cv::Mat_<double>(3, 3) <<
        1165.973932805081, 0.0, 948.4266181668049,
        0.0, 1169.80170222788, 532.6972746178853,
        0.0, 0.0, 1.0
    );
    cv::Mat distortionCoefficients = (cv::Mat_<double>(1, 5) <<
    0.04695031703360765, 0.0386444578469515, -0.004582798246147291, -0.0004132827740623321, -0.1862409065226672
        );

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

        cv::Mat left;
        cv::Mat right;
        if (reference_height >= grayFrame.rows) {
            left = ref;
            right = grayFrame;
        } else {
            left = grayFrame;
            right = ref;
        }
         cv::Mat paddedFrame(
            left.rows,
            right.cols,
            right.type(),
            cv::Scalar(0, 0, 0)
            );
        int cam_offset = (left.rows - right.rows) / 2;
        right.copyTo(paddedFrame(cv::Rect(
            0, cam_offset, right.cols, right.rows
        )));
        cv::hconcat(left, paddedFrame, combined);

        // features ==========================================================================
        sift->detectAndCompute(grayFrame, cv::noArray(), kps2, desc2);

        std::vector<std::vector<cv::DMatch>> matches;

        matcher->knnMatch(desc1, desc2, matches, 2);

        std::vector<cv::Point2f> pts1, pts2;
        std::vector<bool> usedWebcamFeatures(desc2.rows, false);

        for (const auto& match : matches) {
            if (match.size() < 2) continue;

            if (match[0].distance < 0.8 * match[1].distance) {
                int webcamIdx = match[0].trainIdx;

                // enforce a 1:1 feature mapping
                // it seemed like far too many features got mapped from reference to webcam
                // no idea if this is a proper solution
                if (!usedWebcamFeatures[webcamIdx]) {
                    pts1.push_back(kps1[match[0].queryIdx].pt);
                    pts2.push_back(kps2[match[0].trainIdx].pt);

                    usedWebcamFeatures[webcamIdx] = true;
                }
            }
        }

        if (pts1.size() >= 4) {
            // find homography ===============================================
            std::vector<cv::Point3f> objPts;
            std::vector<cv::Point2f> dstPts = pts2;
            // TODO: use actual scaling factors
            float scaleX_ = 0.005;//0.21/640; //physicalWidth / imageWidth (e.g. 0.21m / 640px)
            float scaleY_ = 0.005;//0.15/480; //physicalHeight / imageHeight (e.g. 0.15m / 480px)
            for (const auto& p : pts1)
                objPts.push_back({
                p.x * scaleX_, // pixel col → metric X
                p.y * scaleY_, // pixel row → metric Y
                0.0f // Z = 0 by assumption
                    });

            // rvec and tvec are the outputs here, from there use usual MVP model to display geometry
            // output from solvePnPRansac into these two can be ignored I think
            cv::Mat rvec;
            cv::Mat tvec;

            // - find inliers with ransac
            std::vector<int> inliers;
            bool ok = cv::solvePnPRansac(
                objPts, dstPts,
                cameraMatrix, distortionCoefficients,
                rvec, tvec,
                /*useExtrinsicGuess=*/false,
                /*iterationsCount=*/200,
                /*reprojectionError=*/4.0f,
                /*confidence=*/0.99,
                inliers,
                cv::SOLVEPNP_ITERATIVE);

            if (ok && inliers.size() >= 4) {
                // Refine on inliers only
                std::vector<cv::Point3f> inlierObj;
                std::vector<cv::Point2f> inlierImg;
                for (int idx : inliers)
                {
                    inlierObj.push_back(objPts[idx]);
                    inlierImg.push_back(dstPts[idx]);
                }

                cv::solvePnP(inlierObj, inlierImg, cameraMatrix, distortionCoefficients,
                rvec, tvec, /*useExtrinsicGuess=*/true,
                cv::SOLVEPNP_ITERATIVE);

                // draw cube =======================================================================
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
                    rvec,
                    tvec,
                    cameraMatrix,
                    distortionCoefficients,
                    projected
                );

                // adjust to merged frames
                for (cv::Point2f& pt : projected) {
                    if (reference_height >= grayFrame.rows) {
                        pt.x += reference_width;
                    }
                    pt.y += cam_offset;
                }

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
                    combined,
                    lines,
                    true,
                    cv::Scalar(0, 255, 0),
                    2
                );
            }
        }

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