////////////////////////////////////
// Genetic Programming Example Code
// Young-Min Kang (c) 2015
// Tongmyong University
////////////////////////////////////

#include <stdlib.h>
#ifdef WIN32
#include <GL/glut.h>
#else
#include <OpenGL/OpenGL.h>
#include <GLUT/GLUT.h>
#endif
#include <math.h>
#include <stdio.h>

#define MAX_OBSTACLES 50

// genetice programming parameters
#define POPULATION 1024*16
#define SELECTION_ITER 2
#define COMPETITION_ACCURACY_RATE 0.95
#define MUTE_PROB_IN_GENE 0.2
#define MUTE_PROB_IN_POP 0.2

#define swap(x,y,t) t=x; x=y; y=t;

class Circle {
public:
    float x,y;
    float radius;
};
class Gene {
public:
    //sign(1) + x(20) + sign(1) + y(20) + radius(20);
    bool str[62];
};

bool reset = true;


Circle obstacleCircles[MAX_OBSTACLES];
Circle geneticCircles[POPULATION];
Gene   genes[POPULATION];

float fit[POPULATION];

float convertNumber(bool *str, int idx) {
    float val=0;
    for(int i=0;i<20;i++) {
        val*= 2.0;
        val+= str[i+idx]?1:0;
    }
    val /= 1048576.0;
    return val;
}

void convertGeneToPhenotype(Gene g, float &x, float &y, float &r) {
    float val = convertNumber(g.str, 1);
    x = g.str[0]?val:-val;
    val = convertNumber(g.str, 22);
    y = g.str[21]?val:-val;
    r = convertNumber(g.str, 42);
}

void setGeneToCircle(Gene g, Circle &c) {
    convertGeneToPhenotype(g, c.x, c.y, c.radius);
}

void createRandomGene(Gene &g) {
    for(int i=0;i<62;i++) {
        g.str[i] = (rand()%2)?true:false;
    }
}

void createInitialGeneration() {
    for(int i=0;i<POPULATION;i++) {
        createRandomGene(genes[i]);
        setGeneToCircle(genes[i], geneticCircles[i]);
    }
}

float Distance(Circle a, Circle b) {
    float x = a.x - b.x;
    float y = a.y - b.y;
    return sqrt(x*x+y*y)-a.radius-b.radius;
}

bool OutOfBounds(Circle c) {
    if(c.x>1.0-c.radius) return true;
    if(c.x<-1.0+c.radius) return true;
    if(c.y>1.0-c.radius) return true;
    if(c.y<-1.0+c.radius) return true;
    return false;
}

float fitness(Circle c) {
    for(int i=0;i<MAX_OBSTACLES;i++) {
        if( Distance(c,obstacleCircles[i])<0 ) {
            return 0 ;
        }
        
        
    }
    if( OutOfBounds(c) ) return 0;
    return c.radius ;
}


void createRandomCircles() {
    for(int i=0;i<MAX_OBSTACLES;i++) {
        obstacleCircles[i].x = float(rand()%1000)/500.0 - 1.0;
        obstacleCircles[i].y = float(rand()%1000)/500.0 - 1.0;
        obstacleCircles[i].radius = float(rand()%1000)/10000.0;
    }
}

int findBestGene() {
    int idx=0;
    float f = fit[0];
    for(int i=1;i<POPULATION;i++) {
        if(fit[i]>f) {
            f = fit[i];
            idx = i;
        }
    }
    return idx;
}

void drawCircles() {
    
    for(int i=0;i<MAX_OBSTACLES;i++) {
        float angle = 0.0;
        float angleStep = 3.141592/10.0;
        
        glBegin(GL_POLYGON);
        for(int j=0;j<20;j++) {
            angle+=angleStep;
            glVertex3f(obstacleCircles[i].x + cos(angle)*obstacleCircles[i].radius ,
                       obstacleCircles[i].y + sin(angle)*obstacleCircles[i].radius, 0);
        }
        glEnd();
    }
    
}
void drawGeneticCircles() {
    
    float angle;
    float angleStep = 3.141592/10.0;
    
    int best = findBestGene();
    
    for(int i=0;i<POPULATION;i++) {
        angle = 0.0;
        if(best==i) {
            glColor3f(0,0,1);
            glLineWidth(5);
        }
        else {
            if(fit[i]>0.0)glColor3f(0,0,1);
            else glColor3f(0.9,0.9,0.9);
            glLineWidth(1);
        }
        glBegin(GL_LINE_LOOP);
        for(int j=0;j<20;j++) {
            angle+=angleStep;
            glVertex3f(geneticCircles[i].x + cos(angle)*geneticCircles[i].radius ,
                       geneticCircles[i].y + sin(angle)*geneticCircles[i].radius, 0);
        }
        glEnd();
    }
    
     glLineWidth(1);
}

Gene mutate(Gene g) {
    for(int i=0;i<62;i++) {
        if(float(rand()%1000)/999.0<MUTE_PROB_IN_GENE) g.str[i] = g.str[i]?false:true;
    }
    return g;
}

Gene crossover(Gene g1, Gene g2) {
    Gene newgene;
    int  switchingLen = rand()%62;
    
    bool mode = (rand()%2)?1:0;
    for(int i=0;i<62;i++) {
        newgene.str[i] = (mode)?g1.str[i]:g2.str[i];
        if(i>=switchingLen) mode = !mode;
    }
    return newgene;
}

void shuffleGenes() {
    for(int i=0;i<POPULATION;i++) {
        int j = rand()%POPULATION;
        int k = rand()%POPULATION;
        Gene temp;
        swap(genes[j], genes[k], temp);
        
    }
}

void computeFitness() {
    for(int i=0;i<POPULATION;i++) {
        fit[i] = fitness(geneticCircles[i]);
    }
    
}

int compete() {
    // Selection
    int alive = POPULATION;
    for(int i=0;i<SELECTION_ITER;i++) {
        int div = 1;
        for(int j=0;j<i;j++) div*=2;
        for(int k=0;k<POPULATION/div-1;k+=2) {
            float fitnessCompare = fit[k] - fit[k+1];
            float temp = float(rand()%10000)/10000.0;
            if(temp>COMPETITION_ACCURACY_RATE) fitnessCompare*=-1.0;
            fit[k/2] = (fitnessCompare)>0?fit[k]:fit[k+1];
            genes[k/2] = (fitnessCompare)>0?genes[k]:genes[k+1];
        }
        alive /= 2;
    }
    return alive;
}

void nextGeneration() {
    
    int best = findBestGene();
    Gene g = genes[best];
    
    int alive = compete();
    
    for(int i=alive; i<POPULATION;i++) {
        genes[i] = crossover(genes[rand()%alive], genes[rand()%alive]);
    }
    genes[0] = g;
    shuffleGenes();
    best = findBestGene();
    for(int i=0;i<POPULATION;i++) {
        if(i!=best && float(rand()%1000)/999.0<MUTE_PROB_IN_POP) genes[i] = mutate(genes[i]);
        setGeneToCircle(genes[i], geneticCircles[i]);
    }
}


static void
resize(int width, int height) {
    const float a = (float) width / (float) height;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-a*1.2,a*1.2,-1.2,1.2,1,-1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity() ;
}



static void
display(void)
{
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3d(1,0,0);
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    
    float d;
    glBegin(GL_LINES);
    for(int i=0;i<11;i++) {
        d = float(i)*0.2;
        glVertex3f(-1,1-d,0);
        glVertex3f(1,1-d,0);
        glVertex3f(-1+d,1,0);
        glVertex3f(-1+d,-1,0);
    }
    glEnd();
    
    if(reset) {
        createRandomCircles();
        createInitialGeneration();
        reset = false;
    }
    glColor3f(1,0,0);
    drawCircles();
    
    
    computeFitness();
    drawGeneticCircles();
    
    nextGeneration();
    
    
    glutSwapBuffers();
}


static void
key(unsigned char key, int x, int y) {
    switch (key)  {
        case 27 :exit(0);
        case 'r': reset = true;
    }
    glutPostRedisplay(); 
    
} 

static void idle(void) { glutPostRedisplay(); }


/* Program entry point */ 

int 
main(int argc, char *argv[]) 
{ 
    glutInit(&argc, argv); 
    glutInitWindowSize(640,480); 
    glutInitWindowPosition(10,10); 
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH); 
    
    glutCreateWindow("강영민 - genetic circle");
    
    glutReshapeFunc(resize); 
    glutDisplayFunc(display); 
    glutKeyboardFunc(key); 
    glutIdleFunc(idle); 
    
    glClearColor(1,1,1,1); 
    glEnable(GL_CULL_FACE); 
    glCullFace(GL_BACK); 
    
    glEnable(GL_DEPTH_TEST); 
    glDepthFunc(GL_LESS); 
    
    glDisable(GL_LIGHTING);
    
    createRandomCircles();
    createInitialGeneration();	
    glutMainLoop(); 
    return 0; 
} 
