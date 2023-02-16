#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <iostream>
#include <string>
#include <ctime>
#include <vector>
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <cstdio>
#include <wchar.h>
#include <map>
#include <cmath>
using namespace std;
using namespace cv;
/*---------------------------------------------*/
#define WIDTH 64//��ֳɼ���
#define HEIGHT 35//���ֳɼ���
#define STEP 10//����������ȡ��
/*WIDTH*STEP��HEIGHT*STEP���ɳ���ͼ�����ߴ�*/
#define AMOUNT int(WIDTH)*int(HEIGHT)
#define STARTSTR "E:/file/"//�ļ�Ŀ¼
#define STARTID 1000//��ͼid
#define ENDID 1300//βͼid
#define ENDSTR ".png"
/*����E:/file/1000.png �� E:/file/1100.png*/
/*--------------------------------------------*/
//���ع�꺯��
void HideCursor()
{
	CONSOLE_CURSOR_INFO cursor_info = { 1,0 };//�ڶ���ֵΪ0��ʾ���ع��
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
}
//�����ƶ�
void gotoxy(int y, int x)//����ϰ�� x y����λ�� �õ�һ��������ʾ��
{
	COORD p;
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);//��ȡ��ǰ�������
	p.X = x; p.Y = y;//������Ŀ���ƶ�λ�ô��ݸ��ṹ��
	SetConsoleCursorPosition(handle, p);//�ƶ����
}
//ʱ�䲹��
void AddTime(int t)
{
	if(t < 50){
		Sleep(50-t);
	}
}
//�ַ�ת��
string to_String(int n)
{
	int m = n; char s[10]; char ss[10];
	int i = 0, j = 0;
	if (n < 0) {
		m = 0 - m; j = 1; ss[0] = '-';
	}
	while (m > 0) {
		s[i++] = m % 10 + '0'; m /= 10;
	}
	s[i] = '\0'; i = i - 1;
	while (i >= 0) {
		ss[j++] = s[i--];
	}
	ss[j] = '\0';
	return ss;
}
//��������
void SetFont(int size)
{
	CONSOLE_FONT_INFOEX cfi; //hello imX2G
	cfi.cbSize = sizeof cfi; //hello imX2G
	cfi.dwFontSize.Y = size;  //���������С
	cfi.FontFamily = FF_DONTCARE; //hello imX2G
	cfi.FontWeight = FW_NORMAL; //�����ϸ FW_BOLD
	wcscpy_s(cfi.FaceName, L"����");  //�������壬�����ǿ���̨���е�
	SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi); //hello imX2G
}
//��������
void DocContral()
{
	HWND target;        // Ŀ�괰�ھ��
	HWND parent;        // Ŀ�길���ھ��
	INT width, height;  // �����¿��
	width = 800;//���и���
	height = 618;//���и���
	target = GetForegroundWindow();
	if (target != NULL && target != INVALID_HANDLE_VALUE) {
		parent = GetParent(target);
		while (parent != NULL && parent != INVALID_HANDLE_VALUE)
		{
			target = parent;
			parent = GetParent(target);
		}
		// ��ȡ����ԭʼ��С
		RECT rect;
		GetWindowRect(target, &rect);
		// �������ô��ڴ�С
		MoveWindow(target, rect.left, rect.top, width, height, true);
	}
}
//���������r g b�ı�
void chargeColor(int r, int g, int b)
{
	wprintf(L"\x1b[38;2;%d;%d;%dm", r, g, b);
}
//������
class XY
{
public:
	XY(int x, int y) :m_X(x), m_Y(y){}
	int m_X;int m_Y;
};
//�����һ��ͼ
void setFirstPng(vector<Vec3b>&v_FirstPng)
{
	v_FirstPng.reserve(AMOUNT);
	string firstStr;
	firstStr = string(STARTSTR) + to_String(STARTID) + string(ENDSTR);
	Mat image;
	image = imread(firstStr);
	for (int i = 0; i < image.rows; i += STEP)
	{
		for (int j = 0; j < image.cols; j += STEP)
		{
			v_FirstPng.push_back(image.at<Vec3b>(i, j));
		}
	}
}
//��ӡ��һ��ͼ
void getFirstPng(vector<Vec3b>&v_FirstPng)
{
	int key = -1;
	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			key++;
			int b = int(v_FirstPng[key][0]);//B
			int g = int(v_FirstPng[key][1]);//G
			int r = int(v_FirstPng[key][2]);//R
			chargeColor(r, g, b);
			printf("��");
		}
		printf("\n");
	}
}
//��ȡ
void ifsAll(vector<Vec3b>&v_RGB, map<int, map<int, vector<XY>>>&m_Local_RGB)
{
	Mat image;string next;
	int num = int(STARTID) + 1;//�ڶ���ͼ��ʼ���ݵ�һ��ͼ����
	string endStr = to_String(ENDID-STARTID);
	vector<Vec3b>v_nextRGB;//���뵱ǰͼ Ϊ��һ��ͼ�ж�׼��
	v_nextRGB.reserve(WIDTH*HEIGHT);
	map<int, vector<XY>>m_thePng;//����ͼ�������ݻ���// rgb_id  location
	while (true)
	{
		int pngId = num - int(STARTID);
		SetConsoleTitle(("��ȡ�� " + to_String(pngId) + " / "+ endStr).c_str());
		next = string(STARTSTR) + to_String(num) + string(ENDSTR);
		image = imread(next);
		int color_add = 0;
		auto it = v_RGB.begin();//��һ��ͼ��Ҫ�жϵ�����
		//����ͼ����
		for (int i = 0; i < image.rows; i += STEP) {
			for (int j = 0; j < image.cols; j += STEP)
			{
				//��һ��ͼ�ж�׼��
				Vec3b vec = image.at<Vec3b>(i, j);
				v_nextRGB.push_back(vec);
				int rgbid = int(1e9 + double(vec[2]) * 1e6 + double(vec[1]) * 1e3 + double(vec[0]));
				if ((*it)[0]== vec[0] && (*it)[1] == vec[1] && (*it)[2] == vec[2])
				{
					++it;continue;
				}
				else
				{
					bool find = false;
					for (map<int,vector<XY>>::iterator it_Int = m_thePng.begin(); it_Int != m_thePng.end(); it_Int++)
					{
						if (it_Int->first==rgbid){
							find = true;break;
						}
					}
					if (find)
					{
						//����λ�����������ֵ
						XY location(i/int(STEP), j/int(STEP));
						m_thePng.at(rgbid).push_back(location);
					}
					else
					{
						//�����λ�ú�����ֵ
						XY location(i/ int(STEP), j/int(STEP));
						vector<XY>v_XY;
						v_XY.push_back(location);
						m_thePng[rgbid] = v_XY;
					}
				}
				++it;
			}
		}
		v_RGB.swap(v_nextRGB);//��һ��ͼ����ͬ��
		m_Local_RGB.insert(make_pair(pngId, m_thePng));
		m_thePng.clear(); v_nextRGB.clear();
		if (pngId == int(ENDID)-int(STARTID)){ break; }num++;
	}
}
//��ӡ
void printAll(map<int, map<int, vector<XY>>>&m_Local_RGB)
{
	clock_t startTime, endTime;//��ʱ
	for (int i = 1; i <= int(ENDID) - int(STARTID); i++)
	{
		startTime = clock();//��ʱ��ʼ
		for (map<int, vector<XY>>::iterator it2 = m_Local_RGB[i].begin(); it2 != m_Local_RGB[i].end(); it2++)
		{
			int R = (it2->first%int(1e9)) / int(1e6);
			int G = (it2->first%int(1e6)) / int(1e3);
			int B = (it2->first%int(1e3));
			chargeColor(R, G, B);
			for (vector<XY>::iterator it3 = it2->second.begin(); it3 != it2->second.end(); it3++)
			{
				gotoxy(it3->m_X, (it3->m_Y) * 2);
				printf("��");
			}
		}
		endTime = clock();//��ʱ����
		AddTime(int(endTime - startTime));//ʱ�䲹��
	}
}

int main()
{
	SetConsoleTitle("");//��ʼ�ޱ���
	SetFont(10);//�����ʼֵ
	//DocContral();//���ڳ�ʼֵ(��������)
	utils::logging::setLogLevel(utils::logging::LOG_LEVEL_SILENT);//���������־
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut == INVALID_HANDLE_VALUE)return GetLastError();
	DWORD dwMode = 0;
	if (!GetConsoleMode(hOut, &dwMode))return GetLastError();
	dwMode |= 0x0004;
	if (!SetConsoleMode(hOut, dwMode))return GetLastError();
	vector<Vec3b>v_FirstPng;//��һ��ͼ
	vector<Vec3b>v_RGB;//Vec3b��  [0]B [1]G [2]R
	map<int, map<int, vector<XY>>>m_Local_RGB;//  png_id   rgb_id    pixel_id
	//���õ�һ��ͼ
	setFirstPng(v_FirstPng);
	//���õڶ���ͼ
	v_RGB.clear();v_RGB.reserve(AMOUNT);
	v_RGB.insert(v_RGB.begin(), v_FirstPng.begin(), v_FirstPng.end());
	ifsAll(v_RGB, m_Local_RGB);//����
	SetConsoleTitle("Title");//��������
	cout << "��ȡ���" << endl;
	system("pause");system("cls");
	HideCursor();//���ع��
	getFirstPng(v_FirstPng);//��ӡ��һ��ͼ
	printAll(m_Local_RGB);//���´�ӡ
	system("cls");
	system("pause");
	return 0;
}