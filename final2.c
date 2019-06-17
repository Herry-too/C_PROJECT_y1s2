/*
WARNING: This code is only tested in Manjaro 18.0.4 Illyria with gcc (GCC) 8.2.1,
if you use MinGW on Windows, some features don't work!
Please follow the instructions to change some of the code in lines 168 to 175!
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
double t,t_delay_max,t_delay_fall;
double G_d,G_print_fix;
int G_fall_stop = 0;

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
    
    glPointSize(5.0); 
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
    int stop_1_max = 0;
	for(t=0; t <= G_t0 && stop_image == 0; t+=0.0005) {
        glBegin(GL_POINTS);
        glColor3f(1.0f, 1.0f, 1.0f); 
        glVertex2f(10*(G_v0*cos(G_angle_RAD_0)*t)*G_print_fix,10*(G_v0*sin(G_angle_RAD_0)*t-0.5*g*t*t)*G_print_fix);
        glColor3f(1.0f, 0.0f, 0.0f); 
        if((G_v0*cos(G_angle_RAD_0)*t) <= (G_d-G_v1*cos(G_angle_RAD_1_rise)*t)){
            glVertex2f(10*(G_d-G_v1*cos(G_angle_RAD_1_rise)*t)*G_print_fix,10*(G_v1*sin(G_angle_RAD_1_rise)*t-0.5*g*t*t)*G_print_fix);
        }
        glColor3f(0.0f, 1.0f, 0.0f); 
        if(t-t_delay_max>=0 && t_delay_max>=0 && stop_1_max == 0){
            glVertex2f(10*(G_d-G_v1*cos(G_angle_RAD_1_maxheight)*(t-t_delay_max))*G_print_fix,10*(G_v1*sin(G_angle_RAD_1_maxheight)*(t-t_delay_max)-0.5*g*(t-t_delay_max)*(t-t_delay_max))*G_print_fix);
            if(fabs((G_v0*cos(G_angle_RAD_0)*t)-(G_d-G_v1*cos(G_angle_RAD_1_maxheight)*(t-t_delay_max)))<0.1){
                stop_1_max = 1;
            }
        }
        glColor3f(1.0f, 1.0f, 0.0f); 
        if(t-t_delay_fall>=0 && t_delay_fall>=0 && (G_v0*cos(G_angle_RAD_0)*t) <= 3*range(G_v0,G_angle_RAD_0)/4 && G_fall_stop == 0){
            glVertex2f(10*(G_d-G_v1*cos(G_angle_RAD_1_fall)*(t-t_delay_fall))*G_print_fix,10*(G_v1*sin(G_angle_RAD_1_fall)*(t-t_delay_fall)-0.5*g*(t-t_delay_fall)*(t-t_delay_fall))*G_print_fix);
        }
        if(t_delay_max>=0){
            if((G_v0*cos(G_angle_RAD_0)*t) > 3*range(G_v0,G_angle_RAD_0)/4){
                stop_image = 1;
            }
        }else
        {
            if((G_v0*cos(G_angle_RAD_0)*t) > (G_d-G_v1*cos(G_angle_RAD_1_rise)*t)){
                stop_image = 1;
            }
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
    scanf("%lf", &angle_d_0);//change into angle_d_0 = 70
    printf("Input Interceptor Speed(v0):");
    scanf("%lf", &G_v1);//change into G_v1 = 40
    printf("Input distance between two points:");
    scanf("%lf",&G_d);//change into G_d = 60
    G_angle_RAD_0 = (angle_d_0*PI)/180;

    G_print_fix = 80 / G_d;
    if(maxheight(G_v0,G_angle_RAD_0)*G_print_fix>60){
        G_print_fix = 60/(maxheight(G_v0,G_angle_RAD_0));
    }
    if(range(G_v0,G_angle_RAD_0)*G_print_fix>80){
        G_print_fix = 80/(range(G_v0,G_angle_RAD_0));
    }

    G_t0 = time(G_v0,G_angle_RAD_0);
    G_angle_RAD_1_rise = asin(G_v0*sin(G_angle_RAD_0)/G_v1);
    G_angle_RAD_1_maxheight = angle_RAD_with_maxheight_calculate(G_v0,G_angle_RAD_0,G_v1,G_d);
    G_angle_RAD_1_fall = angle_RAD_with_fall_calculate(G_v0,G_angle_RAD_0,G_v1,G_d);

    double t_v0_maxh,t_v1_maxh;
    t_v0_maxh = (range(G_v0,G_angle_RAD_0)/2)/(G_v0*cos(G_angle_RAD_0));
    t_v1_maxh = (G_d-(range(G_v0,G_angle_RAD_0)/2))/(G_v1*cos(G_angle_RAD_1_maxheight));
    t_delay_max = t_v0_maxh - t_v1_maxh;

    double t_v0_fall,t_v1_fall;
    t_v0_fall = t_v0_maxh/2*3;
    t_v1_fall = (G_d-(range(G_v0,G_angle_RAD_0)*3)/4)/(G_v1*cos(G_angle_RAD_1_fall));
    t_delay_fall = t_v0_fall - t_v1_fall;

    if(G_d > range(G_v0,G_angle_RAD_0) + range(G_v1,G_angle_RAD_1_rise) || isnan(G_angle_RAD_1_rise) == 1){
        printf("Interceptor speed is too low, intercepting missile is impossible!\n");
        exit(1);
    }
    int rubbish = 0;
    if(t_delay_max>0){
        printf("When the collision point is on the rise of the missile:\n");
        printf("Delay time: 0.00s\n");
        printf("Launch angle(Degree): %.2lf°\n",G_angle_RAD_1_rise*180/PI);
    }
    if(t_delay_max>=0){
        printf("When the collision point is the maximum height of the missile:\n");
        printf("Delay time: %.2lfs\n",t_delay_max);
        printf("Launch angle(Degree): %.2lf°\n",G_angle_RAD_1_maxheight*180/PI);
    }else
    {
        printf("It is impossible to intercept the missile on the rise or its max height!\n");
        printf("When the collision point is on the fall of the missile:\n");
        printf("Delay time: 0.00s\n");
        printf("Launch angle(Degree): %.2lf°\n",G_angle_RAD_1_rise*180/PI);
        rubbish = 1;
        G_fall_stop = 1;
    }
    if(t_delay_fall>0 && rubbish == 0){
        printf("When the collision point is on the fall of the missile:\n");
        printf("Delay time: %.2lfs\n",t_delay_fall);
        printf("Launch angle(Degree): %.2lf°\n",G_angle_RAD_1_fall*180/PI);
    }
    printf("Press <ENTER> to draw image...\n");
    getchar();//only use to pause
    rubbish = getchar();//only use to pause

    glutInit(&argc, argv); 
    
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB); 
    
    glutInitWindowSize(1200, 900);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Project - Herry001 - Part A");

    glutKeyboardFunc(keyPressFunc);

    initialize(); 
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}