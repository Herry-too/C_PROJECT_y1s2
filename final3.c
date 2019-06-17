/*
WARNING: This code is only tested in Manjaro 18.0.4 Illyria with gcc (GCC) 8.2.1,
if you use MinGW on Windows, some features don't work!
Please follow the instructions to change some of the code in lines 246 to 261!
*/
#include <stdio.h> 
#include <GL/glut.h> 
#include <math.h> 
#include <unistd.h>

#define ESC 27
#define PI 3.14159265
#define g 9.8

double G_r0, G_h0, G_t0,G_v0,G_angle_RAD_0;
double G_r1, G_h1, G_t1,G_v1,G_angle_RAD_1_rise,G_angle_RAD_1_maxheight,G_angle_RAD_1_fall;
double G_r2, G_h2, G_t2,G_v2,G_angle_RAD_2_rise,G_angle_RAD_2_maxheight,G_angle_RAD_2_fall;
double G_r3, G_h3, G_t3,G_v3,G_angle_RAD_3_rise,G_angle_RAD_3_maxheight,G_angle_RAD_3_fall;
double t,t_rise_delay;
double t_delay_rise_1,t_delay_max_1,t_delay_fall_1;
double t_delay_rise_2,t_delay_max_2,t_delay_fall_2;
double t_delay_rise_3,t_delay_max_3,t_delay_fall_3;
double G_d_1,G_d_2,G_d_3,G_print_fix;

double max3(double x1,double x2,double x3)
{
    double i;
    if(x1>x2){
        i = x1;
    }else
    {
        i = x2;
    }
    if(i<x3){
        i = x3;
    }
    return i;
}

double time(double v,double angle_R)
{
    return (2*v*sin(angle_R))/g;
}
double maxheight(double v,double angle_R)
{
    return (v*v*sin(angle_R)*sin(angle_R))/(2*g);
}
double range(double v,double angle_R)
{
    return (v*v*sin(angle_R*2))/(g);
}

double lagrange_calculate(double v0,double a0,double xp)
{
    double yp;
    int i,j;
    double x0 = 0,x1,x2,y0 = 0,y1,y2 = 0;
    x1 = range(v0,a0)/2;
    x2 = range(v0,a0);
    y1 = maxheight(v0,a0);
    double X[3] = {x0,x1,x2};
    double Y[3] = {y0,y1,y2};
    double Z[3] = {1,1,1};
    for(i=0;i<3;i++){
        for(j=0;j<3;j++){
            if(i!=j){
                Z[i] = Z[i] * (xp - X[j])/(X[i] - X[j]);
            }
        }
    }
    yp = Y[0]*Z[0] + Y[1]*Z[1] + Y[2]*Z[2];
    return yp;
}

double time_calculate_no_delay(double v0,double angle_RAD_0,double v1,double angle_RAD_1,double d)
{
    double t1;
    t1 = d/(v0*cos(angle_RAD_0)+v1*cos(angle_RAD_1));
    return t1;
}

double angle_RAD_with_point_calculate(double x,double y,double v1)
{
    double angle_RAD_1;
    double angle_RAD_1_avaliable[2] = {999,999};
    double t1,t2;
    double v = v1;
    t1 = (sqrt(2)*sqrt(sqrt((-2*g*v*v*y)-g*g*x*x+v*v*v*v)-g*y+v*v))/g;
    t2 = (sqrt(2)*sqrt((-sqrt((-2*g*v*v*y)-g*g*x*x+v*v*v*v))-g*y+v*v))/g;
    angle_RAD_1_avaliable[0] = acos(x/(v*t1));
    angle_RAD_1_avaliable[1] = acos(x/(v*t2));
    if(angle_RAD_1_avaliable[0]<PI/2){
        angle_RAD_1 = fmin(angle_RAD_1_avaliable[0],angle_RAD_1_avaliable[1]);
    }else
    {
        angle_RAD_1 = fmax(angle_RAD_1_avaliable[0],angle_RAD_1_avaliable[1]);
    }
    return angle_RAD_1;
}

double angle_RAD_with_maxheight_calculate(double v0,double angle_RAD_0,double v1,double d)
{
    double angle_RAD_1;
    double angle_RAD_1_avaliable[2] = {999,999};
    double t1,t2;
    double v = v1,x = d-(range(v0,angle_RAD_0)/2),y = maxheight(v0,angle_RAD_0);
    t1 = (sqrt(2)*sqrt(sqrt((-2*g*v*v*y)-g*g*x*x+v*v*v*v)-g*y+v*v))/g;
    t2 = (sqrt(2)*sqrt((-sqrt((-2*g*v*v*y)-g*g*x*x+v*v*v*v))-g*y+v*v))/g;
    angle_RAD_1_avaliable[0] = acos(x/(v*t1));
    angle_RAD_1_avaliable[1] = acos(x/(v*t2));
    if(angle_RAD_1_avaliable[0]<PI/2){
        angle_RAD_1 = fmin(angle_RAD_1_avaliable[0],angle_RAD_1_avaliable[1]);
    }else
    {
        angle_RAD_1 = fmax(angle_RAD_1_avaliable[0],angle_RAD_1_avaliable[1]);
    }
    return angle_RAD_1;
}

double angle_RAD_with_fall_calculate(double v0,double angle_RAD_0,double v1,double d)
{
    double angle_RAD_1;
    double angle_RAD_1_avaliable[2] = {999,999};
    double t1,t2;
    double v = v1,x = d-(range(v0,angle_RAD_0)*3)/4;
    double y = lagrange_calculate(v0,angle_RAD_0,(range(v0,angle_RAD_0)*3)/4);
    t1 = (sqrt(2)*sqrt(sqrt((-2*g*v*v*y)-g*g*x*x+v*v*v*v)-g*y+v*v))/g;
    t2 = (sqrt(2)*sqrt((-sqrt((-2*g*v*v*y)-g*g*x*x+v*v*v*v))-g*y+v*v))/g;
    angle_RAD_1_avaliable[0] = acos(x/(v*t1));
    angle_RAD_1_avaliable[1] = acos(x/(v*t2));
    if(angle_RAD_1_avaliable[0]<PI/2){
        angle_RAD_1 = fmin(angle_RAD_1_avaliable[0],angle_RAD_1_avaliable[1]);
    }else
    {
        angle_RAD_1 = fmax(angle_RAD_1_avaliable[0],angle_RAD_1_avaliable[1]);
    }
    return angle_RAD_1;
}

void initialize(void) 
{ 
    glClearColor(0.0, 0.0, 0.0, 1.0); 
    glColor3f(0.0, 1.0, 0.0); 
    
    glPointSize(3.0); 
    glMatrixMode(GL_PROJECTION); 
    glLoadIdentity(); 
    
    gluOrtho2D(0, 800, 0, 600); 
} 

static void keyPressFunc(unsigned char key, int x, int y)
{
    switch(key) {
        case ESC:
            exit(1);
    }
}

void display(void) 
{
    int stop_image = 0;
    int stop_1_max = 0,stop_2_max = 0,stop_3_max = 0;
	for(t=0; t <= G_t0 && stop_image == 0; t+=0.0005) {
        glBegin(GL_POINTS);
        glColor3f(1.0f, 1.0f, 1.0f); //导弹
        if((G_v0*cos(G_angle_RAD_0)*t)<=range(G_v0,G_angle_RAD_0)*3/4){
            glVertex2f(10*(G_v0*cos(G_angle_RAD_0)*t)*G_print_fix,10*(G_v0*sin(G_angle_RAD_0)*t-0.5*g*t*t)*G_print_fix);
        }
        glColor3f(1.0f, 0.078f, 0.576f);//拦截体1上升
        if(t-t_delay_rise_1>=0 && t_delay_rise_1>=-0.01 && (G_v0*cos(G_angle_RAD_0)*t) <= (G_d_1-G_v1*cos(G_angle_RAD_1_rise)*(t-t_delay_rise_1))){
            glVertex2f((G_d_1-G_v1*cos(G_angle_RAD_1_rise)*(t-t_delay_rise_1))*10*G_print_fix,10*G_print_fix*(G_v1*sin(G_angle_RAD_1_rise)*(t-t_delay_rise_1)-0.5*g*(t-t_delay_rise_1)*(t-t_delay_rise_1)));
        }
        glColor3f(0.5f, 0.9f, 0.5f); //拦截体1最高点
        if(t-t_delay_max_1>=0 && t_delay_max_1>=-0.01 && stop_1_max == 0){
            glVertex2f(10*(G_d_1-G_v1*cos(G_angle_RAD_1_maxheight)*(t-t_delay_max_1))*G_print_fix,10*(G_v1*sin(G_angle_RAD_1_maxheight)*(t-t_delay_max_1)-0.5*g*(t-t_delay_max_1)*(t-t_delay_max_1))*G_print_fix);
            if(fabs((G_v0*cos(G_angle_RAD_0)*t) - (G_d_1-G_v1*cos(G_angle_RAD_1_maxheight)*(t-t_delay_max_1)))<0.05){
                stop_1_max = 1;
            }
        }
        glColor3f(1.0f, 1.0f, 0.0f); //拦截体1下落
        if(t-t_delay_fall_1>=0 && t_delay_fall_1>=-0.01 && (G_v0*cos(G_angle_RAD_0)*t) <= 3*range(G_v0,G_angle_RAD_0)/4){
            glVertex2f(10*(G_d_1-G_v1*cos(G_angle_RAD_1_fall)*(t-t_delay_fall_1))*G_print_fix,10*(G_v1*sin(G_angle_RAD_1_fall)*(t-t_delay_fall_1)-0.5*g*(t-t_delay_fall_1)*(t-t_delay_fall_1))*G_print_fix);
        }

        glColor3f(1.0f, 0.0f, 0.0f);//拦截体2上升
        if(t-t_delay_rise_2>=0 && t_delay_rise_2>=-0.01 && (G_v0*cos(G_angle_RAD_0)*t) <= (G_d_2-G_v2*cos(G_angle_RAD_2_rise)*(t-t_delay_rise_2))){
            glVertex2f((G_d_2-G_v2*cos(G_angle_RAD_2_rise)*(t-t_delay_rise_2))*10*G_print_fix,10*G_print_fix*(G_v2*sin(G_angle_RAD_2_rise)*(t-t_delay_rise_2)-0.5*g*(t-t_delay_rise_2)*(t-t_delay_rise_2)));
        }
        glColor3f(0.0f, 1.0f, 0.0f); //拦截体2最高点
        if(t-t_delay_max_2>=0 && t_delay_max_2>=-0.01 && stop_2_max == 0){
            glVertex2f(10*(G_d_2-G_v2*cos(G_angle_RAD_2_maxheight)*(t-t_delay_max_2))*G_print_fix,10*(G_v2*sin(G_angle_RAD_2_maxheight)*(t-t_delay_max_2)-0.5*g*(t-t_delay_max_2)*(t-t_delay_max_2))*G_print_fix);
            if(fabs((G_v0*cos(G_angle_RAD_0)*t) - (G_d_2-G_v2*cos(G_angle_RAD_2_maxheight)*(t-t_delay_max_2)))<0.05){
                stop_2_max = 1;
            }
        }
        glColor3f(1.0f, 1.0f, 0.0f); //拦截体2下落
        if(t-t_delay_fall_2>=0 && t_delay_fall_2>=-0.01 && (G_v0*cos(G_angle_RAD_0)*t) <= 3*range(G_v0,G_angle_RAD_0)/4){
            glVertex2f(10*(G_d_2-G_v2*cos(G_angle_RAD_2_fall)*(t-t_delay_fall_2))*G_print_fix,10*(G_v2*sin(G_angle_RAD_2_fall)*(t-t_delay_fall_2)-0.5*g*(t-t_delay_fall_2)*(t-t_delay_fall_2))*G_print_fix);
        }

        glColor3f(0.6f, 0.2f, 0.8f);//拦截体3上升
        if(t-t_delay_rise_3>=0 && t_delay_rise_3>=-0.01 && (G_v0*cos(G_angle_RAD_0)*t) <= (G_d_3-G_v3*cos(G_angle_RAD_3_rise)*(t-t_delay_rise_3))){
            glVertex2f((G_d_3-G_v3*cos(G_angle_RAD_3_rise)*(t-t_delay_rise_3))*10*G_print_fix,10*G_print_fix*(G_v3*sin(G_angle_RAD_3_rise)*(t-t_delay_rise_3)-0.5*g*(t-t_delay_rise_3)*(t-t_delay_rise_3)));
        }
        glColor3f(0.1f, 0.5f, 0.1f); //拦截体3最高点
        if(t-t_delay_max_3>=0 && t_delay_max_3>=-0.01 && stop_3_max == 0){
            glVertex2f(10*(G_d_3-G_v3*cos(G_angle_RAD_3_maxheight)*(t-t_delay_max_3))*G_print_fix,10*(G_v3*sin(G_angle_RAD_3_maxheight)*(t-t_delay_max_3)-0.5*g*(t-t_delay_max_3)*(t-t_delay_max_3))*G_print_fix);
            if(fabs((G_v0*cos(G_angle_RAD_0)*t) - (G_d_3-G_v3*cos(G_angle_RAD_3_maxheight)*(t-t_delay_max_3)))<0.05){
                stop_3_max = 1;
            }
        }
        glColor3f(1.0f, 0.64f, 0.0f); //拦截体3下落
        if(t-t_delay_fall_3>=0 && t_delay_fall_3>=-0.01 && (G_v0*cos(G_angle_RAD_0)*t) <= 3*range(G_v0,G_angle_RAD_0)/4){
            glVertex2f(10*(G_d_3-G_v3*cos(G_angle_RAD_3_fall)*(t-t_delay_fall_3))*G_print_fix,10*(G_v3*sin(G_angle_RAD_3_fall)*(t-t_delay_fall_3)-0.5*g*(t-t_delay_fall_3)*(t-t_delay_fall_3))*G_print_fix);
        }
        glEnd();
	    glFlush();
	}
} 

int main(int argc, char** argv)
{
    double angle_d_0;
    printf("Input Missile Speed(v0):");
    scanf("%lf", &G_v0);//change into G_v0 = 20
    printf("Input Missile Launch angle(Degree):");
    scanf("%lf", &angle_d_0);//change into angle_d_0 = 50
    printf("Input Interceptor P1 Speed(v1):");
    scanf("%lf", &G_v1);//change into G_v1 = 30
    printf("Input distance between missile and interceptor P1:");
    scanf("%lf",&G_d_1);//change into G_d_1 = 40
    G_v2 = 2 * G_v1;
    printf("Interceptor P2 Speed(v2) = %.2lf\n",G_v2);
    printf("Input distance between missile and interceptor P2:");
    scanf("%lf",&G_d_2);//change into G_d_2 = 60
    G_v3 = 3 * G_v1;
    printf("Interceptor P3 Speed(v3) = %.2lf\n",G_v3);
    printf("Input distance between missile and interceptor P3:");
    scanf("%lf",&G_d_3);//change into G_d_3 = 80
    
    G_angle_RAD_0 = (angle_d_0*PI)/180;
    G_t0 = time(G_v0,G_angle_RAD_0);

    G_print_fix = 80 / max3(G_d_1,G_d_2,G_d_3);
    if(maxheight(G_v0,G_angle_RAD_0)*G_print_fix>60){
        G_print_fix = 60/(maxheight(G_v0,G_angle_RAD_0));
    }
    if(range(G_v0,G_angle_RAD_0)*G_print_fix>80){
        G_print_fix = 80/(range(G_v0,G_angle_RAD_0));
    }

    G_angle_RAD_1_rise = asin(G_v0*sin(G_angle_RAD_0)/G_v1);
    G_angle_RAD_1_maxheight = angle_RAD_with_maxheight_calculate(G_v0,G_angle_RAD_0,G_v1,G_d_1);
    G_angle_RAD_1_fall = angle_RAD_with_fall_calculate(G_v0,G_angle_RAD_0,G_v1,G_d_1);

    G_angle_RAD_2_rise = asin(G_v0*sin(G_angle_RAD_0)/G_v2);
    G_angle_RAD_2_maxheight = angle_RAD_with_maxheight_calculate(G_v0,G_angle_RAD_0,G_v2,G_d_2);
    G_angle_RAD_2_fall = angle_RAD_with_fall_calculate(G_v0,G_angle_RAD_0,G_v2,G_d_2);

    G_angle_RAD_3_rise = asin(G_v0*sin(G_angle_RAD_0)/G_v3);
    G_angle_RAD_3_maxheight = angle_RAD_with_maxheight_calculate(G_v0,G_angle_RAD_0,G_v3,G_d_3);
    G_angle_RAD_3_fall = angle_RAD_with_fall_calculate(G_v0,G_angle_RAD_0,G_v3,G_d_3);

    double t_v1_nodelay,t_v2_nodelay,t_v3_nodelay;
    t_v1_nodelay = time_calculate_no_delay(G_v0,G_angle_RAD_0,G_v1,G_angle_RAD_1_rise,G_d_1);
    t_v2_nodelay = time_calculate_no_delay(G_v0,G_angle_RAD_0,G_v2,G_angle_RAD_2_rise,G_d_2);
    t_v3_nodelay = time_calculate_no_delay(G_v0,G_angle_RAD_0,G_v3,G_angle_RAD_3_rise,G_d_3);
    double t_rise = max3(t_v1_nodelay,t_v2_nodelay,t_v3_nodelay);
    
    double t_point_x,t_point_y;
    t_point_x = t_rise * G_v0 * cos(G_angle_RAD_0);
    t_point_y = t_rise * G_v0 * sin(G_angle_RAD_0) - 0.5 * g * t_rise * t_rise;
    
    G_angle_RAD_1_rise = angle_RAD_with_point_calculate(G_d_1-t_point_x,t_point_y,G_v1);
    G_angle_RAD_2_rise = angle_RAD_with_point_calculate(G_d_2-t_point_x,t_point_y,G_v2);
    G_angle_RAD_3_rise = angle_RAD_with_point_calculate(G_d_3-t_point_x,t_point_y,G_v3);

    double t_v0_rise,t_v1_rise,t_v2_rise,t_v3_rise;
    t_v0_rise = t_point_x / (G_v0*cos(G_angle_RAD_0));
    t_v1_rise = (G_d_1 - t_point_x)/ (G_v1*cos(G_angle_RAD_1_rise));
    t_v2_rise = (G_d_2 - t_point_x)/ (G_v2*cos(G_angle_RAD_2_rise));
    t_v3_rise = (G_d_3 - t_point_x)/ (G_v3*cos(G_angle_RAD_3_rise));    
    t_delay_rise_1 = t_v0_rise - t_v1_rise;
    t_delay_rise_2 = t_v0_rise - t_v2_rise;
    t_delay_rise_3 = t_v0_rise - t_v3_rise;

    double t_v0_maxh,t_v1_maxh,t_v2_maxh,t_v3_maxh;
    t_v0_maxh = (range(G_v0,G_angle_RAD_0)/2)/(G_v0*cos(G_angle_RAD_0));
    t_v1_maxh = (G_d_1-(range(G_v0,G_angle_RAD_0)/2))/(G_v1*cos(G_angle_RAD_1_maxheight));
    t_v2_maxh = (G_d_2-(range(G_v0,G_angle_RAD_0)/2))/(G_v2*cos(G_angle_RAD_2_maxheight));
    t_v3_maxh = (G_d_3-(range(G_v0,G_angle_RAD_0)/2))/(G_v3*cos(G_angle_RAD_3_maxheight));
    t_delay_max_1 = t_v0_maxh - t_v1_maxh;
    t_delay_max_2 = t_v0_maxh - t_v2_maxh;
    t_delay_max_3 = t_v0_maxh - t_v3_maxh;

    double t_v0_fall,t_v1_fall,t_v2_fall,t_v3_fall;
    t_v0_fall = t_v0_maxh/2*3;
    t_v1_fall = (G_d_1-(range(G_v0,G_angle_RAD_0)*3)/4)/(G_v1*cos(G_angle_RAD_1_fall));
    t_v2_fall = (G_d_2-(range(G_v0,G_angle_RAD_0)*3)/4)/(G_v2*cos(G_angle_RAD_2_fall));
    t_v3_fall = (G_d_3-(range(G_v0,G_angle_RAD_0)*3)/4)/(G_v3*cos(G_angle_RAD_3_fall));
    t_delay_fall_1 = t_v0_fall - t_v1_fall;
    t_delay_fall_2 = t_v0_fall - t_v2_fall;
    t_delay_fall_3 = t_v0_fall - t_v3_fall;

    glutInit(&argc, argv); 
    
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB); 
    
    glutInitWindowSize(1200, 900);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Project - Herry001 - Part B");

    glutKeyboardFunc(keyPressFunc);

    initialize(); 
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}