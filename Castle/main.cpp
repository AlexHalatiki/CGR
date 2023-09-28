// gcc castle.c -o castle -lglut -lGL -lGLU -lm && ./castle
#include <stdio.h>
#include <GL/glut.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

typedef struct Esfera {
	double raio;
	GLint slices;
	GLint stacks;
};

typedef struct Cilindro {
	GLUquadric* quad;
	GLdouble base;
	GLdouble top;
	GLdouble height;
	GLint slices;
	GLint stacks;
} Cilindro;

typedef struct Coordenada {
	GLfloat x;
	GLfloat y;
	GLfloat z;
} Coordenada;

typedef struct Rgb {
	GLfloat red;
	GLfloat green;
	GLfloat blue;
} Rgb;

typedef struct Rotacao {
	GLfloat angle;
	GLfloat x;
	GLfloat y;
	GLfloat z;
} Rotacao;

typedef struct Paleta {
	unsigned int paredes;
	unsigned int telhado;
	unsigned int chao;
	unsigned int porta;
	unsigned int grama;
	unsigned int chuva;
} Paleta;

typedef struct Particle {
	Coordenada cordenada;
	Coordenada velocidade;
};

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 600
#define T_CUBO 0.5f
#define NUM_PARTICLES 6000
#define GRAVITY 0.0005

static GLfloat posCamera[3] = { 0.0f, 0.0f, 10.0f };
static GLfloat direcaoCamera[3] = { 0.0f, 0.0f, -1.0f };
static GLfloat cameraUp[3] = { 0.0f, 1.0f, 0.0f };
static GLfloat velocCamera = 0.2f;

const GLfloat alturaParedes = (2.7 / 2.2 * 2);

static GLfloat xRot = 45.0f;
static GLfloat yRot = 20.0f;
static GLfloat zoom = -5.0f;

Rotacao rotation;
Coordenada position;
Coordenada coordinate;
Coordenada scale;
Cilindro cylinder;
Particle particles[NUM_PARTICLES];
float translateTower = 1.35;

Paleta cores = {
	0xcfcfcf,
	0x401803,
	0x111414,
	0x331906,
	0x40EA2C,
	0x00FFF
};

Rgb hexTo3f(int hexValue) {
	Rgb rgb;
	rgb.red = ((hexValue >> 16) & 0xFF) / 255.0;
	rgb.green = ((hexValue >> 8) & 0xFF) / 255.0;
	rgb.blue = ((hexValue) & 0xFF) / 255.0;

	return rgb;
}

void escolherCor(int cor) {
	Rgb rgb = hexTo3f(cor);
	glColor3f(rgb.red, rgb.green, rgb.blue);
}

void drawCube(int cor, double size, Rotacao rotation, Coordenada position, Coordenada scale) {
	glPushMatrix();
	escolherCor(cor);

	glTranslatef(position.x, position.y, position.z);

	glRotatef(rotation.angle, rotation.x, rotation.y, rotation.z);

	glScalef(scale.x, scale.y, scale.z);

	glutSolidCube(size);

	glPopMatrix();
}

void drawCilindro(int cor, Cilindro cylinder, Rotacao rotation, Coordenada position) {

	glPushMatrix();
	escolherCor(cor);

	glTranslatef(position.x, position.y, position.z);

	glRotatef(rotation.angle, rotation.x, rotation.y, rotation.z);

	gluCylinder(cylinder.quad, cylinder.base, cylinder.top, cylinder.height, cylinder.slices, cylinder.stacks);

	glPopMatrix();
}

void drawSphere(int cor, Esfera esfera, Rotacao rotacao, Coordenada coordenada) {
	glPushMatrix();

	escolherCor(cor);

	glTranslatef(coordenada.x, coordenada.y, coordenada.z);

	glRotatef(rotacao.angle, rotacao.x, rotacao.y, rotacao.z);

	glutSolidSphere(esfera.raio, esfera.slices, esfera.stacks);

	glPopMatrix();
}

Esfera newEsfera(double raio, GLint slices, GLint stacks) {
	Esfera esfera;
	esfera.raio = raio;
	esfera.slices = slices;
	esfera.stacks = stacks;

	return esfera;
}

Cilindro newCilindro(GLUquadric* quad, GLdouble base, GLdouble top, GLdouble height, GLint slices, GLint stacks) {
	Cilindro cylinder;
	cylinder.quad = quad;
	cylinder.base = base;
	cylinder.top = top;
	cylinder.height = height;
	cylinder.slices = slices;
	cylinder.stacks = stacks;

	return cylinder;
}

Rotacao newRotacao(GLfloat angle, GLfloat x, GLfloat y, GLfloat z) {
	Rotacao rotation;
	rotation.angle = angle;
	rotation.x = x;
	rotation.y = y;
	rotation.z = z;

	return rotation;
}

Coordenada newCoordenada(GLfloat x, GLfloat y, GLfloat z) {
	Coordenada position;
	position.x = x;
	position.y = y;
	position.z = z;

	return position;
}

Particle newParticle(Coordenada position, Coordenada velocity) {
	Particle particle;
	particle.cordenada = position;
	particle.velocidade = velocity;

	return particle;
}

void iniciarParticulas() {
	for (int i = 0; i < NUM_PARTICLES; ++i) {
		Coordenada position = newCoordenada((rand() % 200 - 100) / 20.0f, 4.0f + (rand() % 206) / 20.0f, (rand() % 200 - 100) / 20.0f);
		Coordenada velocity = newCoordenada(0.0f, GRAVITY * (rand() % 10 + 1), 0.0f);
		particles[i].cordenada = position;
		particles[i].velocidade = velocity;
	}
}


void updateParticles() {
	for (int i = 0; i < NUM_PARTICLES; ++i) {
		if (particles[i].cordenada.y > 0.225f) {
			particles[i].velocidade.y += GRAVITY;
			particles[i].cordenada.x -= particles[i].velocidade.x;
			particles[i].cordenada.y -= particles[i].velocidade.y;
			particles[i].cordenada.z -= particles[i].velocidade.z;
		}
		else {
			particles[i].cordenada.x = (rand() % 200 - 100) / 20.0f;
			particles[i].cordenada.y = 4.0f + (rand() % 76) / 20.0f;
			particles[i].cordenada.z = (rand() % 200 - 100) / 20.0f;
			particles[i].velocidade.y = GRAVITY * (rand() % 10 + 1);
		}
	}
}

void criarParedeTraseira(GLfloat anguloX, GLfloat anguloZ, GLfloat translateX, GLfloat translateY, GLfloat translateZ) {
	glPushMatrix();
	coordinate = newCoordenada(5.4, alturaParedes, 0.3);
	rotation = newRotacao(anguloX, 0, anguloZ, 0);

	position = newCoordenada(translateX * translateTower, translateY + coordinate.y / 4, translateZ * translateTower);
	scale = newCoordenada(coordinate.x, coordinate.y, coordinate.z);
	drawCube(cores.paredes, T_CUBO, rotation, position, scale);

	coordinate = newCoordenada(0.77, alturaParedes, 0.3);
	scale = newCoordenada(coordinate.x, coordinate.y / 3, coordinate.z);

	position = newCoordenada(translateX * translateTower, 0.35 + coordinate.y * 0.325, translateZ * translateTower);
	drawCube(cores.paredes, T_CUBO, rotation, position, scale);

	position = newCoordenada(-0.5 * translateTower, 0.35 + coordinate.y * 0.325, translateZ * translateTower);
	drawCube(cores.paredes, T_CUBO, rotation, position, scale);

	position = newCoordenada(0.5 * translateTower, 0.35 + coordinate.y * 0.325, translateZ * translateTower);
	drawCube(cores.paredes, T_CUBO, rotation, position, scale);
	glPopMatrix();
}

void criarParedeFrontal(GLfloat anguloX, GLfloat anguloZ, GLfloat translateX, GLfloat translateY, GLfloat translateZ) {
	glPushMatrix();
	coordinate = newCoordenada(5.4, (alturaParedes) / 2, 0.3);
	rotation = newRotacao(anguloX, 0, anguloZ, 0);

	position = newCoordenada(translateX * translateTower, coordinate.y - (((coordinate.y / 3) / 2) - 0.0375), translateZ * translateTower);
	scale = newCoordenada(coordinate.x, coordinate.y * 0.5, coordinate.z);
	drawCube(cores.paredes, T_CUBO, rotation, position, scale);

	coordinate = newCoordenada(0.77, alturaParedes, 0.3);
	scale = newCoordenada(coordinate.x, coordinate.y / 3, coordinate.z);

	position = newCoordenada(translateX * translateTower, 0.35 + coordinate.y * 0.325, translateZ * translateTower);
	drawCube(cores.paredes, T_CUBO, rotation, position, scale);

	coordinate = newCoordenada(5.4, 0, 0.3);
	scale = newCoordenada(0.35 * coordinate.x, alturaParedes, coordinate.z);
	position = newCoordenada(-translateTower + 0.5, 0.35 + coordinate.y * 0.325, translateZ * translateTower);
	drawCube(cores.paredes, T_CUBO, rotation, position, scale);

	position = newCoordenada(translateTower - 0.5, 0.35 + coordinate.y * 0.325, translateZ * translateTower);
	drawCube(cores.paredes, T_CUBO, rotation, position, scale);

	coordinate = newCoordenada(0.77, alturaParedes, 0.3);
	scale = newCoordenada(coordinate.x, coordinate.y / 3, coordinate.z);

	position = newCoordenada(-0.5 * translateTower, 0.35 + coordinate.y * 0.325, translateZ * translateTower);
	drawCube(cores.paredes, T_CUBO, rotation, position, scale);

	position = newCoordenada(0.5 * translateTower, 0.35 + coordinate.y * 0.325, translateZ * translateTower);
	drawCube(cores.paredes, T_CUBO, rotation, position, scale);

	position = newCoordenada(translateX * translateTower, coordinate.y / 4, 1.4625);
	scale = newCoordenada(translateTower * 1.15f, coordinate.y * 1.0f, coordinate.z / 2);
	drawCube(cores.porta, T_CUBO, rotation, position, scale);

	glPopMatrix();
}

void criarParedeLateral(GLfloat anguloX, GLfloat anguloZ, GLfloat translateX, GLfloat translateY, GLfloat translateZ) {
	glPushMatrix();

	coordinate = newCoordenada(5.4, alturaParedes, 0.3);
	rotation = newRotacao(anguloX, 0, anguloZ, 0);
	position = newCoordenada(translateX * translateTower, translateY + coordinate.y / 4, translateZ * translateTower);
	scale = newCoordenada(coordinate.x, coordinate.y, coordinate.z);
	drawCube(cores.paredes, T_CUBO, rotation, position, scale);

	coordinate = newCoordenada(0.77, alturaParedes, 0.3);
	scale = newCoordenada(coordinate.x, coordinate.y / 3, coordinate.z);
	position = newCoordenada(translateX * translateTower, 0.35 + coordinate.y * 0.325, translateZ * translateTower);
	drawCube(cores.paredes, T_CUBO, rotation, position, scale);

	scale = newCoordenada(coordinate.x, coordinate.y / 3, coordinate.z);
	position = newCoordenada(translateX * translateTower, 0.35 + coordinate.y * 0.325, translateZ * translateTower + (coordinate.x));
	drawCube(cores.paredes, T_CUBO, rotation, position, scale);

	position = newCoordenada(translateX * translateTower, 0.35 + coordinate.y * 0.325, translateZ * translateTower - (coordinate.x));
	drawCube(cores.paredes, T_CUBO, rotation, position, scale);

	glPopMatrix();
}



void ChangeSize(int w, int h) {
	GLfloat fAspect;


	if (h == 0) h = 1;

	glViewport(0, 0, w, h);

	fAspect = (GLfloat)w / (GLfloat)h;


	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();


	gluPerspective(35.0f, fAspect, 1.0, 40.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void keyboardCallback(unsigned char key, int x, int y) {
	switch (key) {

	case 'w':
	case 'W':
		posCamera[1] += velocCamera;
		break;


	case 's':
	case 'S':
		posCamera[1] -= velocCamera;
		break;


	case 'a':
	case 'A':
		posCamera[0] -= velocCamera;
		break;


	case 'D':
	case 'd':
		posCamera[0] += velocCamera;
		break;


	case 'z':
		zoom -= velocCamera * 2;
		break;


	case 'Z':
		zoom += velocCamera * 2;
		break;


	case 27:
		exit(0);
		break;
	}


	glutPostRedisplay();
}

void Setup() {
	GLfloat whiteLight[] = { 0.4, 0.4, 0.4, 1.0f };
	GLfloat sourceLight[] = { 0.25f, 0.25f, 0.25f, 1.0f };
	GLfloat lightPos[] = { -10.f, 5.0f, 5.0f, 1.0f };

	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);

	glEnable(GL_LIGHTING);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, whiteLight);
	glLightfv(GL_LIGHT0, GL_AMBIENT, sourceLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, sourceLight);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glEnable(GL_LIGHT0);

	glEnable(GL_COLOR_MATERIAL);

	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	Rgb rgbColor = hexTo3f(0xE8DACC);

	glClearColor(rgbColor.red, rgbColor.green, rgbColor.blue, 1.0f);
}

void SpecialKeys(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_DOWN:
		yRot -= 5.0f;
		break;

	case GLUT_KEY_UP:
		yRot += 5.0f;
		break;

	case GLUT_KEY_LEFT:
		xRot += 5.0f;
		break;

	case GLUT_KEY_RIGHT:
		xRot -= 5.0f;
		break;
	}

	xRot = (GLfloat)((const int)xRot % 360);
	yRot = (GLfloat)((const int)yRot % 360);


	glutPostRedisplay();
}

void criarTorre(GLUquadricObj* pObj, GLfloat raioTorre, GLfloat alturaTorre, Coordenada position) {
	glPushMatrix();

	rotation = newRotacao(90.0f, -5.0f, 0.0f, 0.0f);


	cylinder = newCilindro(pObj, raioTorre, raioTorre, alturaTorre * 0.7, 26, 13);
	drawCilindro(cores.paredes, cylinder, rotation, position);

	Coordenada coordenadasTopoTorre = newCoordenada(position.x, (alturaTorre * 0.7), position.z);
	cylinder = newCilindro(pObj, raioTorre * 1.2f, raioTorre * 1.1f, alturaTorre * 0.3, 26, 13);
	drawCilindro(cores.paredes, cylinder, rotation, coordenadasTopoTorre);


	cylinder = newCilindro(pObj, 0.0, raioTorre * 1.5f, raioTorre * 1.35f, 26, 13);
	Coordenada topPosition = newCoordenada(position.x, (alturaTorre * 0.99) + cylinder.height, position.z);
	rotation = newRotacao(90.0f, 5.0f, 0.0f, 0.0f);
	drawCilindro(cores.telhado, cylinder, rotation, topPosition);

	glPopMatrix();
}


void criarParedes() {
	criarParedeLateral(90.0, 90.0, -1, 0.0, 0);
	criarParedeLateral(90.0, 90.0, 1, 0.0, 0);
	criarParedeTraseira(0.0, 0.0, 0, 0.0, -1);
	criarParedeFrontal(0.0, 0.0, 0, 0.0, 1);
}

void criarTorres(GLUquadricObj* pObj) {
	criarTorre(pObj, 0.4f, 2.0f, newCoordenada(translateTower, 0.0, translateTower));
	criarTorre(pObj, 0.4f, 2.0f, newCoordenada(-translateTower, 0.0, translateTower));
	criarTorre(pObj, 0.4f, 2.0f, newCoordenada(translateTower, 0.0, -translateTower));
	criarTorre(pObj, 0.4f, 2.0f, newCoordenada(-translateTower, 0.0, -translateTower));
	criarTorre(pObj, 0.45f, 3.0f, newCoordenada(0.0, 0.0, 0.0));
}

void criarChao() {
	GLfloat tamChao = 20.0f;

	position = newCoordenada(0.0, 0.0001, 0.0);
	scale = newCoordenada(tamChao, tamChao, 0.9f);
	rotation = newRotacao(-90.0f, 90.0f, 0.0f, 0.0f);
	drawCube(cores.grama, T_CUBO, rotation, position, scale);
	position = newCoordenada(0.0, 0.01, 0.0f);
	scale = newCoordenada(8.8f, 8.8f, 0.9003f);
	rotation = newRotacao(-90.0f, 90.0f, 0.0f, 0.0f);
	drawCube(cores.chao, T_CUBO, rotation, position, scale);
}

void criarChuva() {
	updateParticles();

	for (int i = 0; i < NUM_PARTICLES; ++i)
		if(particles[i].cordenada.y < 4.0)
			drawSphere(cores.chuva, newEsfera(0.01, 20, 20), newRotacao(0.0f, 0.0f, 0.0f, 0.0f), particles[i].cordenada);

	glutPostRedisplay();
}

void RenderScene(void) {
	GLUquadricObj* pObj;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(posCamera[0], posCamera[1], posCamera[2],
		posCamera[0] + direcaoCamera[0], posCamera[1] + direcaoCamera[1], posCamera[2] + direcaoCamera[2],
		cameraUp[0], cameraUp[1], cameraUp[2]);

	glPushMatrix();

	glTranslatef(0.0f, -1.0f, zoom);
	glRotatef(yRot, 1.0f, 0.0f, 0.0f);
	glRotatef(xRot, 0.0f, 1.0f, 0.0f);

	pObj = gluNewQuadric();
	gluQuadricNormals(pObj, GLU_SMOOTH);

	criarChao();
	criarTorres(pObj);
	criarParedes();
	criarChuva();

	glPopMatrix();
	glutSwapBuffers();
}

int main(int argc, char* argv[]) {

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

	glutCreateWindow("Castle");

	glutReshapeFunc(ChangeSize);

	glutKeyboardFunc(keyboardCallback);

	glutSpecialFunc(SpecialKeys);

	iniciarParticulas();

	glutDisplayFunc(RenderScene);

	glutIdleFunc(criarChuva);

	Setup();

	glutMainLoop();

	return 0;
}