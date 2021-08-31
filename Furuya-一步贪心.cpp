// Ataxx-Furuya һ��̰��

#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <fstream>
using namespace std;

int mycolor; // ����ִ����ɫ��1Ϊ�ڣ�-1Ϊ�ף�����״̬��ͬ��
int gridinfo[7][7] = { 0 }; // �С��У���¼����״̬
int blackcount = 2, whitecount = 2;
static int delta[24][2] = { { 1,1 },{ 0,1 },{ -1,1 },{ -1,0 },
{ -1,-1 },{ 0,-1 },{ 1,-1 },{ 1,0 },
{ 2,0 },{ 2,1 },{ 2,2 },{ 1,2 },
{ 0,2 },{ -1,2 },{ -2,2 },{ -2,1 },
{ -2,0 },{ -2,-1 },{ -2,-2 },{ -1,-2 },
{ 0,-2 },{ 1,-2 },{ 2,-2 },{ 2,-1 } }; // 24�ֿ����߷� 
ifstream fin2;

// �ж��Ƿ��ڵ�ͼ��
inline bool inMap(int x, int y)
{
	if (x < 0 || x > 6 || y < 0 || y > 6)
		return false;
	return true;
}

// ��Direction����Ķ����꣬�������Ƿ�Խ��
inline bool MoveStep(int &x, int &y, int Direction)
{
	x = x + delta[Direction][0];
	y = y + delta[Direction][1];
	return inMap(x, y);
}

// �����괦���ӣ�����Ƿ�Ϸ���ģ������
bool ProcStep(int x0, int y0, int x1, int y1, int color)
{
	if (color == 0)
		return false;
	if (x1 == -1) // ��·���ߣ������˻غ�
		return true;
	if (!inMap(x0, y0) || !inMap(x1, y1)) // �����߽�
		return false;
	if (gridinfo[x0][y0] != color)
		return false;
	int dx, dy, x, y, currCount = 0, dir;
	dx = abs((x0 - x1)), dy = abs((y0 - y1));
	if ((dx == 0 && dy == 0) || dx > 2 || dy > 2) // ��֤�����ƶ���ԭ��λ�ã������ƶ�ʼ����5��5������
		return false;
	if (gridinfo[x1][y1] != 0) // ��֤�ƶ�����λ��Ϊ��
		return false;
	if (dx == 2 || dy == 2) // ����ߵ���5��5����Χ�����Ǹ���ճ��
		gridinfo[x0][y0] = 0;
	else
	{
		if (color == 1)
			blackcount++;
		else
			whitecount++;
	}

	gridinfo[x1][y1] = color;
	for (dir = 0; dir < 8; dir++) // Ӱ���ڽ�8��λ��
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

// ģ������ 
int TryStep(int x0, int y0, int x1, int y1, int color)
{
	int dx, dy, x, y, currCount = 0, dir, my, opps;
	if (color == 1) // ��¼�ҺͶ��ֵ������� 
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
	if (dx == 2 || dy == 2) // ����ߵ���5��5����Χ�����Ǹ���ճ��
		my = my;
	else
		my++; // ����ճ�� 
	for (dir = 0; dir < 8; dir++) // Ӱ���ڽ�8��λ��
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
	return (my - opps); // ���ز�ֵ 
}
int main_Normal()
{
	istream::sync_with_stdio(false);

	int x0, y0, x1, y1;

	// ��ʼ������
	memset(gridinfo, 0, sizeof(gridinfo));
	blackcount = 2;
	whitecount = 2;
	gridinfo[0][0] = gridinfo[6][6] = 1;  //|��|��|
	gridinfo[6][0] = gridinfo[0][6] = -1; //|��|��|

	// �����Լ��յ���������Լ���������������ָ�״̬
	int turnID;
	mycolor = -1;
	fin2.open("step2");
	fin2 >> turnID;
	turnID = turnID / 2 + 1;
	for (int i = 0; i < turnID - 1; i++)
	{
		// ������Щ��������𽥻ָ�״̬����ǰ�غ�
		fin2 >> x0 >> y0 >> x1 >> y1;
		if (x1 >= 0)
			ProcStep(x0, y0, x1, y1, -mycolor); // ģ��Է�����
    	else
    		mycolor = 1; // ��һ�غ��յ�������-1, -1��˵�����Ǻڷ�
		fin2 >> x0 >> y0 >> x1 >> y1;
		if (x1 >= 0)
			ProcStep(x0, y0, x1, y1, mycolor); // ģ�⼺������
	}

	// �����Լ����غ�����
	fin2 >> x0 >> y0 >> x1 >> y1;
	if (x1 >= 0)
		ProcStep(x0, y0, x1, y1, -mycolor); // ģ��Է�����
	else
		mycolor = 1; // ��һ�غ��յ�������-1, -1��˵�����Ǻڷ�

	// �ҳ��Ϸ����ӵ�
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
				insume[posCount] = TryStep(x0, y0, x1, y1, mycolor); // ��������Ŀ��ֵ 
				posCount++;
			}
		}

	// ��������

	extern int startX, startY, resultX, resultY;
	int maxi = -100, maxCount;
	if (posCount > 0)
	{
		for (int i = 0; i < posCount; i++)
		{
			if (maxi < insume[i]) // ѡ������Ŀ��ֵ�����߷� 
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
	else // ��·���� 
	{
		startX = -1;
		startY = -1;
		resultX = -1;
		resultY = -1;
	}

	// ���߽�����������
	fin2.close();
	return 0;
}
