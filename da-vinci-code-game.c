// C��� �ٺ�ġ �ڵ�
// �ܼ�â ��� DARK_GRAY, ���Ž� �ܼ� ���

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>

typedef struct PLAYER {
	char name[10]; // �÷��̾��� �̸��� �Է¹޴� �迭
	int block[13]; // �÷��̾ �� ���� ��� �迭
	int opened_block[13]; // �÷��̾ ������ ���� ��� �迭
	int turn; // �÷��̾��� ���ʸ� �˷��ִ� �迭 (1�̸� �÷��̾��� ����, 0�̸� ����� ����)
	int pick; // �÷��̾ �� ����� ���� ��� ����
	int opened_block_on;
} PLAYER;

PLAYER player1;
PLAYER player2;

// �÷��̾���� �� (ó���� �߾ӿ� ����) ��� �迭
int game_blocks[26] = { 1,  2,  3,  4,  5,  6,  7,  8,  9,  10,  11,  12,  13,
//        ��� ���     0,  1,  2,  3,  4,  5,  6,  7,  8,   9,  10,  11    - 
					   -1, -2, -3, -4, -5, -6, -7, -8, -9, -10, -11, -12, -13 }; // index : 0~25
//      ������ ���     0,  1,  2,  3,  4,  5,  6,  7,  8,   9,  10,  11    - 

// ����� ���õǸ� 1�� ǥ���� �迭 (�ߺ� ���� ����)
int selected_blocks[26] = { 0, };

/* �������������������������������������������������������������������� */

// �� ���
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
}; // �� ���

// �ؽ�Ʈ�� ���� �����ϴ� �Լ�
void change_color(int text, int background) {
	int color = text + background * 16;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// �ܼ�â�� ��ǥ�� ��Ÿ���� �Լ�
void gotoxy(int x, int y) {
	COORD pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

// Ŀ�� ���ִ� �Լ�
void delete_cursor() {
	CONSOLE_CURSOR_INFO CurInfo;
	CurInfo.dwSize = 1;
	CurInfo.bVisible = FALSE;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &CurInfo);
}

/* �������������������������������������������������������������������� */

// �Լ� �����ϴ� �κ�
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

/* �������������������������������������������������������������������� */

// �޴� ��Ÿ���� �ʱ� ȭ�� �Լ�
int init_screen() {
	char key;
	int y = 17;

	change_color(BLOCK, DARK_GRAY);
	gotoxy(15, 5);
	printf("��������������������������������������������");
	gotoxy(15, 7);
	printf("���    ���     ��  �� ���� ��    ��   ��� ����      ���   ���   ���   ����");
	gotoxy(15, 8);
	printf("��  �� ��  ��    ��  ��   ��   ���  �� ��       ��      ��     ��    �� ��  �� ��");
	gotoxy(15, 9);
	printf("��  �� ����    ��  ��   ��   �� �� �� ��       ��      ��     ��    �� ��  �� ����");
	gotoxy(15, 10);
	printf("��  �� ��  ��     ���    ��   ��  ��� ��       ��      ��     ��    �� ��  �� ��");
	gotoxy(15, 11);
	printf("���   ��  ��      ��   ���� ��    ��   ��� ����      ���   ���   ���   ����");
	gotoxy(15, 13);
	printf("��������������������������������������������");

	gotoxy(53, 17); printf(" ���ӽ���");
	gotoxy(53, 21); printf(" ��������");

	while (1) {
		change_color(SKY_BLUE, DARK_GRAY);
		gotoxy(50, y); printf("��");
		key = _getch(); // Ű �Է� ����.

		if (key == 72) { // �� Key
			y = y - 4;
			gotoxy(50, y + 4); puts("  ");
			if (y < 17) y = 21; // �� ������ �� Key�� ������ �� �Ʒ���
		}
		if (key == 80) { // �� Key
			y = y + 4;
			gotoxy(50, y - 4); puts("  ");
			if (y > 21) y = 17; // �� �Ʒ����� �� Key�� ������ �� ����
		}
		if (key == 13) return y; // Enter Key ������ �����ϴ� �޴��� �ִ� y ��ǥ�� return
	}
}

/* �������������������������������������������������������������������� */

void start_game() {
	system("cls");
	player_name();

	system("cls");
	decide_the_turn();

	system("cls");
	frame();
	select_4blocks();

	// ���̴� ��ó�� ���̰� ����
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

	// ��Ģ��� ����� ������.
	sorting_blocks(player1.block, 4);
	sorting_blocks(player2.block, 4);

	while (1) {
		one_turn();
	}
}

// �÷��̾���� �̸��� �Է¹޴� �Լ�
void player_name() {
	change_color(BLOCK, DARK_GRAY);
	gotoxy(5, 9); printf("�� �÷��̾�1�� �̸��� �Է����ּ���. (5���� �̳��� ����, �Է� �� Enter Key)");
	gotoxy(5, 15); printf("�� �÷��̾�2�� �̸��� �Է����ּ���. (5���� �̳��� ����, �Է� �� Enter Key)");

	change_color(SKY_BLUE, DARK_GRAY);
	gotoxy(5, 10); printf("��");
	gotoxy(5, 16); printf("��");
	gotoxy(8, 10); scanf("%s", player1.name);
	gotoxy(8, 16); scanf("%s", player2.name);
}

// num���� ����� �������� ���� �Լ�
void mix_blocks(int block_arr[], int num) {
	int temp, A, B, i = 0;
	for (i = 0; i <= 100; i++) { // ���� �����Ͽ� 100�� ���..
		A = rand() % num;
		B = rand() % num;
		temp = block_arr[A];
		block_arr[A] = block_arr[B];
		block_arr[B] = temp;
	}
}

// ��Ģ�� ���� ��� �����ϴ� �Լ� (���� �� OR ���� ����� ����)
void sorting_blocks(int arr[], int num) {
	int temp;

	for (int i = 0; i < num; i++) {
		for (int j = 0; j < num - 1; j++) { // ���ڿ� ���� ����
			if (abs(arr[i]) == 13 || abs(arr[j]) == 13); // ��Ŀ�� �������� ����.
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

			else if (abs(arr[j]) == abs(arr[j + 1])) { // ��� ���� ���� ����
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

// �־��� ���ڿ� �°� ��ǥ x, y�� ��� �����ϴ� �Լ�
void make_a_block(int num, int x, int y) {
	if (num > 0) change_color(BLOCK, WHITE); // ����� ��� �������
	else change_color(WHITE, BLOCK); // ������ ������ �������

	gotoxy(x, y); printf("    ");
	gotoxy(x, y + 1);
	switch (abs(num)) { // ���ڸ� �״�� ���� ĭ�� ���� �ʾ� Ư�� ����(��+����)�� ���
	case 1:
		printf(" �� ");
		break;
	case 2:
		printf(" �� ");
		break;
	case 3:
		printf(" �� ");
		break;
	case 4:
		printf(" �� ");
		break;
	case 5:
		printf(" �� ");
		break;
	case 6:
		printf(" �� ");
		break;
	case 7:
		printf(" �� ");
		break;
	case 8:
		printf(" �� ");
		break;
	case 9:
		printf(" �� ");
		break;
	case 10:
		printf(" �� ");
		break;
	case 11:
		printf(" 10 ");
		break;
	case 12:
		printf(" 11 ");
		break;
	case 13:
		printf(" �� ");
		break;
	}
	gotoxy(x, y + 2); printf(" �� ");
	change_color(DARK_GRAY, DARK_GRAY);
	x = x + 4;
	gotoxy(x, y); printf(" ");
	gotoxy(x, y + 1); printf(" ");
	gotoxy(x, y + 2); printf(" ");
}

// �־��� ���ڿ� �°� ��ǥ x, y�� ���� ������ ��� �����ϴ� �Լ�
void make_a_hidden_block(int num, int x, int y) {
	if (abs(num) == 13) change_color(WHITE, DARK_RED);
	// �� �־��� �ð��� ª���Ƿ� Ȯ���� ��Ȱ�ϰ� �ϱ� ���� ���� ������ ǥ���Ͽ���.

	else if (num > 0) change_color(BLOCK, WHITE); // ��� ���
	else change_color(WHITE, BLOCK); // ������ ���
	gotoxy(x, y); printf("    ");
	gotoxy(x, y + 1); printf("    ");
	gotoxy(x, y + 2); printf("    ");
	change_color(DARK_GRAY, DARK_GRAY);
	x = x + 4;
	gotoxy(x, y); printf(" ");
	gotoxy(x, y + 1); printf(" ");
	gotoxy(x, y + 2); printf(" ");
}

// ����� ��� �÷��̾��� ���� ���ϴ� �Լ�
int decide_the_turn() {
	int white_block[12] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
	//        ��� ���     0, 1, 2, 3, 4, 5, 6, 7, 8,  9, 10, 11 };
	int black_block[12] = { -1, -2, -3, -4, -5, -6, -7, -8, -9, -10, -11, -12 };
	//      ������ ���      0,  1,  2,  3,  4,  5,  6,  7,  8,   9,  10,  11 };

	int i; // �ݺ����� ����� ����
	int nth_block; // �� ��° ī�带 ������� ����� ����
	int x = 0, y = 0; // ��ǥ�� ��Ÿ�� ����
	char key; // Ű���� �Է¹��� ����

	change_color(BLOCK, DARK_GRAY);
	gotoxy(5, 9); printf("�� [%s]��, ����� �ϳ� ������.", player1.name);
	gotoxy(5, 15); printf("�� [%s]��, ����� �ϳ� ������.", player2.name);

	// 0~11������ ������ ��� ����
	mix_blocks(white_block, 12);
	mix_blocks(black_block, 12);

	// ���� ��Ͽ� ���߾� ���� ������ ��ϵ��� ����
	for (i = 0; i < 12; i++) {
		make_a_hidden_block(white_block[i], 5 + x, 11);
		make_a_hidden_block(black_block[i], 5 + x, 17);
		x = x + 5;
	}

	x = 5, y = 10, nth_block = 0;
	while (1) {
		change_color(SKY_BLUE, DARK_GRAY);
		gotoxy(x, y);
		printf(" ��");
		key = _getch();

		if (key == 75) { // �� Key
			nth_block--;
			x = x - 5;
			gotoxy(x + 5, y); puts("  ");
			if (x < 5) {
				nth_block = 11;
				x = 60;
			}
		}
		else if (key == 77) { // �� Key
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
		printf(" ��");
		key = _getch();

		if (key == 75) { // �� Key
			nth_block--;
			x = x - 5;
			gotoxy(x + 5, y); puts("  ");
			if (x < 5) {
				nth_block = 11;
				x = 60;
			}
		}
		else if (key == 77) { // �� Key
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
	// �÷��̾�1�� ���� ����� ���� �÷��̾�2�� ���� ������ ũ�ٸ�?
	if (abs(player1.pick) > abs(player2.pick)) {
		gotoxy(5, 21); printf("�� [%s]���� �� ����� �� ũ�Ƿ�, [%s]���� ���� �����մϴ�.", player1.name, player1.name);
		gotoxy(5, 22); printf("   �����Ϸ��� Enter Key�� ���� �ּ��� !");

		// �÷��̾�1�� ���ʰ� ����
		player1.turn = 1;
		player2.turn = 0;

		while (1) {
			key = _getch();
			if (key == 13) break;
		}
	}

	// �÷��̾�2�� ���� ����� ���� �÷��̾�1�� ���� ������ ũ�ٸ�?
	else if (abs(player1.pick) < abs(player2.pick)) {
		gotoxy(5, 21); printf("�� [%s]���� �� ����� �� ũ�Ƿ�, [%s]���� ���� �����մϴ�.", player2.name, player2.name);
		gotoxy(5, 22); printf("   �����Ϸ��� Enter Key�� ���� �ּ��� !");

		// �÷��̾�2�� ���ʰ� ����
		player1.turn = 0;
		player2.turn = 1;

		while (1) {
			key = _getch();
			if (key == 13) break;
		}
	}

	// �÷��̾�1�� ���� ����� ���� �÷��̾�2�� ���� ���� ���ٸ�?
	else if (abs(player1.pick) == abs(player2.pick)) {
		gotoxy(5, 21); printf("�� [%s]���� �� ��ϰ� [%s]���� �� ����� ���� �����ϴ�.", player1.name, player2.name);
		gotoxy(5, 22); printf("   Enter Key�� ���� �ٽ� ������ �ּ��� !");

		while (1) {
			key = _getch();
			if (key == 13) return decide_the_turn(); // �ٽ� ����
		}
	}
}

// �⺻ Ʋ �����ִ� �Լ�
void frame() {
	change_color(SKY_BLUE, DARK_GRAY);
	gotoxy(1, 2); printf("�� %s", player1.name);
	gotoxy(1, 21); printf("�� %s", player2.name);

	change_color(BLOCK, DARK_GRAY);
	gotoxy(14, 1);
	printf("����������������������������������������������������������������������");
	for (int i = 2; i <= 8; i++) {
		gotoxy(14, i); 	printf("��                                                                  ��");
	}
	gotoxy(14, 9);
	printf("����������������������������������������������������������������������");

	gotoxy(14, 10);
	printf("����������������������������������������������������������������������");
	for (int i = 11; i <= 18; i++) {
		gotoxy(14, i); 	printf("��                                                                  ��");
	}
	gotoxy(14, 19);
	printf("����������������������������������������������������������������������");

	gotoxy(14, 20);
	printf("����������������������������������������������������������������������");
	for (int i = 21; i <= 27; i++) {
		gotoxy(14, i); 	printf("��                                                                  ��");
	}
	gotoxy(14, 28);
	printf("����������������������������������������������������������������������");
}

// ���� ��ϵ��� �����ִ� �Լ�
void show_mixed_blocks(int block_arr[]) {
	int x = 0;
	mix_blocks(game_blocks, 26);
	for (int i = 0; i < 13; i++) {
		make_a_hidden_block(block_arr[i], 17 + x, 12);
		make_a_hidden_block(block_arr[i + 13], 17 + x, 16);
		x = x + 5;
	}
}

// �÷��̾���� �߾��� ����� �����ϴ� �Լ�
int select_a_block() {
	char key;
	int x, y, nth_block;

	x = 17, y = 11, nth_block = 0;
	while (1) {
		change_color(SKY_BLUE, DARK_GRAY);
		gotoxy(x, y);
		printf(" ��");
		key = _getch();

		if (key == 75) { // �� Key
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

		else if (key == 77) { // �� Key
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
			if (selected_blocks[nth_block] == 1) { // Enter ������ �� �̹� ���õ� ����̶��? 
				change_color(WHITE, DARK_RED);
				gotoxy(87, 18); printf("�̹� ���õ� ����Դϴ� !");
			}
			else {
				gotoxy(87, 18); printf("                        ");
				selected_blocks[nth_block] = 1;
				hide_a_block(nth_block);
				break;
			}
	}
	return nth_block; // �� ��° ����� �����ߴ��� �˷��ֱ� ���� ����
}

// �߰��� �ִ� ��ϵ��� ����� �Լ�
void make_the_center_clear() {
	int x, y;
	for (x = 17; x < 81; x++) {
		for (y = 12; y < 18; y++) {
			gotoxy(x, y); printf("  ");
		}
	}
}

// �÷��̾���� ���� ��ϵ��� ����� �Լ�
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

// ���õ� ����� ����� ���� �Լ�
void hide_a_block(int nth_block) {
	int x;

	change_color(DARK_GRAY, DARK_GRAY);
	if (nth_block <= 12) {
		x = 17 + nth_block * 5;
		gotoxy(x, 11); printf("    ");
		gotoxy(x, 12); printf("    ");
		gotoxy(x, 13); printf("    ");
		gotoxy(x, 14); printf(" �� ");
	}
	else if (nth_block >= 13) {
		x = 17 + (nth_block - 13) * 5;
		gotoxy(x, 15); printf("    ");
		gotoxy(x, 16); printf("    ");
		gotoxy(x, 17); printf("    ");
		gotoxy(x, 18); printf(" �� ");
	}
}

// ��Ŀ�� �������� �� ��Ÿ���� �Լ�
void select_the_joker() {
	system("cls");
	frame();
	change_color(SKY_BLUE, DARK_GRAY);
	gotoxy(36, 14); printf("����� ��Ŀ�� �����߽��ϴ� !");
	gotoxy(40, 15); printf("�ٽ� ���� ���Դϴ� ��");
	Sleep(2000);
}

// �÷��̾���� ó���� 4���� ����� �����ϴ� �Լ�
int select_4blocks() {
	make_the_center_clear();
	show_mixed_blocks(game_blocks);

	change_color(BLOCK, DARK_GRAY);
	gotoxy(84, 13); printf("4���� ����� ������ �� �ֽ��ϴ�.");
	gotoxy(84, 14); printf("��, �� Key�� �̿��Ͽ� �̵��ϰ�,");
	gotoxy(84, 15); printf("Enter Key�� �����ϼ���.");
	gotoxy(84, 16); printf("��Ŀ�� ���� �ٽ� �����մϴ�.");

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

// ������ �������� �����ִ� �Լ�
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

// ���ʸ� �ٲٴ� �Լ�
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

// �÷��̾���� ��� ���¸� �����ִ� �Լ�
void show_blocks(int cnt1, int cnt2) {
	int i;
	make_the_players_clear();

	// �÷��̾�1�� ����� ���̰� �÷��̾�2�� ����� �� ���̰�
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

	// �÷��̾�2�� ����� ���̰� �÷��̾�1�� ����� �� ���̰�
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

// ������ ����� ������ �� �޴� ����
void select_the_turn(int my_block, int nth_block) {
	char key;
	int y = 14;
	int cnt1 = 0, cnt2 = 0;

	who_is_win(); // ���� �̰���� ��� �˻�

	while (player1.block[cnt1] != 0) cnt1++;
	while (player2.block[cnt2] != 0) cnt2++;
	show_blocks(cnt1, cnt2);

	change_color(YELLOW, DARK_GRAY);
	// ���� ���� ī�� �����ֱ�
	if (player1.turn == 1 && player2.turn == 0) {
		if (my_block != -1) gotoxy(17 + 5 * my_block, 6); printf(" ��");
	}
	else if (player1.turn == 0 && player2.turn == 1) {
		if (my_block != -1) gotoxy(17 + 5 * my_block, 25); printf(" ��");
	}

	change_color(SKY_BLUE, DARK_GRAY);
	gotoxy(87, 14); printf("������ ��� �� ���߱�            ");
	gotoxy(87, 15); printf("���� �ѱ��                        ");
	gotoxy(84, 14); puts("  ");
	gotoxy(84, 15); puts("  ");

	while (1) {
		change_color(SKY_BLUE, DARK_GRAY);
		gotoxy(84, y); printf("��");
		key = _getch(); // Ű �Է� ����.

		if (key == 72) { // �� Key
			y = y - 1;
			gotoxy(84, y + 1); puts("  ");
			if (y < 14) y = 15; // �� ������ �� Key�� ������ �� �Ʒ���
		}
		if (key == 80) { // �� Key
			y = y + 1;
			gotoxy(84, y - 1); puts("  ");
			if (y > 15) y = 14; // �� �Ʒ����� �� Key�� ������ �� ����
		}
		if (key == 13) break;
	}

	if (y == 14) return guess_a_block(my_block);
	else if (y == 15) change_the_turn();
}

// ������ ����� �����ϴ� �Լ�
int guess_a_block(int my_block) {

	change_color(BLOCK, DARK_GRAY);
	gotoxy(84, 13); printf("���߷��� ������ ����� ������.   ");
	gotoxy(84, 14); printf("��, �� Key�� �̿��Ͽ� �̵��ϰ�,      ");
	gotoxy(84, 15); printf("Enter Key�� �����ϼ���.              ");

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
		printf(" ��");
		key = _getch();

		if (key == 75) { // �� Key
			nth_block--;
			x = x - 5;
			gotoxy(x + 5, y); puts("  ");
			if (x < 17) {
				nth_block = cnt;
				x = 17 + 5 * cnt;
			}
		}

		else if (key == 77) { // �� Key
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
				gotoxy(87, 18); printf("�̹� ���õ� ����Դϴ� !");
			}
			else if (player1.turn == 1 && player2.opened_block[nth_block] == 1) {
				change_color(WHITE, DARK_RED);
				gotoxy(87, 18); printf("�̹� ���õ� ����Դϴ� !");
			}
			else {
				gotoxy(x, y); puts("  ");
				break;
			}

		}
	}

	gotoxy(84, 16); printf("�� �����ϴ� ����� ����?       ");
	gotoxy(84, 17); printf("(��Ŀ��� 12 �Է�)             ");
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
			gotoxy(84, 14); printf("�¾ҽ��ϴ� !                       ");
			gotoxy(84, 15); printf("������ ����� �����Ǿ����ϴ�.    ");
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
			gotoxy(84, 14); printf("Ʋ�Ƚ��ϴ� !                       ");
			gotoxy(84, 15); printf("����� �� ����� �����Ǿ����ϴ�. ");
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
			gotoxy(84, 14); printf("�¾ҽ��ϴ� !                       ");
			gotoxy(84, 15); printf("������ ����� �����Ǿ����ϴ�.    ");
			select_the_turn(my_block, nth_block);
		}
		else {
			if (my_block != -1) player2.opened_block[my_block] = 1;
			gotoxy(84, 13); printf("                                   ");
			gotoxy(84, 14); printf("                                   ");
			gotoxy(84, 15); printf("                                   ");
			gotoxy(84, 16); printf("                                   ");
			gotoxy(84, 17); printf("                                   ");
			gotoxy(84, 14); printf("Ʋ�Ƚ��ϴ� !                       ");
			gotoxy(84, 15); printf("����� �� ����� �����Ǿ����ϴ�. ");
			Sleep(2000);
			change_the_turn();
		}
	}

	gotoxy(84, 16); printf("                        ");
	gotoxy(84, 17); printf("                  ");
	gotoxy(x, y); puts("  ");
}

// ��Ŀ�� ��ġ �����ϴ� �Լ�
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
		printf("��");
		key = _getch();

		if (key == 75) { // �� Key
			nth_block--;
			x = x - 5;
			gotoxy(x + 5, y); puts("  ");
			if (x < 16) {
				nth_block = cnt;
				x = 16 + 5 * cnt;
			}
		}

		else if (key == 77) { // �� Key
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
	return nth_block; // �� ��° ����� �����ߴ��� �˷��ֱ� ���� ����
}

// ��� �ݺ��ϴ� ���� ���� �Լ�
void one_turn() {
	int i, nth_block, temp, joker, check = 1;
	int cnt1 = 0, cnt2 = 0;
	int my_block_idx = 0;

	// �÷��̾���� ������ �ִ� ��� ��
	while (player1.block[cnt1] != 0) cnt1++;
	while (player2.block[cnt2] != 0) cnt2++;
	show_blocks(cnt1, cnt2);

	// ���� ǥ��
	show_the_turn();

	// ����� �� �����ƴٸ� ����� ���� ����.
	for (i = 0; i < 26; i++) {
		if (selected_blocks[i] == 0) check = 0;
	}

	if (check == 0) {
		// ��� �̱�
		change_color(BLOCK, DARK_GRAY);
		gotoxy(84, 13); printf("�������� ���� ��� �ϳ��� ������.");
		gotoxy(84, 14); printf("��, �� Key�� �̿��Ͽ� �̵��ϰ�,    ");
		gotoxy(84, 15); printf("Enter Key�� �����ϼ���.            ");
		nth_block = select_a_block();

		// ��Ŀ�� �ƴ϶�� ����� �÷��̾� �迭�� ������� ����
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

		// ��Ŀ���?
		else if (abs(game_blocks[nth_block]) == 13) {
			change_color(BLOCK, DARK_GRAY);
			gotoxy(84, 13); printf("��Ŀ�� �ְ� ���� ��ġ�� �����ϼ���.");
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

		// ��Ģ�� �°� ����
		player1.opened_block_on = 1;
		player2.opened_block_on = 0;
		sorting_blocks(player1.block, cnt1);
		player1.opened_block_on = 0;
		player2.opened_block_on = 1;
		sorting_blocks(player2.block, cnt2);
		player1.opened_block_on = 0;
		player2.opened_block_on = 0;


		// ������ �ش��ϴ� ����� ����� ���̰� 
		// ������ �ش����� �ʴ� ����� ����� ������ (���� �� ��� ���̰�)
		show_blocks(cnt1, cnt2);

		// ���� ���� ī�� �����ֱ�
		if (player1.turn == 1 && player2.turn == 0) {
			while (player1.block[my_block_idx] != game_blocks[nth_block]) my_block_idx++;
			change_color(YELLOW, DARK_GRAY);
			gotoxy(17 + 5 * my_block_idx, 6); printf(" ��");
		}
		else if (player1.turn == 0 && player2.turn == 1) {
			while (player2.block[my_block_idx] != game_blocks[nth_block]) my_block_idx++;
			change_color(YELLOW, DARK_GRAY);
			gotoxy(17 + 5 * my_block_idx, 25); printf(" ��");
		}
	}

	if (check == 1) my_block_idx = -1;
	// ������ ��� ���� �� ���߱�
	// ������ ������ ��� ����
	// Ʋ���� ���� �� ī�� ����
	guess_a_block(my_block_idx);

	if (check != -1) gotoxy(17 + 5 * my_block_idx, 2); printf("   ");
	if (check != -1) gotoxy(17 + 5 * my_block_idx, 21); printf("   ");
	show_blocks(cnt1, cnt2);
}

// ���� �̰���� üũ�ϴ� �Լ�
void who_is_win() {
	int i, cnt1 = 0, cnt2 = 0;
	int check1 = 1, check2 = 1;

	// �÷��̾���� ������ �ִ� ��� ��
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
		gotoxy(43, 13); printf("[%s]�� �а� ��� �����Ǿ����ϴ� !", player1.name);
		gotoxy(48, 14); printf("[%s]�� �¸��Ͽ����ϴ�.", player2.name);
		Sleep(10000);
		exit(0);
	}
	else if (check2 == 1) {
		system("cls");
		change_color(SKY_BLUE, DARK_GRAY);
		gotoxy(43, 13); printf("[%s]�� �а� ��� �����Ǿ����ϴ� !", player2.name);
		gotoxy(48, 14); printf("[%s]�� �¸��Ͽ����ϴ�.", player1.name);
		Sleep(10000);
		exit(0);
	}
}

/* �������������������������������������������������������������������� */

void quit_game() {
	system("cls");
	exit(0);
}

/* �������������������������������������������������������������������� */

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
