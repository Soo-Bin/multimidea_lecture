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
void WriteFile(char* name, int* data, int num);
void WriteFile(char* name, double* data, int num);

void histogram(int** image, int width, int height, int* histogram)
{
	for (int i = 0; i < 256; i++) histogram[i] = 0;

	//hist 구하기
	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++)
			histogram[image[y][x]]++;
			
	WriteFile("hist.txt", histogram, 256);
}
void chistogram(int* histogram, int* chistogram)
{
	for (int i = 0; i < 256; i++) chistogram[i] = 0;

	//chist 구하기
	chistogram[0] = histogram[0];
	for (int i = 1; i < 256; i++)
		chistogram[i] = histogram[i] + chistogram[i - 1];

	WriteFile("cist.txt", chistogram, 256);
}
void norm_chistogram(int width, int height, int* chistogram, double* norm_chistogram)
{
	for (int i = 0; i < 256; i++) norm_chistogram[i] = 0;

	//norm_chist 구하기
	for (int i = 0; i < 256; i++)
		norm_chistogram[i] = (double)chistogram[i] / width / height;

	WriteFile("norm_cist.txt", norm_chistogram, 256);
}
void histogramEqualization(int** image, int width, int height, int** image_out, double* norm_chistogram)
{
	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++)
			image_out[y][x] = norm_chistogram[image[y][x]] * 255;
}

void main_2_week_histogram()
{
	int width, height;
	int** img = ReadImage("lena256_4histeq.bmp", &width, &height);
	int** image_out = IntAlloc2(width, height);
	
	int hist[256];
	int chist[256];
	double norm_chist[256];

	histogram(img, width, height, hist);
	chistogram(hist, chist);
	norm_chistogram(width, height, chist, norm_chist);
	histogramEqualization(img, width, height, image_out, norm_chist);

	ImageShow("input image", img, width, height);
	ImageShow("output image", image_out, width, height);
	waitKey(0);

	IntFree2(img, width, height);
	IntFree2(image_out, width, height);
}