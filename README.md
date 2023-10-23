# Images2Video
Reads all images in a given folder and exports them as one single video.

Compile with CMake.
For Windows, binaries for ffmpeg are delivered with the code.
For Macos, use `brew install ffmpeg` and let the CMakeLists.txt file point to the correct location of your ffmpeg installation.
Not tested on Linux, but should be doable to adapt the CMakeLists.txt file to your ffmpeg installation.



Usage: Images2Video "images folde>" "video file" "[options]"

The images in the images folder are expected to have a number in their filename. The files will be sorted according to the number they contain. So ideally you have a folder with image names im0.png, im1.png, ..., im3500.png, ... .

By default only png images are read from the images folder. If you want to read jpg or bmp images, add the option -jpg or -bmp.
