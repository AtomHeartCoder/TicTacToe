#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <time.h>

#define  SIZE  3
#define  MAXLIST  100000

typedef struct NODE {
    char board[SIZE][SIZE];
    int depth;
    int result;
    struct NODE *prior;
    struct NODE *next[8];
} NODE;

int judge(char a[][SIZE], char b[][SIZE]);
int result(char a[][SIZE]);
void build(void);
int evaluate(NODE *node);
NODE *search(NODE *node);
void transform(char a[][SIZE], char b[][SIZE]);
void gotoxy(int x, int y);

NODE *open[MAXLIST], *close[MAXLIST];

int judge(char a[][SIZE], char b[][SIZE])
{
    int i, j, mode, sign;
    for ( mode = 1, sign = 0; mode <= 7; mode++ ) {
        if ( sign )
            break;
        for ( i = 0, sign = 1; sign && i < SIZE; i++ )
            for ( j = 0; sign && j < SIZE; j++ )
                switch ( mode ) {
                    case 1:
                        if ( b[SIZE - 1 - j][i] != a[i][j] ) // anti-90
                            sign = 0;
                        break;
                    case 2:
                        if ( b[SIZE - 1 - i][SIZE - 1 - j] != a[i][j] ) // 180
                            sign = 0;
                        break;
                    case 3:
                        if ( b[j][SIZE - 1 - i] != a[i][j] ) // 90
                            sign = 0;
                        break;
                    case 4:
                        if ( b[j][i] != a[i][j] ) // diagonal
                            sign = 0;
                        break;
                    case 5:
                        if ( b[SIZE - 1 - j][SIZE - 1 - i] != a[i][j] ) // sec_diagonal
                            sign = 0;
                        break;
                    case 6:
                        if ( b[i][SIZE - 1 -j] != a[i][j] ) // left-to-right
                            sign = 0;
                        break;
                    case 7:
                        if ( b[SIZE - 1 - i][j] != a[i][j] ) // up-to-down
                            sign = 0;
                        break;
                }
    }
    return sign;
}

int result(char a[][SIZE])
{
    int i;
    for ( i = 0; i < SIZE; i++ ) {
        if ( a[i][0] == a[i][1] && a[i][0] == a[i][2] ) {
            if ( a[i][0] == 'O' )
                return 1;
            else if ( a[i][0] == 'X' )
                return 2;
        }
        if ( a[0][i] == a[1][i] && a[0][i] == a[2][i] ) {
            if ( a[0][i] == 'O' )
                return 1;
            else if ( a[0][i] == 'X' )
                return 2;
        }
    }
    if ( (a[0][0] == a[1][1] && a[1][1] == a[2][2]) || (a[0][2] == a[1][1] && a[1][1] == a[2][0]) ) {
        if ( a[1][1] == 'O' )
            return 1;
        else if ( a[1][1] == 'X' )
            return 2;
    }
    return 0;
}

void build(void)
{
    int i, j, m, n, pointer = 0, sign, close_num = 0, close_next = 0;
    char piece[2] = { 'O', 'X' };
    for ( i = 0; i < MAXLIST; i++ )
        open[i] = close[i] = NULL;
    open[0] = (NODE *)malloc(sizeof(NODE));
    open[0]->depth = 0;
    open[0]->result = 0;
    open[0]->prior = NULL;
    for ( i = 0; i < SIZE; i++ )
        for ( j = 0; j < SIZE; j++ )
            open[0]->board[i][j] = ' ';
    while ( pointer >= 0 && pointer < MAXLIST )
    {
        close[close_num] = open[pointer];
        open[pointer--] = NULL;
        for ( i = 0; i < 8; i++ )
            close[close_num]->next[i] = NULL;
        for ( m = 0, close_next = 0; m < SIZE; m++ )
            for ( n = 0; n < SIZE; n++ ) {
                NODE *node = (NODE *)malloc(sizeof(NODE));
                for ( i = 0; i < SIZE; i++ )
                    for ( j = 0; j < SIZE; j++ )
                        node->board[i][j] = close[close_num]->board[i][j];
                if ( node->board[m][n] == ' ' ) {
                    node->depth = close[close_num]->depth + 1;
                    node->board[m][n] = piece[(node->depth + 1) % 2];
                    for ( i = 0, sign = 0; i < close_next && close[close_num]->next[i] != NULL; i++ )
                        if ( judge(node->board, close[close_num]->next[i]->board) ) {
                            sign = 1;
                            break;
                        }
                    if ( sign ) {
                        free(node);
                        continue;
                    }
                } else {
                    free(node);
                    continue;
                }
                close[close_num]->next[close_next++] = node;
                node->prior = close[close_num];
                node->result = result(node->board);
                if ( node->result == 0 && node->depth != 9 ) {
                    pointer++;
                    open[pointer] = node;
                } else {
                    close[close_num + 1] = close[close_num];
                    close[close_num++] = node;
                    for ( i = 0; i < 8; i++ )
                        node->next[i] = NULL;
                }
            }
        close_num++;
    }
    return;
}

int evaluate(NODE *node)
{
    int i, sign, win = 1, lose = 2;
    if ( node->depth % 2 == 1 )
        win = 2, lose = 1;
    if ( node->next[0] == NULL )
        return node->result;
    for ( i = 0, sign = 0; node->next[i] != NULL; i++ )
        if ( (node->next[i]->result = evaluate(node->next[i])) == win )
            return win;
        else if ( node->next[i]->result == 0 )
            sign = 1;
    if ( sign )
        return 0;
    else
        return lose;
}

NODE *search(NODE *node)
{
    int i, j, expect = 0, win = 1, lose = 2;
    int choice[3][8], sum[3] = { 0, 0, 0 };
    for ( i = 0; i < 3; i++ )
        for ( j = 0; j < 8; j++ )
            choice[i][j] = -1;
    for ( i = 0; i < 8 && node->next[i] != NULL; i++ )
        choice[node->next[i]->result][sum[node->next[i]->result]++] = i;
    if ( node->depth % 2 == 1 )
        win = 2, lose = 1;
    if ( choice[win][0] != -1 )
        expect = win;
    else if ( choice[0][0] != -1 )
        expect = 0;
    else
        expect = lose;
    srand(time(NULL));
    return node->next[choice[expect][rand() % sum[expect]]];
}

void transform(char a[][SIZE], char b[][SIZE])
{
    int i, j, x = 0, y = 0, mode, count, find, sign;
    char c;
    for ( mode = 1, find = 0; !find && mode <= 7; mode++ ) {
        for ( i = 0, count = 0, sign = 1; sign && i < SIZE; i++ )
            for ( j = 0; sign && j < SIZE; j++ ) {
                switch ( mode ) {
                    case 1:
                        if ( b[SIZE - 1 - j][i] != a[i][j] ) { // anti-90
                            x = i, y = j;
                            c = b[SIZE - 1 - j][i];
                            count++;
                        }
                        break;
                    case 2:
                        if ( b[SIZE - 1 - i][SIZE - 1 - j] != a[i][j] ) { // 180
                            x = i, y = j;
                            c = b[SIZE - 1 - i][SIZE - 1 - j];
                            count++;
                        }
                        break;
                    case 3:
                        if ( b[j][SIZE - 1 - i] != a[i][j] ) { // 90
                            x = i, y = j;
                            c = b[j][SIZE - 1 - i];
                            count++;
                        }
                        break;
                    case 4:
                        if ( b[j][i] != a[i][j] ) { // diagonal
                            x = i, y = j;
                            c = b[j][i];
                            count++;
                        }
                        break;
                    case 5:
                        if ( b[SIZE - 1 - j][SIZE - 1 - i] != a[i][j] ) { // sec_diagonal
                            x = i, y = j;
                            c = b[SIZE - 1 - j][SIZE - 1 - i];
                            count++;
                        }
                        break;
                    case 6:
                        if ( b[i][SIZE - 1 -j] != a[i][j] ) { // left-to-right
                            x = i, y = j;
                            c = b[i][SIZE - 1 -j];
                            count++;
                        }
                        break;
                    case 7:
                        if ( b[SIZE - 1 - i][j] != a[i][j] ) { // up-to-down
                            x = i, y = j;
                            c = b[SIZE - 1 - i][j];
                            count++;
                        }
                        break;
                }
                if ( count > 1 )
                    sign = 0;
            }
        if ( count == 1 )
            find = 1;
    }
    a[x][y] = c;
    gotoxy(y, x);
    putchar(c);
    return;
}

void gotoxy(int x, int y)
{
    COORD pos = { 2 * (2 * x + 1), 2 * y + 1 };
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(hOut, pos);
}

int main(void)
{
    int i, j, x, y, sign, mode;
    char board[SIZE][SIZE];
    char ctrl;
    NODE *node = NULL;
    build();
    evaluate(close[0]);
    while (1) {
        x = 0, y = 0, sign = 0, mode = 0;
        system("cls");
        for ( i = 0; i < SIZE; i++ )
            for ( j = 0; j < SIZE; j++ )
                board[i][j] = ' ';
        printf("Choose mode:\n1. Player first.\n2. Computer first.\n3. Exit\n");
        if ( (ctrl = _getch()) == '1' )
            mode = 1;
        else if ( ctrl == '2' )
            mode = 2;
        else if ( ctrl == '3' )
            break;
        else
            continue;
        system("cls");
        printf("©°©¤©¤©¤©Ð©¤©¤©¤©Ð©¤©¤©¤©´\n");
        printf("©¦   ©¦   ©¦   ©¦\n");
        printf("©À©¤©¤©¤©à©¤©¤©¤©à©¤©¤©¤©È\n");
        printf("©¦   ©¦   ©¦   ©¦\n");
        printf("©À©¤©¤©¤©à©¤©¤©¤©à©¤©¤©¤©È\n");
        printf("©¦   ©¦   ©¦   ©¦\n");
        printf("©¸©¤©¤©¤©Ø©¤©¤©¤©Ø©¤©¤©¤©¼\n\n\n\n");
        printf(" Move by w/a/s/d, place by space.");
        node = close[0];
        if ( mode == 2 ) {
            node = search(node);
            Sleep(300);
            transform(board, node->board);
        }
        gotoxy(0, 0);
        while ( (ctrl = _getch()) != 'p' ) {
            if ( sign )
                sign = 0;
            switch ( ctrl ) {
                case 'w':
                case 'W':
                    if ( y > 0 )
                        y--;
                    break;
                case 'a':
                case 'A':
                    if ( x > 0 )
                        x--;
                    break;
                case 's':
                case 'S':
                    if ( y < SIZE - 1 )
                        y++;
                    break;
                case 'd':
                case 'D':
                    if ( x < SIZE - 1 )
                        x++;
                    break;
                case ' ':
                    sign = 1;
            }
            gotoxy(x, y);
            if ( sign && board[y][x] == ' ' ) {
                if ( mode == 1 ) {
                    putchar('O');
                    board[y][x] = 'O';
                } else {
                    putchar('X');
                    board[y][x] = 'X';
                }
                for ( i = 0; node->next[i] != NULL; i++ )
                    if ( judge(board, node->next[i]->board) ) {
                        node = node->next[i];
                        break;
                    }
                if ( node->depth != 9 ) {
                    node = search(node);
                    Sleep(800);
                    transform(board, node->board);
                }
                gotoxy(x, y);
                if ( node->next[0] == NULL ) {
                    gotoxy(0, 3);
                    switch ( node->result ) {
                        case 0:
                            printf("End in a draw!");
                            break;
                        case 1:
                            if ( mode == 1 )
                                printf("You win!");
                            else if ( mode == 2 )
                                printf("You lose!");
                            break;
                        case 2:
                            if ( mode == 1 )
                                printf("You lose!");
                            else if ( mode == 2 )
                                printf("You win!");
                            break;
                    }
                    printf("\n\n\n Press space to continue.        ");
                    while ( _getch() != ' ' )
                        ;
                    break;
                }
            }
        }
    }
    return 0;
}
