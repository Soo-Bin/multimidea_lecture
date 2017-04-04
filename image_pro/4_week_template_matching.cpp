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

void template_matching(int** image, int width, int height)
{
	int t_width, t_height;
	int** image_template = ReadImage("template.bmp", &t_width, &t_height);
	int errort = 0;
	int sum = 255 * 1024 * 768;
	int sum_x, sum_y;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			errort = 0;
			if ((y < height - t_height) && (x < width - t_width)) {
				for (int i = 0; i < t_height; i++)
					for (int j = 0; j < t_width; j++) {
						errort += abs(image[y + i][x + j] - image_template[i][j]);
					}

				if (errort < sum) {
					sum = errort;
					sum_x = x;
					sum_y = y;
				}
			}
		}

	}
	printf("%d %d \n", sum_x, sum_y);
}

void main_4_week_tmmplate_matching()
{
	int width, height;
	int** img = ReadImage("Koala.bmp", &width, &height);
	

	template_matching(img, width, height);
	//ImageShow("input image", img, width, height);
	//ImageShow("output image", image_template, t_width, t_height);
	waitKey(0);

	IntFree2(img, width, height);
	//IntFree2(image_template, t_width, t_height);
	
}