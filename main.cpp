#include <easyx.h>
#include <graphics.h>
#include <iostream>
#include <stdlib.h>
#include <conio.h>
#include <Windows.h>
#include <fstream>
using namespace std;

IMAGE background;
IMAGE board;
IMAGE black;
IMAGE Black;
IMAGE white;
IMAGE White;
IMAGE ayumi;
IMAGE sonoko;
IMAGE shukichi;
MOUSEMSG m;
int NewOld;
int MODE;
int SIDE;
int turnId;
struct Step
{
	int x0;
	int y0;
	int x1;
	int y1;
} stepping[1000];
extern int startX, startY, resultX, resultY;
int blackSum, whiteSum, gridboard[7][7];
static int delta[24][2] = { { 1,1 },{ 0,1 },{ -1,1 },{ -1,0 },
{ -1,-1 },{ 0,-1 },{ 1,-1 },{ 1,0 },
{ 2,0 },{ 2,1 },{ 2,2 },{ 1,2 },
{ 0,2 },{ -1,2 },{ -2,2 },{ -2,1 },
{ -2,0 },{ -2,-1 },{ -2,-2 },{ -1,-2 },
{ 0,-2 },{ 1,-2 },{ 2,-2 },{ 2,-1 } }; // 24种可能走法 
ifstream statein, stepin;
ofstream stateout, stepout;

void introduction();
void mainboard();
void side();
void mode();
int game();
int official(int color);
bool CheckIfHasValidMove(int color);
bool Procsstep(int x0, int y0, int x1, int y1, int color, bool checkOnly);
extern int main_Easy();
extern int main_Normal();
extern int main_Hard();

int main()
{
	initgraph(1000, 600); //初始化图形系统
	cleardevice(); //清屏幕
	loadimage(&background, _T("界面底图.jpg"), 1000, 600, false);
	loadimage(&ayumi, _T("ayumi.jpg"), 200, 200, false);
	loadimage(&sonoko, _T("sonoko.jpg"), 200, 200, false);
	loadimage(&shukichi, _T("shukichi.jpg"), 200, 200, false);
	introduction();
	cleardevice();
	mainboard();
	_getch();
	closegraph();
	return 0;
}

void introduction() // 开启时输出标题和提示语
{
	putimage(0, 0, &background);
	settextstyle(100, 0, _T("黑体"));
	setbkmode(TRANSPARENT);
	RECT headline = { 0, 0, 1000, 500 };
	drawtext(_T("Conan's Ataxx"), &headline, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	settextstyle(20, 0, _T("黑体"));
	RECT tip = { 0, 300, 1000, 600 };
	drawtext(_T("click to continue"), &tip, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	// 点击进入主菜单
	while (1)
	{
		m = GetMouseMsg();
		if (m.mkLButton)
		{
			FlushMouseMsgBuffer();
			return;
		}
	}
}

void mainboard() // 主菜单
{
	putimage(0, 0, &background); // 载入背景

	// 设置字体
	settextstyle(50, 0, _T("黑体"));
	setbkmode(TRANSPARENT);

	// 输出菜单选项
	RECT NEWGAME = { 100, 140, 400, 190 };
	drawtext(_T("New Game"), &NEWGAME, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	RECT BACK = { 50, 240, 450, 290 };
	drawtext(_T("Return to Game"), &BACK, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	RECT EXIT = { 100, 340, 400, 390 };
	drawtext(_T("Exit"), &EXIT, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	// 点击选择
	while (1)
	{
		m = GetMouseMsg();
		if (m.mkLButton)
		{
			if (m.x > 100 && m.x < 400 && m.y > 140 && m.y < 190) // 选择新游戏
			{
				FlushMouseMsgBuffer();
				NewOld = 1;
				cleardevice();
				mode();
				cleardevice();
				side();
				cleardevice();
				game();
			}
			if (m.x > 100 && m.x < 400 && m.y > 240 && m.y < 290) // 选择已存档
			{
				FlushMouseMsgBuffer();
				NewOld = 0;
				cleardevice();
				mode();
				cleardevice();
				game();
			}
			if (m.x > 100 && m.x < 400 && m.y > 340 && m.y < 390) // 选择退出
			{
				FlushMouseMsgBuffer();
				exit(0);
			}
		}
	}
	return;
}

void mode() // 难度模式
{
	putimage(0, 0, &background);
	settextstyle(30, 0, _T("黑体"));
	setbkmode(TRANSPARENT);

	// 难度模式图和字
	RECT EASY = { 200, 400, 300, 500 };
	drawtext(_T("EASY"), &EASY, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	RECT NORMAL = { 450, 400, 550, 500 };
	drawtext(_T("NORMAL"), &NORMAL, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	RECT HARD = { 700, 400, 800, 500 };
	drawtext(_T("HARD"), &HARD, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	putimage(150, 150, &ayumi);
	putimage(400, 150, &sonoko);
	putimage(650, 150, &shukichi);
	RECT MAINBOARD = { 650, 450, 950, 550 };
	drawtext(_T("Return to Mainboard"), &MAINBOARD, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	settextstyle(20, 0, _T("黑体"));
	RECT tip = { 0, 500, 1000, 550 };
	drawtext(_T("Please click one of the pictures"), &tip, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	

	// 点击选择
	while (1)
	{
		m = GetMouseMsg();
		if (m.mkLButton)
		{
			if (m.x > 650 && m.x < 950 && m.y > 485 && m.y < 515) // 点击返回主菜单
			{
				FlushMouseMsgBuffer();
				cleardevice();
				mainboard();
			}
			if (m.x > 150 && m.x < 350 && m.y > 150 && m.y < 350) // easy
			{
				FlushMouseMsgBuffer();
				MODE = 1;
				return;
			}
			if (m.x > 400 && m.x < 600 && m.y > 150 && m.y < 350) // normal
			{
				FlushMouseMsgBuffer();
				MODE = 2;
				return;
			}
			if (m.x > 650 && m.x < 850 && m.y > 150 && m.y < 350) // hard
			{
				FlushMouseMsgBuffer();
				MODE = 3;
				return;
			}
		}
	}
}

void side() // 玩家选择黑白
{
	putimage(0, 0, &background);
	settextstyle(30, 0, _T("黑体"));
	setbkmode(TRANSPARENT);

	// 黑白棋图和字
	RECT HEI = { 300, 400, 400, 500 };
	drawtext(_T("BLACK"), &HEI, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	RECT BAI = { 600, 400, 700, 500 };
	drawtext(_T("WHITE"), &BAI, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	loadimage(&Black, _T("b.jpg"), 200, 200, false);
	putimage(250, 150, &Black);
	loadimage(&White, _T("w.jpg"), 200, 200, false);
	putimage(550, 150, &White);
	RECT MAINBOARD = { 650, 450, 950, 550 };
	drawtext(_T("Return to Mainboard"), &MAINBOARD, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	settextstyle(20, 0, _T("黑体"));
	RECT tip = { 0, 500, 1000, 550 };
	drawtext(_T("Please click one of the pictures"), &tip, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	

	// 点击选择
	while (1)
	{
		m = GetMouseMsg();
		if (m.mkLButton)
		{
			if (m.x > 250 && m.x < 450 && m.y > 150 && m.y < 350) // 玩家执黑
			{
				FlushMouseMsgBuffer();
				SIDE = 1;
				return;
			}
			if (m.x > 550 && m.x < 750 && m.y > 150 && m.y < 350) // 玩家执白
			{
				FlushMouseMsgBuffer();
				SIDE = -1;
				return;
			}
			if (m.x > 650 && m.x < 950 && m.y > 485 && m.y < 515) // 点击返回主菜单
			{
				FlushMouseMsgBuffer();
				cleardevice();
				mainboard();
			}
		}
	}
}

int game() // 游戏界面
{
	char blacksum[3], whitesum[3];
	memset(stepping, 0, sizeof(stepping));
	memset(gridboard, 0, sizeof(gridboard));
	int turn = 0, flag = 0, gone = 0, ending = 0;
	putimage(0, 0, &background);
	loadimage(&board, _T("棋盘.jpg"), 500, 500, false);
	putimage(50, 50, &board);
	settextstyle(30, 0, _T("黑体"));
	setbkmode(TRANSPARENT);
	loadimage(&black, _T("b.jpg"), 60, 60, false);
	loadimage(&white, _T("w.jpg"), 60, 60, false);
	// 菜单
	RECT MAINBOARD = { 650, 450, 950, 550 };
	drawtext(_T("Return to Mainboard"), &MAINBOARD, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	RECT Side = { 650, 350, 950, 450 };
	RECT result = { 650, 100, 950, 200 };
	RECT resume = { 650, 250, 950, 350 };
	RECT s = { 650, 30, 950, 80 };
	RECT blackpoint = { 650, 80, 800, 130 };
	RECT whitepoint = { 800, 80, 950, 130 };
	RECT illegal = { 650, 170, 950, 250 };
	drawtext(_T("Resume"), &resume, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	if (NewOld) // 新开局
	{
		// 开局盘情况
		putimage(52, 52, &black);
		putimage(490, 490, &black);
		putimage(52, 490, &white);
		putimage(490, 52, &white);
		gridboard[0][0] = 1;
		gridboard[6][6] = 1;
		gridboard[0][6] = -1;
		gridboard[6][0] = -1;
		blackSum = 2;
		whiteSum = 2;
		turnId = 0;
		if (SIDE == 1) // 玩家执黑，先手
		{
			// 清空棋步历史记录
			switch (MODE)
			{
			case 1:
				stepout.open("step1");
				break;
			case 2:
				stepout.open("step2");
				break;
			case 3:
				stepout.open("step3");
				break;
			}
			stepout.close();
			turn = 1;
			putimage(900, 375, &black);
			drawtext(_T("BLACK     WHITE"), &s, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			drawtext(_T("Your Turn"), &Side, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			_itoa_s(whiteSum, whitesum, 10);
			_itoa_s(blackSum, blacksum, 10);
			drawtext(_T(whitesum), &whitepoint, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			drawtext(_T(blacksum), &blackpoint, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}
		else // 电脑执黑，先手
		{
			putimage(900, 375, &white);
			turn = 1;
			stepping[0].x0 = -1;
			stepping[0].y0 = -1;
			stepping[0].x1 = -1;
			stepping[0].y1 = -1;
			drawtext(_T("Opp's Turn"), &Side, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			drawtext(_T("BLACK     WHITE"), &s, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			_itoa_s(whiteSum, whitesum, 10);
			_itoa_s(blackSum, blacksum, 10);
			drawtext(_T(whitesum), &whitepoint, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			drawtext(_T(blacksum), &blackpoint, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			switch (MODE)
			{
			case 1:
				stepout.open("step1");
				break;
			case 2:
				stepout.open("step2");
				break;
			case 3:
				stepout.open("step3");
				break;
			}
			// 往step文档里输入回合和表示电脑执黑的数据
			stepout << turnId << endl;
			stepout << -1 << ' ' << ' ' << -1 << ' ' << -1 << ' ' << -1 << endl;
			turnId++;
			stepout.close();

			Sleep(1000);
			switch (MODE) // 根据难度调用不同的bot
			{
			case 1:
				main_Easy();
				break;
			case 2:
				main_Normal();
				break;
			case 3:
				main_Hard();
				break;
			}
			// 显示电脑选子情况
			setcolor(GREEN);
			rectangle(startX * 72 + 52, startY * 72 + 52, startX * 72 + 115, startY * 72 + 115);
			setcolor(WHITE);
			Sleep(500);
			switch (official(-SIDE))
			{
			case 1: // 合法棋步
				// 存盘
				switch (MODE)
				{
				case 1:
					stateout.open("boardstate1");
					break;
				case 2:
					stateout.open("boardstate2");
					break;
				case 3:
					stateout.open("boardstate3");
					break;
				}
				stateout << SIDE << endl << turn << endl << blackSum << endl << whiteSum << endl;
				for (int i = 0; i < 7; i++)
					for (int j = 0; j < 7; j++)
						stateout << gridboard[i][j] << ' ';
				stateout.close();
				// 显示新的棋局信息
				cleardevice();
				putimage(0, 0, &background);
				putimage(50, 50, &board);
				drawtext(_T("BLACK     WHITE"), &s, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				_itoa_s(whiteSum, whitesum, 10);
				_itoa_s(blackSum, blacksum, 10);
				drawtext(_T(whitesum), &whitepoint, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				drawtext(_T(blacksum), &blackpoint, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				drawtext(_T("Your Turn"), &Side, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				if (SIDE == 1)
					putimage(900, 375, &black);
				else
					putimage(900, 375, &white);
				drawtext(_T("Return to Mainboard"), &MAINBOARD, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				drawtext(_T("Resume"), &resume, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				for (int i = 0; i < 7; i++)
					for (int j = 0; j < 7; j++)
					{
						if (gridboard[i][j] == 1)
							putimage(i * 73 + 52, j * 73 + 52, &black);
						if (gridboard[i][j] == -1)
							putimage(i * 73 + 52, j * 73 + 52, &white);
					}
				break;
			case 0: // 无路可走
				ending = 1;
				// 清除盘信息
				switch (MODE)
				{
				case 1:
					stateout.open("boardstate1");
					break;
				case 2:
					stateout.open("boardstate2");
					break;
				case 3:
					stateout.open("boardstate3");
					break;
				}
				stateout << 0;
				stateout.close();
				cleardevice();
				putimage(0, 0, &background);
				putimage(50, 50, &board);
				drawtext(_T("BLACK     WHITE"), &s, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				_itoa_s(whiteSum, whitesum, 10);
				_itoa_s(blackSum, blacksum, 10);
				drawtext(_T(whitesum), &whitepoint, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				drawtext(_T(blacksum), &blackpoint, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				drawtext(_T("Return to Mainboard"), &MAINBOARD, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				drawtext(_T("Resume"), &resume, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				for (int i = 0; i < 7; i++)
					for (int j = 0; j < 7; j++)
					{
						if (gridboard[i][j] == 1)
							putimage(i * 73 + 52, j * 73 + 52, &black);
						if (gridboard[i][j] == -1)
							putimage(i * 73 + 52, j * 73 + 52, &white);
					}

				// 输出胜负结果
				if (blackSum > whiteSum)
					drawtext(_T("BLACK WINS!"), &result, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				else
					if (blackSum < whiteSum)
						drawtext(_T("WHITE WINS!"), &result, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					else
						drawtext(_T("DEUCE"), &result, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				break;
			case 2: // 非法棋步
				// 清除盘信息
				switch (MODE)
				{
				case 1:
					stateout.open("boardstate1");
					break;
				case 2:
					stateout.open("boardstate2");
					break;
				case 3:
					stateout.open("boardstate3");
					break;
				}
				stateout << 0;
				stateout.close();
				// 显示新的棋盘信息
				ending = 1;
				cleardevice();
				putimage(0, 0, &background);
				putimage(50, 50, &board);
				drawtext(_T("BLACK     WHITE"), &s, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				_itoa_s(whiteSum, whitesum, 10);
				_itoa_s(blackSum, blacksum, 10);
				drawtext(_T(whitesum), &whitepoint, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				drawtext(_T(blacksum), &blackpoint, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				drawtext(_T("Return to Mainboard"), &MAINBOARD, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				drawtext(_T("Resume"), &resume, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				drawtext(_T("ILLEGAL MOVE"), &illegal, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				for (int i = 0; i < 7; i++)
					for (int j = 0; j < 7; j++)
					{
						if (gridboard[i][j] == 1)
							putimage(i * 73 + 52, j * 73 + 52, &black);
						if (gridboard[i][j] == -1)
							putimage(i * 73 + 52, j * 73 + 52, &white);
					}

				// 输出胜负结果
				if (SIDE == 1)
					drawtext(_T("BLACK WINS!"), &result, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				else
					drawtext(_T("WHITE WINS!"), &result, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				break;
			}
		}
	}
	else // 不是新开局，分难度级别读取历史棋盘文件和历史行子文件并输出棋局
	{
		switch (MODE)
		{
		case 1:
			statein.open("boardstate1");
			break;
		case 2:
			statein.open("boardstate2");
			break;
		case 3:
			statein.open("boardstate3");
			break;
		}
		statein >> SIDE >> turn >> blackSum >> whiteSum;
		for (int i = 0; i < 7; i++)
			for (int j = 0; j < 7; j++)
			{
				statein >> gridboard[i][j];
				if (gridboard[i][j] == 1)
					putimage(i * 73 + 52, j * 73 + 52, &black);
				if (gridboard[i][j] == -1)
					putimage(i * 73 + 52, j * 73 + 52, &white);
			}
		statein.close();
		if (SIDE != 0) // 没有存盘资料！
		{
			drawtext(_T("Your Turn"), &Side, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			drawtext(_T("BLACK     WHITE"), &s, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			_itoa_s(whiteSum, whitesum, 10);
			_itoa_s(blackSum, blacksum, 10);
			drawtext(_T(whitesum), &whitepoint, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			drawtext(_T(blacksum), &blackpoint, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			if (SIDE == 1)
				putimage(900, 375, &black);
			else
				putimage(900, 375, &white);
			switch (MODE)
			{
			case 1:
				stepin.open("step1");
				break;
			case 2:
				stepin.open("step2");
				break;
			case 3:
				stepin.open("step3");
				break;
			}
			stepin >> turnId;
			for (int i = 0; i < turnId; i++)
				stepin >> stepping[i].x0 >> stepping[i].y0 >> stepping[i].x1 >> stepping[i].y1;
			stepin.close();
		}
	}

	// 玩家鼠标点击情况
	while (1)
	{
		m = GetMouseMsg();
		if (m.mkLButton)
		{
			if (m.x > 650 && m.x < 950 && m.y > 275 && m.y < 325) // 点击重新开始游戏
			{
				FlushMouseMsgBuffer();
				NewOld = 1;
				if (SIDE == 0) // 没有历史棋局信息
				{
					cleardevice();
					side();
				}
				cleardevice();
				game();
			}
			if (m.x > 650 && m.x < 950 && m.y > 485 && m.y < 515) // 点击返回主菜单
			{
				FlushMouseMsgBuffer();
				// 存盘（未完结棋局）
				if (!ending)
				{
					switch (MODE)
					{
					case 1:
						stateout.open("boardstate1");
						break;
					case 2:
						stateout.open("boardstate2");
						break;
					case 3:
						stateout.open("boardstate3");
						break;
					}
					stateout << SIDE << endl << turn << endl << blackSum << endl << whiteSum << endl;
					for (int i = 0; i < 7; i++)
						for (int j = 0; j < 7; j++)
							stateout << gridboard[i][j] << ' ';
					stateout.close();
				}
				cleardevice();
				mainboard();
			}
			else
				if (m.x > 50 && m.x < 550 && m.y > 50 && m.y < 550 && !ending) // 在棋局完结之前点击棋盘范围
				{
					if (turn) // 轮到玩家下棋
					{
						gone = 0;
						if (flag) // 已经选择了棋
						{
							resultX = (m.x - 50) / 72;
							resultY = (m.y - 50) / 72;
							flag = 0;
							// 任点棋子或不在走棋区域内的空格处取消选棋
							if (gridboard[resultX][resultY] != 0 || abs(resultX - startX) > 2 || abs(resultY - startY) > 2)
							{
								FlushMouseMsgBuffer();
								cleardevice();
								putimage(0, 0, &background);
								putimage(50, 50, &board);
								if (SIDE == 1)
									putimage(900, 375, &black);
								else
									putimage(900, 375, &white);
								drawtext(_T("BLACK     WHITE"), &s, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
								_itoa_s(whiteSum, whitesum, 10);
								_itoa_s(blackSum, blacksum, 10);
								drawtext(_T(whitesum), &whitepoint, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
								drawtext(_T(blacksum), &blackpoint, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
								drawtext(_T("Your Turn"), &Side, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
								drawtext(_T("Return to Mainboard"), &MAINBOARD, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
								drawtext(_T("Resume"), &resume, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
								for (int i = 0; i < 7; i++)
									for (int j = 0; j < 7; j++)
									{
										if (gridboard[i][j] == 1)
											putimage(i * 73 + 52, j * 73 + 52, &black);
										if (gridboard[i][j] == -1)
											putimage(i * 73 + 52, j * 73 + 52, &white);
									}
								continue;
							}
							gone = 1; // 已经走棋
						}
						else
						{
							startX = (m.x - 50) / 72;
							startY = (m.y - 50) / 72;
							if (gridboard[startX][startY] != SIDE) // 没选己方棋子
							{
								FlushMouseMsgBuffer();
								continue;
							}
							// 突出已选中的棋子
							setcolor(GREEN);
							rectangle(startX * 72 + 52, startY * 72 + 52, startX * 72 + 115, startY * 72 + 115);
							setcolor(WHITE);
							flag = 1;
						}
						if (gone) // 玩家已经走棋
						{
							switch (official(SIDE))
							{
							case 1:
								turn = 0; // 轮到电脑走棋
								// 存盘
								switch (MODE)
								{
								case 1:
									stateout.open("boardstate1");
									break;
								case 2:
									stateout.open("boardstate2");
									break;
								case 3:
									stateout.open("boardstate3");
									break;
								}
								stateout << SIDE << endl << turn << endl << blackSum << endl << whiteSum << endl;
								for (int i = 0; i < 7; i++)
									for (int j = 0; j < 7; j++)
										stateout << gridboard[i][j] << ' ';
								stateout.close();
								// 显示新的棋局信息
								cleardevice();
								putimage(0, 0, &background);
								putimage(50, 50, &board);
								if (SIDE == 1)
									putimage(900, 375, &black);
								else
									putimage(900, 375, &white);
								drawtext(_T("BLACK     WHITE"), &s, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
								_itoa_s(whiteSum, whitesum, 10);
								_itoa_s(blackSum, blacksum, 10);
								drawtext(_T(whitesum), &whitepoint, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
								drawtext(_T(blacksum), &blackpoint, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
								drawtext(_T("Opp's Turn"), &Side, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
								drawtext(_T("Return to Mainboard"), &MAINBOARD, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
								drawtext(_T("Resume"), &resume, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
								for (int i = 0; i < 7; i++)
									for (int j = 0; j < 7; j++)
									{
										if (gridboard[i][j] == 1)
											putimage(i * 73 + 52, j * 73 + 52, &black);
										if (gridboard[i][j] == -1)
											putimage(i * 73 + 52, j * 73 + 52, &white);
									}

								// 电脑走棋
								Sleep(1000);
								switch (MODE)
								{
								case 1:
									main_Easy();
									break;
								case 2:
									main_Normal();
									break;
								case 3:
									main_Hard();
									break;
								}
								setcolor(GREEN);
								rectangle(startX * 72 + 52, startY * 72 + 52, startX * 72 + 115, startY * 72 + 115);
								setcolor(WHITE);
								Sleep(500);
								switch (official(-SIDE))
								{
								case 1: // 合法棋步
									turn = 1;
									// 存盘
									switch (MODE)
									{
									case 1:
										stateout.open("boardstate1");
										break;
									case 2:
										stateout.open("boardstate2");
										break;
									case 3:
										stateout.open("boardstate3");
										break;
									}
									stateout << SIDE << endl << turn << endl << blackSum << endl << whiteSum << endl;
									for (int i = 0; i < 7; i++)
										for (int j = 0; j < 7; j++)
											stateout << gridboard[i][j] << ' ';
									stateout.close();
									// 显示新的棋局信息
									cleardevice();
									putimage(0, 0, &background);
									putimage(50, 50, &board);
									if (SIDE == 1)
										putimage(900, 375, &black);
									else
										putimage(900, 375, &white);
									drawtext(_T("BLACK     WHITE"), &s, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
									_itoa_s(whiteSum, whitesum, 10);
									_itoa_s(blackSum, blacksum, 10);
									drawtext(_T(whitesum), &whitepoint, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
									drawtext(_T(blacksum), &blackpoint, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
									drawtext(_T("Your Turn"), &Side, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
									drawtext(_T("Return to Mainboard"), &MAINBOARD, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
									drawtext(_T("Resume"), &resume, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
									for (int i = 0; i < 7; i++)
										for (int j = 0; j < 7; j++)
										{
											if (gridboard[i][j] == 1)
												putimage(i * 73 + 52, j * 73 + 52, &black);
											if (gridboard[i][j] == -1)
												putimage(i * 73 + 52, j * 73 + 52, &white);
										}
									break;
								case 0: // 无路可走
									// 清除盘信息
									switch (MODE)
									{
									case 1:
										stateout.open("boardstate1");
										break;
									case 2:
										stateout.open("boardstate2");
										break;
									case 3:
										stateout.open("boardstate3");
										break;
									}
									stateout << 0;
									stateout.close();
									cleardevice();
									putimage(0, 0, &background);
									putimage(50, 50, &board);
									drawtext(_T("BLACK     WHITE"), &s, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
									_itoa_s(whiteSum, whitesum, 10);
									_itoa_s(blackSum, blacksum, 10);
									drawtext(_T(whitesum), &whitepoint, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
									drawtext(_T(blacksum), &blackpoint, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
									drawtext(_T("Return to Mainboard"), &MAINBOARD, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
									drawtext(_T("Resume"), &resume, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
									for (int i = 0; i < 7; i++)
										for (int j = 0; j < 7; j++)
										{
											if (gridboard[i][j] == 1)
												putimage(i * 73 + 52, j * 73 + 52, &black);
											if (gridboard[i][j] == -1)
												putimage(i * 73 + 52, j * 73 + 52, &white);
										}
									ending = 1;
									if (blackSum > whiteSum)
										drawtext(_T("BLACK WINS!"), &result, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
									else
										if (blackSum < whiteSum)
											drawtext(_T("WHITE WINS!"), &result, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
										else
											drawtext(_T("DEUCE"), &result, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
									break;
								case 2: // 非法棋步
									// 清除盘信息
									switch (MODE)
									{
									case 1:
										stateout.open("boardstate1");
										break;
									case 2:
										stateout.open("boardstate2");
										break;
									case 3:
										stateout.open("boardstate3");
										break;
									}
									stateout << 0;
									stateout.close();
									// 显示新的棋局信息
									ending = 1;
									cleardevice();
									putimage(0, 0, &background);
									putimage(50, 50, &board);
									drawtext(_T("BLACK     WHITE"), &s, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
									_itoa_s(whiteSum, whitesum, 10);
									_itoa_s(blackSum, blacksum, 10);
									drawtext(_T(whitesum), &whitepoint, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
									drawtext(_T(blacksum), &blackpoint, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
									drawtext(_T("Return to Mainboard"), &MAINBOARD, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
									drawtext(_T("Resume"), &resume, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
									drawtext(_T("ILLEGAL MOVE"), &illegal, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
									for (int i = 0; i < 7; i++)
										for (int j = 0; j < 7; j++)
										{
											if (gridboard[i][j] == 1)
												putimage(i * 73 + 52, j * 73 + 52, &black);
											if (gridboard[i][j] == -1)
												putimage(i * 73 + 52, j * 73 + 52, &white);
										}
									if (SIDE == 1)
										drawtext(_T("BLACK WINS!"), &result, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
									else
										drawtext(_T("WHITE WINS!"), &result, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
									break;
								}
								break;
							case 0: // 无路可走
								ending = 1;
								// 清除盘信息
								switch (MODE)
								{
								case 1:
									stateout.open("boardstate1");
									break;
								case 2:
									stateout.open("boardstate2");
									break;
								case 3:
									stateout.open("boardstate3");
									break;
								}
								stateout << 0;
								stateout.close();
								cleardevice();
								putimage(0, 0, &background);
								putimage(50, 50, &board);
								drawtext(_T("BLACK     WHITE"), &s, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
								_itoa_s(whiteSum, whitesum, 10);
								_itoa_s(blackSum, blacksum, 10);
								drawtext(_T(whitesum), &whitepoint, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
								drawtext(_T(blacksum), &blackpoint, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
								drawtext(_T("Return to Mainboard"), &MAINBOARD, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
								drawtext(_T("Resume"), &resume, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
								for (int i = 0; i < 7; i++)
									for (int j = 0; j < 7; j++)
									{
										if (gridboard[i][j] == 1)
											putimage(i * 73 + 52, j * 73 + 52, &black);
										if (gridboard[i][j] == -1)
											putimage(i * 73 + 52, j * 73 + 52, &white);
									}
								if (blackSum > whiteSum)
									drawtext(_T("BLACK WINS!"), &result, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
								else
									if (blackSum < whiteSum)
										drawtext(_T("WHITE WINS!"), &result, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
									else
										drawtext(_T("DEUCE"), &result, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
								break;
							// 玩家是不存在非法棋步的>_<
							}
						}
					}
					FlushMouseMsgBuffer();
				}
		}
	}
	return 0;
}

// 裁判函数
int official(int color)
{
	if (!Procsstep(startX, startY, resultX, resultY, color, false)) // 判断是否合法棋步并走棋
		return 2; // 非法棋步，完结
	if (!CheckIfHasValidMove(-1) || !CheckIfHasValidMove(1) || blackSum == 0 || whiteSum == 0) // 只要有一方无法走棋，就完结了
		return 0;
	return 1; // 合法棋步，仍能继续
}

inline bool inMap(int x, int y)
{
	if (x < 0 || x > 6 || y < 0 || y > 6)
		return false;
	return true;
}

bool Procsstep(int x0, int y0, int x1, int y1, int color, bool checkOnly)
{
	if (color == 0)
		return false;
	if (x0 == -1) // 弃权，判输
		return false;
	if (!inMap(x0, y0) || !inMap(x1, y1)) // 超出边界
		return false;
	if (gridboard[x0][y0] != color)
		return false;
	int dx, dy, x, y, currCount = 0, dir;
	dx = abs((x0 - x1)), dy = abs((y0 - y1));
	if ((dx == 0 && dy == 0) || dx > 2 || dy > 2) // 保证不会移动到原来位置，而且移动始终在5×5区域内
		return false;
	if (gridboard[x1][y1] != 0) // 保证移动到的位置为空
		return false;
	if (checkOnly)
		return true; // 只是查看而不在后台走棋
	if (dx == 2 || dy == 2) // 如果走的是5×5的外围，则不是复制粘贴
		gridboard[x0][y0] = 0;
	else
	{
		if (color == 1)
			blackSum++;
		else
			whiteSum++;
	}

	// 记录走棋，输出到历史记录中
	stepping[turnId].x0 = startX;
	stepping[turnId].y0 = startY;
	stepping[turnId].x1 = resultX;
	stepping[turnId].y1 = resultY;
	gridboard[x1][y1] = color;
	turnId++;
	switch (MODE)
	{
	case 1:
		stepout.open("step1");
		break;
	case 2:
		stepout.open("step2");
		break;
	case 3:
		stepout.open("step3");
		break;
	}
	stepout << turnId << endl;
	for (int i = 0; i < turnId; i++)
		stepout << stepping[i].x0 << ' ' << stepping[i].y0 << ' ' << stepping[i].x1 << ' ' << stepping[i].y1 << endl;
	stepout.close();
	for (dir = 0; dir < 8; dir++) // 影响邻近8个位置
	{
		x = x1 + delta[dir][0];
		y = y1 + delta[dir][1];
		if (!inMap(x, y))
			continue;
		if (gridboard[x][y] == -color)
		{
			currCount++;
			gridboard[x][y] = color;
		}
	}
	if (currCount != 0)
	{
		if (color == 1)
		{
			blackSum += currCount;
			whiteSum -= currCount;
		}
		else
		{
			whiteSum += currCount;
			blackSum -= currCount;
		}
	}
	return true;
}

bool CheckIfHasValidMove(int color) // 看每方是否还可行棋
{
	int x, y;
	for (y = 0; y < 7; y++)
		for (x = 0; x < 7; x++)
			for (int dir = 0; dir < 24; dir++)
				if (Procsstep(x, y, x + delta[dir][0], y + delta[dir][1], color, true))
					return true;
	return false;
}