#include <Windows.h>
#include <time.h>
#include "resource.h"

LRESULT CALLBACK MyWNDPROC(HWND, UINT, WPARAM, LPARAM);

void ShowBackground(HDC hdc);
void AddNode(int x,int y);
void ShowSnake(HDC hdc);
void Move();
void ShowApple(HDC hdc);
BOOL IsEatApple();
void NewApple();
BOOL BumpWall();
BOOL EatItself();

#define FAST 1
#define SLOW 2

typedef struct NODE
{
	int x;
	int y;
	struct NODE *pNext;
	struct NODE *pLast;
}Snake,Apple;

enum FX {UP,DOWN,LEFT,RIGHT};


HBITMAP hBitmap_Backgound;
HBITMAP hBitmap_Apple;
HBITMAP hBitmap_SnakeBody;
HBITMAP hBitmap_SnakeHead_Up;
HBITMAP hBitmap_SnakeHead_Down;
HBITMAP hBitmap_SnakeHead_Left;
HBITMAP hBitmap_SnakeHead_Right;

Snake *pHead = NULL;
Snake *pTail = NULL;
enum FX fx = UP;
Apple apple = {5,5,NULL,NULL};
BOOL bFlag = TRUE;

int CALLBACK WinMain(HINSTANCE hInstance,
					 HINSTANCE hPrevInstance,
					 LPSTR lpCmdLine,
					 int nCmdShow)
{
	WNDCLASSEX ex;
	HWND hWnd;
	MSG msg;
	int i;

	hBitmap_Backgound = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_BITMAP1));
	hBitmap_Apple = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_BITMAP2));
	hBitmap_SnakeBody = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_BITMAP3));
	hBitmap_SnakeHead_Up = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_BITMAP5));
	hBitmap_SnakeHead_Down = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_BITMAP7));
	hBitmap_SnakeHead_Left = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_BITMAP6));
	hBitmap_SnakeHead_Right = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_BITMAP4));

	//初始化蛇
	for(i=0;i<100;i++)
	{
		AddNode(3,3);
		AddNode(3,4);
		AddNode(3,5);
	}

	//设置随机数种子
	srand((unsigned int)time(NULL));

	//1.设计窗口
	ex.style = CS_HREDRAW | CS_VREDRAW;
	ex.cbSize = sizeof(ex);
	ex.cbClsExtra = 0;
	ex.cbWndExtra = 0;
	ex.hInstance = hInstance;
	ex.hIcon = NULL;
	ex.hCursor = NULL;
	ex.hbrBackground = CreateSolidBrush(RGB(0,255,0));
	ex.hIconSm = NULL;
	ex.lpfnWndProc = &MyWNDPROC;
	ex.lpszMenuName = NULL;
	ex.lpszClassName = "aaa";

	//2.注册窗口
	RegisterClassEx(&ex);

	//3.创建窗口
	hWnd = CreateWindow(ex.lpszClassName,"贪吃蛇",WS_OVERLAPPEDWINDOW,30,30,615,638,NULL
		,NULL,hInstance,NULL);

	//4.显示窗口
	ShowWindow(hWnd,SW_SHOW);

	//设置定时器
	SetTimer(hWnd,FAST,200,NULL);
	//SetTimer(hWnd,SLOW,500,NULL);
	
	//消息循环
	while(GetMessage(&msg,NULL,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

LRESULT CALLBACK MyWNDPROC(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;

	switch(message)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		if(bFlag == TRUE)
		{
			switch(wParam)
			{
			case VK_UP:
				if(fx != DOWN)
				{
					fx = UP;
					bFlag = FALSE;
				}
				break;
			case VK_DOWN:
				if(fx != UP)
				{
					fx = DOWN;
					bFlag = FALSE;
				}
				break;
			case VK_LEFT:
				if(fx != RIGHT)
				{
					fx = LEFT;
					bFlag = FALSE;
				}
				break;
			case VK_RIGHT:
				if(fx != LEFT)
				{
					fx = RIGHT;
					bFlag = FALSE;
				}
				break;
			}		
		}

		hdc = GetDC(hWnd);
		//显示背景
		ShowBackground(hdc);
		//显示蛇
		ShowSnake(hdc);
		//显示苹果
		ShowApple(hdc);

		ReleaseDC(hWnd,hdc);

		break;
	case WM_PAINT:		//重绘消息
		hdc = BeginPaint(hWnd,&ps);
		
		//显示背景
		ShowBackground(hdc);
		//显示蛇
		ShowSnake(hdc);
		//显示苹果
		ShowApple(hdc);

		EndPaint(hWnd,&ps);
		break;
	case WM_TIMER:
		
		//蛇移动
		Move();

		//是否吃到苹果
		if(IsEatApple())
		{
			//长个
			AddNode(-15,-15);
			//新苹果
			NewApple();
		}

		//游戏是否结束
			//撞墙
			//自咬
		/*if(BumpWall() || EatItself())
		{
			KillTimer(hWnd,FAST);			
			MessageBox(hWnd,"游戏结束","提示",MB_OK);	
		}*/

		hdc = GetDC(hWnd);
		//显示背景
		ShowBackground(hdc);
		//显示蛇
		ShowSnake(hdc);
		//显示苹果
		ShowApple(hdc);

		ReleaseDC(hWnd,hdc);

		bFlag = TRUE;

		break;
	}

	return DefWindowProc(hWnd,message,wParam,lParam);
}

void ShowBackground(HDC hdc)
{
	//1.创建一个兼容性的DC
	HDC hdcmen = CreateCompatibleDC(hdc);
	//2.给兼容性的DC选择一张位图
	SelectObject(hdcmen,hBitmap_Backgound);
	//3.传输
	BitBlt(hdc,0,0,600,600,hdcmen,0,0,SRCCOPY);
	//4.删除兼容性DC
	DeleteDC(hdcmen);
}

void AddNode(int x,int y)
{
	Snake *pTemp = (Snake*)malloc(sizeof(Snake));
	pTemp->x = x;
	pTemp->y = y;
	pTemp->pLast = NULL;
	pTemp->pNext = NULL;
	
	if(NULL == pHead)
	{
		pHead = pTemp;
	}
	else
	{
		pTail->pNext = pTemp;
		pTemp->pLast = pTail;
	}
	pTail = pTemp;
}

void ShowSnake(HDC hdc)
{
	Snake* pMark = pHead->pNext;
	HDC hdcmen = CreateCompatibleDC(hdc);
	//1.显示蛇头
		//根据蛇的行进方向去贴图，蛇头 
	switch (fx)
	{
	case UP:
		SelectObject(hdcmen,hBitmap_SnakeHead_Up);
		break;
	case DOWN:
		SelectObject(hdcmen,hBitmap_SnakeHead_Down);
		break;
	case LEFT:
		SelectObject(hdcmen,hBitmap_SnakeHead_Left);
		break;
	case RIGHT:
		SelectObject(hdcmen,hBitmap_SnakeHead_Right);
		break;
	default:
		break;
	}
	BitBlt(hdc,pHead->x*30,pHead->y*30,30,30,hdcmen,0,0,SRCCOPY);

	//2.显示蛇身
		//从第二个节点遍历
	SelectObject(hdcmen,hBitmap_SnakeBody);
	while(pMark != NULL)
	{
		BitBlt(hdc,pMark->x*30,pMark->y*30,30,30,hdcmen,0,0,SRCCOPY);
		pMark = pMark->pNext;
	}

	DeleteDC(hdcmen);
}

void Move()
{
	Snake *pMark = pTail;
	//1.移动蛇身
	while(pMark != pHead)
	{
		//后一节等于前一节里的坐标
		pMark->x = pMark->pLast->x;
		pMark->y = pMark->pLast->y;
		pMark = pMark->pLast;
	}
	//2.移动蛇头
		//根据当前的行进方向去改变
	switch (fx)
	{
	case UP:
		pHead->y--;
		break;
	case DOWN:
		pHead->y++;
		break;
	case LEFT:
		pHead->x--;
		break;
	case RIGHT:
		pHead->x++;
		break;
	default:
		break;
	}
}

void ShowApple(HDC hdc)
{
	HDC hdcmen = CreateCompatibleDC(hdc);
	SelectObject(hdcmen,hBitmap_Apple);
	BitBlt(hdc,apple.x*30,apple.y*30,30,30,hdcmen,0,0,SRCCOPY);
	DeleteDC(hdcmen);
}

BOOL IsEatApple()
{
	if(pHead->x == apple.x && pHead->y == apple.y)
	{
		return TRUE;
	}

	return FALSE;
}

void NewApple()
{
	int x,y;
	Snake *pMark = pHead;
	//1.随机生成x，y坐标   范围 1 - 18

	do
	{
		x = rand() % 18 + 1;
		y = rand() % 18 + 1;
		//2.苹果不能出现在蛇身上
		//随机出来的坐标与蛇的每一节蛇身进行比较
		pMark = pHead;
		while(pMark != NULL)
		{
			if(x == pMark->x && y == pMark->y)
			{
				break;
			}
			pMark = pMark->pNext;
		}
	}while(pMark != NULL);

	apple.x = x;
	apple.y = y;
}

BOOL BumpWall()
{
	if(pHead->x == 0 || pHead->x == 19 || pHead->y == 0 || pHead->y == 19)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL EatItself()
{
	Snake *pMark = pHead->pNext;

	while(pMark != NULL)
	{
		if(pMark->x == pHead->x && pMark->y == pHead->y)
		{
			return TRUE;
		}
		pMark = pMark->pNext;
	}

	return FALSE;
}
