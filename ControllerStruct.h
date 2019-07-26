#pragma once
#include<opencv2/opencv.hpp>

const int MAX_GUIDANCE_IMAGE_NUM = 10;
using namespace cv;
struct StylitControllerStruct
{
	int patchSize = 5;
	float uniformityWeight = 2000;
	Mat source_style;
	Mat output_style;
	Mat source_mask;
	Mat target_mask;
	int guidanceNum=1;
	int source_rows;
	int source_cols;
	int target_rows;
	int target_cols;
	Mat source_guidance[MAX_GUIDANCE_IMAGE_NUM];
	Mat target_guidance[MAX_GUIDANCE_IMAGE_NUM];
	float guidanceWeight[MAX_GUIDANCE_IMAGE_NUM];
}StylitController;
