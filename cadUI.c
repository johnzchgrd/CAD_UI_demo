#include "graphics.h"
#include "extgraph.h"
#include "genlib.h"
#include "simpio.h"
#include "conio.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include <windows.h>
#include <olectl.h>
#include <mmsystem.h>
#include <wingdi.h>
#include <ole2.h>
#include <ocidl.h>
#include <winuser.h>

#include "imgui.h"
//#define _CONSOLE //�Ƿ���ֵ��Կ���̨

/***** ���ļ�ȫ�ֱ��� *******/
static double winwidth, winheight;   // ���ڳߴ�
static char * selectedLabel = NULL;//�����ϴ�ѡ��ı�ǩ

/***** �������� *******/
// ����������provided in libgraphics
void DisplayClear(void);
//��ʾ��๤��������
void drawtoolbar(void);
// ��ʾ��ǰ���ֺ���
void display(void);

// �û��ļ����¼���Ӧ����
void KeyboardEventProcess(int key, int event)
{
	uiGetKeyboard(key, event); // GUI��ȡ����
	display(); // ˢ����ʾ
}

// �û�������¼���Ӧ����
void MouseEventProcess(int x, int y, int button, int event)
{
	uiGetMouse(x, y, button, event); //GUI��ȡ���
	display(); // ˢ����ʾ
}

/****** �û���������� ******/
// ����ʼ��ִ��һ��
void Main()
{
	// ��ʼ�����ں�ͼ��ϵͳ
	SetWindowTitle("CAD UI Demo");
	//SetWindowSize(10, 10); // ��λ - Ӣ��
	//SetWindowSize(20, 10);
	//SetWindowSize(10, 20);  // �����Ļ�ߴ粻�����򰴱�����С
	InitGraphics();

	// ��ô��ڳߴ�
	winwidth = GetWindowWidth();
	winheight = GetWindowHeight();

	// ע��ʱ����Ӧ����
	registerKeyboardEvent(KeyboardEventProcess);// ����
	registerMouseEvent(MouseEventProcess);      // ���


	// �򿪿���̨���������������Ϣ�����ڵ���
	#ifdef _CONSOLE
		InitConsole(); 
	#endif // _CONSOLE
}

/***** �������� *******/
/*
����ԭ�ͣ�void drawMenu(void)
����������
����������
����ֵ������
��Ҫ�ֲ��������壺
��Ҫ�ֲ�������;������
�����㷨������
*/
void drawMenu()
{
	static char * menuListFile[] = { "�ļ�",
		"�½�  | Ctrl-N",
		"��  | Ctrl-O", // ��ݼ��������[Ctrl-X]��ʽ�������ַ����Ľ�β
		"����  | Ctrl-S",
		"�ر�  | Ctrl-W",
		"�˳�   | Ctrl-E" ,
		"��ѡ��" };
	static char * menuListTool[] = { "�༭",
		"������һ��",
		"��ɫ   >",
		"����   >" ,
		"���   >" };
	static char * menuListDraw[] = { "��ͼ",
		"����",
		"��ʾ����" };
	static char * menuListMeasure[] = { "����",
		"����",
		"���",
		"ȡ��" };
	static char * menuListWindow[] = { "����",
		"�л���ʾģʽ",//�л��򿪵��ļ�������ͬʱ��ʾ���ļ����Զ��滻
		"����1",
		"����2",
		"..."};
	static char * menuListHelp[] = { "����",
		"������Ϣ  | Ctrl-H",
		"���ڱ�����" };

	double fH = GetFontHeight();
	double x = 0; //fH/8;
	double y = winheight;
	double h = fH * 1.5; // �ؼ��߶ȣ��߶���ͬ
	double w = TextStringWidth(menuListHelp[0]) * 2; // ѡ����
	double wlist = TextStringWidth(menuListTool[3])*2;//�б���
	//double xindent = winheight / 20; // ����
	int    selection;
	x += w;//��ֹ��ס����������ص�������ʵ�����˵�������ܣ��ɽ������ɾ��

	//�˵�ʵ����
	// File �˵�
	selection = menuList(GenUIID(0), x, y - h, w, wlist, h, menuListFile, sizeof(menuListFile) / sizeof(menuListFile[0]),0);
	if (selection > 0)	selectedLabel = menuListFile[selection];
	//�˳�
	if (selection == 5)
		exit(-1); // 
	
	// Tool �˵�
	selection = menuList(GenUIID(0), x + w, y - h, w, wlist, h, menuListTool, sizeof(menuListTool) / sizeof(menuListTool[0]), 0);
	if (selection > 0) selectedLabel = menuListTool[selection];

	//Draw �˵�
	selection = menuList(GenUIID(0), x+2*w, y - h, w, wlist, h, menuListDraw, sizeof(menuListDraw) / sizeof(menuListDraw[0]), 0);
	if (selection > 0) selectedLabel = menuListDraw[selection];

	//Measure �˵�
	selection = menuList(GenUIID(0), x+3*w, y - h, w, wlist, h, menuListMeasure, sizeof(menuListMeasure) / sizeof(menuListMeasure[0]), 0);
	if (selection > 0) selectedLabel = menuListMeasure[selection];

	//Window�˵�
	selection = menuList(GenUIID(0), x+4*w, y - h, w, wlist, h, menuListWindow, sizeof(menuListWindow) / sizeof(menuListWindow[0]), 0);
	if (selection > 0) selectedLabel = menuListWindow[selection];

	// Help �˵�
	selection = menuList(GenUIID(0), x + 5 * w, y - h, w, wlist, h, menuListHelp, sizeof(menuListHelp) / sizeof(menuListHelp[0]), 0);
	if (selection > 0) selectedLabel = menuListHelp[selection];

	
	//�����½���ʾ���ѡ��Ĳ˵���
	SetPenColor("black");
	drawLabel(0.1, 0.1, "���ѡ��:");
	SetPenColor("Red");
	drawLabel(0.1+ TextStringWidth("���ѡ��:"),0.1, selectedLabel);
	
}

/*
����ԭ�ͣ�void drawtoolbar(void)
����������������๤���������ṩ���ƹ���ʵ�ֽӿ�
����������void
����ֵ������void
��Ҫ�ֲ��������壺int selection 
��Ҫ�ֲ�������;������ѡ���˵�ǰ����ѡ��еڼ���������
�����㷨����������menuList��������һʵ��������ѡ������Ʒ����趨Ϊ����
*/
void drawtoolbar() {
	int selection;
	double x, w, y, h, wlist,a,b;
	double fH = GetFontHeight();
	
	static char* toolbarChoose[] = {"��ͷ>",
		"ѡ��",
		"��ת",
		"ƽ��"
	};

	static char* toolbarDot[] = { "��"
	};
	static char* toolbarCircle[] = { "Բ"
	};
	static char* toolbarLine[] = { "�� >",
		"�߶�",
		"����",
		"ֱ��"
	};
	static char* toolbarText[] = { "�ı�"
	};
	static char* toolbarMark[] = { "���"
	};

	x = 0;
	y = winheight;
	h = fH * 3;
	w = TextStringWidth(toolbarChoose[0])*1.5;
	a = winwidth / 15;
	b = winheight / 8 * 7;
	
	//������ʵ����
	selection = menuList(GenUIID(0), x , y - 2 * h, w, w, h, toolbarChoose, sizeof(toolbarChoose) / sizeof(toolbarChoose[0]),1);
	if (selection > 0) selectedLabel = toolbarChoose[selection];

	selection = menuList(GenUIID(0), x, y - 3 * h, w, w, h, toolbarDot, sizeof(toolbarDot) / sizeof(toolbarDot[0]), 1);
	if (selection > 0) selectedLabel = toolbarDot[selection];

	selection = menuList(GenUIID(0), x, y - 4 * h, w, w, h, toolbarCircle, sizeof(toolbarCircle) / sizeof(toolbarCircle[0]), 1);
	if (selection > 0) selectedLabel = toolbarCircle[selection];

	selection = menuList(GenUIID(0), x, y - 5 * h, w, w, h, toolbarLine, sizeof(toolbarLine) / sizeof(toolbarLine[0]), 1);
	if (selection > 0) selectedLabel = toolbarLine[selection];

	selection = menuList(GenUIID(0), x, y - 6 * h, w, w, h, toolbarText, sizeof(toolbarText) / sizeof(toolbarText[0]), 1);
	if (selection > 0) selectedLabel = toolbarText[selection];

	selection = menuList(GenUIID(0), x, y - 7 * h, w, w, h, toolbarMark, sizeof(toolbarMark) / sizeof(toolbarMark[0]), 1);
	if (selection > 0) selectedLabel = toolbarMark[selection];


}

/*
����ԭ�ͣ�void display(void)
��������:��ʾ
����������
����ֵ������
��Ҫ�ֲ��������壺
��Ҫ�ֲ�������;������
�����㷨������
*/
void display()
{
	// ����
	DisplayClear();
	// ���ƺʹ���˵�
	drawMenu();
	drawtoolbar();
}