#include <iostream>
#include <opencv2/opencv.hpp>

int main() {
    // cv::Mat img = cv::Mat::zeros(400, 400, CV_8UC3);
    // cv::putText(img, "Hello OpenCV", {50,200},
    // cv::FONT_HERSHEY_SIMPLEX, 1.0, {0,255,0}, 2);
    // cv::imshow("Demo", img);
    // cv::waitKey(0);

    auto cam = cv::VideoCapture(0);
    // Check if the webcam was opened successfully
    if (!cam.isOpened()) {
        std::cout << "Error: Could not access the webcam." << std::endl;
        return -1;
    } else {
        std::cout << "Webcam accessed successfully!" << std::endl;
    }

    int frame_width = (int) cam.get(cv::CAP_PROP_FRAME_WIDTH);
    int frame_height = (int) cam.get(cv::CAP_PROP_FRAME_HEIGHT);

    // auto fourcc = cv::VideoWriter::fourcc("mp4v");
    // cv::Size frame_size(frame_width, frame_height);
    // auto out = cv::VideoWriter("output.mp4", fourcc ,20.0, frame_size);

    // const std::vector kernel_vec = {1.0/16, 2.0/16, 1.0/16, 2.0/16, 4.0/16, 2.0/16, 1.0/16, 2.0/16, 1.0/16};
    // const cv::Mat gaussian_blur = cv::Mat(kernel_vec).reshape(1, 3);
    // //std::cout << gaussian_blur << std::endl;
    //
    // cv::Mat gaussian_blur2 = (cv::Mat_<double>(3, 3) <<
    //     1.0/16, 2.0/16, 1.0/16,
    //     2.0/16, 4.0/16, 2.0/16,
    //     1.0/16, 2.0/16, 1.0/16);
    // //std::cout << gaussian_blur2 << std::endl;
    //
    // // cv::Mat blur = (cv::Mat_<double>(3, 3) <<
    // //     1.0/9, 1.0/9, 1.0/9,
    // //     1.0/9, 1.0/9, 1.0/9,
    // //     1.0/9, 1.0/9, 1.0/9);
    //
    // cv::Mat sharpening = (cv::Mat_<float>(3,3) <<
    //                0, -1, 0,
    //               -1, 5, -1,
    //                0, -1, 0);



    cv::Mat frame;
    // cv::Mat gaussian_blurred_frame;
    // cv::Mat blurred_frame;
    // cv::Mat sharpened_frame;

    std::vector<cv::Point3f> cube = {
        {-1, -1, -1},
        { 1, -1, -1},
        { 1,  1, -1},
        {-1,  1, -1},

        {-1, -1,  1},
        { 1, -1,  1},
        { 1,  1,  1},
        {-1,  1,  1}
    };
    double f = 500.0;
    double cu = frame_width / 2.0;
    double cv = frame_height / 2.0;

    cv::Mat K = (cv::Mat_<double>(3, 3) <<
        f, 0, cu,
        0, f, cv,
        0, 0, 1
        );
    // // A single 3D point in front of the camera (extrinsic = identity)
    // cv::Point3f P(0.3f, -0.1f, 2.0f); // (x, y, z) in camera frame
    // // projection manually calculated
    // double u_manual = f * P.x / P.z + cu;
    // double v_manual = f * P.y / P.z + cv;

    // Extrinsic = identity: rvec = 0, tvec = 0
    cv::Mat rvec = cv::Mat::zeros(3, 1, CV_64F);
    cv::Mat tvec = (cv::Mat_<float>(3, 1) <<
        0,
        0,
        5
    );
    cv::Mat dist = cv::Mat::zeros(5, 1, CV_64F); // no distortion

    double theta = 0.01;
    cv::Mat rotation = (cv::Mat_<float>(4, 4) <<
        std::cos(theta), 0, std::sin(theta), 0,
        0, 1, 0, 0,
        -std::sin(theta), 0, std::cos(theta), 0,
        0, 0, 0, 1
    );

    while (true) {
        auto ret = cam.read(frame);
        if (!ret) {
            std::cout << "Error: Could not capture frame." << std::endl;
        }

        //cv::filter2D(frame, filtered_frame, -1, filter);
        //cv::filter2D(frame, blurred_frame, -1, blur);
        // cv::filter2D(frame, gaussian_blurred_frame, -1, gaussian_blur);
        // cv::filter2D(frame, sharpened_frame, -1, sharpening);


        //rotate cube ================================================================================================
        for (cv::Point3f &p : cube) {
            cv::Mat p4 = (cv::Mat_<float>(4, 1) <<
                p.x,
                p.y,
                p.z,
                1
            );

            cv::Mat rotated_point = rotation * p4;

            p.x = rotated_point.at<float>(0, 0) / rotated_point.at<float>(3, 0);
            p.y = rotated_point.at<float>(1, 0) / rotated_point.at<float>(3, 0);
            p.z = rotated_point.at<float>(2, 0) / rotated_point.at<float>(3, 0);
        }


        // projection ==================================================================================================
        std::vector<cv::Point2f> projected;
        cv::projectPoints(cube, rvec, tvec, K, dist, projected);

        // std::cout << "printing projected coordinates" << std::endl;
        // for (auto x : projected) {
        //     std::cout << x << std::endl;
        // }

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
            frame,
            lines,
            true,
            cv::Scalar(0, 255, 0),
            2
        );


        //out.write(frame);
        cv::imshow("Captured Frame", frame);
        // //cv::imshow("Blurred Frame", blurred_frame);
        // cv::imshow("Gaussian blurred Frame", gaussian_blurred_frame);
        // cv::imshow("Sharpened Frame", sharpened_frame);

        char key = cv::waitKey(1);
        if (key == 'q') {
            break;
        }
    }

    cam.release();
    //out.release();
    cv::destroyAllWindows();


    return 0;
}
/*
 *
 *
 */