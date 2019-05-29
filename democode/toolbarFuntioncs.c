/* 点击按钮，选择不同状态 */   
/* 选择状态下，左键移动物体 */  
/* 选择状态下，右键改变大小 */  
/* 选择状态下，先选中，再delete和backspace删除图形 */  
/* 文字状态下，先点击一处作为起始点，可以左移、右移、del、backspace*/   
  
#include "cadUI.h"
  
#define chooseStartX 0.25  
#define chooseStopX 0.8  
#define chooseStartY 0.6  
#define chooseStopY 0.9  
  
#define rectangleHeight 0.3  
#define rectangleWidth 0.55  
  
#define lineStartX 0.9  
#define lineStopX 1.45  
#define lineStartY 0.6  
#define lineStopY 0.9  

#define arrowlineStartX 1.6
#define arrowlineStopX 2.15
#define arrowlineStartY 0.6
#define arrowlineStopY 0.9
  
#define rectangleStartX 2.3  
#define rectangleStopX 2.85  
#define rectangleStartY 0.6  
#define rectangleStopY 0.9  
  
#define diamondStartX 3  
#define diamondStopX 3.55  
#define diamondStartY 0.6  
#define diamondStopY 0.9  
 
#define roundedStartX 3.7  
#define roundedStopX 4.25  
#define roundedStartY 0.6  
#define roundedStopY 0.9

#define ellipseStartX 4.3  
#define ellipseStopX 4.95  
#define ellipseStartY 0.6  
#define ellipseStopY 0.9
 
#define textStartX 5  
#define textStopX 5.65  
#define textStartY 0.6  
#define textStopY 0.9  
  
#define M_PI 3.1415926

#define TIMER_BLINK 1  
  
enum{line,arrowline,rectangle,diamond,rounded,ellipse,text,null};  
  
const int mseconds = 1000;  
  
static int choosenow=0;  
static bool isChoose=FALSE;  
static bool startDrawText=FALSE;  
static bool isSave=TRUE;  
  
static double tx,ty;  
static int start=-1;  
  
char Text[100];  
int textlen=0;  
static int location=-1;  

static double G_angle = 0;
  
static bool isBlink = FALSE;    
static bool isDisplay = FALSE;   
  
typedef struct node save;  
static int count=0;  
  
double x_location,y_location;  
  
struct node  
{  
    double x1;  
    double y1;  
    double x2;  
    double y2;  
    int type;  
};  
save onScreen[100];  
//判断是否在选区内   
bool inBox(double x0, double y0, double x1, double x2, double y1, double y2)  
{  
    return (x0 >= x1 && x0 <= x2 && y0 >= y1 && y0 <= y2)||  
           (x0 >= x1 && x0 <= x2 && y0 <= y1 && y0 >= y2)||  
           (x0 <= x1 && x0 >= x2 && y0 >= y1 && y0 <= y2)||  
           (x0 <= x1 && x0 >= x2 && y0 <= y1 && y0 >= y2);  
};  

//画矩形   
void DrawRectangle(double x,double y)  
{  
    DrawLine(x,0);  
    DrawLine(0,-y);  
    DrawLine(-x,0);  
    DrawLine(0,y);  
}  
//画箭头的直线
void DrawArrowline(double x,double y)
{
	double x1,y1,m;
	DrawLine(x,y);
	x1=x+y*tan(M_PI/6);
	y1=y+x*tan(M_PI/6);
	m=sqrt(x1*x1+y1*y1);
	DrawLine(-x1/m/5,-y1/m/5);
	DrawLine(x1/m/5,y1/m/5);
	x1=x-y*tan(M_PI/6);
	y1=y-x*tan(M_PI/6);
	m=sqrt(x1*x1+y1*y1);
	DrawLine(-x1/m/5,-y1/m/5);
} 
//画菱形 
void DrawDiamond(double x,double y)
{
	double x1,y1; 
	x1 = GetCurrentX();
	y1 = GetCurrentY();
	MovePen(x1,y1+y/2);
	DrawLine(x/2,y/2);
	DrawLine(x/2,-y/2);
	DrawLine(-x/2,-y/2);
	DrawLine(-x/2,y/2);
}
//画圆角矩形 
void DrawRounded(double x,double y)
{
	double x1,y1,m; 
	x1 = GetCurrentX();
	y1 = GetCurrentY();
	if(x<0&&y>=0){
		x1=x1+x;
		x=-x;
	}
	else if(y<0&&x>=0){
		y1=y+y1;
		y=-y;
	}
	else if(x<0&&y<0){
		x1=x1+x;
		y1=y1+y;
		x=-x;y=-y;
	}
	if(x>y)m=y;
	else m=x;
	MovePen(x1,y1+m*0.2);
	DrawArc(m*0.2,180,90);
	DrawLine(x-m*0.4,0);
	DrawArc(m*0.2,270,90);
	DrawLine(0,y-m*0.4);
	DrawArc(m*0.2,0,90);
	DrawLine(-x+m*0.4,0);
	DrawArc(m*0.2,90,90);
	DrawLine(0,-y+m*0.4);
}

void forward(double distance){
	DrawLine(distance*cos(G_angle*M_PI/180.0),distance*sin(G_angle*M_PI/180.0));
}

void turn (double angle){
	G_angle -= angle;
}

void move(double distance){
	MovePen(GetCurrentX()+distance*cos(G_angle*M_PI/180.0),GetCurrentY()+distance*sin(G_angle*M_PI/180.0));
}

  
//重新绘制图形   
void ReDraw()  
{  
    int angle;
	int i,cnt=0;  
    for(i=0;i<count;i++){  
        if(onScreen[i].type==line){  
            MovePen(onScreen[i].x1,onScreen[i].y1);  
            DrawLine(onScreen[i].x2-onScreen[i].x1,onScreen[i].y2-onScreen[i].y1);  
        }/*不带箭头的直线*/  
        if(onScreen[i].type==arrowline){  
            MovePen(onScreen[i].x1,onScreen[i].y1);  
            DrawArrowline(onScreen[i].x2-onScreen[i].x1,onScreen[i].y2-onScreen[i].y1);  
        }/*带箭头的直线*/ 
        else if(onScreen[i].type==rectangle){  
            MovePen(onScreen[i].x1,onScreen[i].y1);  
            DrawRectangle(onScreen[i].x2-onScreen[i].x1,onScreen[i].y1-onScreen[i].y2);  
        }/*矩形*/ 
		else if(onScreen[i].type==diamond){ 
			MovePen(onScreen[i].x1,onScreen[i].y1);  
            DrawDiamond(onScreen[i].x2-onScreen[i].x1,onScreen[i].y2-onScreen[i].y1);  
		}/*菱形*/ 
		else if(onScreen[i].type==rounded){
			MovePen(onScreen[i].x1,onScreen[i].y1);  
            DrawRounded(onScreen[i].x2-onScreen[i].x1,onScreen[i].y2-onScreen[i].y1);  
		}/*圆角矩形*/ 
        else if(onScreen[i].type==ellipse){  
            MovePen(onScreen[i].x1,onScreen[i].y1);  
            DrawEllipticalArc((onScreen[i].x1-onScreen[i].x2)/2.0,(onScreen[i].y2-onScreen[i].y1),0,360);  
        }/*椭圆*/   
        else if(onScreen[i].type==text){  
            MovePen(onScreen[i].x1,onScreen[i].y1);  
            int j;  
            char str[2]={0,0};  
            for(j=onScreen[i].x2;j<=onScreen[i].y2;j++){  
                str[0]=Text[j];  
                DrawTextString(str);  
            }  
        }/*文字*/   
    }  
}  
  
//基本布局   
void Set()  
{  
    MovePen(chooseStartX,chooseStopY);  
    DrawRectangle(rectangleWidth,rectangleHeight);  
    MovePen(lineStartX,lineStopY);/*1*/
	DrawRectangle(rectangleWidth,rectangleHeight);  
    MovePen(arrowlineStartX,arrowlineStopY);/*2*/  
    DrawRectangle(rectangleWidth,rectangleHeight);  
    MovePen(rectangleStartX,rectangleStopY);/*3*/  
    DrawRectangle(rectangleWidth,rectangleHeight);
	MovePen(diamondStartX,diamondStopY);/*4*/  
    DrawRectangle(rectangleWidth,rectangleHeight);  
    MovePen(roundedStartX,roundedStopY);/*5*/  
    DrawRectangle(rectangleWidth,rectangleHeight);   
    MovePen(ellipseStartX,ellipseStopY);/*6*/  
    DrawRectangle(rectangleWidth,rectangleHeight);  
    MovePen(textStartX,textStopY);/*7*/  
    DrawRectangle(rectangleWidth,rectangleHeight);  
    MovePen(0.3,0.7);  
    
	DrawTextString("  select       ");  
    DrawTextString(" line     ");  
    DrawTextString("line->        "); 
	DrawTextString("rect   ");
	DrawTextString("    diam"); 
    DrawTextString("     r-rect");  
    DrawTextString("  ellip");
    DrawTextString("         text");
	
	MovePen(lineStartX,lineStopY+0.3);
	forward(0.55);
	
	MovePen(lineStartX+0.7,lineStopY+0.3);
	forward(0.55);
	turn(150);
	forward(0.1);
	move(-0.1);
	turn(60);
	forward(0.1);
	turn(150);
	
	MovePen(rectangleStartX,rectangleStopY+0.5);
	DrawRectangle(rectangleWidth,rectangleHeight);
	
	MovePen(diamondStartX,diamondStopY+0.33);
	turn(30);
	forward(0.318);
	turn(-60);
	forward(0.318);
	turn(-120);
	forward(0.318);
	turn(-60);
	forward(0.318);
	turn(-150);
	
	MovePen(roundedStartX+0.45,roundedStopY+0.5);
    DrawLine(-0.4,0);
    DrawArc(0.05,90,90);
    DrawLine(0,-0.2);
    DrawArc(0.05,180,90);
    DrawLine(0.4,0);
    DrawArc(0.05,270,90);  
    DrawLine(0,0.2);
    DrawArc(0.05,0,90);
	
	MovePen(ellipseStartX+0.55,ellipseStopY+0.3);
	DrawEllipticalArc(0.27, 0.1, 0, 360);
	
	MovePen(textStartX+0.15,textStopY+0.25);
	DrawTextString("Abc");
	   
}  
  
//插入图形对象   
void Insert(int type,double x1,double y1,double x2,double y2)  
{  
    onScreen[count].x1=x1;  
    onScreen[count].y1=y1;  
    onScreen[count].x2=x2;  
    onScreen[count].y2=y2;  
    onScreen[count].type=type;  
    count++;  
}  
  
//改变直线长度   
void ChangeLineLength(double x1,double y1,double x2,double y2,double sx,double sy,double mx,double my,double *lx,double *ly,double *rx,double *ry)  
{  
    double d=(x2-x1)*(my-sy)-(y2-y1)*(mx-sx)/sqrt((x1+x2)*(x1+x2)+(y1+y2)*(y1+y2));  
    double s=(mx-sx)*(mx-sx)+(my-sy)*(my-sy);  
    double l=sqrt(fabs(s-d*d));  
    if(x2==x1){   
        if((sy>(y1+y2)/2.0&&my>sy)||(sy<(y1+y2)/2.0&&my<sy)){  
            *ly-=l;  
            *ry+=2*l;  
        }  
        else if((sy>(y1+y2)/2.0&&my<sy)||(sy<(y1+y2)/2.0&&my>sy)){  
            *ly+=l;  
            *ry-=2*l;  
        }  
    }  
    else if(y2==y1){  
        if((sx>(x1+x2)/2.0&&mx>sx)||(sx<(x1+x2)/2.0&&mx<sx)){  
            *lx+=l;  
            *rx-=2*l;  
        }  
        else if((sx>(x1+x2)/2.0&&mx<sx)||(sx<(x1+x2)/2.0&&mx>sx)){  
            *lx-=l;  
            *rx+=2*l;  
        }  
    }   
    else{  
        double delta_y=l*sqrt((y1-y2)*(y1-y2)/((y1-y2)*(y1-y2)+(x1-x2)*(x1-x2)));  
        double delta_x=l*sqrt((x1-x2)*(x1-x2)/((y1-y2)*(y1-y2)+(x1-x2)*(x1-x2)));  
        if((x2>x1&&y2>y1)||(x2<x1&&y2<y1)){  
            if(((x1-mx)*(x1-mx)+(y1-my)*(y1-my))>((x2-mx)*(x2-mx)+(y2-my)*(y2-my))){  
                //点在右半部分   
                if(mx>sx)*lx-=delta_x,*rx+=2*delta_x,*ly-=delta_y,*ry+=2*delta_y;  
                else *lx+=delta_x,*rx-=2*delta_x,*ly+=delta_y,*ry-=2*delta_y;  
            }  
            else{  
                //点在左半部分   
                if(mx>sx)*lx+=delta_x,*rx-=2*delta_x,*ly+=delta_y,*ry-=2*delta_y;  
                else *lx-=delta_x,*rx+=2*delta_x,*ly-=delta_y,*ry+=2*delta_y;  
            }  
        }  
        else{  
            if(((x1-mx)*(x1-mx)+(y1-my)*(y1-my))>((x2-mx)*(x2-mx)+(y2-my)*(y2-my))){  
                //点在右半部分   
                if(mx>sx)*lx-=delta_x,*rx+=2*delta_x,*ly+=delta_y,*ry-=2*delta_y;  
                else *lx+=delta_x,*rx-=2*delta_x,*ly-=delta_y,*ry+=2*delta_y;  
            }  
            else{  
                //点在左半部分   
                if(mx>sx)*lx+=delta_x,*rx-=2*delta_x,*ly-=delta_y,*ry+=2*delta_y;  
                else *lx-=delta_x,*rx+=2*delta_x,*ly+=delta_y,*ry-=2*delta_y;  
            }  
        }  
    }             
}  
  
void MouseEventProcess(int x, int y, int button, int event)  
{  
    static bool isDrawLine = FALSE;
	static bool isDrawArrowline = FALSE;
	static bool isDrawDiamond = FALSE; 
	static bool isDrawRounded = FALSE;    
    static bool isDrawEllipse = FALSE;  
    static bool isDrawRectangle = FALSE;  
    static bool isDrawText = FALSE;  
      
    static bool startDrawLine = FALSE;
	static bool startDrawArrowline = FALSE;
	static bool startDrawDiamond = FALSE;
	static bool startDrawRounded = FALSE;        
    static bool startDrawRectangle = FALSE;  
    static bool startDrawEllipse = FALSE;  
      
    static bool changeLine = FALSE;
	static bool changeArrowline = FALSE; 
    static bool changeEllipse = FALSE;
	static bool changeDiamond = FALSE;
	static bool changeRounded = FALSE;  
    static bool changeRectangle = FALSE;  
    static bool changeText = FALSE;  
      
    static bool chooseLine = FALSE;
	static bool chooseArrowline = FALSE;
    static bool chooseRectangle = FALSE;
	static bool chooseDiamond = FALSE;
	static bool chooseRounded = FALSE;  
    static bool chooseEllipse = FALSE;  
    static bool chooseText = FALSE;  
      
    static int changenow=0;  
      
    double mx, my;  
    static double lx, ly;  
    static double sx=0.0, sy=0.0;  
    static double rx, ry;  
      
    mx = ScaleXInches(x);  
    my = ScaleYInches(y);  
    switch(event){  
        case BUTTON_DOWN:  
            if(button==LEFT_BUTTON){  
                if(inBox(mx,my,0,GetWindowWidth(),1,GetWindowHeight())){  
                    //扫描哪个按钮处于激活态，并做相应的准备工作   
                    if(isChoose){  
                        int i;  
                        double x1,x2,y1,y2;  
                        for(i=0;i<count;i++){  
                            x1=onScreen[i].x1,x2=onScreen[i].x2,y1=onScreen[i].y1,y2=onScreen[i].y2;  
                            if(onScreen[i].type==text){  
                                string str;  
                                str=SubString(Text,x2,y2);     
                                double len=TextStringWidth(str);  
                                if(inBox(mx,my,x1,x1+len,y1-0.1,y1+0.2)){  
                                    chooseText=TRUE;  
                                    choosenow=i;  
                                    lx=x1,ly=y1;  
                                    sx=mx-x1,sy=my-y1;  
                                    break;  
                                }  
                            }  
                            else if(onScreen[i].type==rectangle){  
                                if(inBox(mx,my,x1,x2,y1,y2)){  
                                    chooseRectangle=TRUE;  
                                    choosenow=i;  
                                    lx=x1,ly=y1;  
                                    sx=mx,sy=my;  
                                    break;  
                                }  
                            }
							else if(onScreen[i].type==diamond){  
                                if(inBox(mx,my,x1,x2,y1,y2)){  
                                    chooseDiamond=TRUE;  
                                    choosenow=i;  
                                    lx=x1,ly=y1;  
                                    sx=mx,sy=my;  
                                    break;  
                                }  
                            }
							else if(onScreen[i].type==rounded){  
                                if(inBox(mx,my,x1,x2,y1,y2)){  
                                    chooseRounded=TRUE;  
                                    choosenow=i;  
                                    lx=x1,ly=y1;  
                                    sx=mx,sy=my;  
                                    break;  
                                }  
                            }    
                            else if(onScreen[i].type==ellipse){  
                                if(inBox(mx,my,x1,x2,2*y1-y2,y2)){  
                                    chooseEllipse=TRUE;  
                                    choosenow=i;  
                                    lx=x1,ly=y1;  
                                    sx=mx,sy=my;  
                                    break;  
                                }  
                            }
							else if(onScreen[i].type==arrowline){  
                                if(fabs((y2-y1)*mx-(x2-x1)*my-x1*(y2-y1)+y1*(x2-x1))<=  
                                    0.5*sqrt((y2-y1)*(y2-y1)+(x2-x1)*(x2-x1))){  
                                    chooseArrowline=TRUE;  
                                    choosenow=i;  
                                    lx=x1,ly=y1;  
                                    sx=mx,sy=my;  
                                    break;  
                                }  
                            }  
                            else if(onScreen[i].type==line){  
                                if(fabs((y2-y1)*mx-(x2-x1)*my-x1*(y2-y1)+y1*(x2-x1))<=  
                                    0.5*sqrt((y2-y1)*(y2-y1)+(x2-x1)*(x2-x1))){  
                                    chooseLine=TRUE;  
                                    choosenow=i;  
                                    lx=x1,ly=y1;  
                                    sx=mx,sy=my;  
                                    break;  
                                }  
                            }  
                        }  
                    }  
                    else if(isDrawLine){  
                        startDrawLine=TRUE;  
                        MovePen(mx,my);  
                        sx=mx,sy=my;  
                        lx=mx,ly=my;  
                    }
					else if(isDrawArrowline){  
                        startDrawArrowline=TRUE;  
                        MovePen(mx,my);  
                        sx=mx,sy=my;  
                        lx=mx,ly=my;  
                    }
                    else if(isDrawRectangle){  
                        startDrawRectangle=TRUE;  
                        MovePen(mx,my);  
                        sx=mx,sy=my;  
                        lx=mx,ly=my;  
                    }
					else if(isDrawDiamond){  
                        startDrawDiamond=TRUE;  
                        MovePen(mx,my);  
                        sx=mx,sy=my;  
                        lx=mx,ly=my;  
                    }
					else if(isDrawRounded){  
                        startDrawRounded=TRUE;  
                        MovePen(mx,my);  
                        sx=mx,sy=my;  
                        lx=mx,ly=my;  
                    }  
                    else if(isDrawEllipse){  
                        startDrawEllipse=TRUE;  
                        MovePen(mx,my);  
                        sx=mx,sy=my;  
                        lx=mx,ly=my;  
                    }  
                    else if(isDrawText){  
                        startTimer(TIMER_BLINK, mseconds);  
                        startDrawText=TRUE;  
                        if(isBlink&&isDisplay){  
                            SetEraseMode(TRUE);  
                            MovePen(GetCurrentX(), GetCurrentY());  
                            DrawTextString("_");  
                            SetEraseMode(FALSE);  
                        }  
                        if(textlen-1!=start&&textlen!=0){  
                            Insert(text,tx,ty,start,textlen-1);  
                            isSave=TRUE;  
                        }  
                        MovePen(mx,my);  
                        tx=mx,ty=my;  
                        start=textlen;  
                    }  
                }  
                //更改按钮的激活态   
                else if(inBox(mx,my,textStartX,textStopX,textStartY,textStopY)){  
                    isBlink=TRUE;  
                    MovePen(textStartX,textStopY);  
                    SetPenColor("Red");  
                    DrawRectangle(rectangleWidth,rectangleHeight);  
                    SetPenColor("Black");  
                    if(isDrawLine){  
                        isDrawLine=FALSE;  
                        MovePen(lineStartX,lineStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);  
                    }
					else if(isDrawArrowline){  
                        isDrawArrowline=FALSE;  
                        MovePen(arrowlineStartX,arrowlineStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);  
                    }
					else if(isDrawRectangle){  
                        isDrawRectangle=FALSE;  
                        MovePen(rectangleStartX,rectangleStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);  
                    }  
                    else if(isDrawDiamond){  
                        isDrawDiamond=FALSE;  
                        MovePen(diamondStartX,diamondStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);  
                    }
					else if(isDrawRounded){  
                        isDrawRounded=FALSE;  
                        MovePen(roundedStartX,roundedStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);  
                    }     
					else if(isDrawEllipse){  
                        isDrawEllipse=FALSE;  
                        MovePen(ellipseStartX,ellipseStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);  
                    }
                    else if(isChoose){  
                        isChoose=FALSE;  
                        MovePen(chooseStartX,chooseStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);  
                    }  
                    if(isDrawText==FALSE)isDrawText=TRUE;  
                }  
                else if(inBox(mx,my,lineStartX,lineStopX,lineStartY,lineStopY)){  
                    MovePen(lineStartX,lineStopY);  
                    SetPenColor("Red");  
                    DrawRectangle(rectangleWidth,rectangleHeight);  
                    SetPenColor("Black");  
                    if(!isSave){  
                        isSave=TRUE;  
                        Insert(text,tx,ty,start,textlen-1);  
                    }  
                    if(isDrawRectangle){  
                        isDrawRectangle=FALSE;  
                        MovePen(rectangleStartX,rectangleStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);  
                    }  
                    else if(isDrawEllipse){  
                        isDrawEllipse=FALSE;  
                        MovePen(ellipseStartX,ellipseStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);  
                    }
					else if(isDrawDiamond){  
                        isDrawDiamond=FALSE;  
                        MovePen(diamondStartX,diamondStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);  
                    }
					else if(isDrawRounded){  
                        isDrawRounded=FALSE;  
                        MovePen(roundedStartX,roundedStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);  
                    }  
					else if(isDrawArrowline){  
                        isDrawArrowline=FALSE;  
                        MovePen(arrowlineStartX,arrowlineStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);  
                    }  
                    else if(isDrawText){  
                        if(isBlink&&isDisplay){  
                            SetEraseMode(TRUE);  
                            MovePen(x_location, y_location);  
                            DrawTextString("_");  
                            SetEraseMode(FALSE);  
                            ReDraw();  
                        }  
                        start=textlen-1;  
                        isBlink=FALSE;  
                        cancelTimer(TIMER_BLINK);  
                        isDrawText=FALSE;  
                        startDrawText=FALSE;  
                        MovePen(textStartX,textStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);                        
                    }  
                    else if(isChoose){  
                        isChoose=FALSE;  
                        MovePen(chooseStartX,chooseStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);  
                    }
                    if(isDrawLine==FALSE)isDrawLine=TRUE;
                }  
                else if(inBox(mx,my,arrowlineStartX,arrowlineStopX,arrowlineStartY,arrowlineStopY)){  
                    MovePen(arrowlineStartX,arrowlineStopY);  
                    SetPenColor("Red");  
                    DrawRectangle(rectangleWidth,rectangleHeight);  
                    SetPenColor("Black");  
                    if(!isSave){  
                        isSave=TRUE;  
                        Insert(text,tx,ty,start,textlen-1);  
                    }  
					if(isDrawRectangle){  
                        isDrawRectangle=FALSE;  
                        MovePen(rectangleStartX,rectangleStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);  
                    }
                    else if(isDrawLine){  
                        isDrawLine=FALSE;  
                        MovePen(lineStartX,lineStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);  
                    }
                    else if(isDrawDiamond){  
                        isDrawDiamond=FALSE;  
                        MovePen(diamondStartX,diamondStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);  
                    }
					else if(isDrawRounded){  
                        isDrawRounded=FALSE;  
                        MovePen(roundedStartX,roundedStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);  
                    }
                    else if(isDrawEllipse){  
                        isDrawEllipse=FALSE;  
                        MovePen(ellipseStartX,ellipseStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);  
                    }  
                    else if(isDrawText){  
                        if(isBlink&&isDisplay){  
                            SetEraseMode(TRUE);  
                            MovePen(x_location, y_location);  
                            DrawTextString("_");  
                            SetEraseMode(FALSE);  
                            ReDraw();  
                        }  
                        start=textlen-1;  
                        isBlink=FALSE;  
                        cancelTimer(TIMER_BLINK);  
                        isDrawText=FALSE;  
                        startDrawText=FALSE;  
                        MovePen(textStartX,textStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);                        
                    }  
                    else if(isChoose){  
                        isChoose=FALSE;  
                        MovePen(chooseStartX,chooseStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);  
                    }
                    if(isDrawArrowline==FALSE)isDrawArrowline=TRUE;
                }  
                else if(inBox(mx,my,rectangleStartX,rectangleStopX,  
                       rectangleStartY,rectangleStopY)){  
                    MovePen(rectangleStartX,rectangleStopY);  
                    SetPenColor("Red");  
                    DrawRectangle(rectangleWidth,rectangleHeight);  
                    SetPenColor("Black");  
                    if(!isSave){  
                        isSave=TRUE;  
                        Insert(text,tx,ty,start,textlen-1);  
                    }  
                    if(isDrawLine){  
                        isDrawLine=FALSE;  
                        MovePen(lineStartX,lineStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);  
                    }
					else if(isDrawArrowline){  
                        isDrawArrowline=FALSE;  
                        MovePen(arrowlineStartX,arrowlineStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);  
                    }
					else if(isDrawDiamond){  
                        isDrawDiamond=FALSE;  
                        MovePen(diamondStartX,diamondStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);  
                    }
					else if(isDrawRounded){  
                        isDrawRounded=FALSE;  
                        MovePen(roundedStartX,roundedStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);  
                    }  
                    else if(isDrawEllipse){  
                        isDrawEllipse=FALSE;  
                        MovePen(ellipseStartX,ellipseStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);  
                    }  
                    else if(isDrawText){  
                        if(isBlink&&isDisplay){  
                            SetEraseMode(TRUE);  
                            MovePen(x_location, y_location);  
                            DrawTextString("_");  
                            SetEraseMode(FALSE);  
                            ReDraw();  
                        }  
                        start=textlen-1;  
                        isBlink=FALSE;  
                        cancelTimer(TIMER_BLINK);  
                        isDrawText=FALSE;  
                        startDrawText=FALSE;  
                        MovePen(textStartX,textStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);  
                    }  
                    else if(isChoose){  
                        isChoose=FALSE;  
                        MovePen(chooseStartX,chooseStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);  
                    }  
                    if(isDrawRectangle==FALSE)isDrawRectangle=TRUE;           
                }//矩形
				else if(inBox(mx,my,diamondStartX,diamondStopX,  
                       diamondStartY,diamondStopY)){  
                    MovePen(diamondStartX,diamondStopY);  
                    SetPenColor("Red");  
                    DrawRectangle(rectangleWidth,rectangleHeight);  
                    SetPenColor("Black");  
                    if(!isSave){  
                        isSave=TRUE;  
                        Insert(text,tx,ty,start,textlen-1);  
                    }  
                    if(isDrawLine){  
                        isDrawLine=FALSE;  
                        MovePen(lineStartX,lineStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);  
                    }
                    else if(isDrawRectangle){  
                        isDrawRectangle=FALSE;  
                        MovePen(rectangleStartX,rectangleStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);  
                    }
					else if(isDrawArrowline){  
                        isDrawArrowline=FALSE;  
                        MovePen(arrowlineStartX,arrowlineStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);  
                    }
					else if(isDrawRounded){  
                        isDrawRounded=FALSE;  
                        MovePen(roundedStartX,roundedStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);  
                    }  
                    else if(isDrawEllipse){  
                        isDrawEllipse=FALSE;  
                        MovePen(ellipseStartX,ellipseStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);  
                    }  
                    else if(isDrawText){  
                        if(isBlink&&isDisplay){  
                            SetEraseMode(TRUE);  
                            MovePen(x_location, y_location);  
                            DrawTextString("_");  
                            SetEraseMode(FALSE);  
                            ReDraw();  
                        }  
                        start=textlen-1;  
                        isBlink=FALSE;  
                        cancelTimer(TIMER_BLINK);  
                        isDrawText=FALSE;  
                        startDrawText=FALSE;  
                        MovePen(textStartX,textStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);  
                    }  
                    else if(isChoose){  
                        isChoose=FALSE;  
                        MovePen(chooseStartX,chooseStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);  
                    }  
                    if(isDrawDiamond==FALSE)isDrawDiamond=TRUE;           
                }//菱形
				else if(inBox(mx,my,roundedStartX,roundedStopX,  
                       roundedStartY,roundedStopY)){  
                    MovePen(roundedStartX,roundedStopY);  
                    SetPenColor("Red");  
                    DrawRectangle(rectangleWidth,rectangleHeight);  
                    SetPenColor("Black");  
                    if(!isSave){  
                        isSave=TRUE;  
                        Insert(text,tx,ty,start,textlen-1);  
                    }  
                    if(isDrawLine){  
                        isDrawLine=FALSE;  
                        MovePen(lineStartX,lineStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);  
                    }
                    else if(isDrawRectangle){  
                        isDrawRectangle=FALSE;  
                        MovePen(rectangleStartX,rectangleStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);  
                    }
					else if(isDrawArrowline){  
                        isDrawArrowline=FALSE;  
                        MovePen(arrowlineStartX,arrowlineStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);  
                    }
					else if(isDrawDiamond){  
                        isDrawDiamond=FALSE;  
                        MovePen(diamondStartX,diamondStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);  
                    } 
                    else if(isDrawEllipse){  
                        isDrawEllipse=FALSE;  
                        MovePen(ellipseStartX,ellipseStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);  
                    }  
                    else if(isDrawText){  
                        if(isBlink&&isDisplay){  
                            SetEraseMode(TRUE);  
                            MovePen(x_location, y_location);  
                            DrawTextString("_");  
                            SetEraseMode(FALSE);  
                            ReDraw();  
                        }  
                        start=textlen-1;  
                        isBlink=FALSE;  
                        cancelTimer(TIMER_BLINK);  
                        isDrawText=FALSE;  
                        startDrawText=FALSE;  
                        MovePen(textStartX,textStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);  
                    }  
                    else if(isChoose){  
                        isChoose=FALSE;  
                        MovePen(chooseStartX,chooseStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);  
                    }  
                    if(isDrawRounded==FALSE)isDrawRounded=TRUE;           
                }//圆角矩形  
                else if(inBox(mx,my,ellipseStartX,ellipseStopX,  
                              ellipseStartY,ellipseStopY)){  
                    MovePen(ellipseStartX,ellipseStopY);  
                    SetPenColor("Red");  
                    DrawRectangle(rectangleWidth,rectangleHeight);  
                    SetPenColor("Black");  
                    if(!isSave){  
                        isSave=TRUE;  
                        Insert(text,tx,ty,start,textlen-1);  
                    }  
                    if(isDrawLine){  
                        isDrawLine=FALSE;  
                        MovePen(lineStartX,lineStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);  
                    }
					else if(isDrawArrowline){
						isDrawArrowline=FALSE;
						MovePen(arrowlineStartX,arrowlineStopY);
						DrawRectangle(rectangleWidth,rectangleHeight);   
					}
					else if(isDrawDiamond){  
                        isDrawDiamond=FALSE;  
                        MovePen(diamondStartX,diamondStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);  
                    }
					else if(isDrawRounded){  
                        isDrawRounded=FALSE;  
                        MovePen(roundedStartX,roundedStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);  
                    }  
                    else if(isDrawRectangle){  
                        isDrawRectangle=FALSE;  
                        MovePen(rectangleStartX,rectangleStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);  
                    }  
                    else if(isDrawText){  
                        if(isBlink&&isDisplay){  
                            SetEraseMode(TRUE);  
                            MovePen(x_location, y_location);  
                            DrawTextString("_");  
                            SetEraseMode(FALSE);  
                            ReDraw();  
                        }  
                        isBlink=FALSE;  
                        start=textlen-1;  
                        cancelTimer(TIMER_BLINK);  
                        isDrawText=FALSE;  
                        startDrawText=FALSE;  
                        MovePen(textStartX,textStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);                        
                    }  
                    else if(isChoose){  
                        isChoose=FALSE;  
                        MovePen(chooseStartX,chooseStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);  
                    }  
                    if(isDrawEllipse==FALSE)isDrawEllipse=TRUE;  
                }//椭圆   
                else if(inBox(mx,my,chooseStartX,chooseStopX,  
                              chooseStartY,chooseStopY)){  
                    MovePen(chooseStartX,chooseStopY);  
                    SetPenColor("Red");  
                    DrawRectangle(rectangleWidth,rectangleHeight);  
                    SetPenColor("Black");  
                    if(!isSave){  
                        isSave=TRUE;  
                        Insert(text,tx,ty,start,textlen-1);  
                    }  
                    if(isDrawLine){  
                        isDrawLine=FALSE;  
                        MovePen(lineStartX,lineStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);  
                    }
					else if(isDrawArrowline){
						isDrawArrowline=FALSE;
						MovePen(arrowlineStartX,arrowlineStopY);
						DrawRectangle(rectangleWidth,rectangleHeight);   
					}
					else if(isDrawDiamond){  
                        isDrawDiamond=FALSE;  
                        MovePen(diamondStartX,diamondStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);  
                    }
					else if(isDrawRounded){  
                        isDrawRounded=FALSE;  
                        MovePen(roundedStartX,roundedStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);  
                    }    
                    else if(isDrawRectangle){  
                        isDrawRectangle=FALSE;  
                        MovePen(rectangleStartX,rectangleStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);  
                    }  
                    else if(isDrawEllipse){  
                        isDrawEllipse=FALSE;  
                        MovePen(ellipseStartX,ellipseStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);  
                    }  
                    else if(isDrawText){  
                        if(isBlink&&isDisplay){  
                            SetEraseMode(TRUE);  
                            MovePen(x_location, y_location);  
                            DrawTextString("_");  
                            SetEraseMode(FALSE);  
                            ReDraw();  
                        }  
                        start=textlen-1;  
                        isBlink=FALSE;  
                        cancelTimer(TIMER_BLINK);  
                        isDrawText=FALSE;  
                        startDrawText=FALSE;  
                        MovePen(textStartX,textStopY);  
                        DrawRectangle(rectangleWidth,rectangleHeight);                        
                    }     
                    if(isChoose==FALSE)isChoose=TRUE;                 
                }  
            }//选择  
            
			//右键选择图形的准备工作   
            else if(button==RIGHT_BUTTON){  
                if(isChoose){  
                    int i;  
                    double x1,x2,y1,y2;  
                    for(i=0;i<count;i++){  
                        x1=onScreen[i].x1,x2=onScreen[i].x2,y1=onScreen[i].y1,y2=onScreen[i].y2;  
                        if(onScreen[i].type==rectangle){  
                            if(inBox(mx,my,x1,x2,y1,y2)){  
                                changeRectangle=TRUE;  
                                changenow=i;  
                                lx=x1,ly=y1;  
                                sx=mx,sy=my;  
                                rx=x2-x1,ry=y1-y2;  
                                break;  
                            }  
                        }
						else if(onScreen[i].type==diamond){  
                            if(inBox(mx,my,x1,x2,y1,y2)){  
                                changeDiamond=TRUE;  
                                changenow=i;  
                                lx=x1,ly=y1;  
                                sx=mx,sy=my;  
                                rx=x2-x1,ry=y1-y2;  
                                break;  
                            }  
                        }
						else if(onScreen[i].type==rounded){  
                            if(inBox(mx,my,x1,x2,y1,y2)){  
                                changeRounded=TRUE;  
                                changenow=i;  
                                lx=x1,ly=y1;  
                                sx=mx,sy=my;  
                                rx=x2-x1,ry=y1-y2;  
                                break;  
                            }  
                        }   
                        else if(onScreen[i].type==ellipse){  
                            if(inBox(mx,my,x1,x2,2*y1-y2,y2)){  
                                changeEllipse=TRUE;  
                                changenow=i;  
                                lx=x1,ly=y1;  
                                sx=mx,sy=my;  
                                rx=(x1-x2)/2.0,ry=y2-y1;  
                                break;  
                            }  
                        }  
                        else if(onScreen[i].type==line){  
                            if(fabs((y2-y1)*mx-(x2-x1)*my-x1*(y2-y1)+y1*(x2-x1))<=  
                                0.3*sqrt((y2-y1)*(y2-y1)+(x2-x1)*(x2-x1))){           
                                changeLine=TRUE;  
                                changenow=i;  
                                lx=x1,ly=y1;  
                                sx=mx,sy=my;  
                                rx=x2-x1,ry=y2-y1;  
                                break;  
                            }  
                        }  
                        else if(onScreen[i].type==arrowline){  
                            if(fabs((y2-y1)*mx-(x2-x1)*my-x1*(y2-y1)+y1*(x2-x1))<=  
                                0.3*sqrt((y2-y1)*(y2-y1)+(x2-x1)*(x2-x1))){           
                                changeArrowline=TRUE;  
                                changenow=i;  
                                lx=x1,ly=y1;  
                                sx=mx,sy=my;  
                                rx=x2-x1,ry=y2-y1;  
                                break;  
                            }  
                        }  
                    }  
                }  
            }  
            break;  
        case BUTTON_UP:  
            if(startDrawLine){  
                ReDraw();  
                if((sx<mx&&sy<my)||(sx>mx&&sy>my))Insert(line,sx,sy,mx,my);  
                else Insert(line,mx,my,sx,sy);  
                startDrawLine=FALSE;  
            }
			else if(startDrawArrowline){  
                ReDraw();  
                Insert(line,sx,sy,mx,my);   
                startDrawArrowline=FALSE;  
            }    
            else if(startDrawDiamond){  
                ReDraw();  
                Insert(diamond,sx,sy,mx,my);  
                startDrawDiamond=FALSE;  
            }
            else if(startDrawRounded){  
                ReDraw();  
                Insert(rounded,sx,sy,mx,my);  
                startDrawRounded=FALSE;  
            } 
			else if(startDrawRectangle){  
                ReDraw();  
                Insert(rectangle,sx,sy,mx,my);  
                startDrawRectangle=FALSE;  
            }   
            else if(startDrawEllipse){  
                ReDraw();  
                Insert(ellipse,sx,sy,mx,my);                   
                startDrawEllipse=FALSE;  
            }  
            else if(chooseRectangle){  
                double xx=onScreen[choosenow].x2-onScreen[choosenow].x1;  
                double yy=onScreen[choosenow].y2-onScreen[choosenow].y1;  
                onScreen[choosenow].x1=lx;  
                onScreen[choosenow].x2=lx+xx;  
                onScreen[choosenow].y1=ly;  
                onScreen[choosenow].y2=ly+yy;  
                ReDraw();  
                chooseRectangle=FALSE;  
            }
			else if(chooseDiamond){  
                double xx=onScreen[choosenow].x2-onScreen[choosenow].x1;  
                double yy=onScreen[choosenow].y2-onScreen[choosenow].y1;  
                onScreen[choosenow].x1=lx;  
                onScreen[choosenow].x2=lx+xx;  
                onScreen[choosenow].y1=ly;  
                onScreen[choosenow].y2=ly+yy;  
                ReDraw();  
                chooseDiamond=FALSE;  
            }
			else if(chooseRounded){  
                double xx=onScreen[choosenow].x2-onScreen[choosenow].x1;  
                double yy=onScreen[choosenow].y2-onScreen[choosenow].y1;  
                onScreen[choosenow].x1=lx;  
                onScreen[choosenow].x2=lx+xx;  
                onScreen[choosenow].y1=ly;  
                onScreen[choosenow].y2=ly+yy;  
                ReDraw();  
                chooseRounded=FALSE;  
            } 
            else if(chooseEllipse){  
                double xx=onScreen[choosenow].x2-onScreen[choosenow].x1;  
                double yy=onScreen[choosenow].y2-onScreen[choosenow].y1;  
                onScreen[choosenow].x1=lx;  
                onScreen[choosenow].x2=lx+xx;  
                onScreen[choosenow].y1=ly;  
                onScreen[choosenow].y2=ly+yy;  
                ReDraw();  
                chooseEllipse=FALSE;  
            }  
            else if(chooseLine){  
                double xx=onScreen[choosenow].x2-onScreen[choosenow].x1;  
                double yy=onScreen[choosenow].y2-onScreen[choosenow].y1;  
                onScreen[choosenow].x1=lx;  
                onScreen[choosenow].x2=lx+xx;  
                onScreen[choosenow].y1=ly;  
                onScreen[choosenow].y2=ly+yy;  
                ReDraw();  
                chooseLine=FALSE;  
            }
			else if(chooseArrowline){  
                double xx=onScreen[choosenow].x2-onScreen[choosenow].x1;  
                double yy=onScreen[choosenow].y2-onScreen[choosenow].y1;  
                onScreen[choosenow].x1=lx;  
                onScreen[choosenow].x2=lx+xx;  
                onScreen[choosenow].y1=ly;  
                onScreen[choosenow].y2=ly+yy;  
                ReDraw();  
                chooseArrowline=FALSE;  
            }  
            else if(chooseText){  
                double xx=onScreen[choosenow].x2-onScreen[choosenow].x1;  
                double yy=onScreen[choosenow].y2-onScreen[choosenow].y1;  
                onScreen[choosenow].x1=lx;  
                onScreen[choosenow].y1=ly;  
                ReDraw();  
                chooseText=FALSE;  
            }  
            else if(changeRectangle){  
                onScreen[changenow].x1=lx;  
                onScreen[changenow].x2=lx+rx;  
                onScreen[changenow].y1=ly;  
                onScreen[changenow].y2=ly-ry;  
                ReDraw();  
                changeRectangle=FALSE;                
            }
			else if(changeDiamond){  
                onScreen[changenow].x1=lx;  
                onScreen[changenow].x2=lx+rx;  
                onScreen[changenow].y1=ly;  
                onScreen[changenow].y2=ly+ry;  
                ReDraw();  
                changeDiamond=FALSE;                
            }
			else if(changeRounded){  
                onScreen[changenow].x1=lx;  
                onScreen[changenow].x2=lx+rx;  
                onScreen[changenow].y1=ly;  
                onScreen[changenow].y2=ly+ry;  
                ReDraw();  
                changeRounded=FALSE;                
            }   
            else if(changeEllipse){  
                onScreen[changenow].x1=lx;  
                onScreen[changenow].x2=lx-rx*2;  
                onScreen[changenow].y1=ly;  
                onScreen[changenow].y2=ly+ry;  
                ReDraw();  
                changeEllipse=FALSE;  
            }  
            else if(changeLine){  
                onScreen[changenow].x1=lx;  
                onScreen[changenow].x2=lx+rx;  
                onScreen[changenow].y1=ly;  
                onScreen[changenow].y2=ly+ry;  
                ReDraw();  
                changeLine=FALSE;  
            }
			else if(changeArrowline){  
                onScreen[changenow].x1=lx;  
                onScreen[changenow].x2=lx+rx;  
                onScreen[changenow].y1=ly;  
                onScreen[changenow].y2=ly+ry;  
                ReDraw();  
                changeArrowline=FALSE;  
            }  
            break;  
        case MOUSEMOVE:  
            //画图   
            if(startDrawLine){  
                SetEraseMode(TRUE);  
                MovePen(sx,sy);  
                DrawLine(lx-sx,ly-sy);  
                SetEraseMode(FALSE);  
                MovePen(sx,sy);  
                DrawLine(mx-sx,my-sy);  
                lx=mx,ly=my;  
            }//画直线 
			else if(startDrawArrowline){  
				SetEraseMode(TRUE);  
                MovePen(sx,sy);  
                DrawArrowline(lx-sx,ly-sy);
				SetEraseMode(FALSE);  
                MovePen(sx,sy);  
                DrawArrowline(mx-sx,my-sy); 
                lx=mx,ly=my;  
            }//画带箭头的直线 
			else if(startDrawDiamond){  
				SetEraseMode(TRUE);  
                MovePen(sx,sy);  
                DrawDiamond(lx-sx,ly-sy);
				SetEraseMode(FALSE);  
                MovePen(sx,sy);  
                DrawDiamond(mx-sx,my-sy); 
                lx=mx,ly=my;  
            }//画菱形
			else if(startDrawRounded){  
				SetEraseMode(TRUE);  
                MovePen(sx,sy);  
                DrawRounded(lx-sx,ly-sy);
				SetEraseMode(FALSE);  
                MovePen(sx,sy);  
                DrawRounded(mx-sx,my-sy); 
                lx=mx,ly=my;  
            }//画圆角矩形        
            else if(startDrawRectangle){  
                SetEraseMode(TRUE);  
                MovePen(sx,sy);  
                DrawRectangle(lx-sx,sy-ly);  
                SetEraseMode(FALSE);  
                MovePen(sx,sy);  
                DrawRectangle(mx-sx,sy-my);  
                lx=mx,ly=my;  
            }//画矩形  
            else if(startDrawEllipse){  
                SetEraseMode(TRUE);  
                MovePen(sx,sy);  
                DrawEllipticalArc((sx-lx)/2.0,(ly-sy),0,360);  
                SetEraseMode(FALSE);  
                MovePen(sx,sy);  
                DrawEllipticalArc((sx-mx)/2.0,(my-sy),0,360);  
                lx=mx,ly=my;  
            }//画椭圆
			  
            //移动   
            else if(chooseRectangle){  
                int i=choosenow;  
                double x1=onScreen[i].x1,x2=onScreen[i].x2,y1=onScreen[i].y1,y2=onScreen[i].y2;  
                SetEraseMode(TRUE);  
                MovePen(lx,ly);  
                DrawRectangle(x2-x1,y1-y2);  
                SetEraseMode(FALSE);  
                lx=mx-(sx-x1),ly=my-(sy-y1);  
                MovePen(lx,ly);  
                DrawRectangle(x2-x1,y1-y2);  
            }  
            else if(chooseEllipse){  
                int i=choosenow;  
                double x1=onScreen[i].x1,x2=onScreen[i].x2,y1=onScreen[i].y1,y2=onScreen[i].y2;  
                SetEraseMode(TRUE);  
                MovePen(lx,ly);  
                DrawEllipticalArc((x1-x2)/2.0,y2-y1,0,360);  
                SetEraseMode(FALSE);  
                lx=mx-(sx-x1),ly=my-(sy-y1);  
                MovePen(lx,ly);  
                DrawEllipticalArc((x1-x2)/2.0,y2-y1,0,360);  
            }  
            else if(chooseLine){  
                int i=choosenow;  
                double x1=onScreen[i].x1,x2=onScreen[i].x2,y1=onScreen[i].y1,y2=onScreen[i].y2;  
                SetEraseMode(TRUE);  
                MovePen(lx,ly);  
                DrawLine(x2-x1,y2-y1);  
                SetEraseMode(FALSE);  
                lx=mx-(sx-x1),ly=my-(sy-y1);  
                MovePen(lx,ly);  
                DrawLine(x2-x1,y2-y1);                
            }
			else if(chooseArrowline){  
                int i=choosenow;  
                double x1=onScreen[i].x1,x2=onScreen[i].x2,y1=onScreen[i].y1,y2=onScreen[i].y2;  
                SetEraseMode(TRUE);  
                MovePen(lx,ly);  
                DrawArrowline(x2-x1,y2-y1);  
                SetEraseMode(FALSE);  
                lx=mx-(sx-x1),ly=my-(sy-y1);  
                MovePen(lx,ly);  
                DrawArrowline(x2-x1,y2-y1);                
            }
			else if(chooseDiamond){  
                int i=choosenow;  
                double x1=onScreen[i].x1,x2=onScreen[i].x2,y1=onScreen[i].y1,y2=onScreen[i].y2;  
                SetEraseMode(TRUE);  
                MovePen(lx,ly);  
                DrawDiamond(x2-x1,y2-y1);  
                SetEraseMode(FALSE);  
                lx=mx-(sx-x1),ly=my-(sy-y1);  
                MovePen(lx,ly);  
                DrawDiamond(x2-x1,y2-y1);  
            }
			else if(chooseRounded){  
                int i=choosenow;  
                double x1=onScreen[i].x1,x2=onScreen[i].x2,y1=onScreen[i].y1,y2=onScreen[i].y2;  
                SetEraseMode(TRUE);  
                MovePen(lx,ly);  
                DrawRounded(x2-x1,y2-y1);  
                SetEraseMode(FALSE);  
                lx=mx-(sx-x1),ly=my-(sy-y1);  
                MovePen(lx,ly);  
                DrawRounded(x2-x1,y2-y1);  
            }   
            else if(chooseText){  
                static char str[2]={0,0};  
                int j,i=choosenow;  
                double x1=onScreen[i].x1,x2=onScreen[i].x2,y1=onScreen[i].y1,y2=onScreen[i].y2;  
                SetEraseMode(TRUE);  
                MovePen(lx,ly);  
                for(j=x2;j<=y2;j++){  
                    str[0]=Text[j];  
                    DrawTextString(str);  
                }  
                SetEraseMode(FALSE);  
                lx=mx-sx,ly=my-sy;  
                MovePen(lx,ly);  
                for(j=x2;j<=y2;j++){  
                    str[0]=Text[j];  
                    DrawTextString(str);  
                }         
            }  
            //大小改变   
            else if(changeLine){  
                int i=changenow;  
                double x1=onScreen[i].x1,x2=onScreen[i].x2,y1=onScreen[i].y1,y2=onScreen[i].y2;  
                SetEraseMode(TRUE);  
                MovePen(lx,ly);  
                DrawLine(rx,ry);  
                SetEraseMode(FALSE);  
                ChangeLineLength(x1,y1,x2,y2,sx,sy,mx,my,&lx,&ly,&rx,&ry);  
                MovePen(lx,ly);  
                DrawLine(rx,ry);                  
                sx=mx,sy=my;  
            }
			else if(changeArrowline){  
                int i=changenow;  
                double x1=onScreen[i].x1,x2=onScreen[i].x2,y1=onScreen[i].y1,y2=onScreen[i].y2;  
                SetEraseMode(TRUE);  
                MovePen(lx,ly);  
                DrawArrowline(rx,ry);  
                SetEraseMode(FALSE);  
                ChangeLineLength(x1,y1,x2,y2,sx,sy,mx,my,&lx,&ly,&rx,&ry);  
                MovePen(lx,ly);  
                DrawArrowline(rx,ry);                  
                sx=mx,sy=my;  
            }   
            else if(changeRectangle){  
                int i=changenow;  
                double x1=onScreen[i].x1,x2=onScreen[i].x2,y1=onScreen[i].y1,y2=onScreen[i].y2;  
                SetEraseMode(TRUE);  
                MovePen(lx,ly);  
                DrawRectangle(rx,ry);  
                SetEraseMode(FALSE);  
                ry=(my-(y1+y2)/2.0)*fabs(y1-y2)/fabs(sy-(y1+y2)/2.0);  
                rx=(mx-(x1+x2)/2.0)*fabs(x1-x2)/fabs(sx-(x1+x2)/2.0);  
                lx=(x1+x2-rx)/2.0;  
                ly=(y1+y2+ry)/2.0;  
                MovePen(lx,ly);  
                DrawRectangle(rx,ry);  
            }  
            else if(changeEllipse){  
                int i=changenow;  
                double x1=onScreen[i].x1,x2=onScreen[i].x2,y1=onScreen[i].y1,y2=onScreen[i].y2;  
                SetEraseMode(TRUE);  
                MovePen(lx,ly);  
                DrawEllipticalArc(rx,ry,0,360);  
                SetEraseMode(FALSE);  
                ry=(my-y1)*fabs(y1-y2)/fabs(sy-y1);  
                rx=0.5*(mx-(x1+x2)/2.0)*fabs(x1-x2)/fabs(sx-(x1+x2)/2.0);  
                lx=(x1+x2)/2.0+rx;  
                MovePen(lx,ly);  
                DrawEllipticalArc(rx,ry,0,360);               
            }     
            else if(changeDiamond){  
                int i=changenow;  
                double x1=onScreen[i].x1,x2=onScreen[i].x2,y1=onScreen[i].y1,y2=onScreen[i].y2;  
                SetEraseMode(TRUE);  
                MovePen(lx,ly);  
                DrawDiamond(rx,ry);  
                SetEraseMode(FALSE);  
                ry=(my-(y1+y2)/2.0)*fabs(y1-y2)/fabs(sy-(y1+y2)/2.0);  
                rx=(mx-(x1+x2)/2.0)*fabs(x1-x2)/fabs(sx-(x1+x2)/2.0);  
                lx=(x1+x2-rx)/2.0;  
                ly=(y1+y2-ry)/2.0;  
                MovePen(lx,ly);  
                DrawDiamond(rx,ry);  
            } 
			else if(changeRounded){  
                int i=changenow;  
                double x1=onScreen[i].x1,x2=onScreen[i].x2,y1=onScreen[i].y1,y2=onScreen[i].y2;  
                SetEraseMode(TRUE);  
                MovePen(lx,ly);  
                DrawRounded(rx,ry);  
                SetEraseMode(FALSE);  
                ry=(my-(y1+y2)/2.0)*fabs(y1-y2)/fabs(sy-(y1+y2)/2.0);  
                rx=(mx-(x1+x2)/2.0)*fabs(x1-x2)/fabs(sx-(x1+x2)/2.0);  
                lx=(x1+x2-rx)/2.0;  
                ly=(y1+y2-ry)/2.0;  
                MovePen(lx,ly);  
                DrawRounded(rx,ry);  
            }  
            break;  
     }  
}  
  
void KeyboardEventProcess(int key,int event)  
{  
    static char str[2]={0,0};  
    switch(event){  
        case KEY_DOWN:  
            if(key==VK_DELETE||key==VK_BACK){  
                //删除选区   
                if(isChoose){  
                    SetEraseMode(TRUE);  
                    int i=choosenow;  
                    double x1=onScreen[i].x1,x2=onScreen[i].x2,y1=onScreen[i].y1,y2=onScreen[i].y2;  
                    SetEraseMode(TRUE);  
                    if(onScreen[i].type==line){  
                        MovePen(onScreen[i].x1,onScreen[i].y1);  
                        DrawLine(onScreen[i].x2-onScreen[i].x1,onScreen[i].y2-onScreen[i].y1);  
                    }
					else if(onScreen[i].type==arrowline){  
                        MovePen(onScreen[i].x1,onScreen[i].y1);  
                        DrawArrowline(onScreen[i].x2-onScreen[i].x1,onScreen[i].y2-onScreen[i].y1);  
                    }    
                    else if(onScreen[i].type==rectangle){  
                        MovePen(onScreen[i].x1,onScreen[i].y1);  
                        DrawRectangle(onScreen[i].x2-onScreen[i].x1,onScreen[i].y1-onScreen[i].y2);  
                    }
					else if(onScreen[i].type==rounded){  
                        MovePen(onScreen[i].x1,onScreen[i].y1);  
                        DrawRounded(onScreen[i].x2-onScreen[i].x1,onScreen[i].y2-onScreen[i].y1);  
                    }
					else if(onScreen[i].type==diamond){  
                        MovePen(onScreen[i].x1,onScreen[i].y1);  
                        DrawDiamond(onScreen[i].x2-onScreen[i].x1,onScreen[i].y2-onScreen[i].y1);  
                    }  
                    else if(onScreen[i].type==ellipse){  
                        MovePen(onScreen[i].x1,onScreen[i].y1);  
                        DrawEllipticalArc((onScreen[i].x1-onScreen[i].x2)/2.0,(onScreen[i].y2-onScreen[i].y1),0,360);  
                    }  
                    else if(onScreen[i].type==text){  
                        MovePen(onScreen[i].x1,onScreen[i].y1);  
                        int j;  
                        char str[2]={0,0};  
                        for(j=onScreen[i].x2;j<=onScreen[i].y2;j++){  
                            str[0]=Text[j];  
                            DrawTextString(str);  
                        }  
                    }  
                    SetEraseMode(FALSE);  
                    onScreen[i].type=null;                        
                }  
                //删除文本   
                else if(startDrawText){  
                    if(location<0)return;  
                    int i;  
                    SetEraseMode(TRUE);  
                   if (isBlink && isDisplay) {  
                       MovePen(GetCurrentX(), GetCurrentY());  
                       DrawTextString("_");  
                       MovePen(GetCurrentX()-TextStringWidth("_"), GetCurrentY());  
                    }  
                    str[0]=Text[location];  
                    MovePen(GetCurrentX()-TextStringWidth(str),GetCurrentY());  
                    DrawTextString(str);  
                    if(location==textlen)MovePen(GetCurrentX()-TextStringWidth(str),GetCurrentY());  
                    else{  
                        string s1,s2;  
                        s1=SubString(Text,location+1,textlen);  
                        s2=SubString(Text,location,textlen);   
                        SetEraseMode(TRUE);  
                        DrawTextString(s1);  
                        SetEraseMode(FALSE);  
                        MovePen(GetCurrentX()-TextStringWidth(s2),GetCurrentY());  
                        DrawTextString(s1);  
                        MovePen(GetCurrentX()-TextStringWidth(s1),GetCurrentY());  
                    }  
                    for(i=location;i<textlen;i++){  
                        Text[i]=Text[i+1];  
                    }  
                    Text[--textlen]='\0';  
                    location--;  
                    SetEraseMode(FALSE);  
                    x_location=GetCurrentX();  
                    y_location=GetCurrentY();                     
                }  
            }  
            else if(key==VK_LEFT){  
                if(location<0)return;  
                if (isBlink && isDisplay) {  
                    SetEraseMode(TRUE);  
                    MovePen(GetCurrentX(), GetCurrentY());  
                    DrawTextString("_");  
                    SetEraseMode(FALSE);  
                    MovePen(GetCurrentX()-TextStringWidth("_"), GetCurrentY());  
                }  
                str[0]=Text[location];  
                MovePen(GetCurrentX()-TextStringWidth(str),GetCurrentY());  
                location--;  
                x_location=GetCurrentX();  
                y_location=GetCurrentY();  
            }  
            else if(key==VK_RIGHT){  
                if(location>=textlen)return;   
                if (isBlink && isDisplay){  
                    SetEraseMode(TRUE);  
                    MovePen(GetCurrentX(), GetCurrentY());  
                    DrawTextString("_");  
                    SetEraseMode(FALSE);  
                    MovePen(GetCurrentX()-TextStringWidth("_"), GetCurrentY());  
                }  
                str[0]=Text[location+1];  
                MovePen(GetCurrentX()+TextStringWidth(str),GetCurrentY());  
                location++;  
                x_location=GetCurrentX();  
                y_location=GetCurrentY();  
            }  
    }  
}  
  
void CharEventProcess(char key)  
{  
    if(!startDrawText)return;   
    if(textlen>=99||GetCurrentX()>=GetWindowWidth()-0.2)return;  
    if(key==VK_BACK||key==VK_DELETE)return;  
    else{  
        if(textlen>start)isSave=FALSE;  
        char str[2]={0,0};  
        str[0]=key;  
        if(location==textlen-1){  
            Text[textlen++]=key;  
            Text[textlen]='\0';  
            DrawTextString(str);  
            location++;  
        }  
        else{  
            int i;  
            string s1;  
            s1=SubString(Text,location+1,textlen);   
            SetEraseMode(TRUE);  
            DrawTextString(s1);  
            SetEraseMode(FALSE);  
            MovePen(GetCurrentX()-TextStringWidth(s1),GetCurrentY());  
            DrawTextString(str);  
            DrawTextString(s1);  
            MovePen(GetCurrentX()-TextStringWidth(s1),GetCurrentY());  
            textlen++;  
            location++;  
            for(i=textlen;i>=location+1;i--){  
                Text[i]=Text[i-1];  
            }  
            Text[location]=key;  
        }  
        if (isBlink&&isDisplay) {  
            SetEraseMode(TRUE);  
            MovePen(GetCurrentX()-TextStringWidth(str), GetCurrentY());  
            DrawTextString("_");  
            MovePen(GetCurrentX()-TextStringWidth("_"),GetCurrentY());  
            SetEraseMode(FALSE);  
            DrawTextString(str);  
        }  
        else{  
            MovePen(GetCurrentX()-TextStringWidth(str),GetCurrentY());  
            DrawTextString(str);  
        }  
        x_location=GetCurrentX();  
        y_location=GetCurrentY();  
    }     
}  
  
void TimerEventProcess(int timerID)  
{  
    double x, y;  
    bool erasemode;  
    switch (timerID) {  
        case TIMER_BLINK:  
        if (!isBlink) return;  
        erasemode = GetEraseMode();  
        x = GetCurrentX();  
        y = GetCurrentY();  
        SetEraseMode(isDisplay);  
        DrawTextString("_");  
        MovePen(x, y);  
        if(textlen-1!=location){  
            char str[2]={0,0};  
            str[0]=Text[location+1];  
            SetEraseMode(FALSE);  
            DrawTextString(str);  
            MovePen(x,y);  
        }  
          SetEraseMode(erasemode);  
          isDisplay = !isDisplay;  
          break;  
        default:  
          break;  
      }  
}  
  
void Main()  
{  
    InitGraphics();     
    Set();  
    registerMouseEvent(MouseEventProcess);  
    registerKeyboardEvent(KeyboardEventProcess);  
    registerCharEvent(CharEventProcess);  
    registerTimerEvent(TimerEventProcess);  
}  
