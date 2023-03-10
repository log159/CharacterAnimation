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
#define WIDTH 64//宽分成几份
#define HEIGHT 35//长分成几份
#define STEP 10//隔几个像素取点
/*WIDTH*STEP和HEIGHT*STEP不可超过图像最大尺寸*/
#define AMOUNT int(WIDTH)*int(HEIGHT)
#define STARTSTR "E:/file/"//文件目录
#define STARTID 1000//首图id
#define ENDID 1300//尾图id
#define ENDSTR ".png"
/*例：E:/file/1000.png 到 E:/file/1100.png*/
/*--------------------------------------------*/
//隐藏光标函数
void HideCursor()
{
	CONSOLE_CURSOR_INFO cursor_info = { 1,0 };//第二个值为0表示隐藏光标
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
}
//坐标移动
void gotoxy(int y, int x)//个人习惯 x y换下位置 让第一个参数表示行
{
	COORD p;
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);//获取当前函数句柄
	p.X = x; p.Y = y;//将光标的目标移动位置传递给结构体
	SetConsoleCursorPosition(handle, p);//移动光标
}
//时间补齐
void AddTime(int t)
{
	if(t < 50){
		Sleep(50-t);
	}
}
//字符转换
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
//字体设置
void SetFont(int size)
{
	CONSOLE_FONT_INFOEX cfi; //hello imX2G
	cfi.cbSize = sizeof cfi; //hello imX2G
	cfi.dwFontSize.Y = size;  //设置字体大小
	cfi.FontFamily = FF_DONTCARE; //hello imX2G
	cfi.FontWeight = FW_NORMAL; //字体粗细 FW_BOLD
	wcscpy_s(cfi.FaceName, L"黑体");  //设置字体，必须是控制台已有的
	SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi); //hello imX2G
}
//窗口设置
void DocContral()
{
	HWND target;        // 目标窗口句柄
	HWND parent;        // 目标父窗口句柄
	INT width, height;  // 窗口新宽高
	width = 800;//自行更改
	height = 618;//自行更改
	target = GetForegroundWindow();
	if (target != NULL && target != INVALID_HANDLE_VALUE) {
		parent = GetParent(target);
		while (parent != NULL && parent != INVALID_HANDLE_VALUE)
		{
			target = parent;
			parent = GetParent(target);
		}
		// 获取窗口原始大小
		RECT rect;
		GetWindowRect(target, &rect);
		// 重新设置窗口大小
		MoveWindow(target, rect.left, rect.top, width, height, true);
	}
}
//根据输入的r g b改变
void chargeColor(int r, int g, int b)
{
	wprintf(L"\x1b[38;2;%d;%d;%dm", r, g, b);
}
//坐标类
class XY
{
public:
	XY(int x, int y) :m_X(x), m_Y(y){}
	int m_X;int m_Y;
};
//存入第一张图
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
//打印第一张图
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
			printf("■");
		}
		printf("\n");
	}
}
//读取
void ifsAll(vector<Vec3b>&v_RGB, map<int, map<int, vector<XY>>>&m_Local_RGB)
{
	Mat image;string next;
	int num = int(STARTID) + 1;//第二张图开始根据第一张图更新
	string endStr = to_String(ENDID-STARTID);
	vector<Vec3b>v_nextRGB;//存入当前图 为下一张图判断准备
	v_nextRGB.reserve(WIDTH*HEIGHT);
	map<int, vector<XY>>m_thePng;//单张图更新数据汇总// rgb_id  location
	while (true)
	{
		int pngId = num - int(STARTID);
		SetConsoleTitle(("读取中 " + to_String(pngId) + " / "+ endStr).c_str());
		next = string(STARTSTR) + to_String(num) + string(ENDSTR);
		image = imread(next);
		int color_add = 0;
		auto it = v_RGB.begin();//上一张图需要判断的数据
		//单张图处理
		for (int i = 0; i < image.rows; i += STEP) {
			for (int j = 0; j < image.cols; j += STEP)
			{
				//下一张图判断准备
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
						//索引位置新添加坐标值
						XY location(i/int(STEP), j/int(STEP));
						m_thePng.at(rgbid).push_back(location);
					}
					else
					{
						//新添加位置和索引值
						XY location(i/ int(STEP), j/int(STEP));
						vector<XY>v_XY;
						v_XY.push_back(location);
						m_thePng[rgbid] = v_XY;
					}
				}
				++it;
			}
		}
		v_RGB.swap(v_nextRGB);//上一张图数据同步
		m_Local_RGB.insert(make_pair(pngId, m_thePng));
		m_thePng.clear(); v_nextRGB.clear();
		if (pngId == int(ENDID)-int(STARTID)){ break; }num++;
	}
}
//打印
void printAll(map<int, map<int, vector<XY>>>&m_Local_RGB)
{
	clock_t startTime, endTime;//计时
	for (int i = 1; i <= int(ENDID) - int(STARTID); i++)
	{
		startTime = clock();//计时开始
		for (map<int, vector<XY>>::iterator it2 = m_Local_RGB[i].begin(); it2 != m_Local_RGB[i].end(); it2++)
		{
			int R = (it2->first%int(1e9)) / int(1e6);
			int G = (it2->first%int(1e6)) / int(1e3);
			int B = (it2->first%int(1e3));
			chargeColor(R, G, B);
			for (vector<XY>::iterator it3 = it2->second.begin(); it3 != it2->second.end(); it3++)
			{
				gotoxy(it3->m_X, (it3->m_Y) * 2);
				printf("■");
			}
		}
		endTime = clock();//计时结束
		AddTime(int(endTime - startTime));//时间补齐
	}
}

int main()
{
	SetConsoleTitle("");//初始无标题
	SetFont(10);//字体初始值
	//DocContral();//窗口初始值(自行设置)
	utils::logging::setLogLevel(utils::logging::LOG_LEVEL_SILENT);//不再输出日志
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut == INVALID_HANDLE_VALUE)return GetLastError();
	DWORD dwMode = 0;
	if (!GetConsoleMode(hOut, &dwMode))return GetLastError();
	dwMode |= 0x0004;
	if (!SetConsoleMode(hOut, dwMode))return GetLastError();
	vector<Vec3b>v_FirstPng;//第一张图
	vector<Vec3b>v_RGB;//Vec3b类  [0]B [1]G [2]R
	map<int, map<int, vector<XY>>>m_Local_RGB;//  png_id   rgb_id    pixel_id
	//设置第一张图
	setFirstPng(v_FirstPng);
	//设置第二张图
	v_RGB.clear();v_RGB.reserve(AMOUNT);
	v_RGB.insert(v_RGB.begin(), v_FirstPng.begin(), v_FirstPng.end());
	ifsAll(v_RGB, m_Local_RGB);//读入
	SetConsoleTitle("Title");//标题设置
	cout << "读取完毕" << endl;
	system("pause");system("cls");
	HideCursor();//隐藏光标
	getFirstPng(v_FirstPng);//打印第一张图
	printAll(m_Local_RGB);//更新打印
	system("cls");
	system("pause");
	return 0;
}