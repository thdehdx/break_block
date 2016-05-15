#include <iostream>
#include <windows.h> //GetStdHandle(), Sleep(), SetConsolePosition() 함수 사용
#include <conio.h> //getch()와 kbhit() 함수 사용
#include <time.h>

#define OVER 3//바닥에 닿으면 게임오버를 시키기위해 정의
#define LINE 2
#define BLOCK 1
#define EMPTY 0

#define X_LEFT_BOARDER 5 //게임 보드 왼쪽 경계
#define X_RIGHT_BOARDER 45 //게임 보드 오른쪽 경계
#define Y_TOP_BOARDER 3 //게임 보드 위쪽 경계
#define Y_BOT_BOARDER 28 //게임 보드 아래쪽 경계
#define X_LEN 40 //게임보드 가로 길이
#define Y_LEN 25 //게임보드 세로 길이

#define ESC 27 //게임 종료
#define LEFT 75 //왼쪽 화살표 키 ASCII값
#define RIGHT 77 // 오른쪽 화살표 키 ASCII값

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
void indexCheck(void);//디버깅 전용

int blockX; //보드 안에 막대의 위치
int ex_ballX, ex_ballY;//보드 안의 이전 공 위치
int ballX, ballY;//보드 안의 공의 위치
int directX, directY;//공의 방향 결정
int decide;//초기 공의 방향 결정;

bool gameFlag; //게임 종료 플래그
int BOARD_ARR[Y_LEN][X_LEN] = { EMPTY, };

int block[5] = { 2,2,2,2,2 };

int num; //공 속도 조절
int gameScore;
bool gameStart;
int gameAction; //막대 운영 방식
int INDEX;//디버깅 전용

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
		indexCheck();//디버깅 전용
		num++;
		Sleep(10);
	}

	return 0;
}

void initGame() //게임 초기 상태 설정
{
	srand((unsigned int)time(NULL));

	INDEX = 0;//디버깅 전용
	blockX = X_LEN / 2 - 2;
	ballX = X_LEN / 2;
	ballY = Y_BOT_BOARDER - 5;
	decide = 0;
	int x, y, z = 0;
	int temp1 = 0, temp2 = 0;

	//게임보드 초기화
	for (x = 0; x<X_RIGHT_BOARDER - X_LEFT_BOARDER; x++) {
		BOARD_ARR[0][x] = LINE;
		BOARD_ARR[Y_LEN - 1][x] = OVER;
	}
	for (y = 0; y<Y_BOT_BOARDER - Y_TOP_BOARDER; y++) {
		BOARD_ARR[y][0] = LINE;
		BOARD_ARR[y][X_LEN - 1] = LINE;
	}
	//브레이크보드 초기화(부서질 블록)
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

	//1. 게임화면에 게임보드 창을 그리기
	for (x = 0, k = 0; x<X_LEN; x++, k++) {
		for (y = 0; y<Y_LEN; y++) {
			gotoXY(k + x + X_LEFT_BOARDER, y + Y_TOP_BOARDER);
			if (BOARD_ARR[y][x] == LINE || BOARD_ARR[y][x] == OVER) {
				printf("▣");
			}
			else if (BOARD_ARR[y][x] == BLOCK)
				printf("■");
		}
	}
	gotoXY(8, 2); printf("☆HCI's Break_Block GAME☆");

	//2. 게임보드 창에 공치는 막대 그리기
	for (x = 0; x<5; x++)
	{
		gotoXY(X_LEFT_BOARDER + (blockX + x) * 2, Y_BOT_BOARDER - 4);
		printf("■");
	}

	if (BOARD_ARR[Y_BOT_BOARDER - 4][blockX - 1] != LINE) {
		gotoXY(X_LEFT_BOARDER + blockX * 2 - 2, Y_BOT_BOARDER - 4);//막대 앞 지우기
		printf("  ");
	}
	if (BOARD_ARR[Y_BOT_BOARDER - 4][blockX + 5] != LINE) {
		gotoXY(X_LEFT_BOARDER + (blockX + 4) * 2 + 2, Y_BOT_BOARDER - 4);//막대 뒤 지우기
		printf("  ");
	}
	//3. 공 그리기
	gotoXY(X_LEFT_BOARDER + ballX * 2, ballY);
	printf("○");
	//gotoXY(87, INDEX++);//디버깅 전용
	//printf("%d,%d", ballX, ballY);//디버깅 전용
	//4. 공 지우기
	if (ballX != ex_ballX || ballY != ex_ballY)
	{
		gotoXY(X_LEFT_BOARDER + ex_ballX * 2, ex_ballY);
		printf("  ");
	}
}

int getGameAction()
{
	int keyValue, act = gameAction;
	if (kbhit() != 0) //키보드를 눌렀는지 확인함 
	{
		keyValue = getch();
		if (keyValue == 224) //특수 키 눌렀을 때 버퍼에 2Byte가 발생, 첫번째 값은 224값을 발생하고 두번째 값은 특수키에 따라 다름
		{
			keyValue = getch(); //특수 키를 확인하기 위해 2번의 getch()함수를 호출해야 함
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

void updateBall(void)//볼 업데이트 함수
{
	ex_ballX = ballX;
	ex_ballY = ballY;
	if (num == 2)//bar속도가 공속도의 2배
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

void updateblock(void)//부서지는 블럭 업데이트 함수
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

bool bar_collision(int blockX) //게임막대와 옆 벽의 충돌 검사
{
	int x;

	for (x = 0; x < 5; x++)
	{
		if (BOARD_ARR[Y_BOT_BOARDER - 4][blockX + x] == LINE)
			return true;
	}//충돌 체크

	return false;
}

bool ball_collision(int ballX, int ballY)//공 충돌 검사 함수
{
	int x;

	for (x = 0; x<5; x++)//막대와 공의 충돌 판별
	{
		if (X_LEFT_BOARDER + (blockX + x) * 2 == X_LEFT_BOARDER + ballX * 2 && ballY == Y_BOT_BOARDER - 4)
			return true;
	}

	if (BOARD_ARR[ballY - 3][ballX] == LINE)//공과 게임보드 충돌 판별
		return true;

	return false;
}

void corner_collision(void)//공과 모서리 충돌 함수
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

bool direct_ball(int ballX, int ballY)//공의 방향 결정함수(세로벽이면 true)
{
	if (BOARD_ARR[ballY][ballX + 1] == LINE || BOARD_ARR[ballY][ballX - 1] == LINE)
		return true;

	return false;
}

bool block_collision(int ballX, int ballY)//부서지는 블럭과 공의 충돌 검사 함수
{
	if (BOARD_ARR[ballY - 3][ballX] == BLOCK)
		return true;

	return false;
}

bool end_collision(int ballX, int ballY)//게임보드 밑바닥과 공의 충돌 검사 함수(닿으면 게임오버)
{
	if (ballY == Y_BOT_BOARDER - 1)
		return true;

	return false;
}
//콘솔 화면에서 커서를 특정 위치로 이동
void gotoXY(int x, int y)
{
	COORD Pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}

void indexCheck(void)//디버깅 전용
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