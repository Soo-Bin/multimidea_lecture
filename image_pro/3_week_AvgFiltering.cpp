#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>

#include <opencv2/opencv.hpp>   
#include <opencv2/core/core.hpp>   
#include <opencv2/highgui/highgui.hpp>  

#define NUMBER 3

using namespace cv;

double** DoubleAlloc2(int width, int height);
int** IntAlloc2(int width, int height);
void DoubleFree2(double** image, int width, int height);
void IntFree2(int** image, int width, int height);
int** ReadImage(char* name, int* width, int* height);
void ImageShow(char* winname, int** image, int width, int height);
void WriteFile(char* name, int* data, int num);
void WriteFile(char* name, double* data, int num);

void streching2() //교수님이 수업한 내용
{
	int width, height;
	int** img = ReadImage("Hydrangeas.jpg", &width, &height);
	int** image_out = IntAlloc2(width, height);

	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++)
		{
			if (x == 0 || y == 0 || x >= width - 1 || y >= height - 1)
				image_out[y][x] = img[y][x];
			else {
				int sum = 0;
				for (int i = -1; i <= 1; i++)
					for (int j = -1; j <= 1; j ++)
						sum += img[y + i][x + j];

				image_out[y][x] = (int)(sum / 9.0 + 0.5);
			}
		}

	ImageShow("input image", img, width, height);
	ImageShow("output image", image_out, width, height);
	waitKey(0);

	IntFree2(img, width, height);
	IntFree2(image_out, width, height);
}

double AvgCalc(int** src, int avgNum, int x, int y, double** h)
{
	double sum = 0.0;
	
	for (int i = -avgNum; i <= avgNum; i++)
		for (int j = -avgNum; j <= avgNum; j++)
			sum += src[i + y][j + x] * h[i+avgNum][j+avgNum];

	//value /= NUMBER * NUMBER + 0.5;

	if (sum > 255)
		sum = 255;

	return sum;
}
void AvgFiltering(int** src, int width, int height, int** dst, double** h)
{
	int avgNum = NUMBER / 2;
	
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {

			if ((x - avgNum) < 0 || (x + avgNum) >= width || (y - avgNum) < 0 || (y + avgNum) >= height) {
				dst[y][x] = src[y][x];
			}
			else
				dst[y][x] = AvgCalc(src, avgNum, x, y, h);
		}
	}
}


void main_3_week_AvgFiltering()
{
	int width, height;
	int** img = ReadImage("Hydrangeas.jpg", &width, &height);
	int** image_out = IntAlloc2(width, height);
	double** h = (double**)DoubleAlloc2(NUMBER, NUMBER);

	double sum = 0.0;

	h[0][0] = 1;	h[0][1] = 0;	h[0][2] = -1;
	h[1][0] = 1;	h[1][1] = 0;	h[1][2] = -1;
	h[2][0] = 1;	h[2][1] = 0;	h[2][2] = -1;

	/*for (int i = 0; i < NUMBER; i++)
		for (int j = 0; j < NUMBER; j++)
			sum += h[i][j];

	for (int i = 0; i < NUMBER; i++)
		for (int j = 0; j < NUMBER; j++)
			h[i][j] /= sum;
			*/

	AvgFiltering(img, width, height, image_out, h);

	ImageShow("input image", img, width, height);
	ImageShow("output image", image_out, width, height);
	waitKey(0);

	IntFree2(img, width, height);
	IntFree2(image_out, width, height);
	DoubleFree2(h, NUMBER, NUMBER);
}