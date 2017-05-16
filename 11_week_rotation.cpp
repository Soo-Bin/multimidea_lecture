#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>

#include <opencv2/opencv.hpp>   
#include <opencv2/core/core.hpp>   
#include <opencv2/highgui/highgui.hpp>  

using namespace cv;
#define PI 3.14159265359

int** IntAlloc2(int width, int height);
void IntFree2(int** image, int width, int height);
int** ReadImage(char* name, int* width, int* height);
void WriteImage(char* name, int** image, int width, int height);
void ImageShow(char* winname, int** image, int width, int height);
void CopyImage(int** src, int width, int height, int** dst);

int BilinearInterpolation(int** image, int width, int height, int y, int x)
{
	int orig_x = (int)x; //(xcenter + ((double)y - ycenter)*ss + ((double)x - xcenter)*cc)
	int orig_y = (int)y;

	double delta_x = x - orig_x;
	double delta_y = y - orig_y;
	double pixel;

	uchar P1, P2, P3, P4;

	if ((orig_y >= 0 && orig_y < height - 1) && (orig_x >= 0 && orig_x < width - 1)) {
		P1 = image[orig_y][orig_x];
		P2 = image[orig_y][orig_x + 1];
		P3 = image[orig_y + 1][orig_x];
		P4 = image[orig_y + 1][orig_x + 1];
	}
	else {
		P1 = 0;	P2 = 0;	P3 = 0;	P4 = 0;
	}

	if (x < 0 || x >= width - 1 || y < 0 || y >= height - 1)
		pixel = 0;
	else
		pixel = (1 - delta_y)*(P1*(1 - delta_x) + P2*(delta_x)) + (delta_y)*(P3*(1 - delta_x) + P4*(delta_x));

	return (int)pixel;
}

void Rotation(int** image, int width, int height, int** image_out, double degree)
{
	double radian = degree*PI / 180.0;
	double cc = cos(radian), ss = sin(-radian);
	double xcenter = (double)width / 2.0, ycenter = (double)height / 2.0;
		
	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++) {
			double _x = (xcenter + ((double)y - ycenter)*ss + ((double)x - xcenter)*cc);
			double _y = (ycenter + ((double)y - ycenter)*cc - ((double)x - xcenter)*ss);
			
			image_out[y][x] = BilinearInterpolation(image, width, height, _y, _x);
		}
}

void main_11_week_rotation()
{
	int width, height;
	int** image = ReadImage("Hydrangeas.jpg", &width, &height);
	int** image_out = (int**)IntAlloc2(width, height); // image_out[height][width]

	Rotation(image, width, height, image_out, 45);

	ImageShow("input window", image, width, height);
	ImageShow("output window", image_out, width, height);
	waitKey(0);

	IntFree2(image, width, height);
	IntFree2(image_out, width, height);

}
