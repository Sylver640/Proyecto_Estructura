#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

void gotoxy(int x, int y)
{
    HANDLE manipulator = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(manipulator, coord);
}

void centerText (const char *text, int y)
{
        int text_size = strlen(text);
        gotoxy(40 - (text_size/2), y);
        printf("%s", text);
}

void logo()
{
    printf("\n");
    centerText(" _    ___  _    ___      _     ___  _     _  ____     ___  _  __  __   ___  _     ____ ___  _    ", 1);
    printf("\n");
    centerText("\\ \\_// / \\| | || |_)    | |\\/|/ / \\ \\  /| || |_      | | \\| |( (`/ /` / / \\ \\  /| |_ | |_)\\ \\_/ ", 2);
    printf("\n");
    centerText(" |_| \\_\\_/\\_\\_/|_| \\    |_|  |\\_\\_/ \\_\\/  |_||_|__   |_|_/|_|_)_)\\_\\_,\\_\\_/  \\_\\/ |_|__|_| \\ |_| ", 3);
}
