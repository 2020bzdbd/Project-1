#include<iostream>
#include<opencv2/opencv.hpp>
#include <opencv2\imgproc\types_c.h>
#include<math.h>
#include<vector>
#include<strstream>
#include<fstream>
#include <windows.h>
using namespace std;
using namespace cv;

//��ʼ������
void initArr(int* arr) {
	for (int i = 0; i < 6; i++) {
		arr[i] = 0;
	}
}
void mov(int* len) {
	for (int i = 0; i < 4; i++) {
		len[i] = len[i + 2];
	}
}

//�ж�����Ƿ�ɱ�1:1:3:1:1��
bool judge(int* len) {
	float arr[5];
	float average=0;
	for (int i = 0; i < 5; i++) {
		arr[i] = len[i + 1] - len[i];
	}
	arr[2] = arr[2] / 3;//�����γ���3���ж�����Ƿ����
	for (int i = 0; i < 5; i++) {
		average += arr[i] / 5;
	}
	for (int i = 1; i < 5; i++) {
		//�ж������Ƿ��ھ�ֵ���²���������Ϊ1/4
		if (fabs(average - arr[i]) > (average / 4))
		return false;
	}
  	return true;
}



int* scan(Mat img, Point pos, int end, int axis) {
	int len[7] = { 0,0,0,0,0,0,0 };
	int i = 0;			//�����±�
	bool flag = false;
	if (axis == 1) {
		//������
		for (int col = pos.y; col < end; col++) {
			Scalar color = img.at<uchar>(Point(pos.x, col));
			//�״�����������ʱ��ʼ��������¼��ʼ������
			if (i == 0 && color.val[0] < 10) {
				len[i] = col;
				i++;
				flag = true;
			}
			//δ������Σ�������ͬ�������¼��һ��
			else if (i > 0) {

				if (flag != (color.val[0] < 10)) {
					len[i] = col;
					i++;
					flag = (!flag);
				}
				if (col == end - 1) {
					//�����߽�ʱ��¼���һ�εĳ���
					len[i] = col + 1;
					i++;
				}
				if (i == 6) {

					//������ξ�����ж��Ƿ�ɱ���
					if (judge(len)) {
						//���ɱ������˳�,�����ʶ������Ϊ1
						len[6] = 1;
						break;
					}
					else {
						//�ж�ʧ�������ǰ���ξ��룬����
						mov(len);
						i = 4;
					}
				}
			}
		}
	}

	if (axis == 0) {
		//������
		for (int row = pos.x; row < end; row++) {
			Scalar color = img.at<uchar>(Point(row, pos.y));
			//�״�����������ʱ��ʼ����
			if (i == 0 && color.val[0] < 10) {
				len[i] = row;
				i++;
				flag = true;
			}
			//δ������Σ�������ͬ�������¼��һ��
			else if (i > 0) {
				if (flag != (color.val[0] < 10)) {
					len[i] = row;
					i++;
					flag = (!flag);
				}
				if (row == end - 1) {
					//�����߽�ʱ��¼���һ�εĳ���
					len[i] = row + 1;
					i++;
				}
				if (i == 6) {
					//������ξ�����ж��Ƿ�ɱ���
					if (judge(len)) {
						//���ɱ������˳�,�����ʶ������Ϊ1
						len[6] = 1;
						break;
					}
					else {
						//�ж�ʧ�������ǰ���ξ��룬����
						mov(len);
						i = 4;
					}
				}
			}
		}
	}

	return len;
}

//�ҵ�λ��̽�����λ��
Rect findPosiation(Mat img, Point topLeft, Point bottomRight) {
	int* len;
	int left=0, right=0, top=0, bottom=0;
	for (int row = topLeft.x; row < bottomRight.x;row++) {
	//����ɨ���ҵ����±߿�
		len = scan(img, Point(row, topLeft.y), bottomRight.y, 1);
		if (len[6] == 1) {
		//�״��ҵ���Ӧ�к�ɨ��֮���10�����ж�
			int sum=0;
			for (int i = 0; i < 10;i++) {
				sum += scan(img,Point(row+i,topLeft.y),bottomRight.y,1)[6];
			}
			if (sum > 8) {
				top = len[0];
				bottom = len[5] - 1;
				break;
			}
		}
	}

	for (int col = topLeft.y; col < bottomRight.y;col++) {
		//����ɨ���ҵ����ұ߿�
		len = scan(img, Point(topLeft.x, col), bottomRight.x, 0);
		if (len[6] == 1) {
			//�״��ҵ���Ӧ�к�ɨ��֮���10�����ж�
			int sum=0;
			for (int i = 0; i < 10; i++) {
				sum += scan(img, Point(topLeft.x, col+i), bottomRight.x, 0)[6];
			}
			if (sum > 8) {
				left = len[0];
				right = len[5] - 1;
				break;
			}
		}
	}

	return Rect(Point(left, top), Point(right, bottom));
}

//�ҵ���Ч����
Point2f* getROI(Mat img) {
	//�ֱ��ҵ����ϣ����£����ϵ�̽���
	Rect topLeft = findPosiation(img, Point(0, 0), Point(img.cols, img.rows));
	Rect bottomLeft = findPosiation(img, Point(topLeft.tl().x , topLeft.br().y+1), Point(topLeft.br().x+5, img.rows));
	Rect topRight = findPosiation(img, Point(topLeft.br().x+1, topLeft.tl().y), Point(img.cols, topLeft.br().y+5));
	Point2f ROI[4];
	ROI[0] = topLeft.tl();
	ROI[1] = Point(topRight.br().x,topRight.y);
	ROI[2] = Point(bottomLeft.x,bottomLeft.br().y );
	ROI[3] = Point(topRight.br().x+ bottomLeft.x-topLeft.x, topRight.y+ bottomLeft.br().y-topLeft.y);
	return(ROI);
}

string decode(Mat img) {
	//����
	string code;
	for (int row = 0; row < 30; row++) {
		for (int col = 0; col < 30; col++) {
			Point pos(col * 20, row * 20), center(10, 10),tr(120,120);
			Scalar color = img.at<uchar>(tr+pos + center);
			if (color.val[0] < 10) {
				code.push_back('1');
			}
			else code.push_back('0');
		}
	}
	return code;
}

string codeToText(string codes) {
	string text="";
	auto it = codes.begin();
	int num = 0;
	while (true) {
		num=0;
		for (int i = 0; i < 8; i++,it++) {
			num += (*it-'0') * pow(2, 7 - i);
		}
		if (num == 0)
			break;
		text.push_back(char(num));
	}
	return text;
}

Mat handleImg(Mat&img) {
	//��ͼ����
	cvtColor(img, img, CV_RGB2GRAY);	//�ҶȻ�
	blur(img, img, Size(5, 5));			//�˲�
	threshold(img, img, 70, 255, THRESH_BINARY);	//��ֵ��
	resize(img, img, Size(720, 720), 0, 0, INTER_NEAREST);//������С

	return img;
}

string code_recognition(Mat&srcImage) {
	Mat dstImage;
	string code="";
	//����ԭʼͼƬ
	srcImage = handleImg(srcImage);
	imshow("srcImg", srcImage);
	waitKey();
	//�õ���Ч����
	try {
		Point2f* ROI = getROI(srcImage);
		//���û�ҵ�̽����ͷ��ؿ�ֵ
		if (ROI->x == 0)
			return "";
		Point2f srcArea[4];
		Point2f dstArea[4];
		for (int i = 0; i < 4; i++) {
			srcArea[i] = ROI[i];
		}
		dstArea[0] = Point2f(0, 0);
		dstArea[1] = Point2f(srcImage.cols - 1, 0);
		dstArea[2] = Point2f(0, srcImage.rows - 1);
		dstArea[3] = Point2f(srcImage.cols - 1, srcImage.rows - 1);

		//����3����ά���֮��ķ���任����2��x3�У�
		Mat warpPerspective_mat = getPerspectiveTransform(srcArea, dstArea);
		//Ӧ�÷���任�����Իָ���ԭͼ
		warpPerspective(srcImage, dstImage, warpPerspective_mat, srcImage.size());


		//resize(dstImage, dstImage, Size(720, 720), 0, 0, INTER_NEAREST);
		imshow("dstImg", dstImage);
		waitKey();
		//����
		code = decode(dstImage);
	}
	catch(...){
		//����ֱ�ӷ���
	}
	return code;
}

void readVideo(string videopath,vector<Mat>&srcImages) {
	//��ȡ��Ƶ,����Ϊ0�������ͷ
	VideoCapture capture(videopath);
	Mat frame;
	while (true) {
		//һ֡һ֡��
		capture >> frame;
		if (frame.empty()) {
			break;
		}
		else srcImages.push_back(frame.clone()) ;
	}
}

int main()
{
	vector<Mat> srcImages;
	string videopath("G:\\Desktop\\pa\\t.mp4");
	string outfile("G:\\Desktop\\pa\\out.txt");
	ofstream out(outfile);

	readVideo(videopath, srcImages);
	string codes;
	vector<string> sv;
	string text;

	for (Mat img : srcImages) {
		string code;
		code = code_recognition(img);
		if (code.empty())
			continue;
		else sv.push_back(code);
	}
	for (int i = 0; i < sv.size(); i += 3) {
		codes.append(sv[i]);
	}
	text = codeToText(codes);
	//cout << codes;
	out << text;
	system("pause");
	return 0;
}