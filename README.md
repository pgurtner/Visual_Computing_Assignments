# setup
OpenGL libraries are only needed for assignment 1. (I wrote the code in one git project, if the assignment 1 stuff is too annoying, I can extract the assignment 2 parts)

Libraries: libglm-dev, mesa-utils, build-essential, cmake, pkg-config, libgl1-mesa-dev, libglu1-mesa-dev, libglfw3-dev, libopencv-dev.

Some Linux defaults might have been left out.

# Assignment 2
`assignment2.cpp` is the final submission for assignment 2, i.e. checkpoint A2.3. `calibration.cpp` is checkpoint A2.1, `features.cpp` is checkpoint A2.2

## Compile and Run
```bash
cmake -S . -B ./build
cmake --build ./build
./build/Assignment2
```

## usage
* close window by pressing q
* hold copy of images/target.png into the webcam, then a cube with estimated pose should appear

# Assignment 1
`main.cpp` contains the first few OpenCV assignments, which you can ignore. `myfirstopenglprogram.cpp` is assignment 1.

## Compile and Run
```bash
cmake -S . -B ./build

# this line had a typo in the submission for assignment 1
cmake --build ./build
# originally it read "cmake -B ./build"

./build/MyOpenGLProject
```

## usage
* press P to pause rotation (simple interaction)
* press F to use the webcam filter
