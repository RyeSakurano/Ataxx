// Ataxx-Hattori 随机+筛选落子

#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <fstream>
using namespace std;

int my_color; // 我所执子颜色（1为黑，-1为白，棋盘状态亦同）
int grid_info[7][7] = { 0 }; // 行、列，记录棋盘状态
int black_count = 2, white_count = 2;
static int delta[24][2] = { { 1,1 },{ 0,1 },{ -1,1 },{ -1,0 },
{ -1,-1 },{ 0,-1 },{ 1,-1 },{ 1,0 },
{ 2,0 },{ 2,1 },{ 2,2 },{ 1,2 },
{ 0,2 },{ -1,2 },{ -2,2 },{ -2,1 },
{ -2,0 },{ -2,-1 },{ -2,-2 },{ -1,-2 },
{ 0,-2 },{ 1,-2 },{ 2,-2 },{ 2,-1 } }; // 24种可能走法 
ifstream fin1;

// 判断是否在地图内
inline bool inMap(int x, int y)
{
	if (x < 0 || x > 6 || y < 0 || y > 6)
		return false;
	return true;
}

// 在坐标处落子，检查是否合法或模拟落子
bool Procstep(int x0, int y0, int x1, int y1, int color)
{
	if (color == 0)
		return false;
	if (x1 == -1) // 无路可走，跳过此回合
		return true;
	if (!inMap(x0, y0) || !inMap(x1, y1)) // 超出边界
		return false;
	if (grid_info[x0][y0] != color)
		return false;
	int dx, dy, x, y, currCount = 0, dir;
	dx = abs((x0 - x1)), dy = abs((y0 - y1));
	if ((dx == 0 && dy == 0) || dx > 2 || dy > 2) // 保证不会移动到原来位置，而且移动始终在5×5区域内
		return false;
	if (grid_info[x1][y1] != 0) // 保证移动到的位置为空
		return false;
	if (dx == 2 || dy == 2) // 如果走的是5×5的外围，则不是复制粘贴
		grid_info[x0][y0] = 0;
	else
	{
		if (color == 1)
			black_count++;
		else
			white_count++;
	}

	grid_info[x1][y1] = color;
	for (dir = 0; dir < 8; dir++) // 影响邻近8个位置
	{
		x = x1 + delta[dir][0];
		y = y1 + delta[dir][1];
		if (!inMap(x, y))
			continue;
		if (grid_info[x][y] == -color)
		{
			currCount++;
			grid_info[x][y] = color;
		}
	}
	if (currCount != 0)
	{
		if (color == 1)
		{
			black_count += currCount;
			white_count -= currCount;
		}
		else
		{
			white_count += currCount;
			black_count -= currCount;
		}
	}
	return true;
}

// 模拟落子 
int Trystep(int x0, int y0, int x1, int y1, int color)
{
	int dx, dy, x, y, currCount = 0, dir, my, opps;
	if (color == 1) // 记录我和对手的棋子数 
	{
		my = black_count;
		opps = white_count;
	}
	else
	{
		my = white_count;
		opps = black_count;
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
		if (grid_info[x][y] == -color)
			currCount++;
	}
	if (currCount != 0)
	{
		my += currCount;
		opps -= currCount;
	}
	return (my - opps); // 返回差值 
}
int main_Easy()
{
	istream::sync_with_stdio(false);

	int x0, y0, x1, y1;

	// 初始化棋盘
	memset(grid_info, 0, sizeof(grid_info));
	grid_info[0][0] = grid_info[6][6] = 1;  //|黑|白|
	grid_info[6][0] = grid_info[0][6] = -1; //|白|黑|

											// 分析自己收到的输入和自己过往的输出，并恢复状态
	int turnID;
	my_color = -1;
	fin1.open("step1");
	fin1 >> turnID;
	turnID = turnID / 2 + 1;
	for (int i = 0; i < turnID - 1; i++)
	{
		// 根据这些输入输出逐渐恢复状态到当前回合
		fin1 >> x0 >> y0 >> x1 >> y1;
		if (x1 >= 0)
			Procstep(x0, y0, x1, y1, -my_color); // 模拟对方落子
		else
			my_color = 1; // 第一回合收到坐标是-1, -1，说明我是黑方
		fin1 >> x0 >> y0 >> x1 >> y1;
		if (x1 >= 0)
			Procstep(x0, y0, x1, y1, my_color); // 模拟己方落子
	}

	// 看看自己本回合输入
	fin1 >> x0 >> y0 >> x1 >> y1;
	if (x1 >= 0)
		Procstep(x0, y0, x1, y1, -my_color); // 模拟对方落子
	else
		my_color = 1; // 第一回合收到坐标是-1, -1，说明我是黑方

					  // 找出合法落子点
	int beginPos[1000][2] = { 0 }, possiblePos[1000][2] = { 0 }, insume[1000] = { 0 }, posCount = 0, choice, dir, maxi = -INT_MAX;

	for (y0 = 0; y0 < 7; y0++)
		for (x0 = 0; x0 < 7; x0++)
		{
			if (grid_info[x0][y0] != my_color)
				continue;
			for (dir = 0; dir < 24; dir++)
			{
				x1 = x0 + delta[dir][0];
				y1 = y0 + delta[dir][1];
				if (!inMap(x1, y1))
					continue;
				if (grid_info[x1][y1] != 0)
					continue;
				beginPos[posCount][0] = x0;
				beginPos[posCount][1] = y0;
				possiblePos[posCount][0] = x1;
				possiblePos[posCount][1] = y1;
				insume[posCount] = Trystep(x0, y0, x1, y1, my_color); // 计算子数目差值
				if (maxi < insume[posCount])
					maxi = insume[posCount]; // 记录差值最大的一个
				posCount++;
			}
		}

	// 做出决策

	extern int startX, startY, resultX, resultY;
	if (posCount > 0)
	{
		while (1)
		{
			srand(time(0));
			choice = rand() % posCount;
			if (maxi < 0)
				break; // 每种方法都救不回来了，随缘吧
			else
				if (insume[choice] >= -5) // 筛中能够使本方棋数不比对方少5个的走法
					break;
		}
		startX = beginPos[choice][0];
		startY = beginPos[choice][1];
		resultX = possiblePos[choice][0];
		resultY = possiblePos[choice][1];
	}
	else // 无路可走 
	{
		startX = -1;
		startY = -1;
		resultX = -1;
		resultY = -1;
	}

	// 决策结束，输出结果

	fin1.close();
	return 0;
}