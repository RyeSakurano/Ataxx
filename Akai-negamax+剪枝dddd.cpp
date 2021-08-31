// ATAXX-Akai Alpha-Beta 剪枝 

#include <iostream>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cmath>
#include <fstream>

using namespace std;

int myColor; // 我所执子颜色（1为黑，-1为白，棋盘状态亦同）
int gridInfo[7][7] = { 0 }; // 行、列，记录棋盘状态
int Round[10][8] = { 0 };
int visit[7][7], space, dd, startTime, edge;
int blackCount = 2, whiteCount = 2, sumCount;
int startX, startY, resultX, resultY;
static int delta[24][2] = { { 1,1 },{ 0,1 },{ -1,1 },{ -1,0 },
{ -1,-1 },{ 0,-1 },{ 1,-1 },{ 1,0 },
{ 2,0 },{ 2,1 },{ 2,2 },{ 1,2 },
{ 0,2 },{ -1,2 },{ -2,2 },{ -2,1 },
{ -2,0 },{ -2,-1 },{ -2,-2 },{ -1,-2 },
{ 0,-2 },{ 1,-2 },{ 2,-2 },{ 2,-1 } }; // 24种可能走法 
static int valueFIRST[7][7] = { { 4, 3, 3, 2, 3, 3, 4 },{ 3, 2, 2, 1, 2, 2, 3 },
{ 3, 2, 2, 1, 2, 2, 3 },{ 2, 1, 1, 1, 1, 1, 2 },{ 3, 2, 2, 1, 2, 2, 3 },
{ 3, 2, 2, 1, 2, 2, 3 },{ 4, 3, 3, 2, 3, 3, 4 } };
static int valueSECOND[7][7] = { { 1, 2, 2, 2, 2, 2, 1 },{ 2, 3, 4, 4, 4, 3, 2 },
{ 2, 4, 3, 3, 3, 4, 2 },{ 2, 4, 3, 3, 3, 4, 2 },{ 2, 4, 3, 3, 3, 4, 2 },
{ 2, 3, 4, 4, 4, 3, 2 },{ 1, 2, 2, 2, 2, 2, 1 } }; // 每个格子的评价 
ifstream fin3;

												   // 判断是否在地图内
inline bool inMap(int x, int y)
{
	if (x < 0 || x > 6 || y < 0 || y > 6)
		return false;
	return true;
}

int rec(int color) // 计算最大矩形 
{
	int res = 0, height[7] = { 0 }, leftt[7] = { 0 }, rightt[7];
	for (int i = 0; i < 7; i++)
		rightt[i] = 7;
	for (int i = 0; i < 7; i++)
	{
		int cur_left = 0, cur_right = 7;
		for (int j = 0; j < 7; j++)
		{
			if (gridInfo[i][j] == color)
			{
				height[j]++;
				leftt[j] = max(leftt[j], cur_left);
			}
			else
			{
				height[j] = 0;
				leftt[j] = 0;
				cur_left = j + 1;
			}
		}
		for (int j = 6; j >= 0; j--)
		{
			if (gridInfo[i][j] == color)
			{
				rightt[j] = min(rightt[j], cur_right);
			}
			else
			{
				rightt[j] = 7;
				cur_right = j;
			}
			res = max(res, (rightt[j] - leftt[j]) * height[j]);
		}
	}
	return res;
}

// 蜜汁估值函数 
int Evaluate()
{
	int modality = 0, sp = 0, dir;
	sumCount = blackCount + whiteCount;
	for (int i = 0; i < 7; i++)
		for (int j = 0; j < 7; j++)
		{
			if (sumCount <= 15) // 分阶段 
				if (myColor == -1)
					modality += gridInfo[i][j] * valueFIRST[i][j]; // 占有的格子
				else
					modality -= gridInfo[i][j] * valueFIRST[i][j];
			else
				if (myColor == -1)
					modality += gridInfo[i][j] * valueSECOND[i][j];
				else
					modality -= gridInfo[i][j] * valueSECOND[i][j];
		}
	sp = rec(myColor) - rec(-myColor);
	return modality + sp * sp;
}

// 在坐标处落子，检查是否合法或模拟落子
bool ProcStep(int x0, int y0, int x1, int y1, int color, int depth)
{
	if (color == 0)
		return false;
	if (x1 == -1) // 无路可走，跳过此回合
		return true;
	if (!inMap(x0, y0) || !inMap(x1, y1)) // 超出边界
		return false;
	if (gridInfo[x0][y0] != color)
		return false;
	int dx, dy, x, y, currCount = 0, dir;
	dx = abs((x0 - x1));
	dy = abs((y0 - y1));
	if ((dx == 0 && dy == 0) || dx > 2 || dy > 2) // 保证不会移动到原来位置，而且移动始终在5×5区域内
		return false;
	if (gridInfo[x1][y1] != 0) // 保证移动到的位置为空
		return false;
	if (dx == 2 || dy == 2) // 如果走的是5×5的外围，则不是复制粘贴
		gridInfo[x0][y0] = 0;
	else
	{
		if (color == 1)
			blackCount++;
		else
			whiteCount++;
	}

	gridInfo[x1][y1] = color;
	for (dir = 0; dir < 8; dir++) // 影响邻近8个位置
	{
		Round[depth][dir] = 0; // 初始化 
		x = x1 + delta[dir][0];
		y = y1 + delta[dir][1];
		if (!inMap(x, y))
			continue;
		if (gridInfo[x][y] == -color)
		{
			currCount++;
			Round[depth][dir] = 1; // 标记这个是被影响了的 
			gridInfo[x][y] = color;
		}
	}
	if (currCount != 0)
	{
		if (color == 1)
		{
			blackCount += currCount;
			whiteCount -= currCount;
		}
		else
		{
			whiteCount += currCount;
			blackCount -= currCount;
		}
	}
	return true;
}

// 在坐标处落子，检查是否合法或模拟落子
bool ProcStep1(int x0, int y0, int x1, int y1, int color)
{
	if (color == 0)
		return false;
	if (x1 == -1) // 无路可走，跳过此回合
		return true;
	if (!inMap(x0, y0) || !inMap(x1, y1)) // 超出边界
		return false;
	if (gridInfo[x0][y0] != color)
		return false;
	int dx, dy, x, y, currCount = 0, dir;
	dx = abs((x0 - x1));
	dy = abs((y0 - y1));
	if ((dx == 0 && dy == 0) || dx > 2 || dy > 2) // 保证不会移动到原来位置，而且移动始终在5×5区域内
		return false;
	if (gridInfo[x1][y1] != 0) // 保证移动到的位置为空
		return false;
	if (dx == 2 || dy == 2) // 如果走的是5×5的外围，则不是复制粘贴
		gridInfo[x0][y0] = 0;
	else
	{
		if (color == 1)
			blackCount++;
		else
			whiteCount++;
	}

	gridInfo[x1][y1] = color;
	for (dir = 0; dir < 8; dir++) // 影响邻近8个位置
	{
		x = x1 + delta[dir][0];
		y = y1 + delta[dir][1];
		if (!inMap(x, y))
			continue;
		if (gridInfo[x][y] == -color)
		{
			currCount++;
			gridInfo[x][y] = color;
		}
	}
	if (currCount != 0)
	{
		if (color == 1)
		{
			blackCount += currCount;
			whiteCount -= currCount;
		}
		else
		{
			whiteCount += currCount;
			blackCount -= currCount;
		}
	}
	return true;
}

// 清除落子 
bool DeleteStep(int x0, int y0, int x1, int y1, int color, int depth)
{
	gridInfo[x1][y1] = 0;
	int dx, dy, x, y, currCount = 0, dir;
	dx = abs((x0 - x1));
	dy = abs((y0 - y1));
	if (dx == 2 || dy == 2) // 如果走的是5*5的外围，则不是复制粘贴
		gridInfo[x0][y0] = color;
	else
	{
		if (color == 1)
			blackCount--;
		else
			whiteCount--;
	}

	// 恢复影响邻近8个位置 
	for (dir = 0; dir < 8; dir++)
	{
		x = x1 + delta[dir][0];
		y = y1 + delta[dir][1];
		if (!inMap(x, y))
			continue;
		if (Round[depth][dir])
		{
			currCount++;
			gridInfo[x][y] = -color;
		}
	}
	if (currCount != 0)
	{
		if (color == 1)
		{
			blackCount -= currCount;
			whiteCount += currCount;
		}
		else
		{
			whiteCount -= currCount;
			blackCount += currCount;
		}
	}
	return true;
}

bool Timeout()
{
	if (clock() - startTime > CLOCKS_PER_SEC)
		return true;
}

// AlphaBeta剪枝 
int AlphaBeta(int depth, int alpha, int beta, int color)
{
	int x0, x1, y0, y1, dir, val, posCount = 0;
	if (depth == 0)
		if (color == myColor)
			return -Evaluate(); // 搜到了所需层数，返回评估函数 
		else
			return Evaluate();

						   // 在之前的层数 
	for (y0 = 0; y0 < 7; y0++)
		for (x0 = 0; x0 < 7; x0++) // 寻找出棋点 
		{
			if (gridInfo[x0][y0] != color)
				continue; // 不是当前正在持子者的棋，忽略 
			for (dir = 0; dir < 24; dir++) // 24种落子法 
			{
				x1 = x0 + delta[dir][0];
				y1 = y0 + delta[dir][1];
				if (!inMap(x1, y1))
					continue; // 没越界 
				if (gridInfo[x1][y1] != 0)
					continue; // 已经有棋在了
				posCount++;
				ProcStep(x0, y0, x1, y1, color, depth); // 落子 
				val = -AlphaBeta(depth - 1, -beta, -alpha, -color); // 递归子节点，换方，颜色改变，ab对换 
				DeleteStep(x0, y0, x1, y1, color, depth); // 清除落子 
				if (val >= beta)
				{
					return beta; // 比beta大，坏 
				}
				if (val > alpha)
				{
					if (depth == dd)
					{
						startX = x0;
						startY = y0;
						resultX = x1;
						resultY = y1;
					}
					alpha = val; // 比alpha好，是目前最好的
				}
			}
		}
	if (posCount)
		return alpha;
	else
		if (color == myColor)
			return -Evaluate(); // 搜到了所需层数，返回评估函数 
		else
			return Evaluate();
}

int main_Hard()
{
	istream::sync_with_stdio(false);

	int x0, y0, x1, y1, flag = 0; 
	startX = -1;
	startY = -1;
	resultX = -1;
	resultY = -1;

	// 初始化棋盘
	memset(gridInfo, 0, sizeof(gridInfo));
	blackCount = 2;
	whiteCount = 2;
	memset(Round, 0, sizeof(Round));
	gridInfo[0][0] = gridInfo[6][6] = 1;  //|黑|白|
	gridInfo[6][0] = gridInfo[0][6] = -1; //|白|黑|

										  // 分析自己收到的输入和自己过往的输出，并恢复状态
	int turnID;
	myColor = -1;
	fin3.open("step3");
	fin3 >> turnID;
	turnID = turnID / 2 + 1;
	for (int i = 0; i < turnID - 1; i++)
	{
		// 根据这些输入输出逐渐恢复状态到当前回合
		fin3 >> x0 >> y0 >> x1 >> y1;
		if (x1 >= 0)
			ProcStep1(x0, y0, x1, y1, -myColor); // 模拟对方落子
		else
			myColor = 1; // 第一回合收到坐标是-1, -1，说明我是黑方
		fin3 >> x0 >> y0 >> x1 >> y1;
		if (x1 >= 0)
			ProcStep1(x0, y0, x1, y1, myColor); // 模拟己方落子
	}

	// 自己本回合输入
	fin3 >> x0 >> y0 >> x1 >> y1;
	if (x1 >= 0)
		ProcStep1(x0, y0, x1, y1, -myColor); // 模拟对方落子
	else
		myColor = 1; // 第一回合收到坐标是-1, -1，说明我是黑方

					 // 做出决策
	int val;
	startTime = clock();
	for (dd = 1; ; dd++)
	{
		val = AlphaBeta(dd, -INT8_MAX, INT8_MAX, myColor);
		if (Timeout())
			break;
	}
	
	// 决策结束，输出结果

	fin3.close();
	return 0;
}
