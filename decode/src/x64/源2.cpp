#include<iostream>
#include<opencv2/opencv.hpp>
#include <opencv2\imgproc\types_c.h>
#include<math.h>
using namespace std;
using namespace cv;

//��ʼ������
void initArr(int* arr) {
	for (int i = 0; i < 6; i++) {
		arr[i] = 0;
	}
}

//�ж�����Ƿ�ɱ���
bool judge(int* len) {
	float arr[5];
	for (int i = 0; i < 5; i++) {
		arr[i] = len[i + 1] - len[i];
	}
	arr[2] = arr[2] / 3;
	for (int i = 1; i < 5; i++) {
		if (fabs(arr[i] - arr[i - 1]) > (arr[i] + arr[i - 1] / 5))
			return false;
	}
	return true;
}

//�ҵ�λ��̽�����λ��
Rect findPosiation(Mat img, Point topLeft, Point buttonRight) {
	int len[6] = { 0 };
	int i = 0;			//�����±�
	bool flag = false;
	Point lend(0, 0), lbegin(0, 0), temp(0, 0), rend(0, 0), rbegin(0, 0);
	//���б���
	for (int row = topLeft.x; row < buttonRight.x; row++) {
		for (int col = topLeft.y; col < buttonRight.y; col++) {
			Scalar color = img.at<uchar>(Point(row, col));
			//�״�����������ʱ��ʼ����
			if (i == 0 && color.val[0] < 10) {
				len[i] = col;
				i++;
				flag = true;
				temp = Point(row, col);
			}
			//δ������Σ�������ͬ�������¼��һ��
			else if (i > 0) {

				if (flag != (color.val[0] < 10)) {
					len[i] = col;
					i++;
					flag = (!flag);
				}
				if (col == buttonRight.y - 1) {
					len[i] = col + 1;
					i++;
				}
				if (i == 6) {
					//������ξ�����ж��Ƿ�ɱ���
					if (judge(len)) {
						lbegin = temp;
						lend = Point(row, col - 1);
					}
					break;
				}

			}
		}
		initArr(len);
		i = 0;
		flag = false;
		if (lend != Point(0, 0))
			break;
	}

	//���б���
	for (int col = topLeft.y; col < buttonRight.y; col++) {
		for (int row = topLeft.x; row < buttonRight.x; row++) {
			Scalar color = img.at<uchar>(Point(row, col));
			//�״�����������ʱ��ʼ����
			if (i == 0 && color.val[0] < 10) {
				len[i] = row;
				i++;
				flag = true;
				temp = Point(row, col);
			}
			//δ������Σ�������ͬ�������¼��һ��
			else if (i > 0) {
				if (flag != (color.val[0] < 10)) {
					len[i] = row;
					i++;
					flag = (!flag);
				}
				if (row == buttonRight.x - 1) {
					len[i] = row + 1;
					i++;
				}
				if (i == 6) {
					if (judge(len)) {
						rbegin = temp;
						rend = Point(row - 1, col);
					}
					break;
				}
			}
		}
		initArr(len);
		i = 0;
		flag = false;
		if (rend != Point(0, 0))
			break;

	}

	Point tl = Point(rbegin.x, lbegin.y);
	Point br = Point(rend.x, lend.y);
	return(Rect(tl, br));
}

//�ҵ���Ч����
Rect getROI(Mat img) {
	Rect topLeft = findPosiation(img, Point(0, 0), Point(img.rows, img.cols));
	Rect buttonLeft = findPosiation(img, Point(topLeft.br().x + 1, topLeft.tl().y), Point(img.rows, topLeft.br().y + 1));
	Rect topRight = findPosiation(img, Point(topLeft.tl().x, topLeft.br().y + 1), Point(topLeft.br().x + 1, img.cols));
	Point tl = Point(topRight.tl().x, buttonLeft.tl().y);
	Point br = Point(buttonLeft.br().x, topRight.br().y);

	return(Rect(tl, br));
}

string decode(Mat img) {
	string code;
	for (int row = 0; row < 30; row++) {
		for (int col = 0; col < 30; col++) {
			Point pos(row * 20, col * 20), center(10, 10),tr(120,120);
			Scalar color = img.at<uchar>(tr+pos + center);
			if (color.val[0] < 10) {
				code.append("1");
			}
			else code.append("0");
		}
	}
	return code;
}

Mat handleImg(Mat img) {
	cvtColor(img, img, CV_RGB2GRAY);
	blur(img, img, Size(5, 5));
	threshold(img, img, 20, 255, THRESH_BINARY);
	resize(img, img, Size(720, 720), 0, 0, INTER_NEAREST);
	return img;
}

int main()
{
	Mat srcImage, dstImage;
	srcImage = imread("G:\\Desktop\\QD4.png");
	string code;
	//����ԭʼͼƬ
	srcImage=handleImg(srcImage);
	imshow("srcImg", srcImage);
	waitKey();
	//�õ���Ч����
	Rect ROI = getROI(srcImage);
	dstImage = srcImage(ROI);
	resize(dstImage, dstImage, Size(720, 720), 0, 0, INTER_NEAREST);
	imshow("dstImg", dstImage);
	waitKey();
	//����
	code = decode(dstImage);
	cout << code << endl;
	system("pause");
	return 0;
}