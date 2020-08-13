// k-means��OpenCV.cpp : ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include "k-means��OpenCV.h"
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
// ���� ����:
HINSTANCE hInst;                                // ���� �ν��Ͻ��Դϴ�.
WCHAR szTitle[MAX_LOADSTRING];                  // ���� ǥ���� �ؽ�Ʈ�Դϴ�.
WCHAR szWindowClass[MAX_LOADSTRING];            // �⺻ â Ŭ���� �̸��Դϴ�.

// �� �ڵ� ��⿡ ��� �ִ� �Լ��� ������ �����Դϴ�.
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

    // TODO: ���⿡ �ڵ带 �Է��մϴ�.

    // ���� ���ڿ��� �ʱ�ȭ�մϴ�.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_KMEANSOPENCV, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // ���� ���α׷� �ʱ�ȭ�� �����մϴ�.
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_KMEANSOPENCV));

    MSG msg;

    // �⺻ �޽��� �����Դϴ�.
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
//  �Լ�: MyRegisterClass()
//
//  ����: â Ŭ������ ����մϴ�.
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
//   �Լ�: InitInstance(HINSTANCE, int)
//
//   ����: �ν��Ͻ� �ڵ��� �����ϰ� �� â�� ����ϴ�.
//
//   ����:
//
//        �� �Լ��� ���� �ν��Ͻ� �ڵ��� ���� ������ �����ϰ�
//        �� ���α׷� â�� ���� ���� ǥ���մϴ�.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // �ν��Ͻ� �ڵ��� ���� ������ �����մϴ�.

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
//Program : K-means�� �̿��� �̹��� ����
//Start_Date : 2017.06
//End_Date : 2017.08
//Name : �̽���

#define MAX_FIXEL 1000000
int data[MAX_FIXEL][4];  //ù�� ° �� �ȼ� max��  ������ �Ҽ� �׷� ��ȣ
int Cluster[1000][3]; //ù�� ° �� Ŭ������ max��
int Cluster_num = 2; //Ŭ������ ����
int pixel_num; //���� �ȼ� ����
int initiate_check = 0; //�ʱ⿡ Ŭ������ �߽� ��ġ�� 3������������ �����ϴ� �ȼ� ������������ �����ϰ� �� �ڷ� Ŭ�����͸��� ���� ��� ���� ��ġ�� ������������ �׳� ���
void make_data(Mat image) //���� �ȼ��� �ֱ�
{
	for (int i = 0; i < image.rows; i++)
	{
		for (int j = 0; j < image.cols; j++)
		{
			data[i*image.cols + j][0] = image.at<Vec3b>(i, j)[0]; //�Ķ�
			data[i*image.cols + j][1] = image.at<Vec3b>(i, j)[1]; //�ʷ�
			data[i*image.cols + j][2] = image.at<Vec3b>(i, j)[2]; //����
		}
	}
	pixel_num = image.rows*image.cols;
}
void Set_Cluster_initiate()//�ʱ� Ŭ������ ��ġ ����(�������� �ȼ����� ������ �ִ� ���� �� �ϳ� ����)
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
void Set_Cluster_k_means() //������ ���鼭 �����ڽŰ� ���� ����� Ŭ������ ã��
{
	std::vector<double> distance(MAX_FIXEL,987654321);//����� ������ Ŭ�������߽ɱ����ǰŸ�
	//for (int i = 0; i < pixel_num; i++)distance[i] = 987654321;

	for (int i = 0; i < Cluster_num; i++)//3���� ���� �������� �Ÿ� ������ ���� ����� Ŭ������ ã��
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

double dis_cluster[MAX_FIXEL][4] = { 0, };  //������ �׷�� ����
void Set_ClusterPos()//�ڱ�Ҽ������� ���鼭 Ŭ������ �߽��缳��(��� ������ ��ǥ�� ���)
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
Mat Change_image(Mat image)//��� Ŭ�����Ϳ� ���� �����ʹ� �� Ŭ�������� ��ǥ ����� ��ȯ (��ǥ������ � Ŭ�������� ���� �������� �ȼ������� ���)
{
	for (int i = 0; i < image.rows; i++)//���� �ȼ��� ��ȯ
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
            // �޴� ������ ���� �м��մϴ�.
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

            // TODO: ���⿡ hdc�� ����ϴ� �׸��� �ڵ带 �߰��մϴ�.
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

// ���� ��ȭ ������ �޽��� ó�����Դϴ�.
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
