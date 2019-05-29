#include"cadUI.h"
//#define _CONSOLE //�Ƿ���ֵ��Կ���̨

/***** ���ļ�ȫ�ֱ��� *******/

static char * selectedLabel = NULL;//�����ϴ�ѡ��ı�ǩ


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
���������������˵����������û���ѡ����д���
����������void
����ֵ������void
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
	double wlist = TextStringWidth(menuListTool[3])*2.5;//�б���
	//double xindent = winheight / 20; // ����
	int    selection;
	x += w;//��ֹ��ס����������ص�������ʵ�����˵�������ܣ��ɽ������ɾ��

	//--�˵�ʵ����--//
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
	if (selection > 0) {
		selectedLabel = menuListHelp[selection];
		switch (selection) {
		case 1://��ʾ��������
			WinExec("notepad ./readme.txt", SW_SHOW);
			break;
		case 2://��ʾ���ڽ��棬��Ҫ����
			MessageBox(NULL, "�˳�����ѭGNU GENERAL PUBLIC LICENSE\n�ο�http://www.gnu.org/licenses/gpl-3.0.html\n������Ա��xxx | xxx | xxx\n\n����״̬��60�����ã�Ϊʲô?��Ҳ��֪����", "���ڱ�����", MB_OK | MB_ICONINFORMATION);
			break;
		default:break;
		}
	}

	
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
void drawtoolbar(void) {
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
	
	//������ʵ����
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
	//�޷���ʾ���Ӳ˵��Ĺ���ѡ�
	//�����½���ʾ���ѡ��Ĳ˵���
	SetPenColor("black");
	drawLabel(0.1, 0.1, "���ѡ�");
	SetPenColor("Red");
	drawLabel(0.1 + TextStringWidth("���ѡ�"), 0.1, selectedLabel);
}


// ��ť��ʾ����
void drawButtons(void)
{
	double fH = GetFontHeight();
	double h = fH * 2;  // �ؼ��߶�
	double x = winwidth / 2;
	double y = winheight / 2 - h;
	double w = TextStringWidth("�˳�") * 2; // �ؼ����

	if (button(GenUIID(0), x, y, w, h, "�˳�")) {
		exit(-1);
	}

}

//void drawLOGO(void) {
//	char* logo[] = "CAD";
//	SetPenColor("Red");
//	drawLabel(0, winheight - GetFontHeight()*1.5, logo);
//}
/*
����ԭ�ͣ�void display(void)
��������:��ʾ
����������
����ֵ������
��Ҫ�ֲ��������壺
��Ҫ�ֲ�������;������
�����㷨������
*/
void display(void)
{
	// ����
	DisplayClear();
	// ���ƺʹ���˵�
	//drawLOGO();
	drawMenu();
	drawtoolbar();
	showMenuState();
	drawButtons();
}