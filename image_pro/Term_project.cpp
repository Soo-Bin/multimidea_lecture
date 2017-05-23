#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <opencv2/opencv.hpp>   
#include <opencv2/core/core.hpp>   
#include <opencv2/highgui/highgui.hpp> 

#define NUM 10

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

void TempleteMatching(int** image_final, int width, int height, int** db_image, int bw, int bh, int db_num, int sum_i[])
{
	int errort = 0;
	int sum = 255 * width * height;
	
	for (int y = 1; y < height - 1; y++) {
		for (int x = 1; x < width - 1; x++) {
			errort = 0;
			if ((y < height - bh) && (x < width - bw)) {
				for (int i = 0; i < bh; i++)
					for (int j = 0; j < bw; j++) {
						errort += abs(image_final[y + i][x + j] - db_image[i][j]);
					}
				if (errort < sum) {
					sum = errort;
					sum_i[db_num] = sum;
				}
			}
		}
	}
}

int B_Interpolation(int** image_out, int height, int width, double y, double x, double ratio)
{

	int _Px = (int)x;
	int _Py = (int)y;

	double delta_x = x - _Px;
	double delta_y = y - _Py;
	double result;

	uchar P1, P2, P3, P4;

	if (_Px > 0 && _Px < width - 1 && _Py < height - 1 && _Py>0) {
		P1 = image_out[_Py][_Px];
		P2 = image_out[_Py][_Px + 1];
		P3 = image_out[_Py + 1][_Px];
		P4 = image_out[_Py + 1][_Px + 1];
	}
	else {
		P1 = 0;	P2 = 0;	P3 = 0;	P4 = 0;
	}

	result = P1*(1 - delta_x)*(1 - delta_y) + P2*(delta_x)*(1 - delta_y) + P3*(1 - delta_x)*(delta_y)+P4*(delta_x)*(delta_y);

	return (int)result;
}

void ResizeImage(int** image_out, int width, int height, int bw, int bh, int** db_image, int db_num, int sum_i[])
{
	double ratio = (bw*bh) / (double)(width*height);
	int** image_final = (int**)IntAlloc2(bw, bh);

	for (int y = 0; y < bh; y++)
		for (int x = 0; x < bw; x++) {
			image_final[y][x] = B_Interpolation(image_out, height, width, (double)y / ratio, (double)x / ratio, ratio);
		}
	ImageShow("final_image", image_final, bw, bh);
	waitKey(0);
	IntFree2(image_final, bw, bh);

	TempleteMatching(image_final, width, height, db_image, bw, bh, db_num, sum_i);
}

void CuttingImage(int** image, int x1, int x2, int y1, int y2, int i)
{
	char name[100];
	int width = x2 - x1 + 1;
	int height = y2 - y1 + 1;
	int** image_out = IntAlloc2(width, height);

	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++)
			image_out[y][x] = image[y1 + y][x1 + x];
	
	if (i >= 0) {
		sprintf(name, ".\\cutting_number\\%01d_org.bmp", i);
		WriteImage(name, image_out, width, height);
	}
	else {
		int sum_i[10];
		char name[30];
		int** db[NUM];
		int bw, bh;
				
		for (int db_num = 0; db_num < NUM; db_num++) {
			sprintf(name, ".\\cutting_number\\%01d_org.bmp", db_num);
			db[db_num] = (int**)ReadImage(name, &bw, &bh);
			ResizeImage(image_out, width, height, bw, bh, db[db_num], db_num, sum_i);
		}
		printf("에러율 index %d = %d\n", 2, sum_i[2]);
		waitKey(0);
	}
}

void Find_min_max(int** image, int width, int height, int i)
{
	int minY = height, minX = width;
	int maxY = 0, maxX = 0;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++)
			if (image[y][x] > 200) {
				minY = imin(y, minY); minX = imin(x, minX);
				maxY = imax(y, maxY); maxX = imax(x, maxX);
			}
	}
	CuttingImage(image, minX, maxX, minY, maxY, i);
}

void main_Term_project()
{
	int width, height;
	int** img = ReadImage("0_resize.bmp", &width, &height);
	
	char name[30];
	int** db[NUM];
	int bw, bh;

	for (int i = 0; i < NUM; i++) {
		sprintf(name, ".\\number\\%01d_org.bmp", i);
		db[i] = (int**)ReadImage(name, &bw, &bh);
		Find_min_max(db[i], bw, bh, i);
	}
		
	Find_min_max(img, width, height, -1);

}