#pragma once
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
#include <math.h>  

#include "imgui.h"
double winwidth, winheight;   // ���ڳߴ�
/***** �������� *******/

//UI����
// ����������provided in libgraphics
void DisplayClear(void);
//��ʾ��๤��������
void drawtoolbar(void);
// ��ʾ��ǰ���ֺ���
void display(void);
//������ť����
void drawButtons(void);
//������-��ͼ����


//�Ƿ��˳�����
void ExitornotBox(void);