// Ataxx-Hattori ���+ɸѡ����

#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <fstream>
using namespace std;

int my_color; // ����ִ����ɫ��1Ϊ�ڣ�-1Ϊ�ף�����״̬��ͬ��
int grid_info[7][7] = { 0 }; // �С��У���¼����״̬
int black_count = 2, white_count = 2;
static int delta[24][2] = { { 1,1 },{ 0,1 },{ -1,1 },{ -1,0 },
{ -1,-1 },{ 0,-1 },{ 1,-1 },{ 1,0 },
{ 2,0 },{ 2,1 },{ 2,2 },{ 1,2 },
{ 0,2 },{ -1,2 },{ -2,2 },{ -2,1 },
{ -2,0 },{ -2,-1 },{ -2,-2 },{ -1,-2 },
{ 0,-2 },{ 1,-2 },{ 2,-2 },{ 2,-1 } }; // 24�ֿ����߷� 
ifstream fin1;

// �ж��Ƿ��ڵ�ͼ��
inline bool inMap(int x, int y)
{
	if (x < 0 || x > 6 || y < 0 || y > 6)
		return false;
	return true;
}

// �����괦���ӣ�����Ƿ�Ϸ���ģ������
bool Procstep(int x0, int y0, int x1, int y1, int color)
{
	if (color == 0)
		return false;
	if (x1 == -1) // ��·���ߣ������˻غ�
		return true;
	if (!inMap(x0, y0) || !inMap(x1, y1)) // �����߽�
		return false;
	if (grid_info[x0][y0] != color)
		return false;
	int dx, dy, x, y, currCount = 0, dir;
	dx = abs((x0 - x1)), dy = abs((y0 - y1));
	if ((dx == 0 && dy == 0) || dx > 2 || dy > 2) // ��֤�����ƶ���ԭ��λ�ã������ƶ�ʼ����5��5������
		return false;
	if (grid_info[x1][y1] != 0) // ��֤�ƶ�����λ��Ϊ��
		return false;
	if (dx == 2 || dy == 2) // ����ߵ���5��5����Χ�����Ǹ���ճ��
		grid_info[x0][y0] = 0;
	else
	{
		if (color == 1)
			black_count++;
		else
			white_count++;
	}

	grid_info[x1][y1] = color;
	for (dir = 0; dir < 8; dir++) // Ӱ���ڽ�8��λ��
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

// ģ������ 
int Trystep(int x0, int y0, int x1, int y1, int color)
{
	int dx, dy, x, y, currCount = 0, dir, my, opps;
	if (color == 1) // ��¼�ҺͶ��ֵ������� 
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
		if (grid_info[x][y] == -color)
			currCount++;
	}
	if (currCount != 0)
	{
		my += currCount;
		opps -= currCount;
	}
	return (my - opps); // ���ز�ֵ 
}
int main_Easy()
{
	istream::sync_with_stdio(false);

	int x0, y0, x1, y1;

	// ��ʼ������
	memset(grid_info, 0, sizeof(grid_info));
	grid_info[0][0] = grid_info[6][6] = 1;  //|��|��|
	grid_info[6][0] = grid_info[0][6] = -1; //|��|��|

											// �����Լ��յ���������Լ���������������ָ�״̬
	int turnID;
	my_color = -1;
	fin1.open("step1");
	fin1 >> turnID;
	turnID = turnID / 2 + 1;
	for (int i = 0; i < turnID - 1; i++)
	{
		// ������Щ��������𽥻ָ�״̬����ǰ�غ�
		fin1 >> x0 >> y0 >> x1 >> y1;
		if (x1 >= 0)
			Procstep(x0, y0, x1, y1, -my_color); // ģ��Է�����
		else
			my_color = 1; // ��һ�غ��յ�������-1, -1��˵�����Ǻڷ�
		fin1 >> x0 >> y0 >> x1 >> y1;
		if (x1 >= 0)
			Procstep(x0, y0, x1, y1, my_color); // ģ�⼺������
	}

	// �����Լ����غ�����
	fin1 >> x0 >> y0 >> x1 >> y1;
	if (x1 >= 0)
		Procstep(x0, y0, x1, y1, -my_color); // ģ��Է�����
	else
		my_color = 1; // ��һ�غ��յ�������-1, -1��˵�����Ǻڷ�

					  // �ҳ��Ϸ����ӵ�
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
				insume[posCount] = Trystep(x0, y0, x1, y1, my_color); // ��������Ŀ��ֵ
				if (maxi < insume[posCount])
					maxi = insume[posCount]; // ��¼��ֵ����һ��
				posCount++;
			}
		}

	// ��������

	extern int startX, startY, resultX, resultY;
	if (posCount > 0)
	{
		while (1)
		{
			srand(time(0));
			choice = rand() % posCount;
			if (maxi < 0)
				break; // ÿ�ַ������Ȳ������ˣ���Ե��
			else
				if (insume[choice] >= -5) // ɸ���ܹ�ʹ�����������ȶԷ���5�����߷�
					break;
		}
		startX = beginPos[choice][0];
		startY = beginPos[choice][1];
		resultX = possiblePos[choice][0];
		resultY = possiblePos[choice][1];
	}
	else // ��·���� 
	{
		startX = -1;
		startY = -1;
		resultX = -1;
		resultY = -1;
	}

	// ���߽�����������

	fin1.close();
	return 0;
}