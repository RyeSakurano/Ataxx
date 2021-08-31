// Ataxx-Furuya 一步贪心

#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <fstream>
using namespace std;

int mycolor; // 我所执子颜色（1为黑，-1为白，棋盘状态亦同）
int gridinfo[7][7] = { 0 }; // 行、列，记录棋盘状态
int blackcount = 2, whitecount = 2;
static int delta[24][2] = { { 1,1 },{ 0,1 },{ -1,1 },{ -1,0 },
{ -1,-1 },{ 0,-1 },{ 1,-1 },{ 1,0 },
{ 2,0 },{ 2,1 },{ 2,2 },{ 1,2 },
{ 0,2 },{ -1,2 },{ -2,2 },{ -2,1 },
{ -2,0 },{ -2,-1 },{ -2,-2 },{ -1,-2 },
{ 0,-2 },{ 1,-2 },{ 2,-2 },{ 2,-1 } }; // 24种可能走法 
ifstream fin2;

// 判断是否在地图内
inline bool inMap(int x, int y)
{
	if (x < 0 || x > 6 || y < 0 || y > 6)
		return false;
	return true;
}

// 向Direction方向改动坐标，并返回是否越界
inline bool MoveStep(int &x, int &y, int Direction)
{
	x = x + delta[Direction][0];
	y = y + delta[Direction][1];
	return inMap(x, y);
}

// 在坐标处落子，检查是否合法或模拟落子
bool ProcStep(int x0, int y0, int x1, int y1, int color)
{
	if (color == 0)
		return false;
	if (x1 == -1) // 无路可走，跳过此回合
		return true;
	if (!inMap(x0, y0) || !inMap(x1, y1)) // 超出边界
		return false;
	if (gridinfo[x0][y0] != color)
		return false;
	int dx, dy, x, y, currCount = 0, dir;
	dx = abs((x0 - x1)), dy = abs((y0 - y1));
	if ((dx == 0 && dy == 0) || dx > 2 || dy > 2) // 保证不会移动到原来位置，而且移动始终在5×5区域内
		return false;
	if (gridinfo[x1][y1] != 0) // 保证移动到的位置为空
		return false;
	if (dx == 2 || dy == 2) // 如果走的是5×5的外围，则不是复制粘贴
		gridinfo[x0][y0] = 0;
	else
	{
		if (color == 1)
			blackcount++;
		else
			whitecount++;
	}

	gridinfo[x1][y1] = color;
	for (dir = 0; dir < 8; dir++) // 影响邻近8个位置
	{
		x = x1 + delta[dir][0];
		y = y1 + delta[dir][1];
		if (!inMap(x, y))
			continue;
		if (gridinfo[x][y] == -color)
		{
			currCount++;
			gridinfo[x][y] = color;
		}
	}
	if (currCount != 0)
	{
		if (color == 1)
		{
			blackcount += currCount;
			whitecount -= currCount;
		}
		else
		{
			whitecount += currCount;
			blackcount -= currCount;
		}
	}
	return true;
}

// 模拟落子 
int TryStep(int x0, int y0, int x1, int y1, int color)
{
	int dx, dy, x, y, currCount = 0, dir, my, opps;
	if (color == 1) // 记录我和对手的棋子数 
	{
		my = blackcount;
		opps = whitecount;
	}
	else
	{
		my = whitecount;
		opps = blackcount;
	}
	dx = abs((x0 - x1)), dy = abs((y0 - y1));
	if (dx == 2 || dy == 2) // 如果走的是5×5的外围，则不是复制粘贴
		my = my;
	else
		my++; // 复制粘贴 
	for (dir = 0; dir < 8; dir++) // 影响邻近8个位置
	{
		x = x1 + delta[dir][0];
		y = y1 + delta[dir][1];
		if (!inMap(x, y))
			continue;
		if (gridinfo[x][y] == -color)
			currCount++;
	}
	if (currCount != 0)
	{
		my += currCount;
		opps -= currCount;
	}
	return (my - opps); // 返回差值 
}
int main_Normal()
{
	istream::sync_with_stdio(false);

	int x0, y0, x1, y1;

	// 初始化棋盘
	memset(gridinfo, 0, sizeof(gridinfo));
	blackcount = 2;
	whitecount = 2;
	gridinfo[0][0] = gridinfo[6][6] = 1;  //|黑|白|
	gridinfo[6][0] = gridinfo[0][6] = -1; //|白|黑|

	// 分析自己收到的输入和自己过往的输出，并恢复状态
	int turnID;
	mycolor = -1;
	fin2.open("step2");
	fin2 >> turnID;
	turnID = turnID / 2 + 1;
	for (int i = 0; i < turnID - 1; i++)
	{
		// 根据这些输入输出逐渐恢复状态到当前回合
		fin2 >> x0 >> y0 >> x1 >> y1;
		if (x1 >= 0)
			ProcStep(x0, y0, x1, y1, -mycolor); // 模拟对方落子
    	else
    		mycolor = 1; // 第一回合收到坐标是-1, -1，说明我是黑方
		fin2 >> x0 >> y0 >> x1 >> y1;
		if (x1 >= 0)
			ProcStep(x0, y0, x1, y1, mycolor); // 模拟己方落子
	}

	// 看看自己本回合输入
	fin2 >> x0 >> y0 >> x1 >> y1;
	if (x1 >= 0)
		ProcStep(x0, y0, x1, y1, -mycolor); // 模拟对方落子
	else
		mycolor = 1; // 第一回合收到坐标是-1, -1，说明我是黑方

	// 找出合法落子点
	int beginPos[1000][2] = { 0 }, possiblePos[1000][2] = { 0 }, insume[1000] = { 0 }, posCount = 0, dir;

	for (y0 = 0; y0 < 7; y0++)
		for (x0 = 0; x0 < 7; x0++)
		{
			if (gridinfo[x0][y0] != mycolor)
				continue;
			for (dir = 0; dir < 24; dir++)
			{
				x1 = x0 + delta[dir][0];
				y1 = y0 + delta[dir][1];
				if (!inMap(x1, y1))
					continue;
				if (gridinfo[x1][y1] != 0)
					continue;
				beginPos[posCount][0] = x0;
				beginPos[posCount][1] = y0;
				possiblePos[posCount][0] = x1;
				possiblePos[posCount][1] = y1;
				insume[posCount] = TryStep(x0, y0, x1, y1, mycolor); // 计算子数目差值 
				posCount++;
			}
		}

	// 做出决策

	extern int startX, startY, resultX, resultY;
	int maxi = -100, maxCount;
	if (posCount > 0)
	{
		for (int i = 0; i < posCount; i++)
		{
			if (maxi < insume[i]) // 选择子数目差值最大的走法 
			{
				maxi = insume[i];
				maxCount = i;
			}
		}
		startX = beginPos[maxCount][0];
		startY = beginPos[maxCount][1];
		resultX = possiblePos[maxCount][0];
		resultY = possiblePos[maxCount][1];
	}
	else // 无路可走 
	{
		startX = -1;
		startY = -1;
		resultX = -1;
		resultY = -1;
	}

	// 决策结束，输出结果
	fin2.close();
	return 0;
}
