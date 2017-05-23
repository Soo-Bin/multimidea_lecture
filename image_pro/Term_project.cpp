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
void IntFree2(int** image, int width, int height);
int** ReadImage(char* name, int* width, int* height);
void WriteImage(char* name, int** image, int width, int height);
void ImageShow(char* winname, int** image, int width, int height);
void CopyImage(int** src, int width, int height, int** dst);

void _BubbleSorting(int* data, int num)
{
	for (int j = num - 1; j > 0; j--) {
		for (int i = 0; i < j; i++) {
			if (data[i + 1] > data[i])
				SWAP(data[i + 1], data[i]);
		}
	}
}

void CuttingImage(int** image, int x1, int x2, int y1, int y2)
{
	int width = x2 - x1;
	int height = y2 - y1;
	int** image_out = IntAlloc2(width, height);

	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++)
			image_out[y][x] = image[y1 + y][x1 + x];
	
	ImageShow("output image", image_out, width, height);
	waitKey(0);
		
	IntFree2(image_out, width, height);
}

void Find_min_max(int** image, int width, int height)
{
	int minY = height, minX = width;
	int maxY = 0, maxX = 0;

	for (int y = 0; y < height; y++) 
		for (int x = 0; x < width; x++)	
			if (image[y][x] > 200) {
				minY = imin(y, minY); minX = imin(x, minX);
				maxY = imax(y, maxY); maxX = imax(x, maxX);
			}
	CuttingImage(image, minX, maxX, minY, maxY);

}

void main_Term_project()
{
	int width, height;
	int** img = ReadImage("6_org.bmp", &width, &height);
	
	Find_min_max(img, width, height);

	//ImageShow("input image", img, width, height);
	//ImageShow("output image", image_out, width, height);
	//waitKey(0);

	//IntFree2(img, width, height);
	//IntFree2(image_out, width, height);

}