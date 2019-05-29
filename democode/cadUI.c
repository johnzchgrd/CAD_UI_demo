#include"cadUI.h"
//#define _CONSOLE //是否出现调试控制台

/***** 本文件全局变量 *******/

static char * selectedLabel = NULL;//保存上次选择的标签


// 用户的键盘事件响应函数
void KeyboardEventProcess(int key, int event)
{
	uiGetKeyboard(key, event); // GUI获取键盘
	display(); // 刷新显示
}

// 用户的鼠标事件响应函数
void MouseEventProcess(int x, int y, int button, int event)
{
	uiGetMouse(x, y, button, event); //GUI获取鼠标
	display(); // 刷新显示
}

/****** 用户主程序入口 ******/
// 仅初始化执行一次
void Main()
{
	// 初始化窗口和图形系统
	SetWindowTitle("CAD UI Demo");
	//SetWindowSize(10, 10); // 单位 - 英寸
	//SetWindowSize(20, 10);
	//SetWindowSize(10, 20);  // 如果屏幕尺寸不够，则按比例缩小
	InitGraphics();

	// 获得窗口尺寸
	winwidth = GetWindowWidth();
	winheight = GetWindowHeight();

	// 注册时间响应函数
	registerKeyboardEvent(KeyboardEventProcess);// 键盘
	registerMouseEvent(MouseEventProcess);      // 鼠标


	// 打开控制台，方便输出变量信息，便于调试
	#ifdef _CONSOLE
		InitConsole(); 
	#endif // _CONSOLE
}

/***** 函数定义 *******/
/*
函数原型：void drawMenu(void)
功能描述：画出菜单栏，并对用户的选择进行处理
参数描述：void
返回值描述：void
重要局部变量定义：
重要局部变量用途描述：
函数算法描述：
*/
void drawMenu()
{
	static char * menuListFile[] = { "文件",
		"新建  | Ctrl-N",
		"打开  | Ctrl-O", // 快捷键必须采用[Ctrl-X]格式，放在字符串的结尾
		"保存  | Ctrl-S",
		"关闭  | Ctrl-W",
		"退出   | Ctrl-E" ,
		"首选项" };
	static char * menuListTool[] = { "编辑",
		"撤销上一步",
		"颜色   >",
		"线条   >" ,
		"填充   >" };
	static char * menuListDraw[] = { "绘图",
		"函数",
		"显示网格" };
	static char * menuListMeasure[] = { "度量",
		"长度",
		"面积",
		"取点" };
	static char * menuListWindow[] = { "窗口",
		"切换显示模式",//切换打开的文件，分屏同时显示，文件名自动替换
		"窗口1",
		"窗口2",
		"..."};
	static char * menuListHelp[] = { "帮助",
		"帮助信息  | Ctrl-H",
		"关于本程序" };

	double fH = GetFontHeight();
	double x = 0; //fH/8;
	double y = winheight;
	double h = fH * 1.5; // 控件高度，高度相同
	double w = TextStringWidth(menuListHelp[0]) * 2; // 选项卡宽度
	double wlist = TextStringWidth(menuListTool[3])*2.5;//列表宽度
	//double xindent = winheight / 20; // 缩进
	int    selection;
	x += w;//防止挡住工具条造成重叠，若能实现填充菜单项背景功能，可将此语句删除

	//--菜单实例化--//
	// File 菜单
	selection = menuList(GenUIID(0), x, y - h, w, wlist, h, menuListFile, sizeof(menuListFile) / sizeof(menuListFile[0]),0);
	if (selection > 0)	selectedLabel = menuListFile[selection];
	//退出
	if (selection == 5)
		exit(-1); // 
	
	// Tool 菜单
	selection = menuList(GenUIID(0), x + w, y - h, w, wlist, h, menuListTool, sizeof(menuListTool) / sizeof(menuListTool[0]), 0);
	if (selection > 0) selectedLabel = menuListTool[selection];

	//Draw 菜单
	selection = menuList(GenUIID(0), x+2*w, y - h, w, wlist, h, menuListDraw, sizeof(menuListDraw) / sizeof(menuListDraw[0]), 0);
	if (selection > 0) selectedLabel = menuListDraw[selection];

	//Measure 菜单
	selection = menuList(GenUIID(0), x+3*w, y - h, w, wlist, h, menuListMeasure, sizeof(menuListMeasure) / sizeof(menuListMeasure[0]), 0);
	if (selection > 0) selectedLabel = menuListMeasure[selection];

	//Window菜单
	selection = menuList(GenUIID(0), x+4*w, y - h, w, wlist, h, menuListWindow, sizeof(menuListWindow) / sizeof(menuListWindow[0]), 0);
	if (selection > 0) selectedLabel = menuListWindow[selection];

	// Help 菜单
	selection = menuList(GenUIID(0), x + 5 * w, y - h, w, wlist, h, menuListHelp, sizeof(menuListHelp) / sizeof(menuListHelp[0]), 0);
	if (selection > 0) {
		selectedLabel = menuListHelp[selection];
		switch (selection) {
		case 1://显示帮助界面
			WinExec("notepad ./readme.txt", SW_SHOW);
			break;
		case 2://显示关于界面，需要更新
			MessageBox(NULL, "此程序遵循GNU GENERAL PUBLIC LICENSE\n参考http://www.gnu.org/licenses/gpl-3.0.html\n开发人员：xxx | xxx | xxx\n\n激活状态：60天试用（为什么?我也不知道）", "关于本程序", MB_OK | MB_ICONINFORMATION);
			break;
		default:break;
		}
	}

	
}

/*
函数原型：void drawtoolbar(void)
功能描述：画出左侧工具栏，并提供控制功能实现接口
参数描述：void
返回值描述：void
重要局部变量定义：int selection 
重要局部变量用途描述：选择了当前工具选项卡中第几个工具项
函数算法描述：调用menuList函数，逐一实例化工具选项卡，绘制方向设定为向下
*/
void drawtoolbar(void) {
	int selection;
	double x, w, y, h, wlist,a,b;
	double fH = GetFontHeight();
	
	static char* toolbarChoose[] = {"箭头>",
		"选择",
		"旋转",
		"平移"
	};

	static char* toolbarDot[] = { "点"
	};
	static char* toolbarCircle[] = { "圆"
	};
	static char* toolbarLine[] = { "线 >",
		"线段",
		"射线",
		"直线"
	};
	static char* toolbarText[] = { "文本"
	};
	static char* toolbarMark[] = { "标记"
	};

	x = 0;
	y = winheight;
	h = fH * 3;
	w = TextStringWidth(toolbarChoose[0])*1.5;
	
	//工具条实例化
	selection = menuList(GenUIID(0), x , y - 2 * h, w, w, h, toolbarChoose, sizeof(toolbarChoose) / sizeof(toolbarChoose[0]),1);
	if (selection > 0) selectedLabel = toolbarChoose[selection];

	selection = menuList(GenUIID(0), x, y - 3 * h, w, w, h, toolbarDot, sizeof(toolbarDot) / sizeof(toolbarDot[0]), 1);
	if (selection > 0) selectedLabel = toolbarDot[0];

	selection = menuList(GenUIID(0), x, y - 4 * h, w, w, h, toolbarCircle, sizeof(toolbarCircle) / sizeof(toolbarCircle[0]), 1);
	if (selection > 0) selectedLabel = toolbarCircle[0];

	selection = menuList(GenUIID(0), x, y - 5 * h, w, w, h, toolbarLine, sizeof(toolbarLine) / sizeof(toolbarLine[0]), 1);
	if (selection > 0) selectedLabel = toolbarLine[selection];

	selection = menuList(GenUIID(0), x, y - 6 * h, w, w, h, toolbarText, sizeof(toolbarText) / sizeof(toolbarText[0]), 1);
	if (selection > 0) selectedLabel = toolbarText[0];

	selection = menuList(GenUIID(0), x, y - 7 * h, w, w, h, toolbarMark, sizeof(toolbarMark) / sizeof(toolbarMark[0]), 1);
	if (selection > 0) selectedLabel = toolbarMark[0];


}

void showMenuState(void) {
	//无法显示无子菜单的工具选项卡
	//在左下角显示最近选择的菜单项
	SetPenColor("black");
	drawLabel(0.1, 0.1, "最近选项：");
	SetPenColor("Red");
	drawLabel(0.1 + TextStringWidth("最近选项："), 0.1, selectedLabel);
}


// 按钮演示程序
void drawButtons(void)
{
	double fH = GetFontHeight();
	double h = fH * 2;  // 控件高度
	double x = winwidth / 2;
	double y = winheight / 2 - h;
	double w = TextStringWidth("退出") * 2; // 控件宽度

	if (button(GenUIID(0), x, y, w, h, "退出")) {
		exit(-1);
	}

}

//void drawLOGO(void) {
//	char* logo[] = "CAD";
//	SetPenColor("Red");
//	drawLabel(0, winheight - GetFontHeight()*1.5, logo);
//}
/*
函数原型：void display(void)
功能描述:显示
参数描述：
返回值描述：
重要局部变量定义：
重要局部变量用途描述：
函数算法描述：
*/
void display(void)
{
	// 清屏
	DisplayClear();
	// 绘制和处理菜单
	//drawLOGO();
	drawMenu();
	drawtoolbar();
	showMenuState();
	drawButtons();
}