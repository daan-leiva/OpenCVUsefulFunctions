#include <opencv2\highgui\highgui.hpp> // files for user interface
#include <opencv2\imgproc\imgproc.hpp> // files to process images
#include <iostream>
#include <fstream>

using namespace std; // for IO purposes
using namespace string_literals; // in rder to use the 'R' raw string literal
using namespace cv; // opencv functions/variables

// function calls
void display_menu();
int display_image();
int display_video_basic();
int display_video_trackbar();
void onTrackbarSlide(int, void*);
int image_transform_gausian_blur();
int image_transform_blur();
int image_transform_median_blur();
int image_transform_bilateral_filter();

// global variables
const char* image_file_path = R"(C:\Users\ResearchBeast\Documents\Visual Studio 2015\Projects\OpenCVUSefulPrograms\OpenCVUSefulPrograms\Debug\SamplePipeCrack.jpg)";
const char* video_file_path = R"(C:\Users\ResearchBeast\Documents\E16-SMH-041-E16-SMH-039_012215_1372_1.MPG)";

int main(int argc, char** argv) {
	int failed; // will store value of program to check if it successed
	int choice; // user choice to run program
	while (1)
	{
		display_menu();
		cin >> choice;

		switch (choice) {
		case 1:
			failed = display_image();
			break;
		case 2:
			failed = display_video_basic();
			break;
		case 3:
			failed = display_video_trackbar();
			break;
		case 4:
			failed = image_transform_gausian_blur();
			break;
		case 5:
			failed = image_transform_blur();
			break;
		case 6:
			failed = image_transform_median_blur();
			break;
		case 7:
			failed = image_transform_bilateral_filter();
			break;
		default:
			break;
		}

		if (failed)
			cout << "Program failed\n";
		else
			cout << "Program executed succesfully!\n";
	}
}

void display_menu() {
	cout << "Select your program choice 1-n\n";
	cout << "1. Display image\n";
	cout << "2. Display video - basic\n";
	cout << "3. Display video - trackbar\n";
	cout << "4. Image transform - Gausian Blur\n";
	cout << "5. Image transform - Blur\n";
	cout << "6. Image transform - Median Blur\n";
	cout << "7. Image transform - Bilateral Filter -- ((Still fixing))\n";
}

//
// Program # 1
//

// simple image display
// no processing
int display_image() {
	Mat img = imread(image_file_path); // standard opencv object to hold matrices

	// check if img loaded
	if (img.empty())
		return -1;
	// create a window to display image
	char* window_name = "Example image";
	namedWindow(window_name, WINDOW_AUTOSIZE);
	// push image to window
	// correct window is called by name, not variable
	// image is called by variable name or reference
	imshow(window_name, img);
	// waits for key press
	// 0 means wait indefinetely (otherwise it's the number of milliseconds)
	waitKey(0);
	// destroys window object by name
	// deallocates memory attached to window
	destroyWindow(window_name);
	// succesful execution
	return 0;
}

//
// Program #2
//

// simple video display
int display_video_basic() {
	char* window_name = "Example video";
	namedWindow(window_name, WINDOW_AUTOSIZE);
	// VideoCaptures contains information about the video about to be displayed
	VideoCapture cap;
	cap.open(video_file_path);
	// check that video opened succesfully
	if (!cap.isOpened())
		return -1;
	// video is played frame by frame
	// the mat object will hold the current frame being displayed
	Mat frame;
	// continours play until internal end check breaks out
	while (1) {
		// load current frame from stream
		cap >> frame;
		// check that there is any frames left
		if (!frame.data)
			break;
		// load frame to window
		imshow(window_name, frame);
		// waits 33 ms to take user input to exit loop
		if (waitKey(33) >= 0)
			break;
	}
	return 0;
}

//
// Program #3
//

// variables for trackbar program
// keeps track globally of the trackbar position
int g_slider_position = 0;

// controls whether the video is runnning or not
// g_run > 0 : number of frames displayed before stopping
// g_run < 0 : systems runs in continuous video mode
// g_run == 0 : video is not running
int g_run = 1;

// 
int g_dontset = 0;

// make this a global variable so that display_vide_trackbar
// and onTrackbarSlide can access it
VideoCapture g_cap;

// video with track bar and frame step
int display_video_trackbar() {
	cv::namedWindow("Example2_4", cv::WINDOW_AUTOSIZE);
	g_cap.open(video_file_path);
	int frames = (int)g_cap.get(cv::CAP_PROP_FRAME_COUNT);
	int tmpw = (int)g_cap.get(cv::CAP_PROP_FRAME_WIDTH);
	int tmph = (int)g_cap.get(cv::CAP_PROP_FRAME_HEIGHT);
	cout << "Video has " << frames << " frames of dimensions("
		<< tmpw << ", " << tmph << ")." << endl;
	cv::createTrackbar("Position", "Example2_4", &g_slider_position, frames,
		onTrackbarSlide);
	cv::Mat frame;
	while (1) {
		if (g_run != 0) {
			g_cap >> frame; if (!frame.data) break;
			int current_pos = (int)g_cap.get(cv::CAP_PROP_POS_FRAMES);
			g_dontset = 1;
			cv::setTrackbarPos("Position", "Example2_4", current_pos);
			cv::imshow("Example2_4", frame);
			g_run -= 1;
		}
		char c = (char)cv::waitKey(10);
		if (c == 's') // single step
		{
			g_run = 1; cout << "Single step, run = " << g_run << endl;
		}
		if (c == 'r') // run mode
		{
			g_run = -1; cout << "Run mode, run = " << g_run << endl;
		}
		if (c == 27)
			break;
	}
	return(0);
}

// function to call trackbar
void onTrackbarSlide(int pos, void *) {
	// VideoCapture.set : allows ou to configure properties of the VideoCapture object
	// we are seting the read position in units of frames
	g_cap.set(cv::CAP_PROP_POS_FRAMES, pos);
	if (!g_dontset)
		g_run = 1;
	g_dontset = 0;
}

// simple transformation of image
int image_transform_gausian_blur() {
	Mat image = imread(image_file_path);

	// Create some windows to show the input
	// and output images in.
	//
	cv::namedWindow("Image in", cv::WINDOW_AUTOSIZE);
	cv::namedWindow("Gaussian Blur", cv::WINDOW_AUTOSIZE);
	// Create a window to show our input image
	//
	cv::imshow("Image in", image);
	// Create an image to hold the smoothed output
	cv::Mat out;
	// Do the smoothing
	// Could use GaussianBlur(), blur(), medianBlur() or bilateralFilter().
	cv::GaussianBlur(image, out, cv::Size(5, 5), 3, 3);
	cv::GaussianBlur(out, out, cv::Size(5, 5), 3, 3);
	// Show the smoothed image in the output window
	//
	cv::imshow("Gaussian Blur", out);
	// Wait for the user to hit a key, windows will self destruct
	//
	cv::waitKey(0);
	return(0);
}

// simple transformation of image
int image_transform_blur() {
	Mat image = imread(image_file_path);

	// Create some windows to show the input
	// and output images in.
	//
	cv::namedWindow("Image in", cv::WINDOW_AUTOSIZE);
	cv::namedWindow("Blur", cv::WINDOW_AUTOSIZE);
	// Create a window to show our input image
	//
	cv::imshow("Image in", image);
	// Create an image to hold the smoothed output
	cv::Mat out;
	// Do the smoothing
	// Could use GaussianBlur(), blur(), medianBlur() or bilateralFilter().
	cv::blur(image, out, cv::Size(5, 5));
	cv::blur(out, out, cv::Size(5, 5));
	// Show the smoothed image in the output window
	//
	cv::imshow("Blur", out);
	// Wait for the user to hit a key, windows will self destruct
	//
	cv::waitKey(0);
	return(0);
}

// simple transformation of image
int image_transform_median_blur() {
	Mat image = imread(image_file_path);

	// Create some windows to show the input
	// and output images in.
	//
	cv::namedWindow("Image in", cv::WINDOW_AUTOSIZE);
	cv::namedWindow("Median Blur", cv::WINDOW_AUTOSIZE);
	// Create a window to show our input image
	//
	cv::imshow("Image in", image);
	// Create an image to hold the smoothed output
	cv::Mat out;
	// Do the smoothing
	// Could use GaussianBlur(), blur(), medianBlur() or bilateralFilter().
	cv::medianBlur(image, out, 5);
	cv::medianBlur(out, out, 5);
	// Show the smoothed image in the output window
	//
	cv::imshow("Median Blur", out);
	// Wait for the user to hit a key, windows will self destruct
	//
	cv::waitKey(0);
	return(0);
}

// simple trasformation of an image
int image_transform_bilateral_filter() {
	Mat image = imread(image_file_path);

	// Create some windows to show the input
	// and output images in.
	//
	cv::namedWindow("Image in", cv::WINDOW_AUTOSIZE);
	cv::namedWindow("Bilateral Filter", cv::WINDOW_AUTOSIZE);
	// Create a window to show our input image
	//
	cv::imshow("Image in", image);
	// Create an image to hold the smoothed output
	cv::Mat out;
	// Do the smoothing
	// Could use GaussianBlur(), blur(), medianBlur() or bilateralFilter().
	cv::bilateralFilter(image, out, 5, 3, 3);
	cv::bilateralFilter(out, out, 5, 3, 3);
	// Show the smoothed image in the output window
	//
	cv::imshow("Bilateral Filter", out);
	// Wait for the user to hit a key, windows will self destruct
	//
	cv::waitKey(0);
	return(0);
}

int complex_transformation() {
	cv::Mat img = cv::imread(argv[1]), img2;
	cv::namedWindow("Example1", cv::WINDOW_AUTOSIZE);
	cv::namedWindow("Example2", cv::WINDOW_AUTOSIZE);
	cv::imshow("Example1", img);
	cv::pyrDown(img, img2);
	cv::imshow("Example2", img2);
	cv::waitKey(0);
	return 0;
}

int image_transformation_canny_edge() {

}

int camera_input() {

}


