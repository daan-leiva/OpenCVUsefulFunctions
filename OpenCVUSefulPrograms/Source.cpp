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

int main(int argc, char** argv) {
	int failed; // will store value of program to check if it successed
	int choice; // user choice to run program

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
	default:
		break;
	}

	if (failed)
		cout << "Program failed\n";
	else
		cout << "Program executed succesfully!\n";
}

void display_menu() {
	cout << "Select your program choice 1-n\n";
	cout << "1. Display image\n";
	cout << "2. Display video - basic\n";
	cout << "3. Display video - trackbar\n";
}

//
// Program # 1
//

// simple image display
// no processing
int display_image() {
	const char* image_file_path = R"(C:\Users\ResearchBeast\Documents\Visual Studio 2015\Projects\OpenCVUSefulPrograms\OpenCVUSefulPrograms\Debug\SamplePipeCrack.jpg)";
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
	cap.open(R"(C:\Users\ResearchBeast\Documents\E16-SMH-041-E16-SMH-039_012215_1372_1.MPG)");
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
	char* window_name = "Trackbar Example";
	namedWindow(window_name, WINDOW_AUTOSIZE);
	g_cap.open(R"(C:\Users\ResearchBeast\Documents\E16-SMH-041-E16-SMH-039_012215_1372_1.MPG)");
	if (!g_cap.isOpened())
		return -1;
	// total number of frames in the video
	int frames = (int)g_cap.get(cv::CAP_PROP_FRAME_COUNT);
	// width of video
	int tmpw = (int)g_cap.get(cv::CAP_PROP_FRAME_WIDTH);
	// height of video
	int tmph = (int)g_cap.get(cv::CAP_PROP_FRAME_HEIGHT);
	cout << "Video has " << frames << " frames of dimensions("
		<< tmpw << ", " << tmph << ")." << endl;
	// create trackbar with label
	//
	createTrackbar("Position", window_name, &g_slider_position, frames, onTrackbarSlide);
	Mat frame;
	while (1) {
		if (g_run != 0) {
			g_cap >> frame;
			if (!frame.data)
				break;
			int current_pos = (int)g_cap.get(cv::CAP_PROP_POS_FRAMES);
			g_dontset = 1;
			cv::setTrackbarPos("Position", window_name, current_pos);
			cv::imshow(window_name, frame);
			g_run -= 1;
		}
		char c = (char)cv::waitKey(1);
		if (c == 's') // single step
		{
			g_run = 1;
			cout << "Single step, run = " << g_run << endl;
		}
		if (c == 'r') // run mode
		{
			g_run = -1;
			cout << "Run mode, run = " << g_run << endl;
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