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
double winwidth, winheight;   // 窗口尺寸
/***** 函数声明 *******/

//UI界面
// 清屏函数，provided in libgraphics
void DisplayClear(void);
//显示左侧工具栏函数
void drawtoolbar(void);
// 显示当前布局函数
void display(void);
//画出按钮函数
void drawButtons(void);
//工具栏-绘图功能


//是否退出函数
void ExitornotBox(void);