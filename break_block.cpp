#include <iostream>
#include <windows.h> //GetStdHandle(), Sleep(), SetConsolePosition() �Լ� ���
#include <conio.h> //getch()�� kbhit() �Լ� ���
#include <time.h>

#define OVER 3//�ٴڿ� ������ ���ӿ����� ��Ű������ ����
#define LINE 2
#define BLOCK 1
#define EMPTY 0

#define X_LEFT_BOARDER 5 //���� ���� ���� ���
#define X_RIGHT_BOARDER 45 //���� ���� ������ ���
#define Y_TOP_BOARDER 3 //���� ���� ���� ���
#define Y_BOT_BOARDER 28 //���� ���� �Ʒ��� ���
#define X_LEN 40 //���Ӻ��� ���� ����
#define Y_LEN 25 //���Ӻ��� ���� ����

#define ESC 27 //���� ����
#define LEFT 75 //���� ȭ��ǥ Ű ASCII��
#define RIGHT 77 // ������ ȭ��ǥ Ű ASCII��

using namespace std;

enum GameAction { MOVE_LEFT = 1, MOVE_RIGHT, GAME_QUIT };

void initGame();
void drawGame(int blockX);
int getGameAction();
void updateGame();
void updateBall(void);
void updateblock(void);
bool bar_collision(int blockX);
bool ball_collision(int ballX, int ballY);
void corner_collision(void);
bool direct_ball(int ballX, int ballY);
bool block_collision(int ballX, int ballY);
bool end_collision(int ballX, int ballY);
void gotoXY(int x, int y);
void indexCheck(void);//����� ����

int blockX; //���� �ȿ� ������ ��ġ
int ex_ballX, ex_ballY;//���� ���� ���� �� ��ġ
int ballX, ballY;//���� ���� ���� ��ġ
int directX, directY;//���� ���� ����
int decide;//�ʱ� ���� ���� ����;

bool gameFlag; //���� ���� �÷���
int BOARD_ARR[Y_LEN][X_LEN] = { EMPTY, };

int block[5] = { 2,2,2,2,2 };

int num; //�� �ӵ� ����
int gameScore;
bool gameStart;
int gameAction; //���� � ���
int INDEX;//����� ����

int main(void)
{
START:
	num = 0;
	initGame();

	while (gameAction != GAME_QUIT)
	{
		gameAction = getGameAction();
		updateGame();
		updateBall();
		updateblock();
		drawGame(blockX);
		if (end_collision(ballX, ballY))
		{
			system("cls");
			gameAction = 0;
			directX = 0, directY = 0;
			goto START;
		}
		indexCheck();//����� ����
		num++;
		Sleep(10);
	}

	return 0;
}

void initGame() //���� �ʱ� ���� ����
{
	srand((unsigned int)time(NULL));

	INDEX = 0;//����� ����
	blockX = X_LEN / 2 - 2;
	ballX = X_LEN / 2;
	ballY = Y_BOT_BOARDER - 5;
	decide = 0;
	int x, y, z = 0;
	int temp1 = 0, temp2 = 0;

	//���Ӻ��� �ʱ�ȭ
	for (x = 0; x<X_RIGHT_BOARDER - X_LEFT_BOARDER; x++) {
		BOARD_ARR[0][x] = LINE;
		BOARD_ARR[Y_LEN - 1][x] = OVER;
	}
	for (y = 0; y<Y_BOT_BOARDER - Y_TOP_BOARDER; y++) {
		BOARD_ARR[y][0] = LINE;
		BOARD_ARR[y][X_LEN - 1] = LINE;
	}
	//�극��ũ���� �ʱ�ȭ(�μ��� ���)
	for (x = 1; x < (X_LEN - 1); x++) {
		for (y = 1; y < (Y_LEN / 4); y++) {
			z = rand() % 2;
			temp2 = temp1;
			temp1 = z;
			while (1) {
				if (temp1 == 0 && temp2 == 0) {
					z = rand() % 2;
					temp2 = temp1;
					temp1 = z;
				}
				else
					break;
			}
			BOARD_ARR[y][x] = z;
		}
	}
}

void drawGame(int blockX)
{
	int x, y, k;

	//1. ����ȭ�鿡 ���Ӻ��� â�� �׸���
	for (x = 0, k = 0; x<X_LEN; x++, k++) {
		for (y = 0; y<Y_LEN; y++) {
			gotoXY(k + x + X_LEFT_BOARDER, y + Y_TOP_BOARDER);
			if (BOARD_ARR[y][x] == LINE || BOARD_ARR[y][x] == OVER) {
				printf("��");
			}
			else if (BOARD_ARR[y][x] == BLOCK)
				printf("��");
		}
	}
	gotoXY(8, 2); printf("��HCI's Break_Block GAME��");

	//2. ���Ӻ��� â�� ��ġ�� ���� �׸���
	for (x = 0; x<5; x++)
	{
		gotoXY(X_LEFT_BOARDER + (blockX + x) * 2, Y_BOT_BOARDER - 4);
		printf("��");
	}

	if (BOARD_ARR[Y_BOT_BOARDER - 4][blockX - 1] != LINE) {
		gotoXY(X_LEFT_BOARDER + blockX * 2 - 2, Y_BOT_BOARDER - 4);//���� �� �����
		printf("  ");
	}
	if (BOARD_ARR[Y_BOT_BOARDER - 4][blockX + 5] != LINE) {
		gotoXY(X_LEFT_BOARDER + (blockX + 4) * 2 + 2, Y_BOT_BOARDER - 4);//���� �� �����
		printf("  ");
	}
	//3. �� �׸���
	gotoXY(X_LEFT_BOARDER + ballX * 2, ballY);
	printf("��");
	//gotoXY(87, INDEX++);//����� ����
	//printf("%d,%d", ballX, ballY);//����� ����
	//4. �� �����
	if (ballX != ex_ballX || ballY != ex_ballY)
	{
		gotoXY(X_LEFT_BOARDER + ex_ballX * 2, ex_ballY);
		printf("  ");
	}
}

int getGameAction()
{
	int keyValue, act = gameAction;
	if (kbhit() != 0) //Ű���带 �������� Ȯ���� 
	{
		keyValue = getch();
		if (keyValue == 224) //Ư�� Ű ������ �� ���ۿ� 2Byte�� �߻�, ù��° ���� 224���� �߻��ϰ� �ι�° ���� Ư��Ű�� ���� �ٸ�
		{
			keyValue = getch(); //Ư�� Ű�� Ȯ���ϱ� ���� 2���� getch()�Լ��� ȣ���ؾ� ��
		}
		switch (keyValue)
		{
		case ESC:
			act = GAME_QUIT;
			break;
		case LEFT:
			act = MOVE_LEFT;
			break;
		case RIGHT:
			act = MOVE_RIGHT;
			break;
		default:
			break;
		}
	}

	return act;
}

void updateGame()
{
	switch (gameAction)
	{
	case MOVE_LEFT:
		blockX--;
		if (decide == 0) {
			decide++;
			directX = -1, directY = -1;
		}
		if (bar_collision(blockX)) blockX++;
		break;

	case MOVE_RIGHT:
		blockX++;
		if (decide == 0) {
			decide++;
			directX = 1, directY = -1;
		}
		if (bar_collision(blockX)) blockX--;
		break;

	default:
		break;
	}
}

void updateBall(void)//�� ������Ʈ �Լ�
{
	ex_ballX = ballX;
	ex_ballY = ballY;
	if (num == 2)//bar�ӵ��� ���ӵ��� 2��
	{
		ballX += directX, ballY += directY;
		num = 0;
	}

	Sleep(10);

	if (ball_collision(ballX, ballY)) {
		corner_collision();
		if (direct_ball(ballX, ballY)) {
			ballX -= directX, ballY -= directY;
			directX *= -1;
		}
		else {
			ballX -= directX, ballY -= directY;
			directY *= -1;
			ballX += directX;
		}
	}

	else if (block_collision(ballX, ballY)) {
		directY *= -1;
	}
}

void updateblock(void)//�μ����� �� ������Ʈ �Լ�
{
	int x, k;

	if (block_collision(ballX, ballY)) {

		BOARD_ARR[ballY - 3][ballX] = EMPTY;
		Sleep(10);
		gotoXY(ballX, ballY - 3);
		if (BOARD_ARR[ballY - 3][ballX] == EMPTY)
			printf("  ");

	}
}

bool bar_collision(int blockX) //���Ӹ���� �� ���� �浹 �˻�
{
	int x;

	for (x = 0; x < 5; x++)
	{
		if (BOARD_ARR[Y_BOT_BOARDER - 4][blockX + x] == LINE)
			return true;
	}//�浹 üũ

	return false;
}

bool ball_collision(int ballX, int ballY)//�� �浹 �˻� �Լ�
{
	int x;

	for (x = 0; x<5; x++)//����� ���� �浹 �Ǻ�
	{
		if (X_LEFT_BOARDER + (blockX + x) * 2 == X_LEFT_BOARDER + ballX * 2 && ballY == Y_BOT_BOARDER - 4)
			return true;
	}

	if (BOARD_ARR[ballY - 3][ballX] == LINE)//���� ���Ӻ��� �浹 �Ǻ�
		return true;

	return false;
}

void corner_collision(void)//���� �𼭸� �浹 �Լ�
{
	int x, y, k;
	for (x = 0; x < X_LEN; x++) {
		for (y = 0; y < (Y_LEN - 3); y++) {
			if (x == 0) {
				if (y == 0) {
					if ((X_LEFT_BOARDER + ballX * 2) == (x * 2 + X_LEFT_BOARDER) && ballY == (y + Y_TOP_BOARDER)) {
						ballX -= directX, ballY -= directY;
						directX *= -1;
						directY *= -1;
					}
				}
				else if (y == (Y_LEN - 4)) {
					if ((X_LEFT_BOARDER + ballX * 2) == (x * 2 + X_LEFT_BOARDER) && ballY == (y + Y_TOP_BOARDER)) {
						ballX -= directX, ballY -= directY;
						directX *= -1;
						directY *= -1;
					}
				}
			}
			else if (x == (X_LEN - 1)) {
				if (y == 0) {
					if ((X_LEFT_BOARDER + ballX * 2) == (x * 2 + X_LEFT_BOARDER) && ballY == (y + Y_TOP_BOARDER)) {
						ballX -= directX, ballY -= directY;
						directX *= -1;
						directY *= -1;
					}
				}
				else if (y == (Y_LEN - 4)) {
					if ((X_LEFT_BOARDER + ballX * 2) == (x * 2 + X_LEFT_BOARDER) && ballY == (y + Y_TOP_BOARDER)) {
						ballX -= directX, ballY -= directY;
						directX *= -1;
						directY *= -1;
					}
				}
			}
		}
	}
}

bool direct_ball(int ballX, int ballY)//���� ���� �����Լ�(���κ��̸� true)
{
	if (BOARD_ARR[ballY][ballX + 1] == LINE || BOARD_ARR[ballY][ballX - 1] == LINE)
		return true;

	return false;
}

bool block_collision(int ballX, int ballY)//�μ����� ���� ���� �浹 �˻� �Լ�
{
	if (BOARD_ARR[ballY - 3][ballX] == BLOCK)
		return true;

	return false;
}

bool end_collision(int ballX, int ballY)//���Ӻ��� �عٴڰ� ���� �浹 �˻� �Լ�(������ ���ӿ���)
{
	if (ballY == Y_BOT_BOARDER - 1)
		return true;

	return false;
}
//�ܼ� ȭ�鿡�� Ŀ���� Ư�� ��ġ�� �̵�
void gotoXY(int x, int y)
{
	COORD Pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}

void indexCheck(void)//����� ����
{
	int i = 0;
	if (INDEX > 28) {
		for (i = 0; i <= 29; i++) {
			gotoXY(87, i);
			printf("            ");
		}
		INDEX = 0;
	}
}