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

typedef struct {
	int index, minError;
}ERR;

int** IntAlloc2(int width, int height);
void IntFree2(int** image, int width, int height);
int** ReadImage(char* name, int* width, int* height);
void WriteImage(char* name, int** image, int width, int height);
void ImageShow(char* winname, int** image, int width, int height);
void CopyImage(int** src, int width, int height, int** dst);

int** CutImage(char* name, int* width, int* height)
{
	Mat img = imread(name, IMREAD_GRAYSCALE);
	int minY = img.rows, minX = img.cols;
	int maxY = 0, maxX = 0;

	for (int y = 0; y<img.rows; y++)
		for (int x = 0; x<img.cols; x++)
			if (img.at<unsigned char>(y, x) > 200) {
				minY = imin(y, minY); minX = imin(x, minX);
				maxY = imax(y, maxY); maxX = imax(x, maxX);
			}

	*width = maxX - minX + 1;
	*height = maxY - minY + 1;
	int** image = (int**)IntAlloc2((*width), (*height));

	for (int i = 0; i < (*height); i++)
		for (int j = 0; j < (*width); j++)
			image[i][j] = img.at<unsigned char>((i + minY), (j + minX));

	return(image);
}

ERR PrintNumber(int sum_i[])
{
	ERR error;
	error.minError = sum_i[0];

	for (int i = 0; i < 10; i++)
		error.minError = imin(sum_i[i], error.minError);

	for (int i = 0; i < 10; i++)
		if (sum_i[i] == error.minError)
			error.index = i;
	
	return  (error);
}

void TempleteMatching(int** image_out, int** db_image, int bw, int bh, int db_num, int sum_i[])
{
	int errort = 0;
	int sum = 255 * bw * bh;

	for (int y = 0; y < bh; y++)
		for (int x = 0; x < bw; x++)
			errort += abs(image_out[y][x] - db_image[y][x]);

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
		P1 = 0; P2 = 0; P3 = 0; P4 = 0;
	}

	result = P1*(1 - delta_x)*(1 - delta_y) + P2*(delta_x)*(1 - delta_y) + P3*(1 - delta_x)*(delta_y)+P4*(delta_x)*(delta_y);
	
	return (int)result;
}

int** ResizeImage(int** image, int width, int height, int bw, int bh)
{
	double ratio_x = bw / (double)width;
	double ratio_y = bh / (double)height;
	int** image_out = (int**)IntAlloc2(bw, bh);

	for (int y = 0; y < bh; y++)
		for (int x = 0; x < bw; x++)
			image_out[y][x] = B_Interpolation(image, height, width, (double)y / ratio_y, (double)x / ratio_x);
	
	return (image_out);
}

int** Diagonalmage(int** image, int width, int height, int bw, int bh)
{
	int** image_rotate = (int**)IntAlloc2(width * 2, height * 2);
	
	dot_point P1, P2;

	P1.x = width;	P1.y = 0;
	P2.x = 0;		P2.y = height;

	int dx = P1.x - P2.x;
	int dy = P1.y - P2.y;

	double rad = atan2((double)dx, (double)dy);
	double degree = abs(rad * 180) / PI - 90;

	double radian = -degree*PI / 180.0;
	double cc = cos(radian), ss = sin(-radian);
	double xcenter = (double)width / 2.0, ycenter = (double)height / 2.0;

	for (int y = 0; y < height * 2; y++)
		for (int x = 0; x < width * 2; x++) {
			double _x = (xcenter + ((double)y - ycenter)*ss + ((double)x - xcenter)*cc);
			double _y = (ycenter + ((double)y - ycenter)*cc - ((double)x - xcenter)*ss);
			image_rotate[y][x] = B_Interpolation(image, height, width, _y, _x);
		}
		
	return (image_rotate);
}

void main_Term_project()
{
	int width, height;
	int** img = CutImage(".\\number\\0_resize.bmp", &width, &height);
	int** image_out, **image_rotate;
	ERR error, error1;

	char name[30];
	int** db[NUM];
	int sum_i[10];
	int bw, bh;
	
	for (int i = 0; i < NUM; i++) {
		sprintf(name, ".\\number\\%01d_org.bmp", i);
		db[i] = (int**)CutImage(name, &bw, &bh);
		image_out = ResizeImage(img, width, height, bw, bh);
		TempleteMatching(image_out, db[i], bw, bh, i, sum_i);
		IntFree2(db[i], bw, bh);
	}
	
	error = PrintNumber(sum_i);
	
	printf("인식된 숫자는 %d입니다! minerror = %d \n", error.index, error.minError);
	
	IntFree2(image_out, bw, bh);

	image_rotate = Diagonalmage(img, width, height, bw, bh);
	WriteImage("image_rotate.bmp", image_rotate, width * 2, height * 2);
	IntFree2(image_rotate, width * 2, height * 2);

	image_out = CutImage("image_rotate.bmp", &width, &height);
	
	for (int i = 0; i < NUM; i++) {
		sprintf(name, ".\\number\\%01d_org.bmp", i);
		db[i] = (int**)CutImage(name, &bw, &bh);
		image_rotate = ResizeImage(image_out, width, height, bw, bh);
		TempleteMatching(image_rotate, db[i], bw, bh, i, sum_i);
		IntFree2(db[i], bw, bh);
	}
	
	error1 = PrintNumber(sum_i);

	printf("인식된 숫자는 %d입니다! minerror = %d \n", error1.index, error1.minError);

	//ImageShow("Number Image", img, bw, bh);
	//waitKey(0);

	//IntFree2(img, width, height);
	
}