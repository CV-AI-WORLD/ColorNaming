#include<iostream>
using namespace std;

#include<opencv2\core\core.hpp>
#include<opencv2\highgui\highgui.hpp>
using namespace cv;
Mat floorMat(const Mat & doubleMat);

int main(int argc,char * argv[])
{
	//读取图片
	Mat image = imread(argv[1],1);
	
	//判断是否有图片
	if(!image.data)
		return 0;
	
	//判断是否是彩色图片
	if( image.channels() != 3)
		return 0;

	int rows = image.rows;
	int cols = image.cols;
	int areas = rows*cols;
	
	//分离通道
	vector<Mat> bgr_planes;
	split(image,bgr_planes);

	//把各通道转为64F
	Mat bplanes,gplanes,rplanes;
	bgr_planes[0].convertTo(bplanes,CV_64FC1);
	bgr_planes[1].convertTo(gplanes,CV_64FC1);
	bgr_planes[2].convertTo(rplanes,CV_64FC1);
	
	//floor(各通道/8.0)
	Mat fbplanes,fgplanes,frplanes;
	fbplanes = floorMat(bplanes);
	fgplanes = floorMat(gplanes);
	frplanes = floorMat(rplanes);
	Mat index_im = frplanes+32*fgplanes+32*32*fbplanes;//index_im最大值可能为:31+31*32+32*32*31=32767

	Mat index_im_col;
	index_im_col = index_im.reshape(1,areas);

	//读取w2cM
	FileStorage fs(argv[2],FileStorage::READ);
	Mat w2cM;
	fs["w2cM"]>>w2cM;
	cout << "12"<<endl;
	//index_col存放的0-10之间的数值，代表11种颜色
	Mat index_col(areas,1,CV_32SC1);
	int tempIndex;
	for(int r = 0;r< areas;r++)
	{
		tempIndex = index_im_col.at<int>(r,0);
		index_col.at<int>(r,0) = w2cM.at<int>(tempIndex,0);
	}
	
	//reshape
	Mat out2 = index_col.reshape(1,rows);
	
	/* ----------------------11种颜色值----------------------------------*/
	Mat color_values(11,1,CV_64FC3);

	//black-黑色 [0 0 0]
	color_values.at<Vec3d>(0,0) = Vec3d(0,0,0);

	//blue-蓝色 [1 0 0]
	color_values.at<Vec3d>(1,0) = Vec3d(1,0,0);

	//brown-棕色(褐色) [0.25 0.4 0.5]
	color_values.at<Vec3d>(2,0) = Vec3d(0.25,0.4,0.5);

	//grey-灰色[0.5 0.5 0.5]
	color_values.at<Vec3d>(3,0) = Vec3d(0.5,0.5,0.5);

	//green-绿色[0 1 0]
	color_values.at<Vec3d>(4,0) = Vec3d(0,1,0);

	//orange-橘色[0 0.8 1]
	color_values.at<Vec3d>(5,0) = Vec3d(0,0.8,1);

	//pink-粉红色[1 0.5 1]
	color_values.at<Vec3d>(6,0) = Vec3d(1,0.5,1);

	//purple-紫色[1 0 1]
	color_values.at<Vec3d>(7,0) = Vec3d(1,0,1);

	//red-红色 [0 0 1]
	color_values.at<Vec3d>(8,0) = Vec3d(0,0,1);

	//white-白色 [1 1 1]
	color_values.at<Vec3d>(9,0) = Vec3d(1,1,1);

	//yellow-黄色[0 1 1]
	color_values.at<Vec3d>(10,0) = Vec3d(0,1,1);

	/*--------------------------------------------------------*/
	Mat out(rows,cols,CV_64FC3);
	for(int r = 0 ;r<rows;r++)
	{
		for(int c =0;c<cols;c++)
		{
			int tindex = out2.at<int>(r,c);
			out.at<Vec3d>(r,c) = color_values.at<Vec3d>(tindex,0)*255;
		}
	}
	Mat colorMap;
	out.convertTo(colorMap,CV_8UC3);
	namedWindow("colorMap",1);
	imshow("colorMap",colorMap);
	
	//关闭文件
	fs.release();
	
	waitKey(0);
	return 0;
}

//注意参数只为 CV_64FC1
Mat floorMat(const Mat & doubleMat)
{
	int rows = doubleMat.rows;
	int cols = doubleMat.cols;
	Mat flo(rows,cols,CV_32SC1);

	for(int r = 0;r < rows;r++)
	{
		for(int c = 0;c < cols ;c++)
		{
			flo.at<int>(r,c)= floor(doubleMat.at<double>(r,c)/8.0);
		}
	}
	return flo;
}
