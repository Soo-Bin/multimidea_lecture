//숫자 이미지 인식하여 출력하기

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <opencv2/opencv.hpp>   
#include <opencv2/core/core.hpp>   
#include <opencv2/highgui/highgui.hpp> 

#define NUM 10
#define PI 3.14159265359

#define imax(a,b) ((a > b) ?/*yes*/a : /*no*/b)
#define imin(a,b) ((a < b) ?/*yes*/a : /*no*/b)
#define SWAP(a, b) { (a) ^= (b) ^= (a) ^= (b); }

using namespace cv;

typedef struct {
	int x, y;
}dot_point;

int** IntAlloc2(int width, int height);
void IntFree2(int** image, int width, int height);
int** ReadImage(char* name, int* width, int* height);
void WriteImage(char* name, int** image, int width, int height);
void ImageShow(char* winname, int** image, int width, int height);
void CopyImage(int** src, int width, int height, int** dst);
void Find_min_max(int** image, int width, int height, int i);

void PrintNumber(int sum_i[])
{
	int minError = sum_i[0];

	for (int i = 0; i < 10; i++)
		minError = imin(sum_i[i], minError);

	for (int i = 0; i < 10; i++)
		if (sum_i[i] == minError)
			printf("인식된 숫자는 %d입니다!\n", i);

}

void TempleteMatching(int** image_final, int** db_image, int bw, int bh, int db_num, int sum_i[])
{
	int errort = 0;
	int sum = 255 * bw * bh;

	for (int y = 0; y < bh; y++)
		for (int x = 0; x < bw; x++)
			errort += abs(image_final[y][x] - db_image[y][x]);

	sum_i[db_num] = errort;
}


int B_Interpolation(int** image_out, int height, int width, double y, double x)
{
	int _Px = (int)x;
	int _Py = (int)y;

	double delta_x = x - _Px;
	double delta_y = y - _Py;
	double result;

	uchar P1, P2, P3, P4;

	if (_Px > 0 && _Px < width - 2 && _Py < height - 2 && _Py>0) {
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
	double ratio_x = bw / (double)width;
	double ratio_y = bh / (double)height;
	int** image_final = (int**)IntAlloc2(bw, bh);

	for (int y = 0; y < bh; y++)
		for (int x = 0; x < bw; x++) {
			image_final[y][x] = B_Interpolation(image_out, height, width, (double)y / ratio_y, (double)x / ratio_x);
		}

	TempleteMatching(image_final, db_image, bw, bh, db_num, sum_i);
}

void Rotation_num(int** image_out, int width, int height, double degree)
{
	int** image_rotate = (int**)IntAlloc2(width * 2, height * 2);
	double radian = -degree*PI / 180.0;
	double cc = cos(radian), ss = sin(-radian);
	double xcenter = (double)width / 2.0, ycenter = (double)height / 2.0;

	for (int y = 0; y < height*2; y++)
		for (int x = 0; x < width*2; x++) {
			double _x = (xcenter + ((double)y - ycenter)*ss + ((double)x - xcenter)*cc);
			double _y = (ycenter + ((double)y - ycenter)*cc - ((double)x - xcenter)*ss);

			image_rotate[y][x] = B_Interpolation(image_out, height, width, _y, _x);
		}
	//ImageShow("rotate image", image_rotate, width*2, height*2);
	Find_min_max(image_rotate, width*2, height*2, -1);
}

void Diagonalmage(int** image_out, int width, int height)
{
	dot_point P1, P2, P3, P4;
	double a, b, dot_y;
	
	P1.x = 0;			P1.y = 0;
	P2.x = width;		P2.y = 0;
	P3.x = 0;			P3.y = height;
	P4.x = width;		P4.y = height;

	for (int x = 0; x < width; x++) {
		a = (double)(P2.y - P3.y) / (P2.x - P3.x);
		b = (double)(P2.x * P3.y - P3.x * P2.y) / (P2.x - P3.x);
		dot_y = a*(double)x + b - 1;

		image_out[(int)dot_y][x] = 125;
	}
	int dx = P2.x - P3.x;
	int dy = P2.y - P3.y;
	double rad = atan2((double)dx, (double)dy);
	double degree = abs(rad * 180) / PI - 90;
	
	Rotation_num(image_out, width, height, degree);

	printf("degree : %f \n\n", degree);
	ImageShow("cutting image", image_out, width, height);
	waitKey(0);
	IntFree2(image_out, width, height);
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
	else if (i == -2) {
		Diagonalmage(image_out, width, height);
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
		PrintNumber(sum_i);
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
	//printf("minx = %d maxx = %d \n miny = %d maxy = %d \n\n", minX, maxX, minY, maxY);
	CuttingImage(image, minX, maxX, minY, maxY, i);
}

void main_Term_project()
{
	int width, height;
	int** img = ReadImage(".\\rotate_image\\9_resize_rotate.bmp", &width, &height);
		
	char name[30];
	int** db[NUM];
	int bw, bh;

	for (int i = 0; i < NUM; i++) {
		sprintf(name, ".\\number\\%01d_org.bmp", i);
		db[i] = (int**)ReadImage(name, &bw, &bh);
		Find_min_max(db[i], bw, bh, i);
	}

	//Find_min_max(img, width, height, -1);
	

	Find_min_max(img, width, height, -2);

	//ImageShow("Number Image", img, width, height);
	//waitKey(0);
	//IntFree2(img, width, height);

}