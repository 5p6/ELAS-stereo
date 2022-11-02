
#include <iostream>
#include <time.h>
#include "elas.h"
#include "image.h"
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
  /*
  *左右视图的路径
  */
	string path1 = "--left image path";
	string path2 = "--rigth image path";

	//读取左右视图  
	Mat left = imread(path1, -1);
	Mat right = imread(path2, -1);

	if (left.size != right.size) {
		cerr << "左右视图必须拥有相同的尺寸" << endl;
		return -1;
	}

	if (left.channels() == 3)
		cvtColor(left, left, COLOR_RGB2GRAY);
	if (right.channels() == 3)
		cvtColor(right, right, COLOR_RGB2GRAY);

	int width = left.cols;
	int height = left.rows;
	int dim[3] = { width, height, width };

	Mat disp_left = Mat::zeros(Size(width, height), CV_32FC1);
	Mat disp_right = Mat::zeros(Size(width, height), CV_32FC1);

	// 参数设置	
	Elas::parameters param;
	param.disp_min = 0;                                     // 最小视差	
	param.disp_max = 256;                                // 最大视差 最好设置一下啊
	param.support_threshold = 0.85;              // 比率测试：最低match VS 次低match
	param.support_texture = 10;                     // 支持点的最小纹理
	param.candidate_stepsize = 5;                  // 用于支持点的sobel特征匹配的邻域半径
	param.incon_window_size = 5;                  // 不连续性窗口的尺寸
	param.incon_threshold = 5;                       // 不连续性窗口内的视差范围阈值
	param.incon_min_support = 5;                 // 不连续性窗口内的最低支持点数量
	param.add_corners = true;                        // 是否添加角点
	param.grid_size = 20;                                  // 网格尺寸
	param.beta = 0.02;                                      // 图像相似性度量的参数
	param.gamma = 3;                                      // 先验概率常数
	param.sigma = 1;                                         // 先验概率的标准差
	param.sradius = 3;                                       // 标准差半径
	param.match_texture = 1;                         // 最低纹理
	param.lr_threshold = 1;                             // 左右一致性检验阈值
	param.speckle_sim_threshold = 1;          // 连通域判断阈值
	param.speckle_size = 200;                        // 连通域噪声尺寸判断阈值
	param.ipol_gap_width = 3;                       // 空洞宽
	param.filter_median = false;                     // 是否中值滤波
	param.filter_adaptive_mean = true;        // 是否自适应中值滤波
	param.postprocess_only_left = true;     // 是否只对左视差图后处理，设置为True可以节省时间
	param.subsampling = false;                     // 每个两个像素进行视差计算，设置为True可以节省时间，但是传入的D1和D2的分辨率必须为(w/2) x (h/2)

	clock_t start = clock();
	Elas elas(param);
	elas.process(left.data, right.data, disp_left.ptr<float>(0), disp_right.ptr<float>(0), dim);
	clock_t end = clock();
	cout << "running time：" << (double)(1000 * (end - start) / CLOCKS_PER_SEC) << endl;
  //输出视差图
	disp_left.convertTo(disp_left, CV_8U);
	cv::imshow("picure", disp_left);
	cv::waitKey(0);

}
