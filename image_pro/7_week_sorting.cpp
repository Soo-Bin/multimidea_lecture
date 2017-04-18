#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <opencv2/opencv.hpp>   
#include <opencv2/core/core.hpp>   
#include <opencv2/highgui/highgui.hpp> 

#define NUMBER 3

#define imax(a,b) ((a > b) ?/*yes*/a : /*no*/b)
#define imin(a,b) ((a < b) ?/*yes*/a : /*no*/b)
#define SWAP(a, b) { (a) ^= (b) ^= (a) ^= (b); }

using namespace cv;

int** IntAlloc2(int width, int height);
void DoubleFree2(double** image, int width, int height);
void IntFree2(int** image, int width, int height);
int** ReadImage(char* name, int* width, int* height);
void ImageShow(char* winname, int** image, int width, int height);
void WriteFile(char* name, int* data, int num);
void WriteFile(char* name, double* data, int num);
void WriteImage(char* name, int** image, int width, int height);

int FindMax(int* data, int num) 
{
	int max_value = data[0];

	for (int i = 0; i < num; i++) {
		max_value = imax(data[i], max_value);
	}
	return (max_value);

}
int FindMin(int* data, int num) 
{

	int min_value = data[0];

	for (int i = 0; i < num; i++) {
		min_value = imin(data[i], min_value);
	}
	return (min_value);

}

void BubbleSorting(int* data, int num)
{
	for (int j = num - 1; j > 0; j--) {
		for (int i = 0; i < j; i++) {
			if (data[i + 1] > data[i])
				SWAP(data[i + 1], data[i]);
		}
	}
}

int Dilation_C(int** src, int avgNum, int x, int y)
{
	int data[NUMBER*NUMBER];
	int num = 0;

	for (int i = -avgNum; i <= avgNum; i++) {
		for (int j = -avgNum; j <= avgNum; j++) {
			data[num] = src[i + y][j + x];
			num++;
		}
	}

	int max_value = FindMax(data, NUMBER*NUMBER);
	max_value = imax(imin(max_value, 255), 0); //0~255 사이에 있으면 a=c 255 크면 255 0 작으면 0

	return max_value;
}
int Erosion_C(int** src, int avgNum, int x, int y)
{
	int data[NUMBER*NUMBER];
	int num = 0;

	for (int i = -avgNum; i <= avgNum; i++) {
		for (int j = -avgNum; j <= avgNum; j++) {
			data[num] = src[i + y][j + x];
			num++;
		}
	}

	int min_value = FindMin(data, NUMBER*NUMBER);
	min_value = imax(imin(min_value, 255), 0);

	return min_value;
}

int Median_C(int** src, int avgNum, int x, int y)
{
	int data[NUMBER*NUMBER];
	int num = 0;

	for (int i = -avgNum; i <= avgNum; i++) {
		for (int j = -avgNum; j <= avgNum; j++) {
			data[num] = src[i + y][j + x];
			num++;
		}
	}
	BubbleSorting(data, NUMBER*NUMBER);
	int median_value = data[NUMBER*NUMBER / 2];
	median_value = imax(imin(median_value, 255), 0);

	return median_value;
}
void Dilation(int** src, int width, int height, int** dst)
{
	int avgNum = NUMBER / 2;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {

			if ((x - avgNum) < 0 || (x + avgNum) >= width || (y - avgNum) < 0 || (y + avgNum) >= height) {
				dst[y][x] = src[y][x];
			}
			else {
				dst[y][x] = Dilation_C(src, avgNum, x, y);
			}
		}
	}
}
void Erosion(int** src, int width, int height, int** dst)
{
	int avgNum = NUMBER / 2;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {

			if ((x - avgNum) < 0 || (x + avgNum) >= width || (y - avgNum) < 0 || (y + avgNum) >= height) {
				dst[y][x] = src[y][x];
			}
			else {
				dst[y][x] = Erosion_C(src, avgNum, x, y);
				
			}
		}
	}
}
void Median(int** src, int width, int height, int** dst)
{
	int avgNum = NUMBER / 2;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {

			if ((x - avgNum) < 0 || (x + avgNum) >= width || (y - avgNum) < 0 || (y + avgNum) >= height) {
				dst[y][x] = src[y][x];
			}
			else {
				dst[y][x] = Median_C(src, avgNum, x, y);
			}
		}
	}
}
void CopyImage(int** src, int width, int height, int** dst) 
{
	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++)
		{
			dst[y][x] = src[y][x];
		}
}

void main_7_sorting()
{
	int width, height;
	int** img = ReadImage("cclabeling_input.bmp", &width, &height);
	int** image_out = IntAlloc2(width, height);
	int** temp = IntAlloc2(width, height);

	Dilation(img, width, height, image_out);

	for (int i = 0; i < 10; i++) {
				
		CopyImage(image_out, width, height, temp);
		Dilation(temp, width, height, image_out);

	}
	for (int i = 0; i < 10; i++) {

		CopyImage(image_out, width, height, temp);
		Erosion(temp, width, height, image_out);

	}
	for (int i = 0; i < 10; i++) {

		CopyImage(image_out, width, height, temp);
		Median(temp, width, height, image_out);

	}
	
	ImageShow("input image", img, width, height);
	ImageShow("output image", image_out, width, height);
	waitKey(0);

	IntFree2(img, width, height);
	IntFree2(image_out, width, height);
		
}