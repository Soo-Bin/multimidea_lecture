#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <opencv2/opencv.hpp>   
#include <opencv2/core/core.hpp>   
#include <opencv2/highgui/highgui.hpp> 
#define NUM 5000
using namespace cv;
typedef struct {
	int r, g, b;
}int_rgb;
void IntColorFree2(int_rgb** image, int width, int height);
int_rgb** IntColorAlloc2(int width, int height);
int_rgb** ReadColorImage(char* name, int* width, int* height);
void WriteColorImage(char* name, int_rgb** image, int width, int height);
void ColorImageShow(char* winname, int_rgb** image, int width, int height);

void Color_template_matching(int_rgb** img, int width, int height, int_rgb** block, int t_width, int t_height)
{
	int error = 0;
	int sum = 255 * 1024 * 768;
	int sum_x, sum_y;
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			error = 0;
			if ((y < height - t_height) && (x < width - t_width)) {
				for (int i = 0; i < t_height; i++)
					for (int j = 0; j < t_width; j++) {
						error = error + abs(img[y + i][x + j].r - block[i][j].r) + abs(img[y + i][x + j].g - block[i][j].g) + abs(img[y + i][x + j].b - block[i][j].b);
					}
				if (error < sum) {
					sum = error;
					sum_x = x;
					sum_y = y;
				}
			}
		}
	}
	for (int y = sum_y; y <= sum_y + t_height; y++) {
		img[y][sum_x].r = 0;
		img[y][sum_x].g = 0;
		img[y][sum_x].b = 0;
	}
	for (int y = sum_y; y <= sum_y + t_height; y++) {
		img[y][sum_x + t_height].r = 0;
		img[y][sum_x + t_height].g = 0;
		img[y][sum_x + t_height].b = 0;
	}
	for (int x = sum_x; x <= sum_x + t_width; x++) {
		img[sum_y][x].r = 0;
		img[sum_y][x].g = 0;
		img[sum_y][x].b = 0;
	}
	for (int x = sum_x; x <= sum_x + t_width; x++) {
		img[sum_y + t_width][x].r = 0;
		img[sum_y + t_width][x].g = 0;
		img[sum_y + t_width][x].b = 0;
	}
	printf("x = %d \t y = %d \n\n", sum_x, sum_y);
}

void koala_matching(int_rgb** img, int height, int width, int_rgb** image_out)
{
	int bsize = 8;
	int_rgb** block2 = IntColorAlloc2(bsize, bsize);
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
	for (int y = 0; y < height; y += bsize)
	{
		for (int x = 0; x < width; x += bsize)
		{
			sum = 255 * 1024 * 768;
			if ((y + bsize < height) && (x + bsize < width)) {
				for (int db_num = 0; db_num < NUM; db_num++) {
					error = 0;
					for (int i = 0; i < bsize; i++)
						for (int j = 0; j < bsize; j++)
						{
							error = error + abs(img[y + i][x + j].r - db[db_num][i][j].r)
								+ abs(img[y + i][x + j].g - db[db_num][i][j].g)
								+ abs(img[y + i][x + j].b - db[db_num][i][j].b);
						}
					if (error < sum) {
						sum = error;
						sum_i = db_num;
					}
				}

				for (int i = 0; i < bsize; i++)
					for (int j = 0; j < bsize; j++)
					{
						image_out[y + i][x + j].r = db[sum_i][i][j].r;
						image_out[y + i][x + j].g = db[sum_i][i][j].g;
						image_out[y + i][x + j].b = db[sum_i][i][j].b;
					}

			}
		}
	}
}

void main_5_week_color_image()
{
	int width, height, t_width, t_height;
	int_rgb** img = ReadColorImage("Koala.jpg", &width, &height);
	int_rgb** block = ReadColorImage("template_color.bmp", &t_width, &t_height);
	int_rgb** image_out = IntColorAlloc2(width, height);

	//Color_template_matching(img, width, height, block, t_width, t_height);
	koala_matching(img, height, width, image_out);

	ColorImageShow("input image", img, width, height);
	//ColorImageShow("output image", block, t_width, t_height);
	ColorImageShow("output image", image_out, width, height);
	waitKey(0);

	IntColorFree2(img, width, height);
	//IntColorFree2(block, t_width, t_height);
	IntColorFree2(image_out, width, height);
}