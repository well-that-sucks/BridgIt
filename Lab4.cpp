#include <windows.h>
#include <windowsx.h>
#include <ctime>
#include <string>
#include "Lab4.h"

#define MAX_LOADSTRING 100

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];
HMENU hmenu;
MENUITEMINFO menuItem;
int** solutionAH;
int** solutionBH;
int** solutionAV;
int** solutionBV;
int** bsH;
int** bsV;
bool hasPlayerWon;
bool hasBotWon;
POINT** aDots;
int difficulty;

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
void nullifyField();
void copyArray(int**, int**, int, int);
void fill2DArray(int**, int, int, int);
bool findPathA(int**, int**, int**, int, int, int, int, int, int&, int&, int&);
bool hasPlayerCrossed(int**, int**);
bool findPathB(int**, int**, int**, int, int, int, int, int, int&, int&, int&);
bool hasBotCrossed(int**, int**);
int estimatingFunction(int**, int**, int**, int**);
int findSolution(int**, int**, int**, int**, int, int, int, int**, int**);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);
int displayPlayerWin(HWND hWnd);
int displayBotWin(HWND hWnd);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LAB4, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
    if (!InitInstance (hInstance, nCmdShow)) {
        return FALSE;
    }
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB4));
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance) {
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = nullptr;
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LAB4);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = nullptr;

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
   hInst = hInstance;

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      200, 200, 480, 600, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd) {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

void clearField() {
    fill2DArray(solutionBH, 4, 4, 0);
    fill2DArray(solutionAV, 4, 4, 0);
    fill2DArray(solutionBV, 5, 3, 0);
    fill2DArray(solutionAH, 5, 3, 0);
    hasPlayerWon = false;
    hasBotWon = false;
}

void copyArray(int** a, int** b, int n, int m) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            a[i][j] = b[i][j];
}

void fill2DArray(int** a, int n, int m, int num) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            a[i][j] = num;
}

bool findPathA(int** solutionAH, int** solutionAV, int** visitedDots, int x, int y, int length, int depth, int branches, int& max, int& depthMax, int& branchesMax) {
    if (length > max)
        max = length;
    if (depth > depthMax)
        depthMax = depth;
    if (y == 4) {
        if (branches > branchesMax)
            branchesMax = branches;
        return true;
    }
    bool f = false;
    int br = 0;
    if (x > 0 && solutionAH[y][x - 1] > 0 && visitedDots[y][x - 1] == 0) {
        visitedDots[y][x - 1] = 1;
        f = f || findPathA(solutionAH, solutionAV, visitedDots, x - 1, y, length + 1, depth, branches, max, depthMax, branchesMax);
        br++;
        visitedDots[y][x - 1] = 0;
    }
    if (x < 3 && solutionAH[y][x] > 0 && visitedDots[y][x + 1] == 0 && !f) {
        visitedDots[y][x + 1] = 1;
        f = f || findPathA(solutionAH, solutionAV, visitedDots, x + 1, y, length + 1, depth, branches + br, max, depthMax, branchesMax);
        br++;
        visitedDots[y][x + 1] = 0;
    }
    if (y > 0 && solutionAV[x][y - 1] > 0 && visitedDots[y - 1][x] == 0 && !f) {
        visitedDots[y - 1][x] = 1;
        f = f || findPathA(solutionAH, solutionAV, visitedDots, x, y - 1, length + 1, depth - 1, branches + br, max, depthMax, branchesMax);
        br++;
        visitedDots[y - 1][x] = 0;
    }
    if (y < 4 && solutionAV[x][y] > 0 && visitedDots[y + 1][x] == 0 && !f) {
        visitedDots[y + 1][x] = 1;
        f = f || findPathA(solutionAH, solutionAV, visitedDots, x, y + 1, length + 1, depth + 1, branches + br, max, depthMax, branchesMax);
        br++;
        visitedDots[y + 1][x] = 0;
    }
    if (br == 0 && branches > branchesMax)
        branchesMax = branches;
    return f;
}

bool hasPlayerCrossed(int** solutionAH, int** solutionAV) {
    bool f = false;
    int** visitedDots = new int*[5];
    for (int i = 0; i < 5; i++)
        visitedDots[i] = new int[4];
    fill2DArray(visitedDots, 5, 4, 0);
    for (int i = 0; i < 4; i++) {
        visitedDots[0][i] = 1;
        int max = 0;
        f = f || findPathA(solutionAH, solutionAV, visitedDots, i, 0, 0, 0, 1, max, max, max);
        visitedDots[0][i] = 0;
    }
    for (int i = 0; i < 5; i++)
        delete[] visitedDots[i];
    delete[] visitedDots;
    return f;
}

bool findPathB(int** solutionBH, int** solutionBV, int** visitedDots, int x, int y, int length, int depth, int branches, int& max, int& depthMax, int& branchesMax) {
    if (length > max)
        max = length;
    if (depth > depthMax)
        depthMax = depth;
    if (x == 4) {
        if (branches > branchesMax)
            branchesMax = branches;
        return true;
    }
    bool f = false;
    int br = 0;
    if (x > 0 && solutionBH[y][x - 1] > 0 && visitedDots[y][x - 1] == 0) {
        visitedDots[y][x - 1] = 1;
        f = f || findPathB(solutionBH, solutionBV, visitedDots, x - 1, y, length + 1, depth - 1, branches, max, depthMax, branchesMax);
        br++;
        visitedDots[y][x - 1] = 0;
    }
    if (x < 4 && solutionBH[y][x] > 0 && visitedDots[y][x + 1] == 0 && !f) {
        visitedDots[y][x + 1] = 1;
        f = f || findPathB(solutionBH, solutionBV, visitedDots, x + 1, y, length + 1, depth + 1, branches + br, max, depthMax, branchesMax);
        br++;
        visitedDots[y][x + 1] = 0;
    }
    if (y > 0 && solutionBV[x][y - 1] > 0 && visitedDots[y - 1][x] == 0 && !f) {
        visitedDots[y - 1][x] = 1;
        f = f || findPathB(solutionBH, solutionBV, visitedDots, x, y - 1, length + 1, depth, branches + br, max, depthMax, branchesMax);
        br++;
        visitedDots[y - 1][x] = 0;
    }
    if (y < 3 && solutionBV[x][y] > 0 && visitedDots[y + 1][x] == 0 && !f) {
        visitedDots[y + 1][x] = 1;
        f = f || findPathB(solutionBH, solutionBV, visitedDots, x, y + 1, length + 1, depth, branches + br, max, depthMax, branchesMax);
        br++;
        visitedDots[y + 1][x] = 0;
    }
    if (br == 0 && branches > branchesMax)
        branchesMax = branches;
    return f;
}

bool hasBotCrossed(int** solutionBH, int** solutionBV) {
    bool f = false;
    int** visitedDots = new int*[4];
    for (int i = 0; i < 4; i++)
        visitedDots[i] = new int[5];
    fill2DArray(visitedDots, 4, 5, 0);
    for (int i = 0; i < 4; i++) {
        visitedDots[i][0] = 1;
        int max = 0;
        f = f || findPathB(solutionBH, solutionBV, visitedDots, 0, i, 0, 0, 1, max, max, max);
        visitedDots[i][0] = 0;
    }
    for (int i = 0; i < 4; i++)
        delete[] visitedDots[i];
    delete[] visitedDots;
    return f;
}

int estimatingFunction(int** solutionAH, int** solutionAV, int** solutionBH, int** solutionBV) {
    int** aField = new int*[5];
    for (int i = 0; i < 5; i++)
        aField[i] = new int[4];
    int** bField = new int*[4];
    for (int i = 0; i < 4; i++)
        bField[i] = new int[5];
    fill2DArray(aField, 5, 4, 0);
    fill2DArray(bField, 4, 5, 0);
    int maxPlayer = 0;
    int maxBot = 0;
    int dMaxPlayer = 0;
    int dMaxBot = 0;
    int bMaxPlayer = 0;
    int bMaxBot = 0;
    int win = 0;
    for (int i = 0; i < 5; i++)
        for (int j = 0; j < 4; j++) {
            aField[i][j] = 1;
            int tMax = 0;
            int dMax = 0;
            int bMax = 0;
            bool tf = findPathA(solutionAH, solutionAV, aField, j, i, 0, 0, 1, tMax, dMax, bMax);
            if (tf)
                win = 1;
            if (tMax > maxPlayer)
                maxPlayer = tMax;
            if (dMax > dMaxPlayer)
                dMaxPlayer = dMax;
            if (bMax > bMaxPlayer)
                bMaxPlayer = bMax;
            aField[i][j] = 0;
        }
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 5; j++) {
            bField[i][j] = 1;
            int tMax = 0;
            int dMax = 0;
            int bMax = 0;
            bool tf = findPathB(solutionBH, solutionBV, aField, j, i, 0, 0, 1, tMax, dMax, bMax);
            if (tf)
                win = -1;
            if (tMax > maxBot)
                maxBot = tMax;
            if (dMax > dMaxBot)
                dMaxBot = dMax;
            if (bMax > bMaxBot)
                bMaxBot = bMax;
            bField[i][j] = 0;
        }
    for (int i = 0; i < 5; i++)
        delete[] aField[i];
    delete[] aField;
    for (int i = 0; i < 4; i++)
        delete[] bField[i];
    delete[] bField;
    return (win * 65 + (dMaxPlayer - dMaxBot) * 20 + (bMaxBot - bMaxPlayer) * 10 + (maxPlayer - maxBot) * 5);
}

int findSolution(int** solutionAH, int** solutionAV, int** solutionBH, int** solutionBV, int alpha, int beta, int step, int** bestSolutionH, int** bestSolutionV) {
    if (step > difficulty)
        return estimatingFunction(solutionAH, solutionAV, solutionBH, solutionBV);
    bool hasPlayerWon = hasPlayerCrossed(solutionAH, solutionAV);
    bool hasBotWon = hasBotCrossed(solutionBH, solutionBV);
    if (hasPlayerWon)
        return 110 - step;
    if (hasBotWon)
        return -110 + step;
    if (step % 2 == 1) {
        int v = 999999;
        int minV = 999999;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (solutionBH[i][j] == 0 && solutionAV[j][i] == 0) {
                    solutionBH[i][j] = 1;
                    int res = findSolution(solutionAH, solutionAV, solutionBH, solutionBV, alpha, beta, step + 1, bestSolutionH, bestSolutionV);
                    if (res < v) {
                        v = res;
                        if (step == 1 && v < minV) {
                            minV = v;
                            copyArray(bestSolutionH, solutionBH, 4, 4);
                            copyArray(bestSolutionV, solutionBV, 5, 3);
                        }
                    }
                    if (v <= alpha) {
                        solutionBH[i][j] = 0;
                        return v;
                    }
                    beta = min(v, beta);
                    solutionBH[i][j] = 0;
                }
            }
        }
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 3; j++) {
                if ((i == 0 && solutionBV[i][j] == 0) || (i == 4 && solutionBV[i][j] == 0) || solutionBV[i][j] == 0 && solutionAH[j + 1][i - 1] == 0) {
                    solutionBV[i][j] = 1;
                    int res = findSolution(solutionAH, solutionAV, solutionBH, solutionBV, alpha, beta, step + 1, bestSolutionH, bestSolutionV);
                    if (res < v) {
                        v = res;
                        if (step == 1 && v < minV) {
                            minV = v;
                            copyArray(bestSolutionH, solutionBH, 4, 4);
                            copyArray(bestSolutionV, solutionBV, 5, 3);
                        }
                    }
                    if (v <= alpha) {
                        solutionBV[i][j] = 0;
                        return v;
                    }
                    beta = min(v, beta);
                    solutionBV[i][j] = 0;
                }
            }
        }
        return v;
    } else {
        int v = -999999;
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 3; j++) {
                if ((i == 0 && solutionAH[i][j] == 0) || (i == 4 && solutionAH[i][j] == 0) || solutionAH[i][j] == 0 && solutionBV[j + 1][i - 1] == 0) {
                    solutionAH[i][j] = 1;
                    v = max(v, findSolution(solutionAH, solutionAV, solutionBH, solutionBV, alpha, beta, step + 1, bestSolutionH, bestSolutionV));
                    if (v >= beta) {
                        solutionAH[i][j] = 0;
                        return v;
                    }
                    alpha = max(v, alpha);
                    solutionAH[i][j] = 0;
                }
            }
        }
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (solutionAV[i][j] == 0 && solutionBH[j][i] == 0) {
                    solutionAV[i][j] = 1;
                    v = max(v, findSolution(solutionAH, solutionAV, solutionBH, solutionBV, alpha, beta, step + 1, bestSolutionH, bestSolutionV));
                    if (v >= beta) {
                        solutionAV[i][j] = 0;
                        return v;
                    }
                    alpha = max(v, alpha);
                    solutionAV[i][j] = 0;
                }
            }
        }
        return v;
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_CREATE: {
            difficulty = 3;
            hmenu = GetMenu(hWnd);
            menuItem.cbSize = sizeof(MENUITEMINFO);
            menuItem.fMask = MIIM_STATE;
            GetMenuItemInfo(hmenu, ID_32772, FALSE, &menuItem);
            menuItem.fState = MFS_CHECKED;
            SetMenuItemInfo(hmenu, ID_32772, FALSE, &menuItem);
            aDots = new POINT*[5];
            for (int i = 0; i < 5; i++)
                aDots[i] = new POINT[4];
            for (int i = 0; i < 5; i++) {
                for (int j = 0; j < 4; j++) {
                    aDots[i][j].x = 110 + j * 80;
                    aDots[i][j].y = 110 + i * 80;
                }
            }
            hasPlayerWon = false;
            hasBotWon = false;
            solutionBH = new int* [4];
            solutionAV = new int* [4];
            bsH = new int* [4];
            for (int i = 0; i < 4; i++) {
                solutionBH[i] = new int[4];
                solutionAV[i] = new int[4];
                bsH[i] = new int[4];
            }
            fill2DArray(solutionBH, 4, 4, 0);
            fill2DArray(solutionAV, 4, 4, 0);
            solutionBV = new int* [5];
            solutionAH = new int* [5];
            bsV = new int* [5];
            for (int i = 0; i < 5; i++) {
                solutionBV[i] = new int[3];
                solutionAH[i] = new int[3];
                bsV[i] = new int[3];
            }
            fill2DArray(solutionBV, 5, 3, 0);
            fill2DArray(solutionAH, 5, 3, 0);
        }
        break;
        case WM_COMMAND: {
            int wmId = LOWORD(wParam);
            // Разобрать выбор в меню:
            switch (wmId) {
                case ID_32771: {
                    difficulty = 2;
                    GetMenuItemInfo(hmenu, ID_32771, FALSE, &menuItem);
                    menuItem.fState = MFS_CHECKED;
                    SetMenuItemInfo(hmenu, ID_32771, FALSE, &menuItem);
                    GetMenuItemInfo(hmenu, ID_32772, FALSE, &menuItem);
                    menuItem.fState = MFS_UNCHECKED;
                    SetMenuItemInfo(hmenu, ID_32772, FALSE, &menuItem);
                    GetMenuItemInfo(hmenu, ID_32773, FALSE, &menuItem);
                    menuItem.fState = MFS_UNCHECKED;
                    SetMenuItemInfo(hmenu, ID_32773, FALSE, &menuItem);
                }
                break;
                case ID_32772: {
                    difficulty = 3;
                    GetMenuItemInfo(hmenu, ID_32771, FALSE, &menuItem);
                    menuItem.fState = MFS_UNCHECKED;
                    SetMenuItemInfo(hmenu, ID_32771, FALSE, &menuItem);
                    GetMenuItemInfo(hmenu, ID_32772, FALSE, &menuItem);
                    menuItem.fState = MFS_CHECKED;
                    SetMenuItemInfo(hmenu, ID_32772, FALSE, &menuItem);
                    GetMenuItemInfo(hmenu, ID_32773, FALSE, &menuItem);
                    menuItem.fState = MFS_UNCHECKED;
                    SetMenuItemInfo(hmenu, ID_32773, FALSE, &menuItem);
                }
                break;
                case ID_32773: {
                    difficulty = 4;
                    GetMenuItemInfo(hmenu, ID_32771, FALSE, &menuItem);
                    menuItem.fState = MFS_UNCHECKED;
                    SetMenuItemInfo(hmenu, ID_32771, FALSE, &menuItem);
                    GetMenuItemInfo(hmenu, ID_32772, FALSE, &menuItem);
                    menuItem.fState = MFS_UNCHECKED;
                    SetMenuItemInfo(hmenu, ID_32772, FALSE, &menuItem);
                    GetMenuItemInfo(hmenu, ID_32773, FALSE, &menuItem);
                    menuItem.fState = MFS_CHECKED;
                    SetMenuItemInfo(hmenu, ID_32773, FALSE, &menuItem);
                }
                break;
                case IDM_EXIT:
                    DestroyWindow(hWnd);
                break;
                default:
                    return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            HGDIOBJ original = NULL;
            original = SelectObject(hdc, GetStockObject(DC_PEN));
            SelectObject(hdc, GetStockObject(DC_PEN));
            SelectObject(hdc, GetStockObject(DC_BRUSH));
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    if (solutionBH[i][j] == 1) {
                        SetDCPenColor(hdc, RGB(255, 0, 0));
                        MoveToEx(hdc, 69 + j * 80, 149 + i * 80, NULL);
                        LineTo(hdc, 149 + j * 80, 149 + i * 80);
                    }
                    if (solutionAV[i][j] == 1) {
                        SetDCPenColor(hdc, RGB(0, 0, 255));
                        MoveToEx(hdc, 109 + i * 80, 109 + j * 80, NULL);
                        LineTo(hdc, 109 + i * 80, 189 + j * 80);
                    }
                }
            }
            for (int i = 0; i < 5; i++) {
                for (int j = 0; j < 3; j++) {
                    if (solutionBV[i][j] == 1) {
                        SetDCPenColor(hdc, RGB(255, 0, 0));
                        MoveToEx(hdc, 69 + i * 80, 149 + j * 80, NULL);
                        LineTo(hdc, 69 + i * 80, 229 + j * 80);
                    }
                    if (solutionAH[i][j] == 1) {
                        SetDCPenColor(hdc, RGB(0, 0, 255));
                        MoveToEx(hdc, 109 + j * 80, 109 + i * 80, NULL);
                        LineTo(hdc, 189 + j * 80, 109 + i * 80);
                    }
                }
            }
            SelectObject(hdc, GetStockObject(NULL_PEN));
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 5; j++) {
                    SetDCBrushColor(hdc, RGB(0, 0, 255));
                    Ellipse(hdc, 100 + i * 80, 100 + j * 80, 120 + i * 80, 120 + j * 80);
                    SetDCBrushColor(hdc, RGB(255, 0, 0));
                    Ellipse(hdc, 60 + j * 80, 140 + i * 80, 80 + j * 80, 160 + i * 80);
                }
            }
            SelectObject(hdc, original);
            EndPaint(hWnd, &ps);
        }
        break;
        case WM_LBUTTONDOWN: {
            if (!hasBotWon && !hasPlayerWon) {
                POINT closestPoint1;
                closestPoint1.y = 0;
                closestPoint1.x = 0;
                POINT closestPoint2;
                closestPoint2.y = -1;
                closestPoint2.x = -1;
                int mouseX = GET_X_LPARAM(lParam);
                int mouseY = GET_Y_LPARAM(lParam);
                double min = sqrt((mouseX - aDots[0][0].x) * (mouseX - aDots[0][0].x) + (mouseY - aDots[0][0].y) * (mouseY - aDots[0][0].y));
                for (int i = 0; i < 5; i++) {
                    for (int j = 0; j < 4; j++) {
                        double t = sqrt((mouseX - aDots[i][j].x) * (mouseX - aDots[i][j].x) + (mouseY - aDots[i][j].y) * (mouseY - aDots[i][j].y));
                        if (t < min) {
                            min = t;
                            closestPoint1.x = j;
                            closestPoint1.y = i;
                        }
                    }
                }
                min = 999999;
                for (int i = 0; i < 5; i++) {
                    for (int j = 0; j < 4; j++) {
                        double t = sqrt((mouseX - aDots[i][j].x) * (mouseX - aDots[i][j].x) + (mouseY - aDots[i][j].y) * (mouseY - aDots[i][j].y));
                        if (t < min && (closestPoint1.x != j || closestPoint1.y != i)) {
                            min = t;
                            closestPoint2.x = j;
                            closestPoint2.y = i;
                        }
                    }
                }
                bool isChanged = false;
                int t1 = closestPoint2.y;
                int t2 = min(closestPoint1.x, closestPoint2.x);
                if (closestPoint1.y == closestPoint2.y && solutionAH[t1][t2] == 0 && (t1 == 0 || t1 == 4 || solutionBV[t2 + 1][t1 - 1] == 0)) {
                    solutionAH[closestPoint1.y][min(closestPoint1.x, closestPoint2.x)] = 1;
                    isChanged = true;
                }
                if (closestPoint1.x == closestPoint2.x && solutionAV[closestPoint1.x][min(closestPoint1.y, closestPoint2.y)] == 0 && solutionBH[min(closestPoint1.y, closestPoint2.y)][closestPoint1.x] == 0) {
                    solutionAV[closestPoint1.x][min(closestPoint1.y, closestPoint2.y)] = 1;
                    isChanged = true;
                }
                if (isChanged) {
                    hasPlayerWon = hasPlayerCrossed(solutionAH, solutionAV);
                    if (!hasPlayerWon) {
                        findSolution(solutionAH, solutionAV, solutionBH, solutionBV, -999999, 999999, 1, bsH, bsV);
                        copyArray(solutionBH, bsH, 4, 4);
                        copyArray(solutionBV, bsV, 5, 3);
                    }
                    InvalidateRect(hWnd, NULL, FALSE);
                    UpdateWindow(hWnd);
                    hasBotWon = hasBotCrossed(solutionBH, solutionBV);
                    if (hasPlayerWon)
                        displayPlayerWin(hWnd);
                    if (hasBotWon)
                        displayBotWin(hWnd);
                }
            }
        }
        break;
        case WM_DESTROY:
            PostQuitMessage(0);
        break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

int displayPlayerWin(HWND hWnd) {
    int msgboxID = MessageBox(NULL, L"Player has won!", L"", MB_ICONINFORMATION | MB_OK);
    clearField();
    InvalidateRect(hWnd, NULL, TRUE);
    UpdateWindow(hWnd);
    return msgboxID;
}

int displayBotWin(HWND hWnd) {
    int msgboxID = MessageBox(NULL, L"Bot has won!", L"", MB_ICONINFORMATION | MB_OK);
    clearField();
    InvalidateRect(hWnd, NULL, TRUE);
    UpdateWindow(hWnd);
    return msgboxID;
}