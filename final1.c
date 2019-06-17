/*
WARNING: This code is only tested in Manjaro 18.0.4 Illyria with gcc (GCC) 8.2.1,
if you use MinGW on Windows, some features don't work!
Please follow the instructions to change some of the code in lines 72 to 79!
*/
#include <stdio.h> 
#include <GL/glut.h> 
#include <math.h> 
#include <unistd.h>

#define ESC 27
#define PI 3.14159265
#define g 9.8

double G_r0, G_h0, G_t0,G_v0,G_angle_RAD_0;
double G_r1, G_h1, G_t1,G_v1,G_angle_RAD_1;
double t;
double G_d;

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
	for(t=0; t <= G_t0 && (G_v0*cos(G_angle_RAD_0)*t) <= (G_d-G_v1*cos(G_angle_RAD_1)*t); t+=0.0005) {
        glBegin(GL_POINTS);
        glColor3f(1.0f, 0.0f, 0.0f); 
        glVertex2f(10*(G_v0*cos(G_angle_RAD_0)*t),10*(G_v0*sin(G_angle_RAD_0)*t-0.5*g*t*t));
    	glEnd();
	    glFlush();
        glBegin(GL_POINTS);
        glColor3f(0.0f, 1.0f, 0.0f); 
        glVertex2f(10*(G_d-G_v1*cos(G_angle_RAD_1)*t),10*(G_v1*sin(G_angle_RAD_1)*t-0.5*g*t*t));
    	glEnd();
	    glFlush();
	}
} 
  
int main(int argc, char** argv)
{
    double angle_d_0;
    printf("Input Missile velocity(v0):");
    scanf("%lf", &G_v0);//change into G_v0 = 20
    printf("Input Missile Launch angle(Degree):");
    scanf("%lf", &angle_d_0);//change into angle_d_0 = 50
    printf("Input Interceptor velocity(v0):");
    scanf("%lf", &G_v1);//change into G_v1 = 30
    printf("Input distance between two points:");
    scanf("%lf",&G_d);//change into G_d = 60
    G_angle_RAD_0 = (angle_d_0*PI)/180;
    G_t0 = time(G_v0,G_angle_RAD_0);
    G_angle_RAD_1 = asin(G_v0*sin(G_angle_RAD_0)/G_v1);

    glutInit(&argc, argv); 
    
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB); 
    
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Project - Herry001 - Part A");

    glutKeyboardFunc(keyPressFunc);

    initialize(); 
    glutDisplayFunc(display);
    glutMainLoop();
} 
