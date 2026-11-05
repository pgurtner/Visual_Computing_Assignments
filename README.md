# setup
OpenGL libraries are only needed for assignment 1. (I wrote the code in one git project, if the assignment 1 stuff is too annoying, I can extract the assignment 2 parts)

Libraries: libglm-dev, mesa-utils, build-essential, cmake, pkg-config, libgl1-mesa-dev, libglu1-mesa-dev, libglfw3-dev, libopencv-dev.

Some Linux defaults might have been left out.

# Assignment 2
`assignment2.cpp` is the final submission for assignment 2, i.e. checkpoint A2.3. `calibration.cpp` is checkpoint A2.1, `features.cpp` is checkpoint A2.2

todo: get this into the code: Important note: (as of the day before the deadline) the code requires the webcam image to be smaller regarding height than the reference image. Otherwise, the cv::hconcat for merging reference image and webcam into one window call will probably fail. It should work when you exchange variables corresponding to the reference image and to the webcam image with each other from the `paddedFrame` variable initialization to the `cv::hconcat` call.
The adjusted code should be 
```
cv::Mat paddedFrame(
    grayFrame.rows,
    reference_width,
    ref.type(),
    cv::Scalar(0, 0, 0)
    );
int cam_offset = (grayFrame.rows - reference_height) / 2;
ref.copyTo(paddedFrame(cv::Rect(
    0, cam_offset, reference_width, reference_height
)));
cv::hconcat(grayFrame, paddedFrame, combined);
```
but I couldn't test it yet. The coordinate adjustments to the frame concatenations might also have to be switched to
```
for (cv::Point2f& pt : projected) {
    pt.x += grayFrame.cols;
    pt.y += cam_offset;
}
```

ideally I remember to ask this during presentation and remove this note before uploading: The camera calibration parameters were estimated from the sample images from the lecture because I don't have a printer at home. I can't see obvious distortions in the projected cube, so I hope that's ok. I also skipped the storing and loading calibration parameters part from some checkpoint by pasting them directly into the code for the last checkpoint. If that functionality is demanded I can add it of course.

## Compile and Run
```bash
cmake -S . -B ./build
cmake -B ./build
./build/assignment2
```

## usage
* close window by pressing q
* hold copy of images/target.png into the webcam, then a cube with estimated pose should appear

# Assignment 1
`main.cpp` contains the first few OpenCV assignments, which you can ignore. `myfirstopenglprogram.cpp` is assignment 1.

## Compile and Run
```bash
cmake -S . -B ./build
cmake -B ./build
./build/MyOpenGLProject
```

## usage
* press P to pause rotation (simple interaction)
* press F to use the webcam filter