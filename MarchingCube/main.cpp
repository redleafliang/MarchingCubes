//
//  main.cpp
//  MarchingCube
//
//  Created by Hongye Liang on 2019/5/28.
//  Copyright © 2019 Hongye Liang. All rights reserved.
//


#include <iostream>
#include <string>
#include "main.h"
#include "math.h"
#include <stdlib.h>
#include "stdio.h"
#include "GLUT/glut.h"

using namespace std;

#define width 256
#define height 256
#define layer 253

int data[width][height][layer];
int intersectCount = 0;
int triangleCount = 0;
float isoSurface = 100;
float XRotate = 180;
float YRotate = 0;
float ZRotate = -90;
float XOffset = -100;
float YOffset = -100;
float ZOffset = 0;
float Zoom = 100.0;

void render();
void MarchingCube(float X, float Y, float Z);
void MarchingCubes();
void mouseHandlers(int button, int state, int x ,int y);
void keyboardHandlers(unsigned char key, int x, int y);
void SetNormal(Point &Normal, int x, int y, int z);
//void SetColor(Point &Color, Point &Normal);
void resize(int wid, int hei);

int main(int argc, char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    
    FILE *fptr;
    
    if((fptr=fopen("/Users/hongyeliang/Desktop/MarchingCube/MarchingCube/sunxia_256.raw","rb"))==NULL)
    {
        cout<<"文件打开失败"<<endl;;
        return 0;
    }
    
    int i,j,k;
    int c;

    for (i=0;i<layer;i++) {
        for (j=0;j<height;j++) {
            for (k=0;k<width;k++) {
                if ((c = fgetc(fptr)) == EOF) {
                    fprintf(stderr,"Unexpected end of file\n");
                    exit(-1);
                }
                data[i][j][k] = c;
                
                
            }
        }
    }

    fclose(fptr);
    cout<< "读取模型完毕"<<endl;
    
    int wid = 800.0;
    int hei = 800.0;
    
    glutInit(&argc, argv);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(wid, hei);
    glutInitDisplayMode( GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE );
    glutCreateWindow( "Marching Cubes" );
    
    glutDisplayFunc(render);
    glutMouseFunc(mouseHandlers);
    glutKeyboardFunc(keyboardHandlers);
    glClearColor( 0.0, 0.0, 0.0, 1.0 );
    glClearDepth( 1.0 );
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    GLfloat light_ambient[] = {0.25, 0.25, 0.25, 1.00};
    glLightfv( GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glEnable( GL_LIGHT0 );
    
    
    resize(wid, hei);
    glutMainLoop();
    
    return 0;
}
void keyboardHandlers(unsigned char key, int x, int y)
{
    switch(key)
    {
        case 'w' :
            YRotate += 10;
            glutPostRedisplay();
            break;
        case 's' :
            YRotate -= 10;
            glutPostRedisplay();
            break;
        case 'a' :
            XRotate += 10;
            glutPostRedisplay();
            break;
        case 'd' :
            XRotate -= 10;
            glutPostRedisplay();
            break;
    }
}
void mouseHandlers(int button, int state, int x ,int y)
{
    switch (button)
    {
        case GLUT_LEFT_BUTTON:
            if (state == GLUT_DOWN){
                isoSurface += 10;
                cout<<"isoSurface up"<<endl;
                glutPostRedisplay();
            }
            break;
        case GLUT_RIGHT_BUTTON:
            if(state == GLUT_DOWN){
                isoSurface -= 10;
                cout<<"isoSurface down"<<endl;
                glutPostRedisplay();
            }
            break;
        case GLUT_WHEEL_UP:
            isoSurface += 10;
            cout<<"isoSurface up"<<endl;
            glutPostRedisplay();
            break;
        case GLUT_WHEEL_DOWN:
            isoSurface -= 10;
            cout<<"isoSurface down"<<endl;
            glutPostRedisplay();
            break;
            
            
        default:
            break;
    }

}
void resize(int wid, int hei)
{
    glViewport(0, 0, wid, hei);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    GLfloat fAspect = (GLfloat)(hei > wid ? hei / wid : wid / hei);

    glOrtho(-2*Zoom, 2*Zoom, -2*fAspect*Zoom,
            2*fAspect*Zoom, -10*2*Zoom, 10*2*Zoom);
    
    glMatrixMode(GL_MODELVIEW);
    
}

void render(){
    
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glPushMatrix();
    glTranslatef(XOffset, YOffset, ZOffset);
    glRotatef(XRotate, 1.0, 0.0, 0.0);
    glRotatef(YRotate, 0.0, 1.0, 0.0);
    glRotatef(ZRotate, 0.0, 0.0, 1.0);
    glPushAttrib(GL_LIGHTING_BIT);
    glDisable(GL_LIGHTING);
    glPopAttrib();
    glBegin(GL_TRIANGLES);
    MarchingCubes();
    glEnd();
    glPopMatrix();
    glutSwapBuffers();
}

void MarchingCube(float X, float Y, float Z){
    
    Point color;
    float CubeVertexValue[8];
    int i;
    for(i = 0; i < 8; i++)
    {
        CubeVertexValue[i] = (float)data[(int)(X + CubeVertex[i][0])]
        [(int)(Y + CubeVertex[i][1])]
        [(int)(Z + CubeVertex[i][2])] ;
    }
    
    int intersectEdge = 0;
    int intersectEdgeIndex = 0;
    Point intersectVertex[12];
    Point NormalVector[12];
    
    for(i = 0; i < 8; i++)
    {
        if(CubeVertexValue[i] <= isoSurface)
            intersectEdgeIndex |= 1<<i;
    }
    
    intersectEdge = CubeEdgeFlags[intersectEdgeIndex];
    if(intersectEdge == 0) return;
    
    intersectCount++;
    
    for(i = 0; i < 12; i++)
    {
        if(intersectEdge & (1<<i)){
            float difference = CubeVertexValue[CubeEdge[i][1]] - CubeVertexValue[CubeEdge[i][0]];
            float offset  = (isoSurface - CubeVertexValue[CubeEdge[i][0]]) / difference;
            intersectVertex[i].x = X + CubeVertex[CubeEdge[i][0]][0] + CubeEdgeDirection[i][0] * offset;
            intersectVertex[i].y = Y + CubeVertex[CubeEdge[i][0]][1] + CubeEdgeDirection[i][1] * offset;
            intersectVertex[i].z = Z + CubeVertex[CubeEdge[i][0]][2] + CubeEdgeDirection[i][2] * offset;
       
            SetNormal(NormalVector[i], (int)intersectVertex[i].x, (int)intersectVertex[i].y, (int)intersectVertex[i].z);

        }
    }
    
    for(i = 0; i < 5; i++)
    {
        
        if(TriTable[intersectEdgeIndex][3*i] < 0)
            break;
        triangleCount++;
        for(int j = 0; j < 3; j++)
        {
            int vertex = TriTable[intersectEdgeIndex][3*i+j];
            
//            SetColor(color, NormalVector[vertex]);
//            glColor3f(color.x, color.y, color.z);
            
            glColor3f(intersectVertex[vertex].x, intersectVertex[vertex].y, intersectVertex[vertex].z);
            glNormal3f(NormalVector[vertex].x,   NormalVector[vertex].y,   NormalVector[vertex].z);
            glVertex3f(intersectVertex[vertex].x, intersectVertex[vertex].y, intersectVertex[vertex].z);

        }
    }
    
    
}

void MarchingCubes()
{
    int i, j, k;
    for(i = 0; i < width-1; i++)
        for(j = 0; j < height-1; j++)
            for(k = 0; k < layer-1; k++)
                MarchingCube(i, j, k);
    cout<<intersectCount<<endl;
    cout<<triangleCount<<endl;
}


void SetNormal(Point &Normal, int x, int y, int z)
{
    if (x>1 && x<width-1 && y>1 && y<height-1 && z>1 && z<layer-1)
    {
        Normal.x = (float)(data[x-1][y][z] - data[x+1][y][z]);
        Normal.y = (float)(data[x][y-1][z] - data[x][y+1][z]);
        Normal.z = (float)(data[x][y][z-1] - data[x][y][z+1]);
        float length = sqrtf( (Normal.x * Normal.x) + (Normal.y * Normal.y) + (Normal.z * Normal.z) );
        Normal.x = Normal.x * 1.0 / length;
        Normal.y = Normal.y * 1.0 / length;
        Normal.z = Normal.z * 1.0 / length;
    }
}

//这个函数没有任何卵子用
//void SetColor(Point &Color, Point &Normal)
//{
//    GLfloat fX = Normal.x;
//    GLfloat fY = Normal.y;
//    GLfloat fZ = Normal.z;
//    //方法一
//    Color.x = fX ;
//    Color.y = fY ;
//    Color.z = fZ ;
//    //方法二
////    Color.x = fX -0.5*fY -0.5*fZ ;
////    Color.y = fY -0.5*fZ -0.5*fX ;
////    Color.z = fZ -0.5*fX -0.5*fY ;
//    //方法三
////    Color.x = (fX > 0.0 ? fX : 0.0) + (fY < 0.0 ? -0.5*fY : 0.0) + (fZ < 0.0 ? -0.5*fZ : 0.0);
////    Color.y = (fY > 0.0 ? fY : 0.0) + (fZ < 0.0 ? -0.5*fZ : 0.0) + (fX < 0.0 ? -0.5*fX : 0.0);
////    Color.z = (fZ > 0.0 ? fZ : 0.0) + (fX < 0.0 ? -0.5*fX : 0.0) + (fY < 0.0 ? -0.5*fY : 0.0);
//}
