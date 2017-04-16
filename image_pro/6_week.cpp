#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <opencv2/opencv.hpp>   
#include <opencv2/core/core.hpp>   
#include <opencv2/highgui/highgui.hpp> 

#define NUM 50
#define bsize 8

using namespace cv;

typedef struct {
	int r, g, b;
}int_rgb;

void IntColorFree2(int_rgb** image, int width, int height);
int_rgb** IntColorAlloc2(int width, int height);
int_rgb** ReadColorImage(char* name, int* width, int* height);
void WriteColorImage(char* name, int_rgb** image, int width, int height);
void ColorImageShow(char* winname, int_rgb** image, int width, int height);

int ComputeError(int_rgb** img, int_rgb** db[], int x, int y, int sum)
{
	int error;
	int sum_i;

	for (int db_num = 0; db_num < NUM; db_num++) {
		error = 0;
		for (int i = 0; i < bsize; i++)
			for (int j = 0; j < bsize; j++)
			{
				error += abs(img[y + i][x + j].r - db[db_num][i][j].r)
					+ abs(img[y + i][x + j].g - db[db_num][i][j].g)
					+ abs(img[y + i][x + j].b - db[db_num][i][j].b);
			}
		if (error < sum) {
			sum = error;
			sum_i = db_num;
		}
	}
	return sum_i;
}

void WriteBlock(int_rgb** image_out, int x, int y, int sum_i, int_rgb** db[])
{
	for (int i = 0; i < bsize; i++)
		for (int j = 0; j < bsize; j++)
		{
			image_out[y + i][x + j].r = db[sum_i][i][j].r;
			image_out[y + i][x + j].g = db[sum_i][i][j].g;
			image_out[y + i][x + j].b = db[sum_i][i][j].b;
		}
}

void koala_matching2(int_rgb** img, int height, int width, int_rgb** image_out)
{
	int error;
	int sum = 255 * 1024 * 768;
	int sum_i;
	char name[100];
	int_rgb** db[NUM];

	for (int db_num = 0; db_num < NUM; db_num++) {
		sprintf(name, ".\\db\\db%04d.bmp", db_num);
		int bw, bh;
		db[db_num] = (int_rgb**)ReadColorImage(name, &bw, &bh);
	}

	for (int y = 0; y <= height; y += bsize)
	{
		for (int x = 0; x <= width; x += bsize)
		{
			sum = 255 * 1024 * 768;
			if ((y + bsize < height) && (x + bsize < width)) {
				sum_i = ComputeError(img, db, x, y, sum);
				WriteBlock(image_out, x, y, sum_i, db);
			}
		}
	}
}

void main_6()
{
	int width, height, t_width, t_height;
	int_rgb** img = ReadColorImage("Koala.jpg", &width, &height);
	int_rgb** block = ReadColorImage("template_color.bmp", &t_width, &t_height);
	int_rgb** image_out = IntColorAlloc2(width, height);

	//Color_template_matching(img, width, height, block, t_width, t_height);
	koala_matching2(img, height, width, image_out);

	ColorImageShow("input image", img, width, height);
	//ColorImageShow("output image", block, t_width, t_height);
	ColorImageShow("output image", image_out, width, height);
	waitKey(0);

	IntColorFree2(img, width, height);
	//IntColorFree2(block, t_width, t_height);
	IntColorFree2(image_out, width, height);
}