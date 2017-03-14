#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>

#include <opencv2/opencv.hpp>   
#include <opencv2/core/core.hpp>   
#include <opencv2/highgui/highgui.hpp>  

using namespace cv;

int** IntAlloc2(int width, int height);
void IntFree2(int** image, int width, int height);
int** ReadImage(char* name, int* width, int* height);
void ImageShow(char* winname, int** image, int width, int height);

void main_1_week()
{
	int width, height;
	int** img = ReadImage("Hydrangeas.jpg", &width, &height);
	int** image_out = IntAlloc2(width, height);

	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++)
		{
			if (200 <= img[y][x] && img[y][x] < 255)
				image_out[y][x] = (img[y][x] - 255) * 55 / 105 + 255;
			else if (50 <= img[y][x] && img[y][x] < 200)
				image_out[y][x] = (img[y][x] - 50) * 175 / 100 + 25;
			else
				image_out[y][x] = img[y][x] / 2;

			if (image_out[y][x] > 255)
				image_out[y][x] = 255;
			else if (image_out[y][x] < 0)
				image_out[y][x] = 0;

		}

	ImageShow("input image", img, width, height);
	ImageShow("output image", image_out, width, height);
	waitKey(0);

	IntFree2(img, width, height);
	IntFree2(image_out, width, height);
}