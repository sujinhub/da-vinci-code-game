// C언어 다빈치 코드
// 콘솔창 배경 DARK_GRAY, 레거시 콘솔 사용

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>

typedef struct PLAYER {
	char name[10]; // 플레이어의 이름을 입력받는 배열
	int block[13]; // 플레이어가 고른 블럭을 담는 배열
	int opened_block[13]; // 플레이어가 공개한 블럭을 담는 배열
	int turn; // 플레이어의 차례를 알려주는 배열 (1이면 플레이어의 차례, 0이면 상대의 차례)
	int pick; // 플레이어가 고른 블록의 숫자 담는 변수
	int opened_block_on;
} PLAYER;

PLAYER player1;
PLAYER player2;

// 플레이어들이 고를 (처음에 중앙에 놓일) 블록 배열
int game_blocks[26] = { 1,  2,  3,  4,  5,  6,  7,  8,  9,  10,  11,  12,  13,
//        흰색 블록     0,  1,  2,  3,  4,  5,  6,  7,  8,   9,  10,  11    - 
					   -1, -2, -3, -4, -5, -6, -7, -8, -9, -10, -11, -12, -13 }; // index : 0~25
//      검은색 블록     0,  1,  2,  3,  4,  5,  6,  7,  8,   9,  10,  11    - 

// 블록이 선택되면 1로 표시할 배열 (중복 선택 방지)
int selected_blocks[26] = { 0, };

/* ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■ */

// 색 목록
enum {
	BLOCK,
	DARK_BLUE,
	DARK_GREEN,
	DARK_SKY_BLUE,
	DARK_RED,
	DARK_VOILET,
	DARK_YELLOW,
	GRAY,
	DARK_GRAY,
	BLUE,
	GREEN,
	SKY_BLUE,
	RED,
	VOILET,
	YELLOW,
	WHITE,
}; // 색 목록

// 텍스트와 배경색 변경하는 함수
void change_color(int text, int background) {
	int color = text + background * 16;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// 콘솔창의 좌표를 나타내는 함수
void gotoxy(int x, int y) {
	COORD pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

// 커서 없애는 함수
void delete_cursor() {
	CONSOLE_CURSOR_INFO CurInfo;
	CurInfo.dwSize = 1;
	CurInfo.bVisible = FALSE;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &CurInfo);
}

/* ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■ */

// 함수 선언하는 부분
void player_name();
void mix_blocks(int block_arr[], int num);
void make_a_block(int num, int x, int y);
void make_a_hidden_block(int num, int x, int y);
int decide_the_turn();
void frame();
void show_mixed_blocks(int block_arr[]);
int select_a_block();
void make_the_center_clear();
void hide_a_block(int nth_block);
void select_the_joker();
int select_4blocks();
int location_of_joker();
void sorting_blocks(int arr[], int num);
void one_turn();
void who_is_win();
int guess_a_block(int my_block);

/* ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■ */

// 메뉴 나타나는 초기 화면 함수
int init_screen() {
	char key;
	int y = 17;

	change_color(BLOCK, DARK_GRAY);
	gotoxy(15, 5);
	printf("■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■");
	gotoxy(15, 7);
	printf("■■    ■■     ■  ■ ■■■ ■    ■   ■■ ■■■      ■■   ■■   ■■   ■■■");
	gotoxy(15, 8);
	printf("■  ■ ■  ■    ■  ■   ■   ■■  ■ ■       ■      ■     ■    ■ ■  ■ ■");
	gotoxy(15, 9);
	printf("■  ■ ■■■    ■  ■   ■   ■ ■ ■ ■       ■      ■     ■    ■ ■  ■ ■■■");
	gotoxy(15, 10);
	printf("■  ■ ■  ■     ■■    ■   ■  ■■ ■       ■      ■     ■    ■ ■  ■ ■");
	gotoxy(15, 11);
	printf("■■   ■  ■      ■   ■■■ ■    ■   ■■ ■■■      ■■   ■■   ■■   ■■■");
	gotoxy(15, 13);
	printf("■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■");

	gotoxy(53, 17); printf(" 게임시작");
	gotoxy(53, 21); printf(" 게임종료");

	while (1) {
		change_color(SKY_BLUE, DARK_GRAY);
		gotoxy(50, y); printf("☞");
		key = _getch(); // 키 입력 받음.

		if (key == 72) { // ↑ Key
			y = y - 4;
			gotoxy(50, y + 4); puts("  ");
			if (y < 17) y = 21; // 맨 위에서 ↑ Key를 누르면 맨 아래로
		}
		if (key == 80) { // ↓ Key
			y = y + 4;
			gotoxy(50, y - 4); puts("  ");
			if (y > 21) y = 17; // 맨 아래에서 ↓ Key를 누르면 맨 위로
		}
		if (key == 13) return y; // Enter Key 누르면 선택하는 메뉴가 있는 y 좌표가 return
	}
}

/* ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■ */

void start_game() {
	system("cls");
	player_name();

	system("cls");
	decide_the_turn();

	system("cls");
	frame();
	select_4blocks();

	// 모이는 것처럼 보이게 만듦
	for (int i = 1; i <= 3; i++) {
		int j = 0;
		while (17 + i * 5 - j != 17) {
			change_color(DARK_GRAY, DARK_GRAY);
			make_a_hidden_block(player1.block[i], 17 + i * 5 - j, 3);
			make_a_hidden_block(player2.block[i], 17 + i * 5 - j, 22);
			j++;

			if (player1.block[i] > 0) change_color(BLOCK, WHITE);
			else if (player1.block[i] < 0) change_color(WHITE, BLOCK);
			make_a_hidden_block(player1.block[i], 17 + i * 5 - j, 3);

			if (player2.block[i] > 0) change_color(BLOCK, WHITE);
			else if (player2.block[i] < 0) change_color(WHITE, BLOCK);
			make_a_hidden_block(player2.block[i], 17 + i * 5 - j, 22);
			Sleep(20);
		}
	}

	// 규칙대로 블록을 정렬함.
	sorting_blocks(player1.block, 4);
	sorting_blocks(player2.block, 4);

	while (1) {
		one_turn();
	}
}

// 플레이어들의 이름을 입력받는 함수
void player_name() {
	change_color(BLOCK, DARK_GRAY);
	gotoxy(5, 9); printf("※ 플레이어1의 이름을 입력해주세요. (5글자 이내로 권장, 입력 후 Enter Key)");
	gotoxy(5, 15); printf("※ 플레이어2의 이름을 입력해주세요. (5글자 이내로 권장, 입력 후 Enter Key)");

	change_color(SKY_BLUE, DARK_GRAY);
	gotoxy(5, 10); printf("▶");
	gotoxy(5, 16); printf("▶");
	gotoxy(8, 10); scanf("%s", player1.name);
	gotoxy(8, 16); scanf("%s", player2.name);
}

// num개의 블록을 랜덤으로 섞는 함수
void mix_blocks(int block_arr[], int num) {
	int temp, A, B, i = 0;
	for (i = 0; i <= 100; i++) { // 난수 생성하여 100번 섞어서..
		A = rand() % num;
		B = rand() % num;
		temp = block_arr[A];
		block_arr[A] = block_arr[B];
		block_arr[B] = temp;
	}
}

// 규칙에 따라 블록 정렬하는 함수 (작은 수 OR 검은 블록이 왼쪽)
void sorting_blocks(int arr[], int num) {
	int temp;

	for (int i = 0; i < num; i++) {
		for (int j = 0; j < num - 1; j++) { // 숫자에 따라 정렬
			if (abs(arr[i]) == 13 || abs(arr[j]) == 13); // 조커는 정렬하지 않음.
			else if (abs(arr[j]) > abs(arr[j + 1])) {
				temp = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = temp;
				if (player1.opened_block_on == 1) {
					temp = player1.opened_block[j];
					player1.opened_block[j] = player1.opened_block[j + 1];
					player1.opened_block[j + 1] = temp;
				}
				else if (player2.opened_block_on == 1) {
					temp = player2.opened_block[j];
					player2.opened_block[j] = player2.opened_block[j + 1];
					player2.opened_block[j + 1] = temp;
				}
			}

			else if (abs(arr[j]) == abs(arr[j + 1])) { // 블록 색에 따라 정렬
				if (arr[j] > arr[j + 1]) {
					temp = arr[j];
					arr[j] = arr[j + 1];
					arr[j + 1] = temp;
					if (player1.opened_block_on == 1) {
						temp = player1.opened_block[j];
						player1.opened_block[j] = player1.opened_block[j + 1];
						player1.opened_block[j + 1] = temp;
					}
					else if (player2.opened_block_on == 1) {
						temp = player2.opened_block[j];
						player2.opened_block[j] = player2.opened_block[j + 1];
						player2.opened_block[j + 1] = temp;
					}
				}
			}
		}
	}
}

// 주어진 숫자에 맞게 좌표 x, y에 블록 생성하는 함수
void make_a_block(int num, int x, int y) {
	if (num > 0) change_color(BLOCK, WHITE); // 양수면 흰색 블록으로
	else change_color(WHITE, BLOCK); // 음수면 검은색 블록으로

	gotoxy(x, y); printf("    ");
	gotoxy(x, y + 1);
	switch (abs(num)) { // 숫자를 그대로 쓰면 칸이 맞지 않아 특수 문자(ㅈ+한자)를 사용
	case 1:
		printf(" ０ ");
		break;
	case 2:
		printf(" １ ");
		break;
	case 3:
		printf(" ２ ");
		break;
	case 4:
		printf(" ３ ");
		break;
	case 5:
		printf(" ４ ");
		break;
	case 6:
		printf(" ５ ");
		break;
	case 7:
		printf(" ６ ");
		break;
	case 8:
		printf(" ７ ");
		break;
	case 9:
		printf(" ８ ");
		break;
	case 10:
		printf(" ９ ");
		break;
	case 11:
		printf(" 10 ");
		break;
	case 12:
		printf(" 11 ");
		break;
	case 13:
		printf(" ㅡ ");
		break;
	}
	gotoxy(x, y + 2); printf(" ━ ");
	change_color(DARK_GRAY, DARK_GRAY);
	x = x + 4;
	gotoxy(x, y); printf(" ");
	gotoxy(x, y + 1); printf(" ");
	gotoxy(x, y + 2); printf(" ");
}

// 주어진 숫자에 맞게 좌표 x, y에 값이 가려진 블록 생성하는 함수
void make_a_hidden_block(int num, int x, int y) {
	if (abs(num) == 13) change_color(WHITE, DARK_RED);
	// ★ 주어진 시간이 짧으므로 확인을 원활하게 하기 위해 빨간 색으로 표시하였다.

	else if (num > 0) change_color(BLOCK, WHITE); // 흰색 블록
	else change_color(WHITE, BLOCK); // 검은색 블록
	gotoxy(x, y); printf("    ");
	gotoxy(x, y + 1); printf("    ");
	gotoxy(x, y + 2); printf("    ");
	change_color(DARK_GRAY, DARK_GRAY);
	x = x + 4;
	gotoxy(x, y); printf(" ");
	gotoxy(x, y + 1); printf(" ");
	gotoxy(x, y + 2); printf(" ");
}

// 블록을 골라 플레이어의 순서 정하는 함수
int decide_the_turn() {
	int white_block[12] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
	//        흰색 블록     0, 1, 2, 3, 4, 5, 6, 7, 8,  9, 10, 11 };
	int black_block[12] = { -1, -2, -3, -4, -5, -6, -7, -8, -9, -10, -11, -12 };
	//      검은색 블록      0,  1,  2,  3,  4,  5,  6,  7,  8,   9,  10,  11 };

	int i; // 반복문에 사용할 변수
	int nth_block; // 몇 번째 카드를 골랐는지 저장될 변수
	int x = 0, y = 0; // 좌표를 나타낼 변수
	char key; // 키보드 입력받을 변수

	change_color(BLOCK, DARK_GRAY);
	gotoxy(5, 9); printf("※ [%s]님, 블록을 하나 고르세요.", player1.name);
	gotoxy(5, 15); printf("※ [%s]님, 블록을 하나 고르세요.", player2.name);

	// 0~11까지의 값들을 섞어서 저장
	mix_blocks(white_block, 12);
	mix_blocks(black_block, 12);

	// 섞인 블록에 맞추어 값이 가려진 블록들을 생성
	for (i = 0; i < 12; i++) {
		make_a_hidden_block(white_block[i], 5 + x, 11);
		make_a_hidden_block(black_block[i], 5 + x, 17);
		x = x + 5;
	}

	x = 5, y = 10, nth_block = 0;
	while (1) {
		change_color(SKY_BLUE, DARK_GRAY);
		gotoxy(x, y);
		printf(" ▼");
		key = _getch();

		if (key == 75) { // ← Key
			nth_block--;
			x = x - 5;
			gotoxy(x + 5, y); puts("  ");
			if (x < 5) {
				nth_block = 11;
				x = 60;
			}
		}
		else if (key == 77) { // → Key
			nth_block++;
			x = x + 5;
			gotoxy(x - 5, y); puts("  ");
			if (x > 60) {
				nth_block = 0;
				x = 5;
			}
		}
		else if (key == 13) { // Enter
			gotoxy(x, y); puts("  ");
			player1.pick = white_block[nth_block];
			make_a_block(white_block[nth_block], 5 + nth_block * 5, 11);
			break;
		}
	}

	x = 5, y = 16, nth_block = 0;
	while (1) {
		change_color(SKY_BLUE, DARK_GRAY);
		gotoxy(x, y);
		printf(" ▼");
		key = _getch();

		if (key == 75) { // ← Key
			nth_block--;
			x = x - 5;
			gotoxy(x + 5, y); puts("  ");
			if (x < 5) {
				nth_block = 11;
				x = 60;
			}
		}
		else if (key == 77) { // → Key
			nth_block++;
			x = x + 5;
			gotoxy(x - 5, y); puts("  ");
			if (x > 60) {
				nth_block = 0;
				x = 5;
			}
		}
		else if (key == 13) { // Enter
			gotoxy(x, y); puts("  ");
			player2.pick = black_block[nth_block];
			make_a_block(black_block[nth_block], 5 + nth_block * 5, 17);
			break;
		}
	}

	change_color(SKY_BLUE, DARK_GRAY);
	// 플레이어1이 뽑은 블록의 값이 플레이어2가 뽑은 값보다 크다면?
	if (abs(player1.pick) > abs(player2.pick)) {
		gotoxy(5, 21); printf("▶ [%s]님이 고른 블록이 더 크므로, [%s]님이 먼저 시작합니다.", player1.name, player1.name);
		gotoxy(5, 22); printf("   시작하려면 Enter Key를 눌러 주세요 !");

		// 플레이어1의 차례가 먼저
		player1.turn = 1;
		player2.turn = 0;

		while (1) {
			key = _getch();
			if (key == 13) break;
		}
	}

	// 플레이어2가 뽑은 블록의 값이 플레이어1이 뽑은 값보다 크다면?
	else if (abs(player1.pick) < abs(player2.pick)) {
		gotoxy(5, 21); printf("▶ [%s]님이 고른 블록이 더 크므로, [%s]님이 먼저 시작합니다.", player2.name, player2.name);
		gotoxy(5, 22); printf("   시작하려면 Enter Key를 눌러 주세요 !");

		// 플레이어2의 차례가 먼저
		player1.turn = 0;
		player2.turn = 1;

		while (1) {
			key = _getch();
			if (key == 13) break;
		}
	}

	// 플레이어1이 뽑은 블록의 값이 플레이어2가 뽑은 값과 같다면?
	else if (abs(player1.pick) == abs(player2.pick)) {
		gotoxy(5, 21); printf("▶ [%s]님이 고른 블록과 [%s]님이 고른 블록의 값이 같습니다.", player1.name, player2.name);
		gotoxy(5, 22); printf("   Enter Key를 눌러 다시 선택해 주세요 !");

		while (1) {
			key = _getch();
			if (key == 13) return decide_the_turn(); // 다시 선택
		}
	}
}

// 기본 틀 보여주는 함수
void frame() {
	change_color(SKY_BLUE, DARK_GRAY);
	gotoxy(1, 2); printf("▶ %s", player1.name);
	gotoxy(1, 21); printf("▶ %s", player2.name);

	change_color(BLOCK, DARK_GRAY);
	gotoxy(14, 1);
	printf("┌─────────────────────────────────┐");
	for (int i = 2; i <= 8; i++) {
		gotoxy(14, i); 	printf("│                                                                  │");
	}
	gotoxy(14, 9);
	printf("└─────────────────────────────────┘");

	gotoxy(14, 10);
	printf("┌─────────────────────────────────┐");
	for (int i = 11; i <= 18; i++) {
		gotoxy(14, i); 	printf("│                                                                  │");
	}
	gotoxy(14, 19);
	printf("└─────────────────────────────────┘");

	gotoxy(14, 20);
	printf("┌─────────────────────────────────┐");
	for (int i = 21; i <= 27; i++) {
		gotoxy(14, i); 	printf("│                                                                  │");
	}
	gotoxy(14, 28);
	printf("└─────────────────────────────────┘");
}

// 섞인 블록들을 보여주는 함수
void show_mixed_blocks(int block_arr[]) {
	int x = 0;
	mix_blocks(game_blocks, 26);
	for (int i = 0; i < 13; i++) {
		make_a_hidden_block(block_arr[i], 17 + x, 12);
		make_a_hidden_block(block_arr[i + 13], 17 + x, 16);
		x = x + 5;
	}
}

// 플레이어들이 중앙의 블록을 선택하는 함수
int select_a_block() {
	char key;
	int x, y, nth_block;

	x = 17, y = 11, nth_block = 0;
	while (1) {
		change_color(SKY_BLUE, DARK_GRAY);
		gotoxy(x, y);
		printf(" ▼");
		key = _getch();

		if (key == 75) { // ← Key
			nth_block--;
			x = x - 5;
			gotoxy(x + 5, y); puts("  ");
			if (x < 17 && y == 11) {
				nth_block = 25;
				x = 77; y = 15;
			}
			else if (x < 17 && y == 15) {
				x = 77; y = 11;
			}
		}

		else if (key == 77) { // → Key
			nth_block++;
			x = x + 5;
			gotoxy(x - 5, y); puts("  ");
			if (x > 77 && y == 11) {
				x = 17; y = 15;
			}
			else if (x > 77 && y == 15) {
				nth_block = 0;
				x = 17; y = 11;
			}
		}

		if (key == 13) // Enter
			if (selected_blocks[nth_block] == 1) { // Enter 눌렀을 때 이미 선택된 블록이라면? 
				change_color(WHITE, DARK_RED);
				gotoxy(87, 18); printf("이미 선택된 블록입니다 !");
			}
			else {
				gotoxy(87, 18); printf("                        ");
				selected_blocks[nth_block] = 1;
				hide_a_block(nth_block);
				break;
			}
	}
	return nth_block; // 몇 번째 블록을 선택했는지 알려주기 위해 리턴
}

// 중간에 있는 블록들을 숨기는 함수
void make_the_center_clear() {
	int x, y;
	for (x = 17; x < 81; x++) {
		for (y = 12; y < 18; y++) {
			gotoxy(x, y); printf("  ");
		}
	}
}

// 플레이어들이 가진 블록들을 숨기는 함수
void make_the_players_clear() {
	change_color(DARK_GRAY, DARK_GRAY);
	int x, y;
	for (y = 2; y < 9; y++) {
		for (x = 17; x < 81; x++) {
			gotoxy(x, y); printf("  ");
		}
	}
	for (y = 21; y < 28; y++) {
		for (x = 17; x < 81; x++) {
			gotoxy(x, y); printf("  ");
		}
	}
}

// 선택된 블록을 숨기기 위한 함수
void hide_a_block(int nth_block) {
	int x;

	change_color(DARK_GRAY, DARK_GRAY);
	if (nth_block <= 12) {
		x = 17 + nth_block * 5;
		gotoxy(x, 11); printf("    ");
		gotoxy(x, 12); printf("    ");
		gotoxy(x, 13); printf("    ");
		gotoxy(x, 14); printf(" ━ ");
	}
	else if (nth_block >= 13) {
		x = 17 + (nth_block - 13) * 5;
		gotoxy(x, 15); printf("    ");
		gotoxy(x, 16); printf("    ");
		gotoxy(x, 17); printf("    ");
		gotoxy(x, 18); printf(" ━ ");
	}
}

// 조커를 선택했을 때 나타나는 함수
void select_the_joker() {
	system("cls");
	frame();
	change_color(SKY_BLUE, DARK_GRAY);
	gotoxy(36, 14); printf("당신은 조커를 선택했습니다 !");
	gotoxy(40, 15); printf("다시 섞는 중입니다 …");
	Sleep(2000);
}

// 플레이어들이 처음에 4개의 블록을 선택하는 함수
int select_4blocks() {
	make_the_center_clear();
	show_mixed_blocks(game_blocks);

	change_color(BLOCK, DARK_GRAY);
	gotoxy(84, 13); printf("4개의 블록을 가져갈 수 있습니다.");
	gotoxy(84, 14); printf("←, → Key를 이용하여 이동하고,");
	gotoxy(84, 15); printf("Enter Key로 선택하세요.");
	gotoxy(84, 16); printf("조커를 고르면 다시 시작합니다.");

	for (int i = 0; i < 26; i++) selected_blocks[i] = 0;
	for (int i = 0; i <= 3; i++) {
		player1.block[i] = 0;
		player2.block[i] = 0;
	}

	int nth_block;
	if (player1.turn == 1 && player2.turn == 0) {
		for (int i = 0; i <= 3; i++) {
			change_color(WHITE, DARK_RED);
			gotoxy(2, 4); printf("Your turn!");
			change_color(DARK_GRAY, DARK_GRAY);
			gotoxy(2, 23); printf("          ");

			nth_block = select_a_block();
			make_a_block(game_blocks[nth_block], 17 + i * 5, 3);
			player1.block[i] = game_blocks[nth_block];
			if (abs(game_blocks[nth_block]) == 13) {
				select_the_joker();
				return select_4blocks();
			}
		}
		for (int i = 0; i <= 3; i++) {
			for (int j = 0; j < 4; j++) make_a_hidden_block(player1.block[j], 17 + j * 5, 3);
			change_color(WHITE, DARK_RED);
			gotoxy(2, 23); printf("Your turn!");
			change_color(DARK_GRAY, DARK_GRAY);
			gotoxy(2, 4); printf("          ");

			nth_block = select_a_block();
			make_a_block(game_blocks[nth_block], 17 + i * 5, 22);
			player2.block[i] = game_blocks[nth_block];
			if (abs(game_blocks[nth_block]) == 13) {
				select_the_joker();
				return select_4blocks();
			}
		}
	}
	if (player1.turn == 0 && player2.turn == 1) {
		for (int i = 0; i <= 3; i++) {
			change_color(WHITE, DARK_RED);
			gotoxy(2, 23); printf("Your turn!");
			change_color(DARK_GRAY, DARK_GRAY);
			gotoxy(2, 4); printf("          ");

			nth_block = select_a_block();
			make_a_block(game_blocks[nth_block], 17 + i * 5, 22);
			player2.block[i] = game_blocks[nth_block];
			if (abs(game_blocks[nth_block]) == 13) {
				select_the_joker();
				return select_4blocks();
			}
		}
		for (int i = 0; i <= 3; i++) {
			for (int j = 0; j < 4; j++) make_a_hidden_block(player2.block[j], 17 + j * 5, 22);
			change_color(WHITE, DARK_RED);
			gotoxy(2, 4); printf("Your turn!");
			change_color(DARK_GRAY, DARK_GRAY);
			gotoxy(2, 23); printf("          ");

			nth_block = select_a_block();
			make_a_block(game_blocks[nth_block], 17 + i * 5, 3);
			player1.block[i] = game_blocks[nth_block];
			if (abs(game_blocks[nth_block]) == 13) {
				select_the_joker();
				return select_4blocks();
			}
		}
	}
}

// 누구의 차례인지 보여주는 함수
void show_the_turn() {
	if (player1.turn == 1 && player2.turn == 0) {
		change_color(WHITE, DARK_RED);
		gotoxy(2, 4); printf("Your turn!");
		change_color(DARK_GRAY, DARK_GRAY);
		gotoxy(2, 23); printf("          ");
	}
	else if (player1.turn == 0 && player2.turn == 1) {
		change_color(WHITE, DARK_RED);
		gotoxy(2, 23); printf("Your turn!");
		change_color(DARK_GRAY, DARK_GRAY);
		gotoxy(2, 4); printf("            ");
	}
}

// 차례를 바꾸는 함수
void change_the_turn() {
	if (player1.turn == 1 && player2.turn == 0) {
		player1.turn = 0;
		player2.turn = 1;
	}
	else if (player1.turn == 0 && player2.turn == 1) {
		player1.turn = 1;
		player2.turn = 0;
	}
}

// 플레이어들의 블록 상태를 보여주는 함수
void show_blocks(int cnt1, int cnt2) {
	int i;
	make_the_players_clear();

	// 플레이어1의 블록은 보이고 플레이어2의 블록은 안 보이게
	if (player1.turn == 1 && player2.turn == 0) {
		for (i = 0; i < cnt1; i++) {
			if (player1.opened_block[i] == 0) make_a_block(player1.block[i], 17 + i * 5, 3);
			else if (player1.opened_block[i] == 1) make_a_block(player1.block[i], 17 + i * 5, 6);
		}
		for (i = 0; i < cnt2; i++) {
			if (player2.opened_block[i] == 0) make_a_hidden_block(player2.block[i], 17 + i * 5, 22);
			else if (player2.opened_block[i] == 1) make_a_block(player2.block[i], 17 + i * 5, 25);
		}
	}

	// 플레이어2의 블록은 보이고 플레이어1의 블록은 안 보이게
	else if (player1.turn == 0 && player2.turn == 1) {
		for (i = 0; i < cnt1; i++) {
			if (player1.opened_block[i] == 0) make_a_hidden_block(player1.block[i], 17 + i * 5, 3);
			else if (player1.opened_block[i] == 1) make_a_block(player1.block[i], 17 + i * 5, 6);
		}
		for (i = 0; i < cnt2; i++) {
			if (player2.opened_block[i] == 0) make_a_block(player2.block[i], 17 + i * 5, 22);
			else if (player2.opened_block[i] == 1) make_a_block(player2.block[i], 17 + i * 5, 25);
		}
	}
}

// 상대방의 블록을 맞췄을 때 메뉴 선택
void select_the_turn(int my_block, int nth_block) {
	char key;
	int y = 14;
	int cnt1 = 0, cnt2 = 0;

	who_is_win(); // 누가 이겼는지 계속 검사

	while (player1.block[cnt1] != 0) cnt1++;
	while (player2.block[cnt2] != 0) cnt2++;
	show_blocks(cnt1, cnt2);

	change_color(YELLOW, DARK_GRAY);
	// 내가 뽑은 카드 보여주기
	if (player1.turn == 1 && player2.turn == 0) {
		if (my_block != -1) gotoxy(17 + 5 * my_block, 6); printf(" ★");
	}
	else if (player1.turn == 0 && player2.turn == 1) {
		if (my_block != -1) gotoxy(17 + 5 * my_block, 25); printf(" ★");
	}

	change_color(SKY_BLUE, DARK_GRAY);
	gotoxy(87, 14); printf("상대방의 블록 더 맞추기            ");
	gotoxy(87, 15); printf("차례 넘기기                        ");
	gotoxy(84, 14); puts("  ");
	gotoxy(84, 15); puts("  ");

	while (1) {
		change_color(SKY_BLUE, DARK_GRAY);
		gotoxy(84, y); printf("☞");
		key = _getch(); // 키 입력 받음.

		if (key == 72) { // ↑ Key
			y = y - 1;
			gotoxy(84, y + 1); puts("  ");
			if (y < 14) y = 15; // 맨 위에서 ↑ Key를 누르면 맨 아래로
		}
		if (key == 80) { // ↓ Key
			y = y + 1;
			gotoxy(84, y - 1); puts("  ");
			if (y > 15) y = 14; // 맨 아래에서 ↓ Key를 누르면 맨 위로
		}
		if (key == 13) break;
	}

	if (y == 14) return guess_a_block(my_block);
	else if (y == 15) change_the_turn();
}

// 상대방의 블록을 추측하는 함수
int guess_a_block(int my_block) {

	change_color(BLOCK, DARK_GRAY);
	gotoxy(84, 13); printf("맞추려는 상대방의 블록을 고르세요.   ");
	gotoxy(84, 14); printf("←, → Key를 이용하여 이동하고,      ");
	gotoxy(84, 15); printf("Enter Key로 선택하세요.              ");

	char key;
	int x, y, nth_block = 0, cnt = 0;
	int answer;

	if (player1.turn == 1 && player2.turn == 0) {
		while (player2.block[cnt + 1] != 0) cnt++;
		y = 21;
	}
	else if (player1.turn == 0 && player2.turn == 1) {
		while (player1.block[cnt + 1] != 0) cnt++;
		y = 2;
	}

	x = 17;
	while (1) {
		change_color(SKY_BLUE, DARK_GRAY);
		gotoxy(x, y);
		printf(" ▼");
		key = _getch();

		if (key == 75) { // ← Key
			nth_block--;
			x = x - 5;
			gotoxy(x + 5, y); puts("  ");
			if (x < 17) {
				nth_block = cnt;
				x = 17 + 5 * cnt;
			}
		}

		else if (key == 77) { // → Key
			nth_block++;
			x = x + 5;
			gotoxy(x - 5, y); puts("  ");
			if (x > 17 + 5 * cnt) {
				nth_block = 0;
				x = 17;
			}
		}

		if (key == 13) { // Enter
			if (player2.turn == 1 && player1.opened_block[nth_block] == 1) {
				change_color(WHITE, DARK_RED);
				gotoxy(87, 18); printf("이미 선택된 블록입니다 !");
			}
			else if (player1.turn == 1 && player2.opened_block[nth_block] == 1) {
				change_color(WHITE, DARK_RED);
				gotoxy(87, 18); printf("이미 선택된 블록입니다 !");
			}
			else {
				gotoxy(x, y); puts("  ");
				break;
			}

		}
	}

	gotoxy(84, 16); printf("▶ 추측하는 블록의 값은?       ");
	gotoxy(84, 17); printf("(조커라면 12 입력)             ");
	gotoxy(84, 18); printf("                               ");
	gotoxy(110, 16); printf("   ");
	gotoxy(110, 16); scanf("%d", &answer);

	if (player1.turn == 1 && player2.turn == 0) {
		if (answer == abs(player2.block[nth_block]) - 1) {
			player2.opened_block[nth_block] = 1;
			gotoxy(84, 13); printf("                                   ");
			gotoxy(84, 14); printf("                                   ");
			gotoxy(84, 15); printf("                                   ");
			gotoxy(84, 16); printf("                                   ");
			gotoxy(84, 17); printf("                                   ");
			gotoxy(84, 14); printf("맞았습니다 !                       ");
			gotoxy(84, 15); printf("상대방의 블록이 공개되었습니다.    ");
			Sleep(2000);
			select_the_turn(my_block, nth_block);
		}
		else {
			player1.opened_block[nth_block] = 1;
			gotoxy(84, 13); printf("                                   ");
			gotoxy(84, 14); printf("                                   ");
			gotoxy(84, 15); printf("                                   ");
			gotoxy(84, 16); printf("                                   ");
			gotoxy(84, 17); printf("                                   ");
			gotoxy(84, 14); printf("틀렸습니다 !                       ");
			gotoxy(84, 15); printf("당신이 고른 블록이 공개되었습니다. ");
			Sleep(2000);
			change_the_turn();
		}
	}
	else if (player1.turn == 0 && player2.turn == 1) {
		if (answer == abs(player1.block[nth_block]) - 1) {
			if (my_block != -1) player1.opened_block[nth_block] = 1;
			gotoxy(84, 13); printf("                                   ");
			gotoxy(84, 14); printf("                                   ");
			gotoxy(84, 15); printf("                                   ");
			gotoxy(84, 16); printf("                                   ");
			gotoxy(84, 17); printf("                                   ");
			gotoxy(84, 14); printf("맞았습니다 !                       ");
			gotoxy(84, 15); printf("상대방의 블록이 공개되었습니다.    ");
			select_the_turn(my_block, nth_block);
		}
		else {
			if (my_block != -1) player2.opened_block[my_block] = 1;
			gotoxy(84, 13); printf("                                   ");
			gotoxy(84, 14); printf("                                   ");
			gotoxy(84, 15); printf("                                   ");
			gotoxy(84, 16); printf("                                   ");
			gotoxy(84, 17); printf("                                   ");
			gotoxy(84, 14); printf("틀렸습니다 !                       ");
			gotoxy(84, 15); printf("당신이 고른 블록이 공개되었습니다. ");
			Sleep(2000);
			change_the_turn();
		}
	}

	gotoxy(84, 16); printf("                        ");
	gotoxy(84, 17); printf("                  ");
	gotoxy(x, y); puts("  ");
}

// 조커의 위치 리턴하는 함수
int location_of_joker() {
	char key;
	int x, y, nth_block = 0;
	int cnt = 0;

	if (player1.turn == 1 && player2.turn == 0) {
		while (player1.block[cnt] != 0) cnt++;
		y = 2;
	}
	else if (player1.turn == 0 && player2.turn == 1) {
		while (player2.block[cnt] != 0) cnt++;
		y = 21;
	}

	x = 16;
	while (1) {
		change_color(SKY_BLUE, DARK_GRAY);
		gotoxy(x, y);
		printf("▼");
		key = _getch();

		if (key == 75) { // ← Key
			nth_block--;
			x = x - 5;
			gotoxy(x + 5, y); puts("  ");
			if (x < 16) {
				nth_block = cnt;
				x = 16 + 5 * cnt;
			}
		}

		else if (key == 77) { // → Key
			nth_block++;
			x = x + 5;
			gotoxy(x - 5, y); puts("  ");
			if (x > 16 + 5 * cnt) {
				nth_block = 0;
				x = 16;
			}
		}

		if (key == 13) { // Enter
			gotoxy(x, y); puts("  ");
			break;
		}
	}
	return nth_block; // 몇 번째 블록을 선택했는지 알려주기 위해 리턴
}

// 계속 반복하는 게임 진행 함수
void one_turn() {
	int i, nth_block, temp, joker, check = 1;
	int cnt1 = 0, cnt2 = 0;
	int my_block_idx = 0;

	// 플레이어들이 가지고 있는 블록 수
	while (player1.block[cnt1] != 0) cnt1++;
	while (player2.block[cnt2] != 0) cnt2++;
	show_blocks(cnt1, cnt2);

	// 순서 표시
	show_the_turn();

	// 블록이 다 공개됐다면 블록을 뽑지 않음.
	for (i = 0; i < 26; i++) {
		if (selected_blocks[i] == 0) check = 0;
	}

	if (check == 0) {
		// 블록 뽑기
		change_color(BLOCK, DARK_GRAY);
		gotoxy(84, 13); printf("가져오고 싶은 블록 하나를 고르세요.");
		gotoxy(84, 14); printf("←, → Key를 이용하여 이동하고,    ");
		gotoxy(84, 15); printf("Enter Key로 선택하세요.            ");
		nth_block = select_a_block();

		// 조커가 아니라면 블록을 플레이어 배열에 순서대로 넣음
		if (abs(game_blocks[nth_block]) != 13) {
			if (player1.turn == 1 && player2.turn == 0) {
				player1.block[cnt1] = game_blocks[nth_block];
				cnt1++;
			}
			else if (player1.turn == 0 && player2.turn == 1) {
				player2.block[cnt2] = game_blocks[nth_block];
				cnt2++;
			}
		}

		// 조커라면?
		else if (abs(game_blocks[nth_block]) == 13) {
			change_color(BLOCK, DARK_GRAY);
			gotoxy(84, 13); printf("조커를 넣고 싶은 위치를 선택하세요.");
			if (player1.turn == 1 && player2.turn == 0) {
				joker = location_of_joker();
				for (i = cnt1; i > joker; i--) {
					player1.block[i] = player1.block[i - 1];
					player1.opened_block[i] = player1.opened_block[i - 1];
				}
				player1.block[joker] = game_blocks[nth_block];
				player1.opened_block[joker] = 0;
				cnt1++;
			}
			else if (player1.turn == 0 && player2.turn == 1) {
				joker = location_of_joker();
				for (i = cnt2; i > joker; i--) {
					player2.block[i] = player2.block[i - 1];
					player2.opened_block[i] = player2.opened_block[i - 1];
				}
				player2.block[joker] = game_blocks[nth_block];
				player2.opened_block[joker] = 0;
				cnt2++;
			}
		}

		// 규칙에 맞게 정렬
		player1.opened_block_on = 1;
		player2.opened_block_on = 0;
		sorting_blocks(player1.block, cnt1);
		player1.opened_block_on = 0;
		player2.opened_block_on = 1;
		sorting_blocks(player2.block, cnt2);
		player1.opened_block_on = 0;
		player2.opened_block_on = 0;


		// 순서에 해당하는 사람의 블록은 보이게 
		// 순서에 해당하지 않는 사람의 블록은 가려서 (맞춘 건 계속 보이게)
		show_blocks(cnt1, cnt2);

		// 내가 뽑은 카드 보여주기
		if (player1.turn == 1 && player2.turn == 0) {
			while (player1.block[my_block_idx] != game_blocks[nth_block]) my_block_idx++;
			change_color(YELLOW, DARK_GRAY);
			gotoxy(17 + 5 * my_block_idx, 6); printf(" ★");
		}
		else if (player1.turn == 0 && player2.turn == 1) {
			while (player2.block[my_block_idx] != game_blocks[nth_block]) my_block_idx++;
			change_color(YELLOW, DARK_GRAY);
			gotoxy(17 + 5 * my_block_idx, 25); printf(" ★");
		}
	}

	if (check == 1) my_block_idx = -1;
	// 상대방의 블록 고르고 값 맞추기
	// 맞으면 상대방의 블록 공개
	// 틀리면 내가 고른 카드 공개
	guess_a_block(my_block_idx);

	if (check != -1) gotoxy(17 + 5 * my_block_idx, 2); printf("   ");
	if (check != -1) gotoxy(17 + 5 * my_block_idx, 21); printf("   ");
	show_blocks(cnt1, cnt2);
}

// 누가 이겼는지 체크하는 함수
void who_is_win() {
	int i, cnt1 = 0, cnt2 = 0;
	int check1 = 1, check2 = 1;

	// 플레이어들이 가지고 있는 블록 수
	while (player1.block[cnt1] != 0) cnt1++;
	while (player2.block[cnt2] != 0) cnt2++;

	for (i = 0; i < cnt1; i++) {
		if (player1.opened_block[i] == 0) check1 = 0;
	}

	for (i = 0; i < cnt2; i++) {
		if (player2.opened_block[i] == 0) check2 = 0;
	}

	if (check1 == 1) {
		system("cls");
		change_color(SKY_BLUE, DARK_GRAY);
		gotoxy(43, 13); printf("[%s]의 패가 모두 공개되었습니다 !", player1.name);
		gotoxy(48, 14); printf("[%s]가 승리하였습니다.", player2.name);
		Sleep(10000);
		exit(0);
	}
	else if (check2 == 1) {
		system("cls");
		change_color(SKY_BLUE, DARK_GRAY);
		gotoxy(43, 13); printf("[%s]의 패가 모두 공개되었습니다 !", player2.name);
		gotoxy(48, 14); printf("[%s]가 승리하였습니다.", player1.name);
		Sleep(10000);
		exit(0);
	}
}

/* ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■ */

void quit_game() {
	system("cls");
	exit(0);
}

/* ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■ */

int main() {
	change_color(BLOCK, DARK_GRAY);
	system("cls");

	srand((unsigned)time(NULL));
	delete_cursor();

	int menu = init_screen();
	if (menu == 17) {
		start_game();
		Sleep(5000);
	}
	else if (menu == 21) {
		quit_game();
	}
}
