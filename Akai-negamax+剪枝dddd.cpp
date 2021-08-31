// ATAXX-Akai Alpha-Beta ��֦ 

#include <iostream>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cmath>
#include <fstream>

using namespace std;

int myColor; // ����ִ����ɫ��1Ϊ�ڣ�-1Ϊ�ף�����״̬��ͬ��
int gridInfo[7][7] = { 0 }; // �С��У���¼����״̬
int Round[10][8] = { 0 };
int visit[7][7], space, dd, startTime, edge;
int blackCount = 2, whiteCount = 2, sumCount;
int startX, startY, resultX, resultY;
static int delta[24][2] = { { 1,1 },{ 0,1 },{ -1,1 },{ -1,0 },
{ -1,-1 },{ 0,-1 },{ 1,-1 },{ 1,0 },
{ 2,0 },{ 2,1 },{ 2,2 },{ 1,2 },
{ 0,2 },{ -1,2 },{ -2,2 },{ -2,1 },
{ -2,0 },{ -2,-1 },{ -2,-2 },{ -1,-2 },
{ 0,-2 },{ 1,-2 },{ 2,-2 },{ 2,-1 } }; // 24�ֿ����߷� 
static int valueFIRST[7][7] = { { 4, 3, 3, 2, 3, 3, 4 },{ 3, 2, 2, 1, 2, 2, 3 },
{ 3, 2, 2, 1, 2, 2, 3 },{ 2, 1, 1, 1, 1, 1, 2 },{ 3, 2, 2, 1, 2, 2, 3 },
{ 3, 2, 2, 1, 2, 2, 3 },{ 4, 3, 3, 2, 3, 3, 4 } };
static int valueSECOND[7][7] = { { 1, 2, 2, 2, 2, 2, 1 },{ 2, 3, 4, 4, 4, 3, 2 },
{ 2, 4, 3, 3, 3, 4, 2 },{ 2, 4, 3, 3, 3, 4, 2 },{ 2, 4, 3, 3, 3, 4, 2 },
{ 2, 3, 4, 4, 4, 3, 2 },{ 1, 2, 2, 2, 2, 2, 1 } }; // ÿ�����ӵ����� 
ifstream fin3;

												   // �ж��Ƿ��ڵ�ͼ��
inline bool inMap(int x, int y)
{
	if (x < 0 || x > 6 || y < 0 || y > 6)
		return false;
	return true;
}

int rec(int color) // ���������� 
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

// ��֭��ֵ���� 
int Evaluate()
{
	int modality = 0, sp = 0, dir;
	sumCount = blackCount + whiteCount;
	for (int i = 0; i < 7; i++)
		for (int j = 0; j < 7; j++)
		{
			if (sumCount <= 15) // �ֽ׶� 
				if (myColor == -1)
					modality += gridInfo[i][j] * valueFIRST[i][j]; // ռ�еĸ���
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

// �����괦���ӣ�����Ƿ�Ϸ���ģ������
bool ProcStep(int x0, int y0, int x1, int y1, int color, int depth)
{
	if (color == 0)
		return false;
	if (x1 == -1) // ��·���ߣ������˻غ�
		return true;
	if (!inMap(x0, y0) || !inMap(x1, y1)) // �����߽�
		return false;
	if (gridInfo[x0][y0] != color)
		return false;
	int dx, dy, x, y, currCount = 0, dir;
	dx = abs((x0 - x1));
	dy = abs((y0 - y1));
	if ((dx == 0 && dy == 0) || dx > 2 || dy > 2) // ��֤�����ƶ���ԭ��λ�ã������ƶ�ʼ����5��5������
		return false;
	if (gridInfo[x1][y1] != 0) // ��֤�ƶ�����λ��Ϊ��
		return false;
	if (dx == 2 || dy == 2) // ����ߵ���5��5����Χ�����Ǹ���ճ��
		gridInfo[x0][y0] = 0;
	else
	{
		if (color == 1)
			blackCount++;
		else
			whiteCount++;
	}

	gridInfo[x1][y1] = color;
	for (dir = 0; dir < 8; dir++) // Ӱ���ڽ�8��λ��
	{
		Round[depth][dir] = 0; // ��ʼ�� 
		x = x1 + delta[dir][0];
		y = y1 + delta[dir][1];
		if (!inMap(x, y))
			continue;
		if (gridInfo[x][y] == -color)
		{
			currCount++;
			Round[depth][dir] = 1; // �������Ǳ�Ӱ���˵� 
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

// �����괦���ӣ�����Ƿ�Ϸ���ģ������
bool ProcStep1(int x0, int y0, int x1, int y1, int color)
{
	if (color == 0)
		return false;
	if (x1 == -1) // ��·���ߣ������˻غ�
		return true;
	if (!inMap(x0, y0) || !inMap(x1, y1)) // �����߽�
		return false;
	if (gridInfo[x0][y0] != color)
		return false;
	int dx, dy, x, y, currCount = 0, dir;
	dx = abs((x0 - x1));
	dy = abs((y0 - y1));
	if ((dx == 0 && dy == 0) || dx > 2 || dy > 2) // ��֤�����ƶ���ԭ��λ�ã������ƶ�ʼ����5��5������
		return false;
	if (gridInfo[x1][y1] != 0) // ��֤�ƶ�����λ��Ϊ��
		return false;
	if (dx == 2 || dy == 2) // ����ߵ���5��5����Χ�����Ǹ���ճ��
		gridInfo[x0][y0] = 0;
	else
	{
		if (color == 1)
			blackCount++;
		else
			whiteCount++;
	}

	gridInfo[x1][y1] = color;
	for (dir = 0; dir < 8; dir++) // Ӱ���ڽ�8��λ��
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

// ������� 
bool DeleteStep(int x0, int y0, int x1, int y1, int color, int depth)
{
	gridInfo[x1][y1] = 0;
	int dx, dy, x, y, currCount = 0, dir;
	dx = abs((x0 - x1));
	dy = abs((y0 - y1));
	if (dx == 2 || dy == 2) // ����ߵ���5*5����Χ�����Ǹ���ճ��
		gridInfo[x0][y0] = color;
	else
	{
		if (color == 1)
			blackCount--;
		else
			whiteCount--;
	}

	// �ָ�Ӱ���ڽ�8��λ�� 
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

// AlphaBeta��֦ 
int AlphaBeta(int depth, int alpha, int beta, int color)
{
	int x0, x1, y0, y1, dir, val, posCount = 0;
	if (depth == 0)
		if (color == myColor)
			return -Evaluate(); // �ѵ������������������������ 
		else
			return Evaluate();

						   // ��֮ǰ�Ĳ��� 
	for (y0 = 0; y0 < 7; y0++)
		for (x0 = 0; x0 < 7; x0++) // Ѱ�ҳ���� 
		{
			if (gridInfo[x0][y0] != color)
				continue; // ���ǵ�ǰ���ڳ����ߵ��壬���� 
			for (dir = 0; dir < 24; dir++) // 24�����ӷ� 
			{
				x1 = x0 + delta[dir][0];
				y1 = y0 + delta[dir][1];
				if (!inMap(x1, y1))
					continue; // ûԽ�� 
				if (gridInfo[x1][y1] != 0)
					continue; // �Ѿ���������
				posCount++;
				ProcStep(x0, y0, x1, y1, color, depth); // ���� 
				val = -AlphaBeta(depth - 1, -beta, -alpha, -color); // �ݹ��ӽڵ㣬��������ɫ�ı䣬ab�Ի� 
				DeleteStep(x0, y0, x1, y1, color, depth); // ������� 
				if (val >= beta)
				{
					return beta; // ��beta�󣬻� 
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
					alpha = val; // ��alpha�ã���Ŀǰ��õ�
				}
			}
		}
	if (posCount)
		return alpha;
	else
		if (color == myColor)
			return -Evaluate(); // �ѵ������������������������ 
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

	// ��ʼ������
	memset(gridInfo, 0, sizeof(gridInfo));
	blackCount = 2;
	whiteCount = 2;
	memset(Round, 0, sizeof(Round));
	gridInfo[0][0] = gridInfo[6][6] = 1;  //|��|��|
	gridInfo[6][0] = gridInfo[0][6] = -1; //|��|��|

										  // �����Լ��յ���������Լ���������������ָ�״̬
	int turnID;
	myColor = -1;
	fin3.open("step3");
	fin3 >> turnID;
	turnID = turnID / 2 + 1;
	for (int i = 0; i < turnID - 1; i++)
	{
		// ������Щ��������𽥻ָ�״̬����ǰ�غ�
		fin3 >> x0 >> y0 >> x1 >> y1;
		if (x1 >= 0)
			ProcStep1(x0, y0, x1, y1, -myColor); // ģ��Է�����
		else
			myColor = 1; // ��һ�غ��յ�������-1, -1��˵�����Ǻڷ�
		fin3 >> x0 >> y0 >> x1 >> y1;
		if (x1 >= 0)
			ProcStep1(x0, y0, x1, y1, myColor); // ģ�⼺������
	}

	// �Լ����غ�����
	fin3 >> x0 >> y0 >> x1 >> y1;
	if (x1 >= 0)
		ProcStep1(x0, y0, x1, y1, -myColor); // ģ��Է�����
	else
		myColor = 1; // ��һ�غ��յ�������-1, -1��˵�����Ǻڷ�

					 // ��������
	int val;
	startTime = clock();
	for (dd = 1; ; dd++)
	{
		val = AlphaBeta(dd, -INT8_MAX, INT8_MAX, myColor);
		if (Timeout())
			break;
	}
	
	// ���߽�����������

	fin3.close();
	return 0;
}
