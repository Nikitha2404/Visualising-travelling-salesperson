#include <iostream>
#include <cmath>
#include <GL/glut.h>
#include <vector>
#include <set>
#include <string>
#include <sstream>
#include <algorithm>
#include <thread>
#include <chrono>

using namespace std;

const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 800;
int nCitites;const int src=0;
bool isStarted = false;
static int cost=0;
int delay=1000;
// int graph[nCitites][nCitites]={{0,10,15,20},{10,0,35,25},{15,35,0,30},{20,25,30,0}};
GLfloat arr[6][2]={{-0.35f,0.85f},{0.35f,0.85f},{0.85f,0.0f},{0.35f,-0.85f},{-0.35f,-0.85f},{-0.7f,0.0f}};
int graph[6][6];
char ras[6]={'0', '1', '2', '3', '4', '5'};
GLfloat yy=0.9f;
vector<vector<int>> edges;

void drawFilledCircle(GLfloat x, GLfloat y){
	int i;
	int triangleAmount = 20; //# of triangles used to draw circle
	
	GLfloat radius = 0.07f; //radius
	GLfloat twicePi = 2.0f * M_PI;

	glBegin(GL_TRIANGLE_FAN);
		glVertex2f(x, y); // center of circle
		for(i = 0; i <= triangleAmount;i++) { 
			glVertex2f(
		            x + (radius * cos(i *  twicePi / triangleAmount)), 
			    y + (radius * sin(i * twicePi / triangleAmount))
			);
		}
	glEnd();
}

void labelvertices(){
    for(int i=0;i<nCitites;i++)
    {
        
        glColor3f(1.0f, 1.0f, 1.0f); 
        glRasterPos2f(arr[i][0]-0.01,arr[i][1]-0.02);
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,ras[i]);
    }
}

void printarray(int *arr){
    vector<int> temp;
    temp.push_back(0);
    for(int i=0; i<=nCitites; i++){
        if(arr[i]!=-1)
            temp.push_back(arr[i]);
    }
    edges.push_back(temp);
    cout<<endl;
}
void initialise(int *arr){
    for(int i=0; i<=nCitites; i++)
        arr[i]=-1;
}

int tsp(int v ,vector<int>* set,int path[]){

    if(set->empty()){
        
        return graph[v][src];
        vector<int> temp;
        temp.push_back(0);
        temp.push_back(v);
        edges.push_back(temp);
    }
    int size=set->size();
    int mincost=999999;
    for(int i:*set){
        
        vector<int> subset(set->begin(),set->end());
        subset.erase(remove(subset.begin(), subset.end(), i), subset.end());
        int temp[nCitites+1];
        initialise(temp);
        cost=graph[v][i]+tsp(i,&subset,temp);
        
        
        if(cost<mincost){
            mincost=cost;
            temp[size]=i;
            printarray(temp);

            for(int j=0;j<=size;j++){
                path[j]=temp[j];
            }
        }
    }
    return mincost;
}

void displaytext(string ss,GLfloat yy){
    cout<<ss<<endl;
    glColor3f(0.0f, 0.0f, 0.0f);
    glRasterPos2f(-0.9f,yy);
    for (char c : ss) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
    glFlush();
}

void highlightedge(){
    ostringstream ss;
    glColor3f(1,0,0);
    glLineWidth(8.0f);
    glBegin(GL_LINES);
    for(int j=0;j<edges[0].size();j++){
        glVertex2fv(arr[edges[0][j]]);
        ss<<edges[0][j];
    }
    displaytext(ss.str(),yy);
    glEnd();
    for(int j=0;j<edges[0].size();j++){
        glColor3f(0,1,0);
        drawFilledCircle(arr[edges[0][j]][0],arr[edges[0][j]][1]);
    }
    labelvertices();
    int i;
    for(i=1;i<edges.size();i++){
        
        glColor3f(0.0,0.0,0.0);
        glBegin(GL_LINE_STRIP);

        for(int j=0;j<edges[i-1].size();j++)
            glVertex2fv(arr[edges[i-1][j]]);
        for(int j=0;j<edges[i-1].size();j++){
            glColor3f(0.0,0.0,0.0);
            drawFilledCircle(arr[edges[i-1][j]][0],arr[edges[i-1][j]][1]);
        }

        glEnd();
        ss.str("");
        glColor3f(0,1,0);
        glBegin(GL_LINE_STRIP);

        for(int j=0;j<edges[i].size();j++){
            glVertex2fv(arr[edges[i][j]]);
            ss<<"->"<<edges[i][j];
        }
        for(int j=0;j<edges[i].size();j++){
            glColor3f(0,1,0);
            drawFilledCircle(arr[edges[i][j]][0],arr[edges[i][j]][1]);
        }
        yy=yy-0.05f;
        displaytext(ss.str(),yy);
        labelvertices();
        glEnd();
        glFlush();
        this_thread::sleep_for(chrono::milliseconds(2000));
    }

}
bool compareRows(const vector<int>& row1, const vector<int>& row2)
{
    return row1.size() < row2.size();
}
void startTsp(){
    vector<int> set;
    for(int i=1;i<nCitites;i++)
        set.push_back(i);
    int path[nCitites+1];
    cost=tsp(src,&set,path);
    cout<<"cost: "<<cost<<endl;
    path[0]=path[nCitites]=0;
    cout<<"path: "<<endl;
    for(int i=nCitites;i>=0;i--)
        cout<<path[i]<<endl;
    sort(edges.begin(), edges.end(), compareRows);

    highlightedge();
}

void init(){
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

}

void drawgraph(){
    glClear(GL_COLOR_BUFFER_BIT);
    // Draw vertices as points
    glColor3f(0.0f, 0.0f, 0.0f); // Set the color to white
    for(int i = 0; i <nCitites;i++)
    {
        drawFilledCircle(arr[i][0],arr[i][1]);
    }
    glEnd();
    
    // Draw edges as lines
    glColor3f(0.0f, 0.0f, 0.0f); // Set the color to gray
    glLineWidth(8.0f);
    
    for(int i=0;i<nCitites;i++)
    {
        for(int j=0;j<nCitites;j++)
        {
            if(graph[i][j]!=0)
            {
                glBegin(GL_LINES);
                glVertex2fv(arr[i]);
                glVertex2fv(arr[j]);
                glEnd();

                float labelX = (arr[i][0]+arr[j][0])/2 + 0.027f;
                float labelY = (arr[i][1] +arr[j][1] )/2 + 0.013f;
                
                // Render the label as bitmap characters
                glColor3f(0.0f, 0.0f, 0.0f); // Black color for labels
                glRasterPos2f(labelX, labelY);
                string label = to_string(graph[i][j]); // Use edge index as label
                for (char c : label) 
                    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
            
            }
        }
    }
    labelvertices();
    glFlush();
}

void display(){
    glLoadIdentity();
    //glTranslatef(-0.25f, -0.25f, 0.0f); // Translate to center the graph
    glLineWidth(3.0f);

    glColor3f(0.0f, 0.0f, 0.0f);  // black color
    glRasterPos2f(-0.05f, -0.85f);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,'T');
    drawgraph();
    // Draw the start button
    glColor3f(0.0f, 0.8f, 0.0f);     
    glBegin(GL_QUADS);
    glVertex2f(-0.1f, -0.9f);
    glVertex2f(0.1f, -0.9f);
    glVertex2f(0.1f, -0.8f);
    glVertex2f(-0.1f, -0.8f);
    glEnd();
    
    // Draw the button label
    glColor3f(0.0f, 0.0f, 0.0f);  // black color
    glRasterPos2f(-0.05f, -0.85f);
    if (isStarted) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'T');
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'S');
        // glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, '');
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'P');
        startTsp();
    } else {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'S');
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'T');
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'A');
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'R');
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'T');
    }

    glFlush();
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // Convert mouse coordinates to OpenGL coordinates
        float xPos = (2.0f * x) / WINDOW_WIDTH - 1.0f;
        float yPos = 1.0f - (2.0f * y) / WINDOW_HEIGHT;
        
        // Check if the click is within the button area
        if (xPos >= -0.2f && xPos <= 0.2f && yPos >= -0.9f && yPos <= -0.8f) {
            isStarted = !isStarted; 
            glutPostRedisplay();  // Request redrawing the scene
        }
    }
}

void printArray(){
    for(int i=0;i<nCitites;i++){
        for(int j=0;j<nCitites;j++)
            cout<<graph[i][j]<<" ";
        cout<<endl;
    }
}

int main(int argc,char* argv[]){
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("graph ");
    init();
    cout<<"enter no of cities:"<<endl;cin>>nCitites;
    cout<<"enter adjacency matrix"<<endl;
    for(int i=0;i<nCitites;i++){
        for(int j=0;j<nCitites;j++){
            cin>>graph[i][j];
        }
    }
    printArray();
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMainLoop();
    return 0;
}