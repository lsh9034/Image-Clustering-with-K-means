// k-means와OpenCV.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "k-means와OpenCV.h"
#include<algorithm>
#include"opencv2\core\core.hpp"
#include<opencv2\highgui/highgui.hpp>
#include"opencv2/opencv.hpp"
#include"opencv/cv.h"
#include<iostream>
//#include<stdlib.h>
#include <opencv2/imgproc/imgproc.hpp>
#include<stack>
#include <functional>
#include<time.h>
#include<random>
#define MAX_LOADSTRING 100
using namespace cv;
// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_KMEANSOPENCV, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 응용 프로그램 초기화를 수행합니다.
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_KMEANSOPENCV));

    MSG msg;

    // 기본 메시지 루프입니다.
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_KMEANSOPENCV));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_KMEANSOPENCV);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}
//Program : K-means를 이용한 이미지 분할
//Start_Date : 2017.06
//End_Date : 2017.08
//Name : 이시현

#define MAX_FIXEL 1000000
int data[MAX_FIXEL][4];  //첫번 째 수 픽셀 max수  끝에는 소속 그룹 번호
int Cluster[1000][3]; //첫번 째 수 클러스터 max수
int Cluster_num = 2; //클러스터 갯수
int pixel_num; //사진 픽셀 개수
int initiate_check = 0; //초기에 클러스터 중심 위치는 3차원공간에서 존재하는 픽셀 점포지션으로 선택하고 그 뒤로 클러스터링할 때는 어느 정도 위치가 잡혀있음으로 그냥 계산
void make_data(Mat image) //사진 픽셀값 넣기
{
	for (int i = 0; i < image.rows; i++)
	{
		for (int j = 0; j < image.cols; j++)
		{
			data[i*image.cols + j][0] = image.at<Vec3b>(i, j)[0]; //파랑
			data[i*image.cols + j][1] = image.at<Vec3b>(i, j)[1]; //초록
			data[i*image.cols + j][2] = image.at<Vec3b>(i, j)[2]; //빨강
		}
	}
	pixel_num = image.rows*image.cols;
}
void Set_Cluster_initiate()//초기 클러스터 위치 설정(랜덤으로 픽셀들이 가지고 있는 값들 중 하나 선택)
{
	srand(time(NULL));
	std::vector<int>check(pixel_num, 0);
	for (int i = 0; i < Cluster_num; i++)
	{
		int temp = rand() % (pixel_num); 
		if (check[temp] == 1)
		{
			i--;
			continue;
		}
		check[temp] = 1;
		Cluster[i][0] = data[temp][0];
		Cluster[i][1] = data[temp][1];
		Cluster[i][2] = data[temp][2];
	}
}
void Set_Cluster_k_means() //점들을 돌면서 점들자신과 가장 가까운 클러스터 찾기
{
	std::vector<double> distance(MAX_FIXEL,987654321);//점들과 설정한 클러스터중심까지의거리
	//for (int i = 0; i < pixel_num; i++)distance[i] = 987654321;

	for (int i = 0; i < Cluster_num; i++)//3차원 점과 점사이의 거리 공식을 통해 가까운 클러스터 찾기
	{
		double dis;
		for (int j = 0; j < pixel_num; j++)
		{
			dis = (Cluster[i][0] - data[j][0])*(Cluster[i][0] - data[j][0]) +
				  (Cluster[i][1] - data[j][1])*(Cluster[i][1] - data[j][1]) +
				  (Cluster[i][2] - data[j][2])*(Cluster[i][2] - data[j][2]);
			if (dis < distance[j])
			{
				distance[j] = dis;
				data[j][3] = i;
			}
		}
	}
}

double dis_cluster[MAX_FIXEL][4] = { 0, };  //끝에는 그룹원 갯수
void Set_ClusterPos()//자기소속점들을 돌면서 클러스터 중심재설정(모든 데이터 좌표의 평균)
{
	for (int i = 0; i < pixel_num; i++)
	{
		dis_cluster[data[i][3]][0] += data[i][0];
		dis_cluster[data[i][3]][1] += data[i][1];
		dis_cluster[data[i][3]][2] += data[i][2];
		dis_cluster[data[i][3]][3]++;
	}
	for (int i = 0; i < Cluster_num; i++)
	{
		if (dis_cluster[i][3] == 0)continue;
		Cluster[i][0] = dis_cluster[i][0] / dis_cluster[i][3];
		Cluster[i][1] = dis_cluster[i][1] / dis_cluster[i][3];
		Cluster[i][2] = dis_cluster[i][2] / dis_cluster[i][3];
	}
}
Mat Change_image(Mat image)//어떠한 클러스터에 속한 데이터는 그 클러스터의 대표 색깔로 변환 (대표색깔은 어떤 클러스터의 속한 데이터의 픽셀값들의 평균)
{
	for (int i = 0; i < image.rows; i++)//사진 픽셀값 변환
	{
		for (int j = 0; j < image.cols; j++)
		{
			image.at<Vec3b>(i, j)[0] = Cluster[data[i*image.cols + j][3]][0];
			image.at<Vec3b>(i, j)[1] = Cluster[data[i*image.cols + j][3]][1];
			image.at<Vec3b>(i, j)[2] = Cluster[data[i*image.cols + j][3]][2];
		}
	}
	return image;
}
Mat image;
int count;
std::string name;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
		{
			CreateWindow(TEXT("BUTTON"), TEXT("Next"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 620, 475, 100, 25, hWnd, (HMENU)2, hInst, NULL);
			image = imread("car2.jpg");
			make_data(image);
			name = "image";
			imshow(name, image);
		}
	break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다.
            switch (wmId)
            {
			case 2:
			{
				if (!initiate_check)
				{
					initiate_check = 1;
					Set_Cluster_initiate();
				}
				Set_Cluster_k_means();
				Set_ClusterPos();
				count++;
				name = "image" + std::to_string(count);
				image = Change_image(image);
				imshow("image", image);
			}
				break;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다.
            EndPaint(hWnd, &ps);
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

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
