#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <conio.h>
#include <mmsystem.h>
#include "gameInfo.h"
#include "designscreen.h"

#pragma comment(lib,"winmm.lib")
#pragma warning(disable:4996)
#define Q 113
#define W 119
#define E 101
#define R 114
#define I 105
#define LEFT 75
#define RIGHT 77
#define UP 72
#define DOWN 80
#define GBOARD_WIDTH 60
#define GBOARD_HEIGHT 30
#define GBOARD_ORIGIN_X 2
#define GBOARD_ORIGIN_Y 1

int ingItem = 0;
int itemS_t, itemE_t;
int npc_speed = 0;
unsigned int old_t, now_t;
int stageInfo[31][45];
int TIMELIMIT = 300;
int click = 0; // 버튼을 누르는 횟수
int q, w, e, r;
int movement, score;
int n_combo, m_combo, p_combo;
int step = 1;
int p_count;
char blockModel[6][6];
char blockClear[6][6] = { 0 };
char blockSuccess[6][6] = { 0 };
int block_id;
int stage, combo, max_combo, life, speed;
int gBoardPuzzle = GBOARD_ORIGIN_Y + GBOARD_HEIGHT + 3;
int gBoardInformation = GBOARD_ORIGIN_X + GBOARD_WIDTH * 2 - 30;
int gameBoardInfo[100][100];  //0빈자리 1벽 2플레이어 3엔피시 4아이템 5세이브존
int curPosX = GBOARD_ORIGIN_X + 12;
int curPosY = GBOARD_ORIGIN_Y + 6;
char puzzleHide[9][150];
int hideitem = 0;
HANDLE hc = GetStdHandle(STD_OUTPUT_HANDLE);
int randomtime = 3;
COORD NPC_1, NPC_2, NPC_3, NPC_4, NPC_5, NPC_6, NPC_7, NPC_8, NPC_9, NPC_10, NPC_11, NPC_12, NPC_13, NPC_14, NPC_15, NPC_16;
int npc1_movement = 0, npc2_movement = 0, npc3_movement = 2, npc4_movement = 0, npc5_movement = 0, npc6_movement = 2, npc7_movement = 1, npc8_movement = 3, npc9_movement = 0;
int npc10_movement = 3, npc11_movement = 2, npc12_movement = 1, npc13_movement = 3, npc14_movement = 0, npc15_movement = 1, npc16_movement = 0;
int saveItem[2] = { 0,0 };
unsigned int npcS_t, npcE_t;
unsigned int totalS_t, totalE_t;
int random1, random2, random3, random4;
int consume_t;

void RemoveCursor();
void SetCurrentCursorPos(int x, int y);
COORD GetCurrentCursorPos(void);
void RandomPuzzleFactory();
void RemovePuzzle(char blockInfo[6][6]);
void DrawRandPuzzle(char blockInfo[6][6]);
void SelectKeyPuzzleE();
void SelectKeyPuzzleW();
void SelectKeyPuzzleQ();
void ProcessKeyInput();
void drawStageTutorial();
void drawFirstStage();
void drawSecondStage();
void redrawStage();
void DrawFirstMonitor();
void DrawStageClear();
void DrawGameOver();
int ShiftRight();
int ShiftLeft();
int ShiftUp();
int ShiftDown();
int TestSuccessPuzzle(char blockTest[6][6]);
void drawPuzzleZone();
void TimeLimit();
void drawInformation();
void drawMessageBox();
void drawGameBoard();
void ShowPlayer();
void DeletePlayer();
void DetectItemZone();
int DetectCollisionNPC(COORD NPC);
int DetectCollision(int posX, int posY);

void ItemHidePuzzleZone();
void ItemNpcSpeedUp();
void ItemNpcSpeedDown();

int CheckCombo(int random_check);
void SelectItem();
bool timeDetect = false;
void drawInformation2();
void AssignTime(int random_check);
void ItembreakWall();
void UseItem();
void CheckItemTime();
void ResetMessageBox(int now);

void IsGameOver(int consume_t);
void ControlNPC();
void MoveNpc_1();
void MoveNpc_2();
void MoveNpc_3();
void MoveNpc_4();
void MoveNpc_5();
void MoveNpc_6();
void MoveNpc_7();
void MoveNpc_8();
void MoveNpc_9();
void MoveNpc_10();
void MoveNpc_11();
void MoveNpc_12();
void MoveNpc_13();
void MoveNpc_14();
void MoveNpc_15();
void MoveNpc_16();
void DeleteNPC(COORD NPC);
void ShowNPC(COORD NPC);
void DrawItemInfo();
void DrawClearGrade(int time);

int main() {
	DrawFirstMonitor();
	stage = 1;
	life = 3;
	RemoveCursor();
	drawGameBoard();
	drawMessageBox();
	drawInformation();
	drawInformation2();
	ShowPlayer();
	if(stage ==1)
		PlaySound(TEXT("엘리.wav"), NULL, SND_ASYNC | SND_NOSTOP);
	if (stage == 2)
		PlaySound(TEXT("커닝.wav"), NULL, SND_ASYNC | SND_NOSTOP);

	while (1) {
		npcS_t = clock();
		totalS_t = clock();
		totalE_t = clock();

		//플레이어 움직이는 while문
		while ((totalE_t - totalS_t) / 1000 <= randomtime) {
			//이동제한시간 표시
			SetCurrentCursorPos(gBoardInformation + 22, GBOARD_ORIGIN_Y + 2);
			int t_time = (randomtime)-((totalE_t - totalS_t) / 1000);
			SetConsoleTextAttribute(hc, 7);
			printf("( %d초 )", t_time);

			RemovePuzzle(blockClear);

			if (ingItem != 0) {  //아이템 사용시
				CheckItemTime();
			}

			ProcessKeyInput();
			DetectItemZone();
			if (DetectCollisionNPC(NPC_1) == 1) {
				DeletePlayer();
				if (stage == 1) {
					curPosX = GBOARD_ORIGIN_X + 12;
					curPosY = GBOARD_ORIGIN_Y + 6;
				}
				if (stage == 2) {
					curPosX = GBOARD_ORIGIN_X + 2;
					curPosY = GBOARD_ORIGIN_Y + 1;
				}
				ShowPlayer();
				life--;
				drawInformation2();
				IsGameOver(1);
			}

			totalE_t = clock();
			TimeLimit();

			npcE_t = clock();      //여기에만 있으면 퍼즐 맞출때 npc가 안움직여서 밑에 하나 더 추가했습니다
			if ((npcE_t - npcS_t) / (600 + npc_speed) == 1) {
				ControlNPC();
				npcS_t = clock();  //이거 숨길거 찾아보자
			}

		}
		timeDetect = true;

		int random_check = rand() % 4 + 2;

		while (CheckCombo(random_check) == 1) {   // <<이동 제한 시간 끝났을 때>>

			RandomPuzzleFactory();      // 퍼즐그리기

			while (TestSuccessPuzzle(blockSuccess) != 1) {

				if (hideitem == 1) {   // hidepuzzlezone아이템을 먹으면 블록만들어질때 아이템이 사용되어야되서 hideitem변수를 만들어줘서 다음 블록 퍼즐을 풀때 처음에 퍼즐존 가려주기함수 실행
					ItemHidePuzzleZone();
					hideitem = 0;      //한번 실행하면 다음블록부턴 바로 보일수있게
				}

				if (ingItem != 0) {  //아이템 사용시
					CheckItemTime();
				}

				npcE_t = clock();      //여기다가 추가해줌

				if ((npcE_t - npcS_t) / (600 + npc_speed) == 1) {
					ControlNPC();
					npcS_t = clock();  //이거 숨길거 찾아보자
				}
				//다 만들어야 탈출

				if (click == 2) {         //두번 클릭해도 못맞추면
					RemovePuzzle(blockClear);
					DrawRandPuzzle(blockModel);
					click = 0;
					p_combo = 0;
					n_combo = 0;
					CheckCombo(random_check);
					SetConsoleTextAttribute(hc, 15);
					drawInformation2();
					p_count++;

					for (int i = 0; i < 6; i++) {
						for (int j = 0; j < 6; j++) {
							blockSuccess[i][j] = 0;
						}
					}

					continue;
				}
				TimeLimit();
				ProcessKeyInput();
			}

			//퍼즐 한번 맞추면 click 초기화, n_combo++, p_combo++
			click = 0;      n_combo++;      p_combo++;

			if (n_combo >= m_combo)   m_combo = n_combo;

			SetConsoleTextAttribute(hc, 15);
			drawInformation2();

			for (int i = 0; i < 6; i++) {
				for (int j = 0; j < 6; j++) {
					blockSuccess[i][j] = 0;
				}
			}
			p_count++;      //몇 번만에 푼건지

			TimeLimit();
		}

		AssignTime(random_check);      //콤보 탈출 후 맞춘 횟수에 따라 시간 배정

		p_count = 0;   p_combo = 0;   timeDetect = false;
	}
	//DrawClearGrade();
	SetCurrentCursorPos(1, 45);
	getchar();
	return 0;
}


void RandomPuzzleFactory()
{
	srand((unsigned)time(NULL));
	int sect[9];

	for (int i = 0; i < 9; i++)
	{
		sect[i] = rand() % 2;
	}
	blockModel[0][0] = sect[0];
	blockModel[0][1] = sect[0];
	blockModel[1][0] = sect[0];
	blockModel[1][1] = sect[0];
	blockModel[0][2] = sect[1];
	blockModel[0][3] = sect[1];
	blockModel[1][2] = sect[1];
	blockModel[1][3] = sect[1];
	blockModel[0][4] = sect[2];
	blockModel[0][5] = sect[2];
	blockModel[1][4] = sect[2];
	blockModel[1][5] = sect[2];
	blockModel[2][0] = sect[3];
	blockModel[2][1] = sect[3];
	blockModel[3][0] = sect[3];
	blockModel[3][1] = sect[3];
	blockModel[2][2] = sect[4];
	blockModel[2][3] = sect[4];
	blockModel[3][2] = sect[4];
	blockModel[3][3] = sect[4];
	blockModel[2][4] = sect[5];
	blockModel[2][5] = sect[5];
	blockModel[3][4] = sect[5];
	blockModel[3][5] = sect[5];
	blockModel[4][0] = sect[6];
	blockModel[4][1] = sect[6];
	blockModel[5][0] = sect[6];
	blockModel[5][1] = sect[6];
	blockModel[4][2] = sect[7];
	blockModel[4][3] = sect[7];
	blockModel[5][2] = sect[7];
	blockModel[5][3] = sect[7];
	blockModel[4][4] = sect[8];
	blockModel[4][5] = sect[8];
	blockModel[5][4] = sect[8];
	blockModel[5][5] = sect[8];
	//////////랜덤배정
	RemovePuzzle(blockClear);
	DrawRandPuzzle(blockModel);
}
void RemoveCursor(void) {
	CONSOLE_CURSOR_INFO curInfo;
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	curInfo.bVisible = 0;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}
void SetCurrentCursorPos(int x, int y) {
	COORD pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}
COORD GetCurrentCursorPos(void) {
	COORD curPoint;
	CONSOLE_SCREEN_BUFFER_INFO curInfo;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	curPoint.X = curInfo.dwCursorPosition.X;
	curPoint.Y = curInfo.dwCursorPosition.Y;
	return curPoint;
}
void RemovePuzzle(char blockInfo[6][6])
{
	int x, y;
	for (int y = 0; y < 6; y++) {
		for (int x = 0; x < 6; x++) {
			SetCurrentCursorPos(GBOARD_ORIGIN_X + x * 2 + 2, GBOARD_ORIGIN_Y + GBOARD_HEIGHT + y + 6);
			if (blockInfo[y][x] == 0)
			{
				printf(" ");

			}
		}
	}
	for (y = 0; y < 6; y++) {
		for (x = 0; x < 6; x++) {
			SetCurrentCursorPos(GBOARD_ORIGIN_X + x * 2 + 30 + 2, GBOARD_ORIGIN_Y + GBOARD_HEIGHT + y + 6);
			if (blockInfo[y][x] == 0)
				printf(" ");
		}
	}
	for (y = 0; y < 6; y++) {
		for (x = 0; x < 6; x++) {
			SetCurrentCursorPos(GBOARD_ORIGIN_X + x * 2 + 50 + 2, GBOARD_ORIGIN_Y + GBOARD_HEIGHT + y + 6);
			if (blockInfo[y][x] == 0)
				printf(" ");
		}
	}
	for (y = 0; y < 6; y++) {
		for (x = 0; x < 6; x++) {
			SetCurrentCursorPos(GBOARD_ORIGIN_X + x * 2 + 70 + 2, GBOARD_ORIGIN_Y + GBOARD_HEIGHT + y + 6);
			if (y == 0 || y == 1) {
				if (blockInfo[x][y] == 0)
				{
					printf(" ");
				}
			}
			else if (y == 2 || y == 3)
			{
				if (blockInfo[x][y] == 0)
					printf(" ");
			}
			else
				if (blockInfo[x][y] == 0)
					printf(" ");
		}
	}
	for (y = 0; y < 6; y++) {
		for (x = 0; x < 6; x++) {
			SetCurrentCursorPos(GBOARD_ORIGIN_X + x * 2 + 90 + 2, GBOARD_ORIGIN_Y + GBOARD_HEIGHT + y + 6);
			if (y == 0 || y == 1) {
				if (blockInfo[x][y] == 0)
				{
					printf(" ");
				}
			}
			else if (y == 2 || y == 3)
			{
				if (blockInfo[x][y] == 0)
					printf(" ");
			}
			else
				if (blockInfo[x][y] == 0)
					printf(" ");
		}
	}
}
void DrawRandPuzzle(char blockInfo[6][6])
{
	int x, y;
	COORD curPos = GetCurrentCursorPos();
	if (stage == 1) {
		srand((unsigned)time(NULL));
		int i = rand() % 6 + 1;
		int color = (rand() % 15) + 1;
		switch (i) {
		case 1:
			random1 = 0;
			random2 = 1;
			random3 = 2;
			break;
		case 2:
			random1 = 0;
			random2 = 2;
			random3 = 1;
			break;
		case 3:
			random1 = 1;
			random2 = 2;
			random3 = 0;
			break;
		case 4:
			random1 = 1;
			random2 = 0;
			random3 = 2;
			break;
		case 5:
			random1 = 2;
			random2 = 0;
			random3 = 1;
			break;
		case 6:
			random1 = 2;
			random2 = 1;
			random3 = 0;
			break;
		}

		for (y = 0; y < 6; y++) {
			for (x = 0; x < 6; x++) {
				SetCurrentCursorPos(GBOARD_ORIGIN_X + 32 + random1 * 20 + x * 2, GBOARD_ORIGIN_Y + GBOARD_HEIGHT + y + 6);
				if (blockInfo[y][x] == 1)
				{
					SetConsoleTextAttribute(hc, color);
					printf("■");
				}
			}
		}
		for (y = 0; y < 6; y++) {
			for (x = 0; x < 6; x++) {
				SetCurrentCursorPos(GBOARD_ORIGIN_X + 32 + random2 * 20 + x * 2, GBOARD_ORIGIN_Y + GBOARD_HEIGHT + y + 6);
				if (blockInfo[y][x] == 0)
				{
					SetConsoleTextAttribute(hc, ((color + 3) % 15) + 1);
					printf("■");
				}
			}
		}
		for (y = 0; y < 6; y++) {
			for (x = 0; x < 6; x++) {
				SetCurrentCursorPos(GBOARD_ORIGIN_X + 32 + random3 * 20 + x * 2, GBOARD_ORIGIN_Y + GBOARD_HEIGHT + y + 6);
				if (y == 0 || y == 1) {
					if (blockInfo[x][y] == 0)
					{
						SetConsoleTextAttribute(hc, ((color + 5) % 15 + 1));
						printf("■");
					}
				}
				else if (y == 2 || y == 3)
				{
					if (blockInfo[x][y] == 1) {
						SetConsoleTextAttribute(hc, ((color + 5) % 15 + 1));
						printf("■");
					}
				}
				else
					if (blockInfo[x][y] == 1) {
						SetConsoleTextAttribute(hc, ((color + 5) % 15 + 1));
						printf("■");
					}
			}
		}
	}
	else if (stage == 2) {

		srand((unsigned)time(NULL));
		int i = rand() % 24;
		int color = (rand() % 15) + 1;
		switch (i) {
		case 1:
			random1 = 0;
			random2 = 1;
			random3 = 2;
			random4 = 3;
			break;
		case 2:
			random1 = 0;
			random2 = 1;
			random3 = 3;
			random4 = 2;
			break;
		case 3:
			random1 = 0;
			random2 = 2;
			random3 = 1;
			random4 = 3;
			break;
		case 4:
			random1 = 0;
			random2 = 2;
			random3 = 3;
			random4 = 1;
			break;
		case 5:
			random1 = 0;
			random2 = 3;
			random3 = 1;
			random4 = 2;
			break;
		case 6:
			random1 = 0;
			random2 = 3;
			random3 = 2;
			random4 = 1;
			break;
		case 7:
			random1 = 1;
			random2 = 2;
			random3 = 3;
			random4 = 0;
			break;
		case 8:
			random1 = 1;
			random2 = 2;
			random3 = 0;
			random4 = 3;
			break;
		case 9:
			random1 = 1;
			random2 = 3;
			random3 = 2;
			random4 = 0;
			break;
		case 10:
			random1 = 1;
			random2 = 3;
			random3 = 0;
			random4 = 2;
			break;
		case 11:
			random1 = 1;
			random2 = 0;
			random3 = 2;
			random4 = 3;
			break;
		case 12:
			random1 = 1;
			random2 = 0;
			random3 = 3;
			random4 = 2;
			break;
		case 13:
			random1 = 2;
			random2 = 0;
			random3 = 1;
			random4 = 3;
			break;
		case 14:
			random1 = 2;
			random2 = 0;
			random3 = 3;
			random4 = 1;
			break;
		case 15:
			random1 = 2;
			random2 = 1;
			random3 = 0;
			random4 = 3;
			break;
		case 16:
			random1 = 2;
			random2 = 1;
			random3 = 3;
			random4 = 0;
			break;
		case 17:
			random1 = 2;
			random2 = 3;
			random3 = 0;
			random4 = 1;
			break;
		case 18:
			random1 = 2;
			random2 = 3;
			random3 = 1;
			random4 = 0;
			break;
		case 19:
			random1 = 3;
			random2 = 1;
			random3 = 2;
			random4 = 0;
			break;
		case 20:
			random1 = 3;
			random2 = 1;
			random3 = 0;
			random4 = 2;
			break;
		case 21:
			random1 = 3;
			random2 = 2;
			random3 = 1;
			random4 = 0;
			break;
		case 22:
			random1 = 3;
			random2 = 2;
			random3 = 0;
			random4 = 1;
			break;
		case 23:
			random1 = 3;
			random2 = 0;
			random3 = 1;
			random4 = 2;
			break;
		case 24:
			random1 = 3;
			random2 = 0;
			random3 = 1;
			random4 = 2;
			break;
		}


		for (y = 0; y < 6; y++) {
			for (x = 0; x < 6; x++) {
				SetCurrentCursorPos(GBOARD_ORIGIN_X + 32 + random1 * 20 + x * 2, GBOARD_ORIGIN_Y + GBOARD_HEIGHT + y + 6);
				if (blockInfo[y][x] == 1)
				{
					SetConsoleTextAttribute(hc, color);
					printf("■");
				}
			}
		}
		for (y = 0; y < 6; y++) {
			for (x = 0; x < 6; x++) {
				SetCurrentCursorPos(GBOARD_ORIGIN_X + 32 + random2 * 20 + x * 2, GBOARD_ORIGIN_Y + GBOARD_HEIGHT + y + 6);
				if (blockInfo[y][x] == 0)
				{
					SetConsoleTextAttribute(hc, ((color + 5) % 15) + 1);
					printf("■");
				}
			}
		}
		for (y = 0; y < 6; y++) {
			for (x = 0; x < 6; x++) {
				SetCurrentCursorPos(GBOARD_ORIGIN_X + 32 + random3 * 20 + x * 2, GBOARD_ORIGIN_Y + GBOARD_HEIGHT + y + 6);
				if (y == 0 || y == 1) {
					if (blockInfo[x][y] == 0)
					{
						SetConsoleTextAttribute(hc, ((color + 4) % 15) + 1);
						printf("■");
					}
				}
				else if (y == 2 || y == 3)
				{
					if (blockInfo[x][y] == 1) {
						SetConsoleTextAttribute(hc, ((color + 4) % 15) + 1);
						printf("■");
					}
				}
				else
					if (blockInfo[x][y] == 1) {
						SetConsoleTextAttribute(hc, ((color + 4) % 15) + 1);
						printf("■");
					}
			}
		}
		for (y = 0; y < 6; y++) {
			for (x = 0; x < 6; x++) {
				SetCurrentCursorPos(GBOARD_ORIGIN_X + 32 + random4 * 20 + x * 2, GBOARD_ORIGIN_Y + GBOARD_HEIGHT + y + 6);
				if (y == 0 || y == 1) {
					if (blockInfo[x][y] == 0)
					{
						SetConsoleTextAttribute(hc, ((color + 3) % 15) + 1);
						printf("■");
					}
				}
				else if (y == 2 || y == 3)
				{
					if (blockInfo[x][y] == 0) {
						SetConsoleTextAttribute(hc, ((color + 3) % 15) + 1);
						printf("■");
					}
				}
				else
					if (blockInfo[x][y] == 1) {
						SetConsoleTextAttribute(hc, ((color + 3) % 15) + 1);
						printf("■");
					}
			}
		}
	}

}
void SelectKeyPuzzleE()
{
	SetConsoleTextAttribute(hc, 5);
	click++;
	if (random1 == 2)
		for (int y = 0; y < 6; y++) {
			for (int x = 0; x < 6; x++) {
				SetCurrentCursorPos(GBOARD_ORIGIN_X + x * 2 + 2, GBOARD_ORIGIN_Y + GBOARD_HEIGHT + y + 6);
				if (blockModel[y][x] == 1)
				{
					printf("■");
					blockSuccess[y][x] = 1;
				}
			}
		}
	if (random2 == 2)
		for (int y = 0; y < 6; y++) {
			for (int x = 0; x < 6; x++) {
				SetCurrentCursorPos(GBOARD_ORIGIN_X + x * 2 + 2, GBOARD_ORIGIN_Y + GBOARD_HEIGHT + y + 6);
				if (blockModel[y][x] == 0)
				{
					printf("■");
					blockSuccess[y][x] = 1;
				}
			}
		}
	if (random3 == 2)
		for (int y = 0; y < 6; y++) {
			for (int x = 0; x < 6; x++) {
				SetCurrentCursorPos(GBOARD_ORIGIN_X + x * 2 + 2, GBOARD_ORIGIN_Y + GBOARD_HEIGHT + y + 6);
				if (y == 0 || y == 1) {
					if (blockModel[x][y] == 0)
					{
						printf("■");
					}
				}
				else if (y == 2 || y == 3)
				{
					if (blockModel[x][y] == 1)
						printf("■");
				}
				else
					if (blockModel[x][y] == 1)
						printf("■");
			}
		}
	if (random4 == 2)
		for (int y = 0; y < 6; y++) {
			for (int x = 0; x < 6; x++) {
				SetCurrentCursorPos(GBOARD_ORIGIN_X + x * 2 + 2, GBOARD_ORIGIN_Y + GBOARD_HEIGHT + y + 6);
				if (y == 0 || y == 1) {
					if (blockModel[x][y] == 0)
					{
						SetConsoleTextAttribute(hc, 8);
						printf("■");
					}
				}
				else if (y == 2 || y == 3)
				{
					if (blockModel[x][y] == 0) {
						SetConsoleTextAttribute(hc, 8);
						printf("■");
					}
				}
				else
					if (blockModel[x][y] == 1) {
						SetConsoleTextAttribute(hc, 8);
						printf("■");
					}
			}
		}
}
void SelectKeyPuzzleW()
{
	SetConsoleTextAttribute(hc, 5);
	click++;
	if (random1 == 1)
		for (int y = 0; y < 6; y++) {
			for (int x = 0; x < 6; x++) {
				SetCurrentCursorPos(GBOARD_ORIGIN_X + x * 2 + 2, GBOARD_ORIGIN_Y + GBOARD_HEIGHT + y + 6);
				if (blockModel[y][x] == 1)
				{
					printf("■");
					blockSuccess[y][x] = 1;
				}
			}
		}
	if (random2 == 1)
		for (int y = 0; y < 6; y++) {
			for (int x = 0; x < 6; x++) {
				SetCurrentCursorPos(GBOARD_ORIGIN_X + x * 2 + 2, GBOARD_ORIGIN_Y + GBOARD_HEIGHT + y + 6);
				if (blockModel[y][x] == 0)
				{
					printf("■");
					blockSuccess[y][x] = 1;
				}
			}
		}
	if (random3 == 1)
		for (int y = 0; y < 6; y++) {
			for (int x = 0; x < 6; x++) {
				SetCurrentCursorPos(GBOARD_ORIGIN_X + x * 2 + 2, GBOARD_ORIGIN_Y + GBOARD_HEIGHT + y + 6);
				if (y == 0 || y == 1) {
					if (blockModel[x][y] == 0)
					{
						printf("■");
					}
				}
				else if (y == 2 || y == 3)
				{
					if (blockModel[x][y] == 1)
						printf("■");
				}
				else
					if (blockModel[x][y] == 1)
						printf("■");
			}
		}
	if (random4 == 1)
		for (int y = 0; y < 6; y++) {
			for (int x = 0; x < 6; x++) {
				SetCurrentCursorPos(GBOARD_ORIGIN_X + x * 2 + 2, GBOARD_ORIGIN_Y + GBOARD_HEIGHT + y + 6);
				if (y == 0 || y == 1) {
					if (blockModel[x][y] == 0)
					{
						SetConsoleTextAttribute(hc, 8);
						printf("■");
					}
				}
				else if (y == 2 || y == 3)
				{
					if (blockModel[x][y] == 0) {
						SetConsoleTextAttribute(hc, 8);
						printf("■");
					}
				}
				else
					if (blockModel[x][y] == 1) {
						SetConsoleTextAttribute(hc, 8);
						printf("■");
					}
			}
		}
}
void SelectKeyPuzzleQ()
{

	SetConsoleTextAttribute(hc, 5);
	click++;
	if (random1 == 0)
		for (int y = 0; y < 6; y++) {
			for (int x = 0; x < 6; x++) {
				SetCurrentCursorPos(GBOARD_ORIGIN_X + x * 2 + 2, GBOARD_ORIGIN_Y + GBOARD_HEIGHT + y + 6);
				if (blockModel[y][x] == 1)
				{
					printf("■");
					blockSuccess[y][x] = 1;
				}
			}
		}
	else if (random2 == 0)
		for (int y = 0; y < 6; y++) {
			for (int x = 0; x < 6; x++) {
				SetCurrentCursorPos(GBOARD_ORIGIN_X + x * 2 + 2, GBOARD_ORIGIN_Y + GBOARD_HEIGHT + y + 6);
				if (blockModel[y][x] == 0)
				{
					printf("■");
					blockSuccess[y][x] = 1;
				}
			}
		}
	else if (random3 == 0)
		for (int y = 0; y < 6; y++) {
			for (int x = 0; x < 6; x++) {
				SetCurrentCursorPos(GBOARD_ORIGIN_X + x * 2 + 2, GBOARD_ORIGIN_Y + GBOARD_HEIGHT + y + 6);
				if (y == 0 || y == 1) {
					if (blockModel[x][y] == 0)
					{
						printf("■");
					}
				}
				else if (y == 2 || y == 3)
				{
					if (blockModel[x][y] == 1)
						printf("■");
				}
				else
					if (blockModel[x][y] == 1)
						printf("■");
			}
		}
	else if (random4 == 0)
		for (int y = 0; y < 6; y++) {
			for (int x = 0; x < 6; x++) {
				SetCurrentCursorPos(GBOARD_ORIGIN_X + x * 2 + 2, GBOARD_ORIGIN_Y + GBOARD_HEIGHT + y + 6);
				if (y == 0 || y == 1) {
					if (blockModel[x][y] == 0)
					{
						SetConsoleTextAttribute(hc, 8);
						printf("■");
					}
				}
				else if (y == 2 || y == 3)
				{
					if (blockModel[x][y] == 0) {
						SetConsoleTextAttribute(hc, 8);
						printf("■");
					}
				}
				else
					if (blockModel[x][y] == 1) {
						SetConsoleTextAttribute(hc, 8);
						printf("■");
					}
			}
		}
}
void SelectKeyPuzzleR()
{

	SetConsoleTextAttribute(hc, 5);
	click++;
	if (random1 == 3)
		for (int y = 0; y < 6; y++) {
			for (int x = 0; x < 6; x++) {
				SetCurrentCursorPos(GBOARD_ORIGIN_X + x * 2 + 2, GBOARD_ORIGIN_Y + GBOARD_HEIGHT + y + 6);
				if (blockModel[y][x] == 1)
				{
					printf("■");
					blockSuccess[y][x] = 1;
				}
			}
		}
	else if (random2 == 3)
		for (int y = 0; y < 6; y++) {
			for (int x = 0; x < 6; x++) {
				SetCurrentCursorPos(GBOARD_ORIGIN_X + x * 2 + 2, GBOARD_ORIGIN_Y + GBOARD_HEIGHT + y + 6);
				if (blockModel[y][x] == 0)
				{
					printf("■");
					blockSuccess[y][x] = 1;
				}
			}
		}
	else if (random3 == 3)
		for (int y = 0; y < 6; y++) {
			for (int x = 0; x < 6; x++) {
				SetCurrentCursorPos(GBOARD_ORIGIN_X + x * 2 + 2, GBOARD_ORIGIN_Y + GBOARD_HEIGHT + y + 6);
				if (y == 0 || y == 1) {
					if (blockModel[x][y] == 0)
					{
						printf("■");
					}
				}
				else if (y == 2 || y == 3)
				{
					if (blockModel[x][y] == 1)
						printf("■");
				}
				else
					if (blockModel[x][y] == 1)
						printf("■");
			}
		}
	else if (random4 == 3)
		for (int y = 0; y < 6; y++) {
			for (int x = 0; x < 6; x++) {
				SetCurrentCursorPos(GBOARD_ORIGIN_X + x * 2 + 2, GBOARD_ORIGIN_Y + GBOARD_HEIGHT + y + 6);
				if (y == 0 || y == 1) {
					if (blockModel[x][y] == 0)
					{
						SetConsoleTextAttribute(hc, 8);
						printf("■");
					}
				}
				else if (y == 2 || y == 3)
				{
					if (blockModel[x][y] == 0) {
						SetConsoleTextAttribute(hc, 8);
						printf("■");
					}
				}
				else
					if (blockModel[x][y] == 1) {
						SetConsoleTextAttribute(hc, 8);
						printf("■");
					}
			}
		}
}
void ProcessKeyInput()
{
	for (int j = 0; j < 20; j++) {
		if (_kbhit() != 0) {
			int key;
			key = _getch();
			switch (key) {
			case Q:
				if (timeDetect)
					SelectKeyPuzzleQ();

				break;
			case W:
				if (timeDetect)
					SelectKeyPuzzleW();
				break;
			case E:
				if (timeDetect)
					SelectKeyPuzzleE();
				break;
			case I:
				UseItem();
				break;
			case R:
				if (timeDetect)
					SelectKeyPuzzleR();
				break;
			case RIGHT:
				if (!timeDetect)
					ShiftRight();
				break;

			case LEFT:
				if (!timeDetect)
					ShiftLeft();
				break;

			case UP:
				if (!timeDetect)
					ShiftUp();
				break;

			case DOWN:
				if (!timeDetect)
					ShiftDown();
				break;
			}
		}
	}
}
int ShiftRight()
{
	if (DetectCollision(curPosX + 2, curPosY) == 0) {

		DeletePlayer();
		curPosX += 2;
		SetCurrentCursorPos(curPosX, curPosY);
		ShowPlayer();
		return 1;
	}
	else if (DetectCollision(curPosX + 2, curPosY) == 2) {
		DeletePlayer();
		curPosX += 2;
		SetCurrentCursorPos(curPosX, curPosY);
		ShowPlayer();
		stage++;
		if (stage == 3)
			DrawClearGrade(consume_t);
		else
			redrawStage();
	}

	return 0;
}
int ShiftLeft()
{
	if (DetectCollision(curPosX - 2, curPosY) != 1) {
		DeletePlayer();
		curPosX -= 2;
		SetCurrentCursorPos(curPosX, curPosY);
		ShowPlayer();
		return 1;
	}
	//else if (DetectCollisionNPC(curPosX, curPosY - 1) == 1) {
	//   DeletePlayer();
	//   curPosX = GBOARD_ORIGIN_X + 2;
	//   curPosY = GBOARD_ORIGIN_Y + 1;
	//   ShowPlayer();
	//}

	else
		return 0;
}
int ShiftUp()
{
	if (DetectCollision(curPosX, curPosY - 1) != 1) {
		DeletePlayer();
		curPosY -= 1;
		SetCurrentCursorPos(curPosX, curPosY);
		ShowPlayer();
		return 1;
	}
	//else if (DetectCollisionNPC(curPosX, curPosY - 1) == 1) {
	//   DeletePlayer();
	//   curPosX = GBOARD_ORIGIN_X + 2;
	//   curPosY = GBOARD_ORIGIN_Y + 1;
	//   ShowPlayer();
	//}
	else {
		return 0;
	}
}
int ShiftDown()
{
	if (DetectCollision(curPosX, curPosY + 1) == 0) {
		DeletePlayer();
		curPosY += 1;
		SetCurrentCursorPos(curPosX, curPosY);
		ShowPlayer();
		return 1;
	}
	else if (DetectCollision(curPosX, curPosY + 1) == 2) {
		DeletePlayer();
		curPosY++;
		SetCurrentCursorPos(curPosX, curPosY);
		ShowPlayer();
		stage++;
		if (stage == 3)
			DrawClearGrade(consume_t);
		else
			redrawStage();
	}
	return 0;
}
void drawStageTutorial() {
	int x, y;
	for (x = 2; x <= 15 * 2; x += 2) {
		SetCurrentCursorPos(GBOARD_ORIGIN_X + x, GBOARD_ORIGIN_Y + 3);
		printf("■");
	}
	x -= 2;
	for (y = 0; y <= 20; y++) {
		SetCurrentCursorPos((GBOARD_ORIGIN_X + x), GBOARD_ORIGIN_Y + 3 + y);
		printf("■");
	}
	y--;
	for (x = 2; x <= 15 * 2; x += 2) {
		SetCurrentCursorPos(GBOARD_ORIGIN_X + 30 + x, GBOARD_ORIGIN_Y + 3 + y);
		printf("■");
	}

	for (x = 2; x <= 11 * 2; x += 2) {
		SetCurrentCursorPos(GBOARD_ORIGIN_X + x, GBOARD_ORIGIN_Y + 7);
		printf("■");
	}
	x -= 2;
	for (y = 0; y <= 20; y++) {
		SetCurrentCursorPos((GBOARD_ORIGIN_X + x), GBOARD_ORIGIN_Y + 7 + y);
		printf("■");
	}
	y--;
	for (x = 2; x <= 19 * 2; x += 2) {
		SetCurrentCursorPos(GBOARD_ORIGIN_X + 22 + x, GBOARD_ORIGIN_Y + 7 + y);
		printf("■");
	}

	for (x = 1; x <= 14; x++) {  //gameboard
		gameBoardInfo[4][x] = 1;
	}
	for (x = 1; x <= 12; x++) {
		gameBoardInfo[6][x] = 1;
	}
	for (y = 5; y < 25; y++) {
		gameBoardInfo[y][14] = 1;
	}
	for (y = 7; y < 27; y++) {
		gameBoardInfo[y][12] = 1;
	}
	for (x = 15; x <= 30; x++) {
		gameBoardInfo[24][x] = 1;
	}
	for (x = 13; x <= 30; x++) {
		gameBoardInfo[26][x] = 1;
	}

	gameBoardInfo[13][13] = 3;
	gameBoardInfo[25][22] = 3;
	//화면 예시
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 2, GBOARD_ORIGIN_Y + 5);
	printf("▷");
	//SetCurrentCursorPos(GBOARD_ORIGIN_X + 12, GBOARD_ORIGIN_Y + 5);
	//printf("◎");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 26, GBOARD_ORIGIN_Y + 13);
	printf("◆");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 44, GBOARD_ORIGIN_Y + 25);
	printf("◆");
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 60, GBOARD_ORIGIN_Y + 25);
	printf("¶");

}
void drawFirstStage() {

	if (stage == 1) {
		for (int i = 0; i < 31; i++) {
			for (int j = 0; j < 45; j++) {
				stageInfo[i][j] = firstGameBoardInfo[i][j];
			}
		}
	}

	for (int i = 0; i < 31; i++) {
		for (int j = 0; j < 45; j++) {
			SetConsoleTextAttribute(hc, 7);
			if (firstGameBoardInfo[i][j] == 1) {
				SetCurrentCursorPos(GBOARD_ORIGIN_X + j * 2, GBOARD_ORIGIN_Y + i);
				printf("■");
			}
			else if (firstGameBoardInfo[i][j] == 3) {
				SetCurrentCursorPos(GBOARD_ORIGIN_X + j * 2, GBOARD_ORIGIN_Y + i);
				printf("▶");
			}
			else if (firstGameBoardInfo[i][j] == 4) {
				SetCurrentCursorPos(GBOARD_ORIGIN_X + j * 2, GBOARD_ORIGIN_Y + i);
				printf("¶");
			}
			else if (firstGameBoardInfo[i][j] == 9) {
				SetCurrentCursorPos(GBOARD_ORIGIN_X + j * 2, GBOARD_ORIGIN_Y + i);
				SetConsoleTextAttribute(hc, 7);
				printf("▒");
			}
			else if (firstGameBoardInfo[i][j] == 5) {
				SetCurrentCursorPos(GBOARD_ORIGIN_X + j * 2, GBOARD_ORIGIN_Y + i);
				SetConsoleTextAttribute(hc, 14);   //빨간색으로
				printf("?");
			}
			else if (firstGameBoardInfo[i][j] == 11) {         //NPC그리기
				SetConsoleTextAttribute(hc, 12);   //빨간색으로
				SetCurrentCursorPos(GBOARD_ORIGIN_X + j * 2, GBOARD_ORIGIN_Y + i);
				NPC_1 = GetCurrentCursorPos();
				printf("◆");
				SetConsoleTextAttribute(hc, 7);
			}
			else if (firstGameBoardInfo[i][j] == 12) {         //NPC그리기
				SetConsoleTextAttribute(hc, 12);   //빨간색으로
				SetCurrentCursorPos(GBOARD_ORIGIN_X + j * 2, GBOARD_ORIGIN_Y + i);
				NPC_2 = GetCurrentCursorPos();
				printf("◆");
				SetConsoleTextAttribute(hc, 7);

			}
			else if (firstGameBoardInfo[i][j] == 13) {         //NPC그리기
				SetConsoleTextAttribute(hc, 13);   //보라색으로
				SetCurrentCursorPos(GBOARD_ORIGIN_X + j * 2, GBOARD_ORIGIN_Y + i);
				NPC_3 = GetCurrentCursorPos();
				printf("◆");
				SetConsoleTextAttribute(hc, 7);

			}
			else if (firstGameBoardInfo[i][j] == 14) {         //NPC그리기
				SetConsoleTextAttribute(hc, 13);   //보라색으로
				SetCurrentCursorPos(GBOARD_ORIGIN_X + j * 2, GBOARD_ORIGIN_Y + i);
				NPC_4 = GetCurrentCursorPos();
				printf("◆");
				SetConsoleTextAttribute(hc, 7);

			}
			else if (firstGameBoardInfo[i][j] == 15) {         //NPC그리기
				SetConsoleTextAttribute(hc, 13);   //보라색으로
				SetCurrentCursorPos(GBOARD_ORIGIN_X + j * 2, GBOARD_ORIGIN_Y + i);
				NPC_5 = GetCurrentCursorPos();
				printf("◆");
				SetConsoleTextAttribute(hc, 7);

			}
			else if (firstGameBoardInfo[i][j] == 16) {         //NPC그리기
				SetConsoleTextAttribute(hc, 13);   //보라색으로
				SetCurrentCursorPos(GBOARD_ORIGIN_X + j * 2, GBOARD_ORIGIN_Y + i);
				NPC_6 = GetCurrentCursorPos();
				printf("◆");
				SetConsoleTextAttribute(hc, 7);

			}
		}
	}
}
void drawSecondStage() {

	if (stage == 2) {
		for (int i = 0; i < 31; i++) {
			for (int j = 0; j < 45; j++) {
				stageInfo[i][j] = secondGameBoardInfo[i][j];
			}
		}
	}

	for (int i = 0; i < 31; i++) {
		for (int j = 0; j < 45; j++) {
			SetConsoleTextAttribute(hc, 7);
			if (secondGameBoardInfo[i][j] == 1) {
				SetCurrentCursorPos(GBOARD_ORIGIN_X + j * 2, GBOARD_ORIGIN_Y + i);
				SetConsoleTextAttribute(hc, 7);   //빨간색으로
				printf("■");
			}
			else if (secondGameBoardInfo[i][j] == 3) {
				SetCurrentCursorPos(GBOARD_ORIGIN_X + j * 2, GBOARD_ORIGIN_Y + i);
				printf("▶");
			}
			else if (secondGameBoardInfo[i][j] == 4) {
				SetCurrentCursorPos(GBOARD_ORIGIN_X + j * 2, GBOARD_ORIGIN_Y + i);
				printf("¶");
			}
			else if (secondGameBoardInfo[i][j] == 9) {
				SetCurrentCursorPos(GBOARD_ORIGIN_X + j * 2, GBOARD_ORIGIN_Y + i);
				SetConsoleTextAttribute(hc, 7);
				printf("▒");
			}
			else if (secondGameBoardInfo[i][j] == 5) {
				SetCurrentCursorPos(GBOARD_ORIGIN_X + j * 2, GBOARD_ORIGIN_Y + i);
				SetConsoleTextAttribute(hc, 14);   //빨간색으로
				printf("?");
			}
			else if (secondGameBoardInfo[i][j] == 17) {         //NPC그리기
				SetConsoleTextAttribute(hc, 13);   //보라색으로
				SetCurrentCursorPos(GBOARD_ORIGIN_X + j * 2, GBOARD_ORIGIN_Y + i);
				NPC_7 = GetCurrentCursorPos();
				printf("◆");
				SetConsoleTextAttribute(hc, 7);

			}
			else if (secondGameBoardInfo[i][j] == 18) {         //NPC그리기
				SetConsoleTextAttribute(hc, 13);   //보라색으로
				SetCurrentCursorPos(GBOARD_ORIGIN_X + j * 2, GBOARD_ORIGIN_Y + i);
				NPC_8 = GetCurrentCursorPos();
				printf("◆");
				SetConsoleTextAttribute(hc, 7);

			}
			else if (secondGameBoardInfo[i][j] == 19) {         //NPC그리기
				SetConsoleTextAttribute(hc, 13);   //보라색으로
				SetCurrentCursorPos(GBOARD_ORIGIN_X + j * 2, GBOARD_ORIGIN_Y + i);
				NPC_9 = GetCurrentCursorPos();
				printf("◆");
				SetConsoleTextAttribute(hc, 7);

			}
			else if (secondGameBoardInfo[i][j] == 20) {         //NPC그리기
				SetConsoleTextAttribute(hc, 13);   //보라색으로
				SetCurrentCursorPos(GBOARD_ORIGIN_X + j * 2, GBOARD_ORIGIN_Y + i);
				NPC_10 = GetCurrentCursorPos();
				printf("◆");
				SetConsoleTextAttribute(hc, 7);

			}
			else if (secondGameBoardInfo[i][j] == 21) {         //NPC그리기
				SetConsoleTextAttribute(hc, 13);   //보라색으로
				SetCurrentCursorPos(GBOARD_ORIGIN_X + j * 2, GBOARD_ORIGIN_Y + i);
				NPC_11 = GetCurrentCursorPos();
				printf("◆");
				SetConsoleTextAttribute(hc, 7);

			}
			else if (secondGameBoardInfo[i][j] == 22) {         //NPC그리기
				SetConsoleTextAttribute(hc, 13);   //보라색으로
				SetCurrentCursorPos(GBOARD_ORIGIN_X + j * 2, GBOARD_ORIGIN_Y + i);
				NPC_12 = GetCurrentCursorPos();
				printf("◆");
				SetConsoleTextAttribute(hc, 7);

			}
			else if (secondGameBoardInfo[i][j] == 23) {         //NPC그리기
				SetConsoleTextAttribute(hc, 13);   //보라색으로
				SetCurrentCursorPos(GBOARD_ORIGIN_X + j * 2, GBOARD_ORIGIN_Y + i);
				NPC_13 = GetCurrentCursorPos();
				printf("◆");
				SetConsoleTextAttribute(hc, 7);

			}
			else if (secondGameBoardInfo[i][j] == 24) {         //NPC그리기
				SetConsoleTextAttribute(hc, 13);   //보라색으로
				SetCurrentCursorPos(GBOARD_ORIGIN_X + j * 2, GBOARD_ORIGIN_Y + i);
				NPC_14 = GetCurrentCursorPos();
				printf("◆");
				SetConsoleTextAttribute(hc, 7);

			}
			else if (secondGameBoardInfo[i][j] == 25) {         //NPC그리기
				SetConsoleTextAttribute(hc, 13);   //보라색으로
				SetCurrentCursorPos(GBOARD_ORIGIN_X + j * 2, GBOARD_ORIGIN_Y + i);
				NPC_15 = GetCurrentCursorPos();
				printf("◆");
				SetConsoleTextAttribute(hc, 7);
			}
			else if (secondGameBoardInfo[i][j] == 26) {         //NPC그리기
				SetConsoleTextAttribute(hc, 12);   //보라색으로
				SetCurrentCursorPos(GBOARD_ORIGIN_X + j * 2, GBOARD_ORIGIN_Y + i);
				NPC_16 = GetCurrentCursorPos();
				printf("◆");
				SetConsoleTextAttribute(hc, 7);

			}
		}
	}
}
void redrawStage()
{
	for (int i = 0; i < 29; i++) {
		for (int j = 0; j < 120; j++) {
			SetCurrentCursorPos(GBOARD_ORIGIN_X + 2 + j, GBOARD_ORIGIN_Y + 1 + i);
			SetConsoleTextAttribute(hc, 7);
			printf(" ");
		}
	}
	DrawStageClear();

	for (int i = 0; i < 29; i++) {
		for (int j = 0; j < 120; j++) {
			SetCurrentCursorPos(GBOARD_ORIGIN_X + 2 + j, GBOARD_ORIGIN_Y + 1 + i);
			SetConsoleTextAttribute(hc, 7);
			printf(" ");
		}
	}
	drawPuzzleZone();
	drawInformation();
	drawInformation2();
	drawMessageBox();
	drawSecondStage();
	if (stage == 1)
		PlaySound(TEXT("엘리.wav"), NULL, SND_ASYNC | SND_NOSTOP);
	if (stage == 2)
		PlaySound(TEXT("커닝.wav"), NULL, SND_ASYNC | SND_NOSTOP);
	DeletePlayer();
	curPosX = 4;
	curPosY = 2;
	NPC_1.X = GBOARD_ORIGIN_X + 30 * 2;
	NPC_1.Y = GBOARD_ORIGIN_Y + 12;
	NPC_2.X = GBOARD_ORIGIN_X + 12 * 2;
	NPC_2.Y = GBOARD_ORIGIN_Y + 18;
	ShowPlayer();

	SetConsoleTextAttribute(hc, 7);
	SetCurrentCursorPos(gBoardInformation + 14, GBOARD_ORIGIN_Y + 7);
	if (saveItem[0] == 3)
		printf("※");
	else if (saveItem[0] == 4)
		printf("≪");
	else if (saveItem[0] == 5)
		printf("♥");
	SetCurrentCursorPos(gBoardInformation + 19, GBOARD_ORIGIN_Y + 7);
	if (saveItem[1] == 3)
		printf("※");
	else if (saveItem[1] == 4)
		printf("≪");
	else if (saveItem[1] == 5)
		printf("♥");
}
void DrawStageClear()
{
	PlaySound(TEXT("부우웅.wav"), NULL, SND_ASYNC);
	RemoveCursor();
	COORD curPos = GetCurrentCursorPos();
	for (int x = 0; x < 9; x++) {
		for (int i = 0; i < 31; i++) {
			for (int j = 0; j < 62; j++) {
				SetConsoleTextAttribute(hc, x);
				SetCurrentCursorPos(GBOARD_ORIGIN_X + j * 2, GBOARD_ORIGIN_Y + i);
				if (gamestart[x][i][j] == 1) {
					printf("■");
				}
				if (gamestart[x][i][j] == 4)
				{
					SetConsoleTextAttribute(hc, 15);
					printf("■");
				}
				if (gamestart[x][i][j] == 3) {
					SetConsoleTextAttribute(hc, 12);
					printf("■");
				}
			}
		}
		Sleep(1000);
		for (int i = 22; i < 31; i++) {
			for (int j = 0; j < 62; j++) {
				SetCurrentCursorPos(GBOARD_ORIGIN_X + j * 2, GBOARD_ORIGIN_Y + i);
				if (gamestart[x][i][j] == 4) {
					printf(" ");
				}
			}
		}

	}
}
void DrawGameOver()
{
	PlaySound(TEXT("게임오버.wav"), NULL, SND_ASYNC);
	int x = 1;
	RemoveCursor();
	COORD curPos = GetCurrentCursorPos();
	while (_kbhit() == 0) {
		if (x > 15) { x = 1; }
		for (int i = 0; i < 31; i++) {
			for (int j = 0; j < 62; j++) {
				SetConsoleTextAttribute(hc, 15);
				SetCurrentCursorPos(GBOARD_ORIGIN_X + j * 2, GBOARD_ORIGIN_Y + i);
				if (gameover[0][i][j] == 1) {
					printf("■");
				}
				if (gameover[0][i][j] == 4) {
					SetConsoleTextAttribute(hc, (x + 3) % 15);
					printf("■");
				}
				if (gameover[0][i][j] == 5) {
					SetConsoleTextAttribute(hc, (x + 5) % 15);
					printf("■");
				}
				if (gameover[0][i][j] == 3) {
					SetConsoleTextAttribute(hc, x);
					printf("■");
				}
			}
			if (i == 0) { Sleep(100); }
		}
		Sleep(100);

		x++;
	}

	for (int i = 0; i < 31; i++) {
		for (int j = 0; j < 62; j++) {
			SetConsoleTextAttribute(hc, 15);
			SetCurrentCursorPos(GBOARD_ORIGIN_X + j * 2, GBOARD_ORIGIN_Y + i);

			printf(" ");

		}
	}



}

int TestSuccessPuzzle(char blockTest[6][6])//정답이면 리턴 1 정답이 아니면 리턴 0
{
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			if (blockTest[i][j] != 1) {

				return 0;
			}
		}
	}
	return 1;
}
void drawPuzzleZone()
{
	int x, y, s;
	int puzzle_3_3 = 7;

	SetCurrentCursorPos(GBOARD_ORIGIN_X, gBoardPuzzle);
	printf("★ P U Z Z L E   Z O N E ★");
	//완성블럭 칸
	for (y = 0; y <= puzzle_3_3; y++) {
		SetCurrentCursorPos(GBOARD_ORIGIN_X, GBOARD_ORIGIN_Y + GBOARD_HEIGHT + 5 + y);
		if (y == puzzle_3_3)
			printf("└");
		else if (y == 0)
			printf("┌");
		else
			printf("│");
	}
	for (y = 0; y <= puzzle_3_3; y++) {
		SetCurrentCursorPos(GBOARD_ORIGIN_X + puzzle_3_3 * 2, GBOARD_ORIGIN_Y + GBOARD_HEIGHT + 5 + y);
		if (y == puzzle_3_3)
			printf("┘");
		else if (y == 0)
			printf("┐");
		else
			printf("│");
	}
	for (x = 2; x < puzzle_3_3 * 2; x += 2) {
		SetCurrentCursorPos(GBOARD_ORIGIN_X + x, GBOARD_ORIGIN_Y + GBOARD_HEIGHT + 5);
		printf("─");
	}
	for (x = 2; x < puzzle_3_3 * 2; x += 2) {
		SetCurrentCursorPos(GBOARD_ORIGIN_X + x, GBOARD_ORIGIN_Y + GBOARD_HEIGHT + puzzle_3_3 + 5);
		printf("─");
	}
	SetCurrentCursorPos(GBOARD_ORIGIN_X + 21, gBoardPuzzle + 5);
	printf("◀◁");
	//예시블럭 칸

	if (stage == 1)	s = 3;
	if (stage == 2)	s = 4;
	for (int i = 0; i < 20 * s; i += 20) {
		SetCurrentCursorPos(GBOARD_ORIGIN_X + 30 + i + 6, gBoardPuzzle + 1);
		if (i == 0) printf("( Q )");
		if (i == 20) printf("( W )");
		if (i == 40) printf("( E )");
		if (i == 60) printf("( R )");

		for (y = 0; y <= puzzle_3_3; y++) {
			SetCurrentCursorPos(GBOARD_ORIGIN_X + 30 + i, gBoardPuzzle + 2 + y);
			if (y == puzzle_3_3)
				printf("└");
			else if (y == 0)
				printf("┌");
			else
				printf("│");
		}
		for (y = 0; y <= puzzle_3_3; y++) {
			SetCurrentCursorPos(GBOARD_ORIGIN_X + 30 + i + puzzle_3_3 * 2, gBoardPuzzle + 2 + y);
			if (y == puzzle_3_3)
				printf("┘");
			else if (y == 0)
				printf("┐");
			else
				printf("│");
		}
		for (x = 2; x < puzzle_3_3 * 2; x += 2) {
			SetCurrentCursorPos(GBOARD_ORIGIN_X + 30 + i + x, gBoardPuzzle + 2);
			printf("─");
		}
		for (x = 2; x < puzzle_3_3 * 2; x += 2) {
			SetCurrentCursorPos(GBOARD_ORIGIN_X + 30 + i + x, gBoardPuzzle + 2 + puzzle_3_3);
			printf("─");
		}
	}

}
void TimeLimit() {
	SetConsoleTextAttribute(hc, 15);
	now_t = clock();
	consume_t = (now_t - old_t) / 1000;

	SetCurrentCursorPos(gBoardInformation, GBOARD_ORIGIN_Y + 2);
	consume_t = TIMELIMIT - consume_t;
	SetConsoleTextAttribute(hc, 7);
	printf("  Time    : %02d분 %02d초", consume_t / 60, consume_t % 60);
	IsGameOver(consume_t);
}
void drawInformation2() {

	char stageName[30];
	SetConsoleTextAttribute(hc, 7);
	//if (stage == 1)   strcpy(stageName, "House(Tutorial)");
	if (stage == 1)   strcpy(stageName, "Bus Stop");
	if (stage == 2)   strcpy(stageName, "Subway Station");
	if (stage == 3)   strcpy(stageName, "Gwanggaeto Building Elevator");

	SetCurrentCursorPos(gBoardInformation, GBOARD_ORIGIN_Y + 1);
	printf("  Stage %d : %s", stage, stageName);
	SetCurrentCursorPos(gBoardInformation, GBOARD_ORIGIN_Y + 3);
	printf("  Life    : ");        // life(); 함수로 ♥ 개수 처리
	for (int i = 1; i <= life; i++) {
		SetConsoleTextAttribute(hc, 12);
		printf("♥");
		if (life == 2 && i == 2)
			printf("    ");
		else if (life == 1 && i == 1)
			printf("         ");
	}
	SetConsoleTextAttribute(hc, 7);
	SetCurrentCursorPos(gBoardInformation, GBOARD_ORIGIN_Y + 4);
	printf("  Combo   : %3d", n_combo);
	SetCurrentCursorPos(gBoardInformation, GBOARD_ORIGIN_Y + 5);
	printf("  Max     : %3d", m_combo);
}
void drawInformation() {
	int x, y;

	for (y = 1; y < 30; y++) {
		SetCurrentCursorPos(gBoardInformation - 2, GBOARD_ORIGIN_Y + y);
		printf("∥");
	}

	SetCurrentCursorPos(gBoardInformation, GBOARD_ORIGIN_Y + 6);
	printf("  Item    : ");

	for (x = 0; x <= 5; x += 5) {
		for (y = 0; y <= 2; y++) {
			SetCurrentCursorPos(gBoardInformation + x + 12, GBOARD_ORIGIN_Y + 6 + y);
			if (y == 2)
				printf("└");
			else if (y == 0)
				printf("┌");
			else
				printf("│");
		}
		for (y = 0; y <= 2; y++) {
			SetCurrentCursorPos(gBoardInformation + x + 16, GBOARD_ORIGIN_Y + 6 + y);
			if (y == 2)
				printf("┘");
			else if (y == 0)
				printf("┐");
			else
				printf("│");
		}
		SetCurrentCursorPos(gBoardInformation + x + 14, GBOARD_ORIGIN_Y + 6);
		printf("─");
		SetCurrentCursorPos(gBoardInformation + x + 14, GBOARD_ORIGIN_Y + 8);
		printf("─");
	}
	/////////////////////////////////////////////////////////////////////////
	//아이콘 설명창
	SetCurrentCursorPos(gBoardInformation, GBOARD_ORIGIN_Y + 23);
	printf("  ▷ : Start Line");
	SetCurrentCursorPos(gBoardInformation, GBOARD_ORIGIN_Y + 24);
	printf("  ¶ : Finish Line");        // life(); 함수로 ♥ 개수 처리
	SetCurrentCursorPos(gBoardInformation, GBOARD_ORIGIN_Y + 25);
	printf("  ■ : Wall");
	SetCurrentCursorPos(gBoardInformation, GBOARD_ORIGIN_Y + 26);
	printf("  ◎ : Player");
	SetCurrentCursorPos(gBoardInformation, GBOARD_ORIGIN_Y + 27);
	printf("  ◆ : NPC");
	SetCurrentCursorPos(gBoardInformation, GBOARD_ORIGIN_Y + 28);
	printf("  ? : Random Item");
}
void drawMessageBox() {
	int x, y;
	for (y = 0; y <= 9; y++) {
		SetCurrentCursorPos(gBoardInformation, GBOARD_ORIGIN_Y + 10 + y);
		if (y == 9)
			printf("└");
		else if (y == 0)
			printf("┌");
		else
			printf("│");
	}
	for (y = 0; y <= 9; y++) {
		SetCurrentCursorPos(gBoardInformation + 15 * 2, GBOARD_ORIGIN_Y + 10 + y);
		if (y == 9)
			printf("┘");
		else if (y == 0)
			printf("┐");
		else
			printf("│");
	}
	for (x = 2; x < 15 * 2; x += 2) {
		SetCurrentCursorPos(gBoardInformation + x, GBOARD_ORIGIN_Y + 10);
		printf("─");
	}
	for (x = 2; x < 15 * 2; x += 2) {
		SetCurrentCursorPos(gBoardInformation + x, GBOARD_ORIGIN_Y + 10 + 9);
		printf("─");
	}
	SetCurrentCursorPos(gBoardInformation + 2, GBOARD_ORIGIN_Y + 11);
	printf("<< 상태 메시지 >>");
	// 상태메시지 함수 호출~~
}
void drawGameBoard() {
	int x, y;
	for (y = 0; y <= GBOARD_HEIGHT; y++) {
		SetCurrentCursorPos(GBOARD_ORIGIN_X, GBOARD_ORIGIN_Y + y);
		if (y == GBOARD_HEIGHT)
			printf("└");
		else if (y == 0)
			printf("┌");
		else
			printf("│");
	}
	for (y = 0; y <= GBOARD_HEIGHT; y++) {
		SetCurrentCursorPos(GBOARD_ORIGIN_X + GBOARD_WIDTH * 2 + 2, GBOARD_ORIGIN_Y + y);
		if (y == GBOARD_HEIGHT)
			printf("┘");
		else if (y == 0)
			printf("┐");
		else
			printf("│");
	}
	for (x = 2; x <= GBOARD_WIDTH * 2; x += 2) {
		SetCurrentCursorPos(GBOARD_ORIGIN_X + x, GBOARD_ORIGIN_Y);
		printf("─");
	}
	for (x = 2; x <= GBOARD_WIDTH * 2; x += 2) {
		SetCurrentCursorPos(GBOARD_ORIGIN_X + x, GBOARD_ORIGIN_Y + GBOARD_HEIGHT);
		printf("─");
	}

	//drawStageTutorial();
	if (stage == 1)   drawFirstStage();
	if (stage == 2)   drawSecondStage();
	drawInformation();
	drawMessageBox();
	drawPuzzleZone();
}
void ShowPlayer()
{
	SetConsoleTextAttribute(hc, 11);
	COORD curPos = GetCurrentCursorPos();

	SetCurrentCursorPos(curPosX, curPosY);
	printf("◎");
}
void DeletePlayer()
{
	COORD curPos = GetCurrentCursorPos();

	SetCurrentCursorPos(curPosX, curPosY);
	printf("  ");
}
int DetectCollision(int posX, int posY)
{
	int arrX = (posX - GBOARD_ORIGIN_X) / 2;
	int arrY = posY - GBOARD_ORIGIN_Y;

	if (stageInfo[arrY][arrX] == 0 || stageInfo[arrY][arrX] == 3 || stageInfo[arrY][arrX] == 5 || stageInfo[arrY][arrX] > 10)    //충돌하지 않는 경우
		return 0;
	if (stageInfo[arrY][arrX] == 4)
		return 2;
	if (stageInfo[arrY][arrX] == 5)
		return 3;
	return 1;
}
int DetectCollisionNPC(COORD NPC) {            //충돌 1리턴 안충돌 0리턴 

	if (curPosX == NPC.X && curPosY == NPC.Y)
		return 1;

}


void ItemHidePuzzleZone() {            //퍼즐존 가리기  3초
	SetConsoleTextAttribute(hc, 15);
	now_t = clock();
	int consume_t = (now_t - old_t) / 1000;

	consume_t = TIMELIMIT - consume_t;
	consume_t % 60;


	for (int y = 0; y < 8; y++)
		for (int x = 0; x < 150; x++) {
			SetCurrentCursorPos(GBOARD_ORIGIN_X - 1 + x, GBOARD_ORIGIN_Y + 35 + y);
			printf("%c", puzzleHide[y][x]);
		}
	SetCurrentCursorPos(gBoardInformation + 2, GBOARD_ORIGIN_Y + 12);
	SetConsoleTextAttribute(hc, 15);
	drawPuzzleZone();
}

int CheckCombo(int random_check) {
	ResetMessageBox(1);
	SetCurrentCursorPos(gBoardInformation + 2, GBOARD_ORIGIN_Y + 13);
	SetConsoleTextAttribute(hc, 7);
	printf("Combo 퀘스트 : %d 회", random_check);
	SetCurrentCursorPos(gBoardInformation + 2, GBOARD_ORIGIN_Y + 17);
	printf("Combo : %d", p_combo);
	if (p_combo == random_check)   return 0;
	else            return 1;
}

void SelectItem() {
	int arrX = (curPosX - GBOARD_ORIGIN_X) / 2;
	int arrY = (curPosY - GBOARD_ORIGIN_Y);

	srand((unsigned)time(NULL));
	int select = rand() % 7;

	switch (select) {
	case 0:						// 이동시간 3초 추가
		randomtime += 3;
		stageInfo[arrY][arrX] = 0;
		ResetMessageBox(1);
		SetConsoleTextAttribute(hc, 11);
		SetCurrentCursorPos(gBoardInformation + 2, GBOARD_ORIGIN_Y + 14);

		printf("버프");
		SetConsoleTextAttribute(hc, 7);

		printf(" : 이동시간 3초 추가!");
		break;
	case 1:
		randomtime = 1;  //이동시간 0
		stageInfo[arrY][arrX] = 0;
		ResetMessageBox(1);
		SetCurrentCursorPos(gBoardInformation + 2, GBOARD_ORIGIN_Y + 13);
		SetConsoleTextAttribute(hc, 12);

		printf("디버프");
		SetConsoleTextAttribute(hc, 7);
		printf(" : 즉시 퍼즐 맞추기!");
		break;
	case 2:
		stageInfo[arrY][arrX] = 0;
		hideitem = 1;//퍼즐존가리기 함수 실행해주기위해서
		ResetMessageBox(1);
		SetCurrentCursorPos(gBoardInformation + 2, GBOARD_ORIGIN_Y + 13);
		SetConsoleTextAttribute(hc, 12);
		printf("디버프");
		SetConsoleTextAttribute(hc, 7);
		printf(" : 퍼즐존 가림막");

		SetCurrentCursorPos(gBoardInformation + 2, GBOARD_ORIGIN_Y + 14);
		printf("퍼즐존이 1회 가려집니다.");
		break;
	case 3:						// 벽뚫기
		stageInfo[arrY][arrX] = 0;
		ResetMessageBox(1);
		SetCurrentCursorPos(gBoardInformation + 2, GBOARD_ORIGIN_Y + 13);
		SetConsoleTextAttribute(hc, 11);
		printf("버프");
		SetConsoleTextAttribute(hc, 7);
		SetCurrentCursorPos(gBoardInformation + 2, GBOARD_ORIGIN_Y + 14);

		printf("1회 벽을 뚫을 수 있습니다.");

		SetCurrentCursorPos(gBoardInformation + 2, GBOARD_ORIGIN_Y + 15);
		printf("벽앞에서만 가능합니다.");

		if (saveItem[0] != 0 && saveItem[1] != 0) {  //아이템 꽉참
			saveItem[0] = saveItem[1];
			saveItem[1] = 3;
			SetCurrentCursorPos(gBoardInformation + 19, GBOARD_ORIGIN_Y + 7);
			printf("※");
		}
		else if (saveItem[0] != 0 && saveItem[1] == 0) {
			saveItem[1] = 3;
			SetCurrentCursorPos(gBoardInformation + 19, GBOARD_ORIGIN_Y + 7);
			printf("※");
		}
		else if (saveItem[0] == 0 && saveItem[1] == 0) {
			saveItem[0] = 3;
			SetCurrentCursorPos(gBoardInformation + 14, GBOARD_ORIGIN_Y + 7);
			printf("※");
		}
		break;
	case 4:				//NpcSpeedUp
		/*
		stageInfo[arrY][arrX] = 0;

		itemS_t = clock();
		ResetMessageBox(1);
		SetCurrentCursorPos(gBoardInformation + 2, GBOARD_ORIGIN_Y + 13);
		SetConsoleTextAttribute(hc, 12);
		printf("디버프");
		SetConsoleTextAttribute(hc, 7);

		SetCurrentCursorPos(gBoardInformation + 2, GBOARD_ORIGIN_Y + 14);
		printf("Npc 속도가 5초 빨라집니다!");

		ItemNpcSpeedUp();
		*/
		break;

	case 5:				//NpcSpeedDown
		stageInfo[arrY][arrX] = 0;
		ResetMessageBox(1);
		SetCurrentCursorPos(gBoardInformation + 2, GBOARD_ORIGIN_Y + 13);
		SetConsoleTextAttribute(hc, 11);
		printf("버프");
		SetConsoleTextAttribute(hc, 7);

		printf(" : Npc를 5초간 재웁니다.");
		SetCurrentCursorPos(gBoardInformation + 2, GBOARD_ORIGIN_Y + 14);
		printf("사용 아이템입니다.");


		if (saveItem[0] != 0 && saveItem[1] != 0) {  //아이템 꽉참
			saveItem[0] = saveItem[1];
			saveItem[1] = 4;
			SetCurrentCursorPos(gBoardInformation + 19, GBOARD_ORIGIN_Y + 7);
			printf("≪");
		}
		else if (saveItem[0] != 0 && saveItem[1] == 0) {
			saveItem[1] = 4;
			SetCurrentCursorPos(gBoardInformation + 19, GBOARD_ORIGIN_Y + 7);
			printf("≪");
		}
		else if (saveItem[0] == 0 && saveItem[1] == 0) {
			saveItem[0] = 4;
			SetCurrentCursorPos(gBoardInformation + 14, GBOARD_ORIGIN_Y + 7);
			printf("≪");
		}
		break;

	case 6:				//라이프 추가
		stageInfo[arrY][arrX] = 0;
		ResetMessageBox(1);
		SetCurrentCursorPos(gBoardInformation + 2, GBOARD_ORIGIN_Y + 13);
		SetConsoleTextAttribute(hc, 11);
		printf("버프");
		SetConsoleTextAttribute(hc, 7);

		printf(" Life 추가!!");
		SetCurrentCursorPos(gBoardInformation + 2, GBOARD_ORIGIN_Y + 14);
		printf("사용 아이템 입니다.");

		if (saveItem[0] != 0 && saveItem[1] != 0) {  //아이템 꽉참
			saveItem[0] = saveItem[1];
			saveItem[1] = 5;
			SetCurrentCursorPos(gBoardInformation + 19, GBOARD_ORIGIN_Y + 7);
			printf("♥");
		}
		else if (saveItem[0] != 0 && saveItem[1] == 0) {
			saveItem[1] = 5;
			SetCurrentCursorPos(gBoardInformation + 19, GBOARD_ORIGIN_Y + 7);
			printf("♥");
		}
		else if (saveItem[0] == 0 && saveItem[1] == 0) {
			saveItem[0] = 5;
			SetCurrentCursorPos(gBoardInformation + 14, GBOARD_ORIGIN_Y + 7);
			printf("♥");
		}
		break;
	}
	return;
}

void ItembreakWall() {
	char mov;
	int arrX = (curPosX - GBOARD_ORIGIN_X) / 2;
	int arrY = curPosY - GBOARD_ORIGIN_Y;
	ResetMessageBox(1);
	SetCurrentCursorPos(gBoardInformation + 2, GBOARD_ORIGIN_Y + 13);
	SetConsoleTextAttribute(hc, 7);
	printf("뚫고 싶은 벽의 방향을");
	SetCurrentCursorPos(gBoardInformation + 2, GBOARD_ORIGIN_Y + 14);
	printf("1초 안에 입력해주세요.");

	for (int j = 0; j < 50; j++) {
		if (_kbhit() != 0) {
			int key;
			key = _getch();
			switch (key) {
			case UP:
				if (stageInfo[arrY - 1][arrX] == 1) {
					SetCurrentCursorPos(curPosX, curPosY - 1);
					printf(" ");
					stageInfo[arrY - 1][arrX] = 0;
					ResetMessageBox(1);
					SetCurrentCursorPos(gBoardInformation + 2, GBOARD_ORIGIN_Y + 13);
					SetConsoleTextAttribute(hc, 7);
					printf("성공!          ");
					break;
				}
				else {
					ResetMessageBox(1);
					SetCurrentCursorPos(gBoardInformation + 2, GBOARD_ORIGIN_Y + 13);
					SetConsoleTextAttribute(hc, 7);
					printf("벽이 없습니다.");
					break;
				}
				break;

			case DOWN:
				if (stageInfo[arrY + 1][arrX] == 1) {
					SetCurrentCursorPos(curPosX, curPosY + 1);
					printf(" ");
					stageInfo[arrY + 1][arrX] = 0;
					SetCurrentCursorPos(gBoardInformation + 2, GBOARD_ORIGIN_Y + 13);
					SetConsoleTextAttribute(hc, 7);
					printf("성공!    ");
					break;
				}
				else {
					ResetMessageBox(1);
					SetCurrentCursorPos(gBoardInformation + 2, GBOARD_ORIGIN_Y + 13);
					SetConsoleTextAttribute(hc, 7);
					printf("벽이 없습니다.");
					break;
				}
				break;

			case LEFT:
				if (stageInfo[arrY][arrX - 1] == 1) {
					SetCurrentCursorPos(curPosX - 2, curPosY);
					printf(" ");
					stageInfo[arrY][arrX - 1] = 0;
					ResetMessageBox(1);
					SetCurrentCursorPos(gBoardInformation + 2, GBOARD_ORIGIN_Y + 13);
					SetConsoleTextAttribute(hc, 7);
					printf("성공!          ");
					break;
				}
				else {
					ResetMessageBox(1);
					SetCurrentCursorPos(gBoardInformation + 2, GBOARD_ORIGIN_Y + 13);
					SetConsoleTextAttribute(hc, 7);
					printf("벽이 없습니다.");
					break;

				}
				break;
			case RIGHT:
				if (stageInfo[arrY][arrX + 1] == 1) {
					SetCurrentCursorPos(curPosX + 2, curPosY);
					printf(" ");
					stageInfo[arrY][arrX + 1] = 0;
					ResetMessageBox(1);
					SetCurrentCursorPos(gBoardInformation + 2, GBOARD_ORIGIN_Y + 13);
					SetConsoleTextAttribute(hc, 7);
					printf("성공!      ");
					break;
				}
				else {
					ResetMessageBox(1);
					SetCurrentCursorPos(gBoardInformation + 2, GBOARD_ORIGIN_Y + 13);
					SetConsoleTextAttribute(hc, 7);
					printf("벽이 없습니다.");
					break;

				}
				break;
			}
		}
		Sleep(20);
	}
}
void AssignTime(int random_check) {
	if (p_count == random_check)      randomtime = 5;
	else if (p_count == random_check + 1)   randomtime = 4;
	else if (p_count == random_check + 2)   randomtime = 3;
	else if (p_count >= random_check + 3)   randomtime = 2;
	return;
}

void DetectItemZone() {
	int arrX = (curPosX - GBOARD_ORIGIN_X) / 2;
	int arrY = curPosY - GBOARD_ORIGIN_Y;

	if (stageInfo[arrY][arrX] == 5)
		SelectItem();
}

void ShowNPC(COORD NPC)
{
	if ((NPC.X == NPC_1.X && NPC.Y == NPC_1.Y) || (NPC.X == NPC_2.X && NPC.Y == NPC_2.Y) || (NPC.X == NPC_16.X && NPC.Y == NPC_16.Y))
		SetConsoleTextAttribute(hc, 12);
	if ((NPC.X == NPC_3.X && NPC.Y == NPC_3.Y) || (NPC.X == NPC_4.X && NPC.Y == NPC_4.Y) || (NPC.X == NPC_5.X && NPC.Y == NPC_5.Y) || (NPC.X == NPC_6.X && NPC.Y == NPC_6.Y) ||
		(NPC.X == NPC_7.X && NPC.Y == NPC_7.Y) || (NPC.X == NPC_8.X && NPC.Y == NPC_8.Y) || (NPC.X == NPC_9.X && NPC.Y == NPC_9.Y) || (NPC.X == NPC_10.X && NPC.Y == NPC_10.Y) ||
		(NPC.X == NPC_11.X && NPC.Y == NPC_11.Y) || (NPC.X == NPC_12.X && NPC.Y == NPC_12.Y) || (NPC.X == NPC_12.X && NPC.Y == NPC_12.Y) || (NPC.X == NPC_13.X && NPC.Y == NPC_13.Y) ||
		(NPC.X == NPC_14.X && NPC.Y == NPC_14.Y) || (NPC.X == NPC_15.X && NPC.Y == NPC_15.Y))
		SetConsoleTextAttribute(hc, 13);
	COORD curPos = GetCurrentCursorPos();

	SetCurrentCursorPos(NPC.X, NPC.Y);
	printf("◆");
}

void DeleteNPC(COORD NPC) {
	SetCurrentCursorPos(NPC.X, NPC.Y);
	printf(" ");
}


void ControlNPC() {
	MoveNpc_1();	//랜덤NPC
	MoveNpc_2();	//랜덤NPC

	if (stage == 1) {
		MoveNpc_3();	//패턴NPC (반시계)
		MoveNpc_4();	//패턴NPC (시계)
		MoveNpc_5();	//패턴NPC (반시계)
		MoveNpc_6();	//패턴NPC (시계)
	}
	if (stage == 2) {
		MoveNpc_7();	//패턴NPC (시계)
		MoveNpc_8();	//패턴NPC (반시계)
		MoveNpc_9();	//패턴NPC (시계)
		MoveNpc_10();	//패턴NPC (반시계)
		MoveNpc_11();	//패턴NPC (시계)
		MoveNpc_12();	//패턴NPC (시계)
		MoveNpc_13();	//패턴NPC (반시계)
		MoveNpc_14();	//패턴NPC (반시계)
		MoveNpc_15();	//패턴NPC

		MoveNpc_16();	//랜덤NPC

	}
}

void MoveNpc_1() {

	if (DetectCollisionNPC(NPC_1) == 1) {
		DeletePlayer();
		if (stage == 1) {
			curPosX = GBOARD_ORIGIN_X + 12;
			curPosY = GBOARD_ORIGIN_Y + 6;
		}
		if (stage == 2) {
			curPosX = GBOARD_ORIGIN_X + 2;
			curPosY = GBOARD_ORIGIN_Y + 1;
		}
		ShowPlayer();
		life--;
		drawInformation2();
		if (life == 0) {
			for (int i = 0; i < 29; i++) {
				for (int j = 0; j < 120; j++) {
					SetCurrentCursorPos(GBOARD_ORIGIN_X + 2 + j, GBOARD_ORIGIN_Y + 1 + i);
					SetConsoleTextAttribute(hc, 7);
					printf(" ");
				}
			}
			DrawGameOver();
		}
	}

	if (npc1_movement == 0) {   // 오른쪽 방향으로 진행 중일때
		if (DetectCollision(NPC_1.X + 2, NPC_1.Y) == 1 || DetectCollision(NPC_1.X + 2, NPC_1.Y) == 3) {
			npc1_movement = rand() % 4;
			return;
		}
		else {
			DeleteNPC(NPC_1);
			NPC_1.X += 2;
			ShowNPC(NPC_1);
		}
	}
	else if (npc1_movement == 1) {   //왼쪽 방향으로 진행 중일때
		if (DetectCollision(NPC_1.X - 2, NPC_1.Y) == 1 || DetectCollision(NPC_1.X - 2, NPC_1.Y) == 3) {
			npc1_movement = rand() % 4;
			return;
		}
		else   //왼쪽 벽과 충돌하지 않으면 계속 진행
		{
			DeleteNPC(NPC_1);
			NPC_1.X -= 2;
			ShowNPC(NPC_1);
		}
	}
	else if (npc1_movement == 2) {   //위쪽 방향으로 진행 중일때
		if (DetectCollision(NPC_1.X, NPC_1.Y - 1) == 1 || DetectCollision(NPC_1.X, NPC_1.Y - 1) == 3) {
			npc1_movement = rand() % 4;
			return;
		}
		else   //위쪽 벽과 충돌하지 않으면 계속 진행
		{
			DeleteNPC(NPC_1);
			NPC_1.Y--;
			ShowNPC(NPC_1);
		}
	}
	else if (npc1_movement == 3) {      //아래
		if (DetectCollision(NPC_1.X, NPC_1.Y + 1) == 1 || DetectCollision(NPC_1.X, NPC_1.Y + 1) == 3) {
			npc1_movement = rand() % 4;
			return;
		}
		else {
			DeleteNPC(NPC_1);
			NPC_1.Y++;
			ShowNPC(NPC_1);
		}
	}

}
void MoveNpc_2() {

	if (DetectCollisionNPC(NPC_2) == 1) {
		DeletePlayer();
		if (stage == 1) {
			curPosX = GBOARD_ORIGIN_X + 12;
			curPosY = GBOARD_ORIGIN_Y + 6;
		}
		if (stage == 2) {
			curPosX = GBOARD_ORIGIN_X + 2;
			curPosY = GBOARD_ORIGIN_Y + 1;
		}
		ShowPlayer();
		life--;
		drawInformation2();
		if (life == 0) {
			for (int i = 0; i < 29; i++) {
				for (int j = 0; j < 120; j++) {
					SetCurrentCursorPos(GBOARD_ORIGIN_X + 2 + j, GBOARD_ORIGIN_Y + 1 + i);
					SetConsoleTextAttribute(hc, 7);
					printf(" ");
				}
			}
			DrawGameOver();
		}
	}

	if (npc2_movement == 0) {   // 오른쪽 방향으로 진행 중일때
		if (DetectCollision(NPC_2.X + 2, NPC_2.Y) == 1) {
			npc2_movement = rand() % 4;
			return;
		}
		else {
			DeleteNPC(NPC_2);
			NPC_2.X += 2;
			ShowNPC(NPC_2);
		}
	}
	else if (npc2_movement == 1) {   //왼쪽 방향으로 진행 중일때
		if (DetectCollision(NPC_2.X - 2, NPC_2.Y) == 1) {
			npc2_movement = rand() % 4;
			return;
		}
		else   //왼쪽 벽과 충돌하지 않으면 계속 진행
		{
			DeleteNPC(NPC_2);
			NPC_2.X -= 2;
			ShowNPC(NPC_2);
		}
	}
	else if (npc2_movement == 2) {   //위쪽 방향으로 진행 중일때
		if (DetectCollision(NPC_2.X, NPC_2.Y - 1) == 1) {
			npc2_movement = rand() % 4;
			return;
		}
		else   //위쪽 벽과 충돌하지 않으면 계속 진행
		{
			DeleteNPC(NPC_2);
			NPC_2.Y--;
			ShowNPC(NPC_2);
		}
	}
	else if (npc2_movement == 3) {      //아래
		if (DetectCollision(NPC_2.X, NPC_2.Y + 1) == 1) {
			npc2_movement = rand() % 4;
			return;
		}
		else {
			DeleteNPC(NPC_2);
			NPC_2.Y++;
			ShowNPC(NPC_2);
		}
	}

}

void MoveNpc_3() {

	if (DetectCollisionNPC(NPC_3) == 1) {
		DeletePlayer();
		if (stage == 1) {
			curPosX = GBOARD_ORIGIN_X + 12;
			curPosY = GBOARD_ORIGIN_Y + 6;
		}
		if (stage == 2) {
			curPosX = GBOARD_ORIGIN_X + 2;
			curPosY = GBOARD_ORIGIN_Y + 1;
		}
		ShowPlayer();
		life--;
		drawInformation2();
		if (life == 0) {
			for (int i = 0; i < 29; i++) {
				for (int j = 0; j < 120; j++) {
					SetCurrentCursorPos(GBOARD_ORIGIN_X + 2 + j, GBOARD_ORIGIN_Y + 1 + i);
					SetConsoleTextAttribute(hc, 7);
					printf(" ");
				}
			}
			DrawGameOver();
		}
	}

	if (npc3_movement == 0) {   // 오른쪽 방향으로 진행 중일때
		if (NPC_3.X == 32 && NPC_3.Y == 18) {
			npc3_movement = 2;

			DeleteNPC(NPC_3);
			NPC_3.Y--;
			ShowNPC(NPC_3);
		}
		else {
			DeleteNPC(NPC_3);
			NPC_3.X += 2;
			ShowNPC(NPC_3);
		}
	}
	else if (npc3_movement == 1) {   //왼쪽 방향으로 진행 중일때
		if (NPC_3.X == 26 && NPC_3.Y == 13) {      //왼쪽 벽과 충돌하면
			npc3_movement = 3;

			DeleteNPC(NPC_3);
			NPC_3.Y += 1;
			ShowNPC(NPC_3);
		}
		else   //왼쪽 벽과 충돌하지 않으면 계속 진행
		{
			DeleteNPC(NPC_3);
			NPC_3.X -= 2;
			ShowNPC(NPC_3);
		}
	}
	else if (npc3_movement == 2) {   //위쪽 방향으로 진행 중일때
		if (NPC_3.X == 32 && NPC_3.Y == 13) {      //터닝포인트
			npc3_movement = 1;
			DeleteNPC(NPC_3);
			NPC_3.X -= 2;         //왼쪽으로 한번진행
			ShowNPC(NPC_3);
		}
		else   //위쪽 벽과 충돌하지 않으면 계속 진행
		{
			DeleteNPC(NPC_3);
			NPC_3.Y--;
			ShowNPC(NPC_3);
		}
	}
	else if (npc3_movement == 3) {      //아래
		if (NPC_3.X == 26 && NPC_3.Y == 18) {
			npc3_movement = 0;

			DeleteNPC(NPC_3);
			NPC_3.X += 2;
			ShowNPC(NPC_3);
		}
		else {
			DeleteNPC(NPC_3);
			NPC_3.Y++;
			ShowNPC(NPC_3);
		}
	}
}

void MoveNpc_4() {

	if (DetectCollisionNPC(NPC_4) == 1) {
		DeletePlayer();
		if (stage == 1) {
			curPosX = GBOARD_ORIGIN_X + 12;
			curPosY = GBOARD_ORIGIN_Y + 6;
		}
		if (stage == 2) {
			curPosX = GBOARD_ORIGIN_X + 2;
			curPosY = GBOARD_ORIGIN_Y + 1;
		}
		ShowPlayer();
		life--;
		drawInformation2();
		if (life == 0) {
			for (int i = 0; i < 29; i++) {
				for (int j = 0; j < 120; j++) {
					SetCurrentCursorPos(GBOARD_ORIGIN_X + 2 + j, GBOARD_ORIGIN_Y + 1 + i);
					SetConsoleTextAttribute(hc, 7);
					printf(" ");
				}
			}
			DrawGameOver();
		}
	}

	if (npc4_movement == 0) {   // 오른쪽 방향으로 진행 중일때
		if (NPC_4.X == 36 && NPC_4.Y == 22) {
			npc4_movement = 3;

			DeleteNPC(NPC_4);
			NPC_4.Y++;
			ShowNPC(NPC_4);
		}
		else {
			DeleteNPC(NPC_4);
			NPC_4.X += 2;
			ShowNPC(NPC_4);
		}
	}
	else if (npc4_movement == 1) {   //왼쪽 방향으로 진행 중일때
		if (NPC_4.X == 28 && NPC_4.Y == 25) {      //왼쪽 벽과 충돌하면
			npc4_movement = 2;

			DeleteNPC(NPC_4);
			NPC_4.Y--;
			ShowNPC(NPC_4);
		}
		else   //왼쪽 벽과 충돌하지 않으면 계속 진행
		{
			DeleteNPC(NPC_4);
			NPC_4.X -= 2;
			ShowNPC(NPC_4);
		}
	}
	else if (npc4_movement == 2) {   //위쪽 방향으로 진행 중일때
		if (NPC_4.X == 28 && NPC_4.Y == 22) {      //터닝포인트
			npc4_movement = 0;
			DeleteNPC(NPC_4);
			NPC_4.X += 2;         //왼쪽으로 한번진행
			ShowNPC(NPC_4);
		}
		else   //위쪽 벽과 충돌하지 않으면 계속 진행
		{
			DeleteNPC(NPC_4);
			NPC_4.Y--;
			ShowNPC(NPC_4);
		}
	}
	else if (npc4_movement == 3) {      //아래
		if (NPC_4.X == 36 && NPC_4.Y == 25) {
			npc4_movement = 1;

			DeleteNPC(NPC_4);
			NPC_4.X -= 2;
			ShowNPC(NPC_4);
		}
		else {
			DeleteNPC(NPC_4);
			NPC_4.Y++;
			ShowNPC(NPC_4);
		}
	}
}

void MoveNpc_5() {

	if (DetectCollisionNPC(NPC_5) == 1) {
		DeletePlayer();
		if (stage == 1) {
			curPosX = GBOARD_ORIGIN_X + 12;
			curPosY = GBOARD_ORIGIN_Y + 6;
		}
		if (stage == 2) {
			curPosX = GBOARD_ORIGIN_X + 2;
			curPosY = GBOARD_ORIGIN_Y + 1;
		}
		ShowPlayer();
		life--;
		drawInformation2();
		if (life == 0) {
			for (int i = 0; i < 29; i++) {
				for (int j = 0; j < 120; j++) {
					SetCurrentCursorPos(GBOARD_ORIGIN_X + 2 + j, GBOARD_ORIGIN_Y + 1 + i);
					SetConsoleTextAttribute(hc, 7);
					printf(" ");
				}
			}
			DrawGameOver();
		}
	}

	if (npc5_movement == 0) {   // 오른쪽 방향으로 진행 중일때
		if (NPC_5.X == 52 && NPC_5.Y == 17) {
			npc5_movement = 2;

			DeleteNPC(NPC_5);
			NPC_5.Y--;
			ShowNPC(NPC_5);
		}
		else {
			DeleteNPC(NPC_5);
			NPC_5.X += 2;
			ShowNPC(NPC_5);
		}
	}
	else if (npc5_movement == 1) {   //왼쪽 방향으로 진행 중일때
		if (NPC_5.X == 44 && NPC_5.Y == 15) {      //왼쪽 벽과 충돌하면
			npc5_movement = 3;

			DeleteNPC(NPC_5);
			NPC_5.Y += 1;
			ShowNPC(NPC_5);
		}
		else   //왼쪽 벽과 충돌하지 않으면 계속 진행
		{
			DeleteNPC(NPC_5);
			NPC_5.X -= 2;
			ShowNPC(NPC_5);
		}
	}
	else if (npc5_movement == 2) {   //위쪽 방향으로 진행 중일때
		if (NPC_5.X == 52 && NPC_5.Y == 15) {      //터닝포인트
			npc5_movement = 1;
			DeleteNPC(NPC_5);
			NPC_5.X -= 2;         //왼쪽으로 한번진행
			ShowNPC(NPC_5);
		}
		else   //위쪽 벽과 충돌하지 않으면 계속 진행
		{
			DeleteNPC(NPC_5);
			NPC_5.Y--;
			ShowNPC(NPC_5);
		}
	}
	else if (npc5_movement == 3) {      //아래
		if (NPC_5.X == 44 && NPC_5.Y == 17) {
			npc5_movement = 0;

			DeleteNPC(NPC_5);
			NPC_5.X += 2;
			ShowNPC(NPC_5);
		}
		else {
			DeleteNPC(NPC_5);
			NPC_5.Y++;
			ShowNPC(NPC_5);
		}
	}
}

void MoveNpc_6() {

	if (DetectCollisionNPC(NPC_6) == 1) {
		DeletePlayer();
		if (stage == 1) {
			curPosX = GBOARD_ORIGIN_X + 12;
			curPosY = GBOARD_ORIGIN_Y + 6;
		}
		if (stage == 2) {
			curPosX = GBOARD_ORIGIN_X + 2;
			curPosY = GBOARD_ORIGIN_Y + 1;
		}
		ShowPlayer();
		life--;
		drawInformation2();
		if (life == 0) {
			for (int i = 0; i < 29; i++) {
				for (int j = 0; j < 120; j++) {
					SetCurrentCursorPos(GBOARD_ORIGIN_X + 2 + j, GBOARD_ORIGIN_Y + 1 + i);
					SetConsoleTextAttribute(hc, 7);
					printf(" ");
				}
			}
			DrawGameOver();
		}
	}

	if (npc6_movement == 0) {   // 오른쪽 방향으로 진행 중일때
		if (NPC_6.X == 70 && NPC_6.Y == 8) {
			npc6_movement = 3;

			DeleteNPC(NPC_6);
			NPC_6.Y++;
			ShowNPC(NPC_6);
		}
		else {
			DeleteNPC(NPC_6);
			NPC_6.X += 2;
			ShowNPC(NPC_6);
		}
	}
	else if (npc6_movement == 1) {   //왼쪽 방향으로 진행 중일때
		if (NPC_6.X == 60 && NPC_6.Y == 11) {      //왼쪽 벽과 충돌하면
			npc6_movement = 2;

			DeleteNPC(NPC_6);
			NPC_6.Y--;
			ShowNPC(NPC_6);
		}
		else   //왼쪽 벽과 충돌하지 않으면 계속 진행
		{
			DeleteNPC(NPC_6);
			NPC_6.X -= 2;
			ShowNPC(NPC_6);
		}
	}
	else if (npc6_movement == 2) {   //위쪽 방향으로 진행 중일때
		if (NPC_6.X == 60 && NPC_6.Y == 8) {      //터닝포인트
			npc6_movement = 0;
			DeleteNPC(NPC_6);
			NPC_6.X += 2;         //왼쪽으로 한번진행
			ShowNPC(NPC_6);
		}
		else   //위쪽 벽과 충돌하지 않으면 계속 진행
		{
			DeleteNPC(NPC_6);
			NPC_6.Y--;
			ShowNPC(NPC_6);
		}
	}
	else if (npc6_movement == 3) {      //아래
		if (NPC_6.X == 70 && NPC_6.Y == 11) {
			npc6_movement = 1;

			DeleteNPC(NPC_6);
			NPC_6.X -= 2;
			ShowNPC(NPC_6);
		}
		else {
			DeleteNPC(NPC_6);
			NPC_6.Y++;
			ShowNPC(NPC_6);
		}
	}
}

void MoveNpc_7() {

	if (DetectCollisionNPC(NPC_7) == 1) {
		DeletePlayer();
		if (stage == 1) {
			curPosX = GBOARD_ORIGIN_X + 12;
			curPosY = GBOARD_ORIGIN_Y + 6;
		}
		if (stage == 2) {
			curPosX = GBOARD_ORIGIN_X + 2;
			curPosY = GBOARD_ORIGIN_Y + 1;
		}
		ShowPlayer();
		life--;
		drawInformation2();
		if (life == 0) {
			for (int i = 0; i < 29; i++) {
				for (int j = 0; j < 120; j++) {
					SetCurrentCursorPos(GBOARD_ORIGIN_X + 2 + j, GBOARD_ORIGIN_Y + 1 + i);
					SetConsoleTextAttribute(hc, 7);
					printf(" ");
				}
			}
			DrawGameOver();
		}
	}

	if (npc7_movement == 0) {   // 오른쪽 방향으로 진행 중일때
		if (NPC_7.X == 24 && NPC_7.Y == 7) {
			npc7_movement = 3;

			DeleteNPC(NPC_7);
			NPC_7.Y++;
			ShowNPC(NPC_7);
		}
		else {
			DeleteNPC(NPC_7);
			NPC_7.X += 2;
			ShowNPC(NPC_7);
		}
	}
	else if (npc7_movement == 1) {   //왼쪽 방향으로 진행 중일때
		if (NPC_7.X == 20 && NPC_7.Y == 11) {      //왼쪽 벽과 충돌하면
			npc7_movement = 2;

			DeleteNPC(NPC_7);
			NPC_7.Y--;
			ShowNPC(NPC_7);
		}
		else   //왼쪽 벽과 충돌하지 않으면 계속 진행
		{
			DeleteNPC(NPC_7);
			NPC_7.X -= 2;
			ShowNPC(NPC_7);
		}
	}
	else if (npc7_movement == 2) {   //위쪽 방향으로 진행 중일때
		if (NPC_7.X == 20 && NPC_7.Y == 7) {      //터닝포인트
			npc7_movement = 0;
			DeleteNPC(NPC_7);
			NPC_7.X += 2;         //왼쪽으로 한번진행
			ShowNPC(NPC_7);
		}
		else   //위쪽 벽과 충돌하지 않으면 계속 진행
		{
			DeleteNPC(NPC_7);
			NPC_7.Y--;
			ShowNPC(NPC_7);
		}
	}
	else if (npc7_movement == 3) {      //아래
		if (NPC_7.X == 24 && NPC_7.Y == 11) {
			npc7_movement = 1;

			DeleteNPC(NPC_7);
			NPC_7.X -= 2;
			ShowNPC(NPC_7);
		}
		else {
			DeleteNPC(NPC_7);
			NPC_7.Y++;
			ShowNPC(NPC_7);
		}
	}
}

void MoveNpc_8() {

	if (DetectCollisionNPC(NPC_8) == 1) {
		DeletePlayer();
		if (stage == 1) {
			curPosX = GBOARD_ORIGIN_X + 12;
			curPosY = GBOARD_ORIGIN_Y + 6;
		}
		if (stage == 2) {
			curPosX = GBOARD_ORIGIN_X + 2;
			curPosY = GBOARD_ORIGIN_Y + 1;
		}
		ShowPlayer();
		life--;
		drawInformation2();
		if (life == 0) {
			for (int i = 0; i < 29; i++) {
				for (int j = 0; j < 120; j++) {
					SetCurrentCursorPos(GBOARD_ORIGIN_X + 2 + j, GBOARD_ORIGIN_Y + 1 + i);
					SetConsoleTextAttribute(hc, 7);
					printf(" ");
				}
			}
			DrawGameOver();
		}
	}

	if (npc8_movement == 0) {   // 오른쪽 방향으로 진행 중일때
		if (NPC_8.X == 34 && NPC_8.Y == 14) {
			npc8_movement = 2;

			DeleteNPC(NPC_8);
			NPC_8.Y--;
			ShowNPC(NPC_8);
		}
		else {
			DeleteNPC(NPC_8);
			NPC_8.X += 2;
			ShowNPC(NPC_8);
		}
	}
	else if (npc8_movement == 1) {   //왼쪽 방향으로 진행 중일때
		if (NPC_8.X == 28 && NPC_8.Y == 11) {      //왼쪽 벽과 충돌하면
			npc8_movement = 3;

			DeleteNPC(NPC_8);
			NPC_8.Y += 1;
			ShowNPC(NPC_8);
		}
		else   //왼쪽 벽과 충돌하지 않으면 계속 진행
		{
			DeleteNPC(NPC_8);
			NPC_8.X -= 2;
			ShowNPC(NPC_8);
		}
	}
	else if (npc8_movement == 2) {   //위쪽 방향으로 진행 중일때
		if (NPC_8.X == 34 && NPC_8.Y == 11) {      //터닝포인트
			npc8_movement = 1;
			DeleteNPC(NPC_8);
			NPC_8.X -= 2;         //왼쪽으로 한번진행
			ShowNPC(NPC_8);
		}
		else   //위쪽 벽과 충돌하지 않으면 계속 진행
		{
			DeleteNPC(NPC_8);
			NPC_8.Y--;
			ShowNPC(NPC_8);
		}
	}
	else if (npc8_movement == 3) {      //아래
		if (NPC_8.X == 28 && NPC_8.Y == 14) {
			npc8_movement = 0;

			DeleteNPC(NPC_8);
			NPC_8.X += 2;
			ShowNPC(NPC_8);
		}
		else {
			DeleteNPC(NPC_8);
			NPC_8.Y++;
			ShowNPC(NPC_8);
		}
	}
}

void MoveNpc_9() {

	if (DetectCollisionNPC(NPC_9) == 1) {
		DeletePlayer();
		if (stage == 1) {
			curPosX = GBOARD_ORIGIN_X + 12;
			curPosY = GBOARD_ORIGIN_Y + 6;
		}
		if (stage == 2) {
			curPosX = GBOARD_ORIGIN_X + 2;
			curPosY = GBOARD_ORIGIN_Y + 1;
		}
		ShowPlayer();
		life--;
		drawInformation2();
		if (life == 0) {
			for (int i = 0; i < 29; i++) {
				for (int j = 0; j < 120; j++) {
					SetCurrentCursorPos(GBOARD_ORIGIN_X + 2 + j, GBOARD_ORIGIN_Y + 1 + i);
					SetConsoleTextAttribute(hc, 7);
					printf(" ");
				}
			}
			DrawGameOver();
		}
	}

	if (npc9_movement == 0) {   // 오른쪽 방향으로 진행 중일때
		if (NPC_9.X == 48 && NPC_9.Y == 8) {
			npc9_movement = 3;

			DeleteNPC(NPC_9);
			NPC_9.Y++;
			ShowNPC(NPC_9);
		}
		else {
			DeleteNPC(NPC_9);
			NPC_9.X += 2;
			ShowNPC(NPC_9);
		}
	}
	else if (npc9_movement == 1) {   //왼쪽 방향으로 진행 중일때
		if (NPC_9.X == 42 && NPC_9.Y == 11) {      //왼쪽 벽과 충돌하면
			npc9_movement = 2;

			DeleteNPC(NPC_9);
			NPC_9.Y--;
			ShowNPC(NPC_9);
		}
		else   //왼쪽 벽과 충돌하지 않으면 계속 진행
		{
			DeleteNPC(NPC_9);
			NPC_9.X -= 2;
			ShowNPC(NPC_9);
		}
	}
	else if (npc9_movement == 2) {   //위쪽 방향으로 진행 중일때
		if (NPC_9.X == 42 && NPC_9.Y == 8) {      //터닝포인트
			npc9_movement = 0;
			DeleteNPC(NPC_9);
			NPC_9.X += 2;         //왼쪽으로 한번진행
			ShowNPC(NPC_9);
		}
		else   //위쪽 벽과 충돌하지 않으면 계속 진행
		{
			DeleteNPC(NPC_9);
			NPC_9.Y--;
			ShowNPC(NPC_9);
		}
	}
	else if (npc9_movement == 3) {      //아래
		if (NPC_9.X == 48 && NPC_9.Y == 11) {
			npc9_movement = 1;

			DeleteNPC(NPC_9);
			NPC_9.X -= 2;
			ShowNPC(NPC_9);
		}
		else {
			DeleteNPC(NPC_9);
			NPC_9.Y++;
			ShowNPC(NPC_9);
		}
	}
}

void MoveNpc_10() {

	if (DetectCollisionNPC(NPC_10) == 1) {
		DeletePlayer();
		if (stage == 1) {
			curPosX = GBOARD_ORIGIN_X + 12;
			curPosY = GBOARD_ORIGIN_Y + 6;
		}
		if (stage == 2) {
			curPosX = GBOARD_ORIGIN_X + 2;
			curPosY = GBOARD_ORIGIN_Y + 1;
		}
		ShowPlayer();
		life--;
		drawInformation2();
		if (life == 0) {
			for (int i = 0; i < 29; i++) {
				for (int j = 0; j < 120; j++) {
					SetCurrentCursorPos(GBOARD_ORIGIN_X + 2 + j, GBOARD_ORIGIN_Y + 1 + i);
					SetConsoleTextAttribute(hc, 7);
					printf(" ");
				}
			}
			DrawGameOver();
		}
	}

	if (npc10_movement == 0) {   // 오른쪽 방향으로 진행 중일때
		if (NPC_10.X == 46 && NPC_10.Y == 26) {
			npc10_movement = 2;

			DeleteNPC(NPC_10);
			NPC_10.Y--;
			ShowNPC(NPC_10);
		}
		else {
			DeleteNPC(NPC_10);
			NPC_10.X += 2;
			ShowNPC(NPC_10);
		}
	}
	else if (npc10_movement == 1) {   //왼쪽 방향으로 진행 중일때
		if (NPC_10.X == 40 && NPC_10.Y == 21) {      //왼쪽 벽과 충돌하면
			npc10_movement = 3;

			DeleteNPC(NPC_10);
			NPC_10.Y += 1;
			ShowNPC(NPC_10);
		}
		else   //왼쪽 벽과 충돌하지 않으면 계속 진행
		{
			DeleteNPC(NPC_10);
			NPC_10.X -= 2;
			ShowNPC(NPC_10);
		}
	}
	else if (npc10_movement == 2) {   //위쪽 방향으로 진행 중일때
		if (NPC_10.X == 46 && NPC_10.Y == 21) {      //터닝포인트
			npc10_movement = 1;
			DeleteNPC(NPC_10);
			NPC_10.X -= 2;         //왼쪽으로 한번진행
			ShowNPC(NPC_10);
		}
		else   //위쪽 벽과 충돌하지 않으면 계속 진행
		{
			DeleteNPC(NPC_10);
			NPC_10.Y--;
			ShowNPC(NPC_10);
		}
	}
	else if (npc10_movement == 3) {      //아래
		if (NPC_10.X == 40 && NPC_10.Y == 26) {
			npc10_movement = 0;

			DeleteNPC(NPC_10);
			NPC_10.X += 2;
			ShowNPC(NPC_10);
		}
		else {
			DeleteNPC(NPC_10);
			NPC_10.Y++;
			ShowNPC(NPC_10);
		}
	}
}

void MoveNpc_11() {

	if (DetectCollisionNPC(NPC_11) == 1) {
		DeletePlayer();
		if (stage == 1) {
			curPosX = GBOARD_ORIGIN_X + 12;
			curPosY = GBOARD_ORIGIN_Y + 6;
		}
		if (stage == 2) {
			curPosX = GBOARD_ORIGIN_X + 2;
			curPosY = GBOARD_ORIGIN_Y + 1;
		}
		ShowPlayer();
		life--;
		drawInformation2();
		if (life == 0) {
			for (int i = 0; i < 29; i++) {
				for (int j = 0; j < 120; j++) {
					SetCurrentCursorPos(GBOARD_ORIGIN_X + 2 + j, GBOARD_ORIGIN_Y + 1 + i);
					SetConsoleTextAttribute(hc, 7);
					printf(" ");
				}
			}
			DrawGameOver();
		}
	}

	if (npc11_movement == 0) {   // 오른쪽 방향으로 진행 중일때
		if (NPC_11.X == 74 && NPC_11.Y == 8) {
			npc11_movement = 3;

			DeleteNPC(NPC_11);
			NPC_11.Y++;
			ShowNPC(NPC_11);
		}
		else {
			DeleteNPC(NPC_11);
			NPC_11.X += 2;
			ShowNPC(NPC_11);
		}
	}
	else if (npc11_movement == 1) {   //왼쪽 방향으로 진행 중일때
		if (NPC_11.X == 68 && NPC_11.Y == 13) {      //왼쪽 벽과 충돌하면
			npc11_movement = 2;

			DeleteNPC(NPC_11);
			NPC_11.Y--;
			ShowNPC(NPC_11);
		}
		else   //왼쪽 벽과 충돌하지 않으면 계속 진행
		{
			DeleteNPC(NPC_11);
			NPC_11.X -= 2;
			ShowNPC(NPC_11);
		}
	}
	else if (npc11_movement == 2) {   //위쪽 방향으로 진행 중일때
		if (NPC_11.X == 68 && NPC_11.Y == 8) {      //터닝포인트
			npc11_movement = 0;
			DeleteNPC(NPC_11);
			NPC_11.X += 2;         //왼쪽으로 한번진행
			ShowNPC(NPC_11);
		}
		else   //위쪽 벽과 충돌하지 않으면 계속 진행
		{
			DeleteNPC(NPC_11);
			NPC_11.Y--;
			ShowNPC(NPC_11);
		}
	}
	else if (npc11_movement == 3) {      //아래
		if (NPC_11.X == 74 && NPC_11.Y == 13) {
			npc11_movement = 1;

			DeleteNPC(NPC_11);
			NPC_11.X -= 2;
			ShowNPC(NPC_11);
		}
		else {
			DeleteNPC(NPC_11);
			NPC_11.Y++;
			ShowNPC(NPC_11);
		}
	}
}

void MoveNpc_12() {

	if (DetectCollisionNPC(NPC_12) == 1) {
		DeletePlayer();
		if (stage == 1) {
			curPosX = GBOARD_ORIGIN_X + 12;
			curPosY = GBOARD_ORIGIN_Y + 6;
		}
		if (stage == 2) {
			curPosX = GBOARD_ORIGIN_X + 2;
			curPosY = GBOARD_ORIGIN_Y + 1;
		}
		ShowPlayer();
		life--;
		drawInformation2();
		if (life == 0) {
			for (int i = 0; i < 29; i++) {
				for (int j = 0; j < 120; j++) {
					SetCurrentCursorPos(GBOARD_ORIGIN_X + 2 + j, GBOARD_ORIGIN_Y + 1 + i);
					SetConsoleTextAttribute(hc, 7);
					printf(" ");
				}
			}
			DrawGameOver();
		}
	}

	if (npc12_movement == 0) {   // 오른쪽 방향으로 진행 중일때
		if (NPC_12.X == 88 && NPC_12.Y == 8) {
			npc12_movement = 3;

			DeleteNPC(NPC_12);
			NPC_12.Y++;
			ShowNPC(NPC_12);
		}
		else {
			DeleteNPC(NPC_12);
			NPC_12.X += 2;
			ShowNPC(NPC_12);
		}
	}
	else if (npc12_movement == 1) {   //왼쪽 방향으로 진행 중일때
		if (NPC_12.X == 76 && NPC_12.Y == 11) {      //왼쪽 벽과 충돌하면
			npc12_movement = 2;

			DeleteNPC(NPC_12);
			NPC_12.Y--;
			ShowNPC(NPC_12);
		}
		else   //왼쪽 벽과 충돌하지 않으면 계속 진행
		{
			DeleteNPC(NPC_12);
			NPC_12.X -= 2;
			ShowNPC(NPC_12);
		}
	}
	else if (npc12_movement == 2) {   //위쪽 방향으로 진행 중일때
		if (NPC_12.X == 76 && NPC_12.Y == 8) {      //터닝포인트
			npc12_movement = 0;
			DeleteNPC(NPC_12);
			NPC_12.X += 2;         //왼쪽으로 한번진행
			ShowNPC(NPC_12);
		}
		else   //위쪽 벽과 충돌하지 않으면 계속 진행
		{
			DeleteNPC(NPC_12);
			NPC_12.Y--;
			ShowNPC(NPC_12);
		}
	}
	else if (npc12_movement == 3) {      //아래
		if (NPC_12.X == 88 && NPC_12.Y == 11) {
			npc12_movement = 1;

			DeleteNPC(NPC_12);
			NPC_12.X -= 2;
			ShowNPC(NPC_12);
		}
		else {
			DeleteNPC(NPC_12);
			NPC_12.Y++;
			ShowNPC(NPC_12);
		}
	}
}

void MoveNpc_13() {

	if (DetectCollisionNPC(NPC_13) == 1) {
		DeletePlayer();
		if (stage == 1) {
			curPosX = GBOARD_ORIGIN_X + 12;
			curPosY = GBOARD_ORIGIN_Y + 6;
		}
		if (stage == 2) {
			curPosX = GBOARD_ORIGIN_X + 2;
			curPosY = GBOARD_ORIGIN_Y + 1;
		}
		ShowPlayer();
		life--;
		drawInformation2();
		if (life == 0) {
			for (int i = 0; i < 29; i++) {
				for (int j = 0; j < 120; j++) {
					SetCurrentCursorPos(GBOARD_ORIGIN_X + 2 + j, GBOARD_ORIGIN_Y + 1 + i);
					SetConsoleTextAttribute(hc, 7);
					printf(" ");
				}
			}
			DrawGameOver();
		}
	}

	if (npc13_movement == 0) {   // 오른쪽 방향으로 진행 중일때
		if (NPC_13.X == 78 && NPC_13.Y == 27) {
			npc13_movement = 2;

			DeleteNPC(NPC_13);
			NPC_13.Y--;
			ShowNPC(NPC_13);
		}
		else {
			DeleteNPC(NPC_13);
			NPC_13.X += 2;
			ShowNPC(NPC_13);
		}
	}
	else if (npc13_movement == 1) {   //왼쪽 방향으로 진행 중일때
		if (NPC_13.X == 72 && NPC_13.Y == 25) {      //왼쪽 벽과 충돌하면
			npc13_movement = 3;

			DeleteNPC(NPC_13);
			NPC_13.Y += 1;
			ShowNPC(NPC_13);
		}
		else   //왼쪽 벽과 충돌하지 않으면 계속 진행
		{
			DeleteNPC(NPC_13);
			NPC_13.X -= 2;
			ShowNPC(NPC_13);
		}
	}
	else if (npc13_movement == 2) {   //위쪽 방향으로 진행 중일때
		if (NPC_13.X == 78 && NPC_13.Y == 25) {      //터닝포인트
			npc13_movement = 1;
			DeleteNPC(NPC_13);
			NPC_13.X -= 2;         //왼쪽으로 한번진행
			ShowNPC(NPC_13);
		}
		else   //위쪽 벽과 충돌하지 않으면 계속 진행
		{
			DeleteNPC(NPC_13);
			NPC_13.Y--;
			ShowNPC(NPC_13);
		}
	}
	else if (npc13_movement == 3) {      //아래
		if (NPC_13.X == 72 && NPC_13.Y == 27) {
			npc13_movement = 0;

			DeleteNPC(NPC_13);
			NPC_13.X += 2;
			ShowNPC(NPC_13);
		}
		else {
			DeleteNPC(NPC_13);
			NPC_13.Y++;
			ShowNPC(NPC_13);
		}
	}
}

void MoveNpc_14() {

	if (DetectCollisionNPC(NPC_14) == 1) {
		DeletePlayer();
		if (stage == 1) {
			curPosX = GBOARD_ORIGIN_X + 12;
			curPosY = GBOARD_ORIGIN_Y + 6;
		}
		if (stage == 2) {
			curPosX = GBOARD_ORIGIN_X + 2;
			curPosY = GBOARD_ORIGIN_Y + 1;
		}
		ShowPlayer();
		life--;
		drawInformation2();
		if (life == 0) {
			for (int i = 0; i < 29; i++) {
				for (int j = 0; j < 120; j++) {
					SetCurrentCursorPos(GBOARD_ORIGIN_X + 2 + j, GBOARD_ORIGIN_Y + 1 + i);
					SetConsoleTextAttribute(hc, 7);
					printf(" ");
				}
			}
			DrawGameOver();
		}
	}

	if (npc14_movement == 0) {   // 오른쪽 방향으로 진행 중일때
		if (NPC_14.X == 80 && NPC_14.Y == 30) {
			npc14_movement = 2;

			DeleteNPC(NPC_14);
			NPC_14.Y--;
			ShowNPC(NPC_14);
		}
		else {
			DeleteNPC(NPC_14);
			NPC_14.X += 2;
			ShowNPC(NPC_14);
		}
	}
	else if (npc14_movement == 1) {   //왼쪽 방향으로 진행 중일때
		if (NPC_14.X == 72 && NPC_14.Y == 28) {      //왼쪽 벽과 충돌하면
			npc14_movement = 3;

			DeleteNPC(NPC_14);
			NPC_14.Y += 1;
			ShowNPC(NPC_14);
		}
		else   //왼쪽 벽과 충돌하지 않으면 계속 진행
		{
			DeleteNPC(NPC_14);
			NPC_14.X -= 2;
			ShowNPC(NPC_14);
		}
	}
	else if (npc14_movement == 2) {   //위쪽 방향으로 진행 중일때
		if (NPC_14.X == 80 && NPC_14.Y == 28) {      //터닝포인트
			npc14_movement = 1;
			DeleteNPC(NPC_14);
			NPC_14.X -= 2;         //왼쪽으로 한번진행
			ShowNPC(NPC_14);
		}
		else   //위쪽 벽과 충돌하지 않으면 계속 진행
		{
			DeleteNPC(NPC_14);
			NPC_14.Y--;
			ShowNPC(NPC_14);
		}
	}
	else if (npc14_movement == 3) {      //아래
		if (NPC_14.X == 72 && NPC_14.Y == 30) {
			npc14_movement = 0;

			DeleteNPC(NPC_14);
			NPC_14.X += 2;
			ShowNPC(NPC_14);
		}
		else {
			DeleteNPC(NPC_14);
			NPC_14.Y++;
			ShowNPC(NPC_14);
		}
	}
}

void MoveNpc_15() {

	if (DetectCollisionNPC(NPC_15) == 1) {
		DeletePlayer();
		if (stage == 1) {
			curPosX = GBOARD_ORIGIN_X + 12;
			curPosY = GBOARD_ORIGIN_Y + 6;
		}
		if (stage == 2) {
			curPosX = GBOARD_ORIGIN_X + 2;
			curPosY = GBOARD_ORIGIN_Y + 1;
		}
		ShowPlayer();
		life--;
		drawInformation2();
		if (life == 0) {
			for (int i = 0; i < 29; i++) {
				for (int j = 0; j < 120; j++) {
					SetCurrentCursorPos(GBOARD_ORIGIN_X + 2 + j, GBOARD_ORIGIN_Y + 1 + i);
					SetConsoleTextAttribute(hc, 7);
					printf(" ");
				}
			}
			DrawGameOver();
		}
	}

	if (npc15_movement == 0) {   // 오른쪽 방향으로 진행 중일때
		if (NPC_15.X == 84) {
			npc15_movement = 1;

			DeleteNPC(NPC_15);
			NPC_15.X -= 2;
			ShowNPC(NPC_15);
		}
		else {
			DeleteNPC(NPC_15);
			NPC_15.X += 2;
			ShowNPC(NPC_15);
		}
	}
	else if (npc15_movement == 1) {   //왼쪽 방향으로 진행 중일때
		if (NPC_15.X == 82) {      //왼쪽 벽과 충돌하면
			if (NPC_15.Y == 28) {
				npc15_movement = 3;

				DeleteNPC(NPC_15);
				NPC_15.Y += 1;
				ShowNPC(NPC_15);
			}
			if (NPC_15.Y == 30) {
				npc15_movement = 2;

				DeleteNPC(NPC_15);
				NPC_15.Y -= 1;
				ShowNPC(NPC_15);
			}
		}
		else   //왼쪽 벽과 충돌하지 않으면 계속 진행
		{
			DeleteNPC(NPC_15);
			NPC_15.X -= 2;
			ShowNPC(NPC_15);
		}
	}
	else if (npc15_movement == 2) {   //위쪽 방향으로 진행 중일때
		if (NPC_15.X == 82 && NPC_15.Y == 28) {      //터닝포인트
			npc15_movement = 0;
			DeleteNPC(NPC_15);
			NPC_15.X += 2;         //왼쪽으로 한번진행
			ShowNPC(NPC_15);
		}
		else   //위쪽 벽과 충돌하지 않으면 계속 진행
		{
			DeleteNPC(NPC_15);
			NPC_15.Y--;
			ShowNPC(NPC_15);
		}
	}
	else if (npc15_movement == 3) {      //아래
		if (NPC_15.X == 82 && NPC_15.Y == 30) {
			npc15_movement = 0;

			DeleteNPC(NPC_15);
			NPC_15.X += 2;
			ShowNPC(NPC_15);
		}
		else {
			DeleteNPC(NPC_15);
			NPC_15.Y++;
			ShowNPC(NPC_15);
		}
	}
}

void MoveNpc_16() {

	if (DetectCollisionNPC(NPC_16) == 1) {
		DeletePlayer();
		if (stage == 1) {
			curPosX = GBOARD_ORIGIN_X + 12;
			curPosY = GBOARD_ORIGIN_Y + 6;
		}
		if (stage == 2) {
			curPosX = GBOARD_ORIGIN_X + 2;
			curPosY = GBOARD_ORIGIN_Y + 1;
		}
		ShowPlayer();
		life--;
		drawInformation2();
		if (life == 0) {
			for (int i = 0; i < 29; i++) {
				for (int j = 0; j < 120; j++) {
					SetCurrentCursorPos(GBOARD_ORIGIN_X + 2 + j, GBOARD_ORIGIN_Y + 1 + i);
					SetConsoleTextAttribute(hc, 7);
					printf(" ");
				}
			}
			DrawGameOver();
		}
	}

	if (npc16_movement == 0) {   // 오른쪽 방향으로 진행 중일때
		if (DetectCollision(NPC_16.X + 2, NPC_16.Y) == 1) {
			npc16_movement = rand() % 4;
			return;
		}
		else {
			DeleteNPC(NPC_16);
			NPC_16.X += 2;
			ShowNPC(NPC_16);
		}
	}
	else if (npc16_movement == 1) {   //왼쪽 방향으로 진행 중일때
		if (DetectCollision(NPC_16.X - 2, NPC_16.Y) == 1) {
			npc16_movement = rand() % 4;
			return;
		}
		else   //왼쪽 벽과 충돌하지 않으면 계속 진행
		{
			DeleteNPC(NPC_16);
			NPC_16.X -= 2;
			ShowNPC(NPC_16);
		}
	}
	else if (npc16_movement == 2) {   //위쪽 방향으로 진행 중일때
		if (DetectCollision(NPC_16.X, NPC_16.Y - 1) == 1) {
			npc16_movement = rand() % 4;
			return;
		}
		else   //위쪽 벽과 충돌하지 않으면 계속 진행
		{
			DeleteNPC(NPC_16);
			NPC_16.Y--;
			ShowNPC(NPC_16);
		}
	}
	else if (npc16_movement == 3) {      //아래
		if (DetectCollision(NPC_16.X, NPC_16.Y + 1) == 1) {
			npc16_movement = rand() % 4;
			return;
		}
		else {
			DeleteNPC(NPC_16);
			NPC_16.Y++;
			ShowNPC(NPC_16);
		}
	}

}
void ItemNpcSpeedUp() {
	if (ingItem == 7) {         //아이템 끝낫을 때
		ingItem = 0;
		npc_speed = 0;
		itemE_t = 0;   itemS_t = 0;
	}
	else {
		ingItem = 7;
		npc_speed = -500;
	}
}

void ItemNpcSpeedDown() {
	if (ingItem == 8) {         //아이템 끝낫을 때
		ingItem = 0;
		npc_speed = 0;
		itemE_t = 0;   itemS_t = 0;
	}
	else {
		ingItem = 8;
		npc_speed += 30000;
	}

}
void UseItem() {
	if (saveItem[0] == 0) return;
	switch (saveItem[0]) {
	case 3:                  //벽뚫기
		ItembreakWall();
		break;
	case 4:
		itemS_t = clock();
		ItemNpcSpeedDown();
		break;
	case 5:
		if (life != 3) {
			ResetMessageBox(1);
			life++;
			SetConsoleTextAttribute(hc, 7);
			SetCurrentCursorPos(gBoardInformation + 2, GBOARD_ORIGIN_Y + 14);
			printf("Life가 증가하였습니다!");
		}
		else {
			ResetMessageBox(1);
			SetConsoleTextAttribute(hc, 7);
			SetCurrentCursorPos(gBoardInformation + 2, GBOARD_ORIGIN_Y + 14);
			printf("Life가 꽉찾습니다!");
		}
		drawInformation2();
		break;
	}

	if (saveItem[1] != 0) {         //아이템 두개있을때
		saveItem[0] = saveItem[1];
		saveItem[1] = 0;
		SetCurrentCursorPos(gBoardInformation + 19, GBOARD_ORIGIN_Y + 7);
		printf(" ");
		SetCurrentCursorPos(gBoardInformation + 14, GBOARD_ORIGIN_Y + 7);
		if (saveItem[0] == 3)
			printf("※");
		else if (saveItem[0] == 4)
			printf("≪");
		else if (saveItem[0] == 5)
			printf("♥");
	}
	else {
		saveItem[0] = 0;
		SetCurrentCursorPos(gBoardInformation + 14, GBOARD_ORIGIN_Y + 7);
		printf(" ");
	}
}

void CheckItemTime() {
	itemE_t = clock();
	if ((itemE_t - itemS_t) / 1000 == 5) {
		if (ingItem == 7) {         //다시 호출해서 멈춰원래대로
			ItemNpcSpeedUp();
		}
		if (ingItem == 8) {
			ItemNpcSpeedDown();
		}
	}
}


void ResetMessageBox(int now) {
	SetCurrentCursorPos(gBoardInformation + 2, GBOARD_ORIGIN_Y + 13);
	printf("                            ");
	SetCurrentCursorPos(gBoardInformation + 2, GBOARD_ORIGIN_Y + 14);
	printf("                            ");
	SetCurrentCursorPos(gBoardInformation + 2, GBOARD_ORIGIN_Y + 15);
	printf("                            ");
}

void IsGameOver(int consume_t) {
	if (life == 0) {
		for (int i = 0; i < 29; i++) {
			for (int j = 0; j < 120; j++) {
				SetCurrentCursorPos(GBOARD_ORIGIN_X + 2 + j, GBOARD_ORIGIN_Y + 1 + i);
				SetConsoleTextAttribute(hc, 7);
				printf(" ");
			}
		}
		DrawGameOver();
	}
	if (consume_t == 0) {
		for (int i = 0; i < 29; i++) {
			for (int j = 0; j < 120; j++) {
				SetCurrentCursorPos(GBOARD_ORIGIN_X + 2 + j, GBOARD_ORIGIN_Y + 1 + i);
				SetConsoleTextAttribute(hc, 7);
				printf(" ");
			}
		}
		DrawGameOver();
	}
}


void DrawFirstMonitor()
{
	int x = 1; int key;
	RemoveCursor();
	COORD curPos = GetCurrentCursorPos();

	while (1) {

		if (x > 15) { x = 1; }
		for (int i = 0; i < 31; i++) {

			for (int j = 0; j < 62; j++) {
				SetConsoleTextAttribute(hc, 15);
				SetCurrentCursorPos(GBOARD_ORIGIN_X + j * 2, GBOARD_ORIGIN_Y + i);
				if (firstmonitor[i][j] == 1) {
					printf("■");
				}
				if (firstmonitor[i][j] == 4) {
					SetConsoleTextAttribute(hc, (x + 3) % 15);
					printf("■");
				}
				if (firstmonitor[i][j] == 5) {
					SetConsoleTextAttribute(hc, (x + 5) % 15);
					printf("■");
				}
				if (firstmonitor[i][j] == 3) {
					SetConsoleTextAttribute(hc, x);
					printf("■");
				}
				if (firstmonitor[i][j] == 7) {
					SetConsoleTextAttribute(hc, 15);
					printf("Press any key!!");
				}
				if (firstmonitor[i][j] == 8) {
					SetConsoleTextAttribute(hc, x);
					printf("■");
				}
				if (firstmonitor[i][j] == 9) {
					SetConsoleTextAttribute(hc, 15);
					printf("#아이템 설명 (I)키를누르세요.");
				}
			}



		}
		Sleep(100);
		x++;
		if (_kbhit() != 0) {
			key = _getch();
			Sleep(1000);
			break;
		}


	}

	for (int i = 0; i < 31; i++) {
		for (int j = 0; j < 124; j++) {
			SetConsoleTextAttribute(hc, 15);
			SetCurrentCursorPos(GBOARD_ORIGIN_X + j, GBOARD_ORIGIN_Y + i);
	
			printf(" ");
	
		}
	}

	Sleep(1000);

	if (key == I) {
		DrawItemInfo();

	}

}

void DrawItemInfo()
{
	int kkk = 0;
	system("cls");
	while (1) {


		for (int i = 0; i < 31; i++) {
			kkk++;
			for (int j = 0; j < 62; j++) {
				SetConsoleTextAttribute(hc, 15);
				SetCurrentCursorPos(GBOARD_ORIGIN_X + j * 2, GBOARD_ORIGIN_Y + i);
				if (itemInfo[i][j] == 8) {
					printf("아이템에 대하여 알아봅시다!! 게임화면에서 '?'는 아이템 입니다");
				}
				if (itemInfo[i][j] == 1) {
					printf("♥아이템은 라이프를 1추가시켜 줍니다.");
				}
				if (itemInfo[i][j] == 2) {
					printf("※아이템은 벽으로 가로막힌 부분을 뚫어 줍니다.");
				}
				if (itemInfo[i][j] == 3) {
					printf("≪아이템은 돌아다니는 Npc를 5초 재워줍니다.");
				}
				if (itemInfo[i][j] == 4) {
					printf("다음 퍼즐을 맞출때 자동으로 퍼즐존을 가리는 아이템도 존재합니다.");
				}
				if (itemInfo[i][j] == 5) {
					printf("이동가능시간을 즉시 0으로 만드는 아이템도 존재합니다.");
				}
				if (itemInfo[i][j] == 6) {
					printf("이동가능시간을 즉시 3초 추가해주는 아이템도 존재합니다.");
				}
				if (itemInfo[i][j] == 7) {
					printf("Npc스피드를 5초동안 빠르게 해주는 아이템도 존재합니다.");
				}
				if (itemInfo[i][j] == 9) {
					printf("플레이어가 직접 사용하는 아이템은 최대 두개를 소유할수있으니 유의합시다.");
				}

			}


			{ if (kkk < 31)Sleep(100); }
		}

		if (_kbhit() != 0) {
			break;
		}
	}
	Sleep(100);
	while (1) {
		if (getch())
			break;
	}
	for (int i = 0; i < 31; i++) {
		for (int j = 0; j < 124; j++) {
			SetConsoleTextAttribute(hc, 15);
			SetCurrentCursorPos(GBOARD_ORIGIN_X + j, GBOARD_ORIGIN_Y + i);

			printf(" ");

		}

	}
}
void DrawClearGrade(int time)
{
	PlaySound(TEXT("스테이지.wav"), NULL, SND_ASYNC);
	int x = 0;
	if (consume_t > 200)
	{
		x = 0;
	}
	else if (consume_t > 100)
	{
		x = 2;
	}
	else if (consume_t > 0)
	{
		x = 4;
	}
	RemoveCursor();
	system("cls");
	while (1) {
		for (int i = 0; i < 31; i++) {
			for (int j = 0; j < 62; j++) {
				SetConsoleTextAttribute(hc, 15);
				SetCurrentCursorPos(GBOARD_ORIGIN_X + j * 2, GBOARD_ORIGIN_Y + i);
				if (cleargrade[x][i][j] == 1) {
					printf("■");
				}
				if (cleargrade[x][i][j] == 3) {
					SetConsoleTextAttribute(hc, 12);
					printf("■");
				}

				if (cleargrade[x][i][j] == 4) {
					SetConsoleTextAttribute(hc, 14);
					printf("■");
				}

			}

		}
		for (int i = 0; i < 31; i++) {
			for (int j = 0; j < 62; j++) {
				SetConsoleTextAttribute(hc, 15);
				SetCurrentCursorPos(GBOARD_ORIGIN_X + j * 2, GBOARD_ORIGIN_Y + i);
				if (cleargrade[x][i][j] == 4) {
					SetConsoleTextAttribute(hc, 14);
					printf(" ");
				}

			}

		}
		for (int i = 0; i < 31; i++) {
			for (int j = 0; j < 62; j++) {
				SetConsoleTextAttribute(hc, 15);
				SetCurrentCursorPos(GBOARD_ORIGIN_X + j * 2, GBOARD_ORIGIN_Y + i);
				if (cleargrade[x + 1][i][j] == 1) {
					printf("■");
				}
				if (cleargrade[x + 1][i][j] == 3) {
					SetConsoleTextAttribute(hc, 12);
					printf("■");
				}

				if (cleargrade[x + 1][i][j] == 4) {
					SetConsoleTextAttribute(hc, 14);
					printf("■");
				}

			}

		}
	}
}