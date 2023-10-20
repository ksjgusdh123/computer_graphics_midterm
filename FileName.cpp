#define _CRT_SECURE_NO_WARNINGS //--- ���α׷� �� �տ� ������ ��
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <gl/glew.h> // �ʿ��� ������� include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <random>
#include <vector>

char* filetobuf(const char* file);
void make_vertexShaders();
void make_fragmentShaders();
GLvoid make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLvoid Mouse_Click(int button, int state, int x, int y);
GLvoid InitBuffer();
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Make_Tri();
GLvoid change_Tri(int i);
GLvoid Timer_event(int value);
GLvoid Re_Buffer();
GLvoid line_to_tri();
GLvoid tri_to_rec();
GLvoid rec_to_penta();
GLvoid only_line();
GLvoid penta_to_dot();

GLvoid bline_to_tri();
GLvoid btri_to_rec();
GLvoid brec_to_penta();
GLvoid bpenta_to_dot();

GLvoid copy_arr();
//--- �ʿ��� ���� ����
GLint width, height;
GLuint shaderProgramID; //--- ���̴� ���α׷� �̸�
GLuint vertexShader; //--- ���ؽ� ���̴� ��ü
GLuint fragmentShader; //--- �����׸�Ʈ ���̴� ��ü

std::random_device rd;
std::mt19937 dre(rd());
std::uniform_real_distribution<float> uid{ 0.1, 0.4 };

//--- ���� �Լ�
void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ����
{
	//--- ������ �����ϱ�
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Example1");
	//--- GLEW �ʱ�ȭ�ϱ�
	glewExperimental = GL_TRUE;
	glewInit();
	make_shaderProgram();
	InitBuffer();
	glutDisplayFunc(drawScene);
	glutMouseFunc(Mouse_Click);
	glutKeyboardFunc(Keyboard);
	glutTimerFunc(100, Timer_event, 1);
	glutReshapeFunc(Reshape);
	glutMainLoop();
}



//--- �ٽñ׸��� �ݹ� �Լ�
GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ�
{
	glViewport(0, 0, w, h);
}

GLint result;
GLchar errorLog[512];

void make_vertexShaders()
{
	GLchar* vertexSource;
	vertexSource = filetobuf("����vertex.glsl");
	//--- ���ؽ� ���̴� ��ü �����
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//--- ���̴� �ڵ带 ���̴� ��ü�� �ֱ�
	glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);
	//--- ���ؽ� ���̴� �������ϱ�
	glCompileShader(vertexShader);
	//--- �������� ����� ���� ���� ���: ���� üũ
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader ������ ����\n" << errorLog << std::endl;
		return;
	}
}

void make_fragmentShaders()
{
	GLchar* fragmentSource;
	fragmentSource = filetobuf("����fragment.glsl");
	//--- �����׸�Ʈ ���̴� ��ü �����
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	//--- ���̴� �ڵ带 ���̴� ��ü�� �ֱ�
	glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);
	//--- �����׸�Ʈ ���̴� ������
	glCompileShader(fragmentShader);
	//--- �������� ����� ���� ���� ���: ������ ���� üũ
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: fragment shader ������ ����\n" << errorLog << std::endl;
		return;
	}
}

GLvoid make_shaderProgram()
{
	GLuint shaderID;
	make_vertexShaders(); //--- ���ؽ� ���̴� �����
	make_fragmentShaders(); //--- �����׸�Ʈ ���̴� �����
	//-- shader Program
	shaderProgramID = glCreateProgram();
	glAttachShader(shaderProgramID, vertexShader);
	glAttachShader(shaderProgramID, fragmentShader);
	glLinkProgram(shaderProgramID);
	//--- ���̴� �����ϱ�
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	//--- Shader Program ����ϱ�
	glUseProgram(shaderProgramID);

}

char* filetobuf(const char* file)
{
	FILE* fptr;
	long length;
	char* buf;
	fptr = fopen(file, "rb"); // Open file for reading 
	if (!fptr) // Return NULL on failure 
		return NULL;
	fseek(fptr, 0, SEEK_END); // Seek to the end of the file 
	length = ftell(fptr); // Find out how many bytes into the file we are 
	buf = (char*)malloc(length + 1); // Allocate a buffer for the entire length of the file and a null terminator 
	fseek(fptr, 0, SEEK_SET); // Go back to the beginning of the file 
	fread(buf, length, 1, fptr); // Read the contents of the file in to the buffer 
	fclose(fptr); // Close the file 
	buf[length] = 0; // Null terminator 
	return buf; // Return the buffer 
}

GLuint vbo[2], vao;
GLuint line_vbo[2];
GLuint tri_vbo[2];
GLuint rec_vbo[2];
GLuint penta_vbo[2];

GLuint bline_vbo[2];
GLuint btri_vbo[2];
GLuint brec_vbo[2];
GLuint bpenta_vbo[2];

GLfloat line_background[4][3]{ {0, 1, 0}, {0, -1, 0}, {1, 0, 0}, {-1, 0, 0} };
GLfloat back_colors[4][3]{ {1,1,0},{1,1,0},{1,1,0},{1,1,0} };

GLfloat line[3][3]{ {-0.9, 0.3, 0}, {-0.4, 0.8, 0}, {-0.4, 0.8, 0} };
GLfloat line_colors[3][3]{ {1,0,0},{1,0,0},{1,0,0} };

GLfloat tri[4][3]{ {0.2, 0.2, 0}, {0.8, 0.2, 0},  {0.5, 0.8, 0}, {0.5, 0.8, 0} };
GLfloat tri_colors[4][3]{ {1,1,0},{1,1,0},{1,1,0},{1,1,0} };

GLfloat rec[5][3]{ {-0.8, -0.2, 0}, {-0.8, -0.8, 0}, {-0.5, -0.2, 0}, {-0.2, -0.8, 0}, {-0.2, -0.2, 0} };
GLfloat rec_colors[5][3]{ {0,1,0}, {0,1,0}, {0,1,0}, {0,1,0}, {0,1,0} };

GLfloat penta[5][3]{ {0.1, -0.3, 0}, {0.25, -0.8, 0}, {0.5, -0.1, 0}, {0.75, -0.8, 0}, {0.9, -0.3, 0} };
GLfloat penta_colors[5][3]{ {0,0,1}, {0,0,1}, {0,0,1}, {0,0,1}, {0,0,1} };

GLfloat bline[3][3]{ {-0.4, -0.6, 0}, {0.4, 0.6, 0}, {0.4, 0.6, 0} };

GLfloat btri[4][3]{ {-0.6, -0.6, 0}, {0.6, -0.6, 0},  {0.0, 0.8, 0}, {0.0, 0.6, 0} };

GLfloat brec[5][3]{ {-0.8, -0.2, 0}, {-0.8, -0.8, 0}, {-0.5, -0.2, 0}, {-0.2, -0.8, 0}, {-0.2, -0.2, 0} };

GLfloat bpenta[5][3]{ {0.1, -0.3, 0}, {0.25, -0.8, 0}, {0.5, -0.1, 0}, {0.75, -0.8, 0}, {0.9, -0.3, 0} };


float ox, oy;

bool start = false;
bool first = false;
int menu = 0;

GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
{
	GLfloat rColor = 1.0, gColor = 1.0, bColor = 1.0;
	//--- ����� ���� ����
	glClearColor(rColor, gColor, bColor, 1.0f);
	//glClearColor(1.0, 1.0, 1.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//--- ������ ���������ο� ���̴� �ҷ�����
	glUseProgram(shaderProgramID);
	//--- ����� VAO �ҷ�����
	glBindVertexArray(vao);
	//--- �ﰢ�� �׸���
	switch (menu) {
	case 0:
		only_line();
		line_to_tri();
		tri_to_rec();
		rec_to_penta();
		penta_to_dot();
		break;
	case 1:
		bline_to_tri();
		break;
	case 2:
		btri_to_rec();
		break;
	case 3:
		brec_to_penta();
		break;
	case 4:
		bpenta_to_dot();
		break;
	}



	glutSwapBuffers(); //--- ȭ�鿡 ����ϱ�
}


GLvoid ConvertXY_OPENGL(int x, int y)
{
	int w = 800;
	int h = 600;

	ox = (float)(x - (float)w / 2.0) * (float)(1.0 / (float)(w / 2.0));
	oy = -(float)(y - (float)h / 2.0) * (float)(1.0 / (float)(h / 2.0));

}

GLvoid Mouse_Click(int button, int state, int x, int y) {
	ConvertXY_OPENGL(x, y);
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

	}
}

GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 's':
		if (!start && !first) {
			start = true;
			first = true;
		}
		break;
	case 'l':
		start = false;
		first = false;
		copy_arr();
		menu = 1;
		Re_Buffer();
		break;
	case 't':
		first = false;
		start = false;
		copy_arr();
		menu = 2;
		Re_Buffer();
		break;
	case 'r':
		first = false;
		start = false;
		copy_arr();
		menu = 3;
		Re_Buffer();
		break;
	case 'p':
		first = false;
		start = false;
		copy_arr();
		menu = 4;
		Re_Buffer();
		break;
	case 'a':
		first = false;
		start = false;
		copy_arr();
		menu = 0;
		Re_Buffer();
		break;
	}
	glutPostRedisplay(); //--- ������ �ٲ� ������ ��� �ݹ� �Լ��� ȣ���Ͽ� ȭ���� refresh �Ѵ�
}


GLvoid Timer_event(int value) {






	glutPostRedisplay(); //--- ������ �ٲ� ������ ��� �ݹ� �Լ��� ȣ���Ͽ� ȭ���� refresh �Ѵ�
	glutTimerFunc(100, Timer_event, 4);
}


GLvoid InitBuffer()
{
	glGenVertexArrays(1, &vao); //--- VAO �� �����ϰ� �Ҵ��ϱ�
	glBindVertexArray(vao); //--- VAO�� ���ε��ϱ�
	glGenBuffers(2, vbo); //--- 2���� VBO�� �����ϰ� �Ҵ��ϱ�
	//--- 1��° VBO�� Ȱ��ȭ�Ͽ� ���ε��ϰ�, ���ؽ� �Ӽ� (��ǥ��)�� ����
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	//--- ���� diamond ���� ���ؽ� ������ ���� ���ۿ� �����Ѵ�.
	//--- triShape �迭�� ������: 9 * float
	glBufferData(GL_ARRAY_BUFFER, sizeof(line_background), line_background, GL_STATIC_DRAW);
	//--- ��ǥ���� attribute �ε��� 0���� ����Ѵ�: ���ؽ� �� 3* float
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	//--- attribute �ε��� 0���� ��밡���ϰ� ��
	glEnableVertexAttribArray(0);
	//--- 2��° VBO�� Ȱ��ȭ �Ͽ� ���ε� �ϰ�, ���ؽ� �Ӽ� (����)�� ����
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	//--- ���� colors���� ���ؽ� ������ �����Ѵ�.
	//--- colors �迭�� ������: 9 *float 
	glBufferData(GL_ARRAY_BUFFER, sizeof(line_background), back_colors, GL_STATIC_DRAW);
	//--- ������ attribute �ε��� 1���� ����Ѵ�: ���ؽ� �� 3*float 
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	//--- attribute �ε��� 1���� ��� �����ϰ� ��.
	glEnableVertexAttribArray(1);

	glGenBuffers(2, line_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, line_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, line_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line_colors), line_colors, GL_STATIC_DRAW);

	glGenBuffers(2, tri_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, tri_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tri), tri, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, tri_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tri_colors), tri_colors, GL_STATIC_DRAW);

	glGenBuffers(2, rec_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, rec_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rec), rec, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, rec_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rec_colors), rec_colors, GL_STATIC_DRAW);

	glGenBuffers(2, penta_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, penta_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(penta), penta, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, penta_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(penta_colors), penta_colors, GL_STATIC_DRAW);

	glGenBuffers(2, bline_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, bline_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(bline), bline, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, bline_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line_colors), line_colors, GL_STATIC_DRAW);

	glGenBuffers(2, btri_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, btri_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(btri), btri, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, btri_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tri_colors), tri_colors, GL_STATIC_DRAW);

	glGenBuffers(2, brec_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, brec_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(brec), brec, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, brec_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rec_colors), rec_colors, GL_STATIC_DRAW);

	glGenBuffers(2, bpenta_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, bpenta_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(bpenta), bpenta, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, bpenta_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(penta_colors), penta_colors, GL_STATIC_DRAW);
}

GLvoid Re_Buffer() {
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	//--- ���� diamond ���� ���ؽ� ������ ���� ���ۿ� �����Ѵ�.
	//--- triShape �迭�� ������: 9 * float
	glBufferData(GL_ARRAY_BUFFER, sizeof(line_background), line_background, GL_STATIC_DRAW);
	//--- ��ǥ���� attribute �ε��� 0���� ����Ѵ�: ���ؽ� �� 3* float
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	//--- attribute �ε��� 0���� ��밡���ϰ� ��
	glEnableVertexAttribArray(0);
	//--- 2��° VBO�� Ȱ��ȭ �Ͽ� ���ε� �ϰ�, ���ؽ� �Ӽ� (����)�� ����
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	//--- ���� colors���� ���ؽ� ������ �����Ѵ�.
	//--- colors �迭�� ������: 9 *float 
	glBufferData(GL_ARRAY_BUFFER, sizeof(line_background), back_colors, GL_STATIC_DRAW);
	//--- ������ attribute �ε��� 1���� ����Ѵ�: ���ؽ� �� 3*float 
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	//--- attribute �ε��� 1���� ��� �����ϰ� ��.
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, line_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, line_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line_colors), line_colors, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, tri_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tri), tri, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, tri_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tri_colors), tri_colors, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, rec_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rec), rec, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, rec_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rec_colors), rec_colors, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, penta_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(penta), penta, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, penta_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(penta_colors), penta_colors, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, bline_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(bline), bline, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, bline_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line_colors), line_colors, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, btri_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(btri), btri, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, btri_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tri_colors), tri_colors, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, brec_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(brec), brec, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, brec_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rec_colors), rec_colors, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, bpenta_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(bpenta), bpenta, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, bpenta_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(penta_colors), penta_colors, GL_STATIC_DRAW);
}

GLvoid only_line() {
	int PosLocation = glGetAttribLocation(shaderProgramID, "in_Position"); //	: 0  Shader�� 'layout (location = 0)' �κ�
	int ColorLocation = glGetAttribLocation(shaderProgramID, "in_Color"); //	: 1


	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]); // VBO Bind
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		// PosLocation			- Location ��ȣ
		// 3					- VerTex Size (x, y, z �Ӽ��� Vec3�̴� 3) 
		// GL_FLOAT, GL_FALSE	- �ڷ����� Normalize ����
		// sizeof(float) * 3	- VerTex ������ ���� ũ�� (�� �������� �޸� ����)
		//			(0�� ����)	- 0 �� ��� �ڵ����� 2��° ����(3) x 3��° ����(float)�� ����
		// 0					- ������ ���� offset (0�̸� ������ ó������ ����)
	}
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]); // VBO Bind
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	}
	glDrawArrays(GL_LINES, 0, 4); // ������� ���
}

GLvoid line_to_tri() {
	if (start) {
		line[2][0] += 0.04;
		line[2][1] -= 0.083;
		line[1][0] -= 0.025;
	}



	int PosLocation = glGetAttribLocation(shaderProgramID, "in_Position"); //	: 0  Shader�� 'layout (location = 0)' �κ�
	int ColorLocation = glGetAttribLocation(shaderProgramID, "in_Color"); //	: 1


	{
		glBindBuffer(GL_ARRAY_BUFFER, line_vbo[0]); // VBO Bind
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		// PosLocation			- Location ��ȣ
		// 3					- VerTex Size (x, y, z �Ӽ��� Vec3�̴� 3) 
		// GL_FLOAT, GL_FALSE	- �ڷ����� Normalize ����
		// sizeof(float) * 3	- VerTex ������ ���� ũ�� (�� �������� �޸� ����)
		//			(0�� ����)	- 0 �� ��� �ڵ����� 2��° ����(3) x 3��° ����(float)�� ����
		// 0					- ������ ���� offset (0�̸� ������ ó������ ����)
	}
	{
		glBindBuffer(GL_ARRAY_BUFFER, line_vbo[1]); // VBO Bind
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	}
	if (line[1][0] == line[2][0]) {
		glDrawArrays(GL_LINES, 0, 3); // ������� ���
	}
	else {
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 3); // ������� ���
	}
}


GLvoid tri_to_rec() {
	if (start) {
		tri[2][0] -= 0.05;
		tri[3][0] += 0.05;
		Re_Buffer();
		if (tri[2][0] <= 0.2) {
			start = false;
		}
	}

	int PosLocation = glGetAttribLocation(shaderProgramID, "in_Position"); //	: 0  Shader�� 'layout (location = 0)' �κ�
	int ColorLocation = glGetAttribLocation(shaderProgramID, "in_Color"); //	: 1


	{
		glBindBuffer(GL_ARRAY_BUFFER, tri_vbo[0]); // VBO Bind
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		// PosLocation			- Location ��ȣ
		// 3					- VerTex Size (x, y, z �Ӽ��� Vec3�̴� 3) 
		// GL_FLOAT, GL_FALSE	- �ڷ����� Normalize ����
		// sizeof(float) * 3	- VerTex ������ ���� ũ�� (�� �������� �޸� ����)
		//			(0�� ����)	- 0 �� ��� �ڵ����� 2��° ����(3) x 3��° ����(float)�� ����
		// 0					- ������ ���� offset (0�̸� ������ ó������ ����)
	}
	{
		glBindBuffer(GL_ARRAY_BUFFER, tri_vbo[1]); // VBO Bind
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	}
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // ������� ���
}

GLvoid rec_to_penta() {
	if (start) {
		rec[0][0] -= 0.02;
		rec[0][1] -= 0.02;
		rec[1][0] += 0.01;
		rec[2][1] += 0.02;
		rec[3][0] -= 0.01;
		rec[4][0] += 0.02;
		rec[4][1] -= 0.02;
	}

	int PosLocation = glGetAttribLocation(shaderProgramID, "in_Position"); //	: 0  Shader�� 'layout (location = 0)' �κ�
	int ColorLocation = glGetAttribLocation(shaderProgramID, "in_Color"); //	: 1


	{
		glBindBuffer(GL_ARRAY_BUFFER, rec_vbo[0]); // VBO Bind
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		// PosLocation			- Location ��ȣ
		// 3					- VerTex Size (x, y, z �Ӽ��� Vec3�̴� 3) 
		// GL_FLOAT, GL_FALSE	- �ڷ����� Normalize ����
		// sizeof(float) * 3	- VerTex ������ ���� ũ�� (�� �������� �޸� ����)
		//			(0�� ����)	- 0 �� ��� �ڵ����� 2��° ����(3) x 3��° ����(float)�� ����
		// 0					- ������ ���� offset (0�̸� ������ ó������ ����)
	}
	{
		glBindBuffer(GL_ARRAY_BUFFER, rec_vbo[1]); // VBO Bind
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	}
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 5); // ������� ���
}

GLvoid penta_to_dot() {
	if (start && penta[2][1] >= -0.5) {
		penta[0][0] += 0.06;
		penta[0][1] -= 0.06;
		penta[1][0] += 0.03;
		penta[1][1] += 0.03;
		penta[2][1] -= 0.08;
		penta[3][0] -= 0.03;
		penta[3][1] += 0.03;
		penta[4][0] -= 0.06;
		penta[4][1] -= 0.06;
	}

	int PosLocation = glGetAttribLocation(shaderProgramID, "in_Position"); //	: 0  Shader�� 'layout (location = 0)' �κ�
	int ColorLocation = glGetAttribLocation(shaderProgramID, "in_Color"); //	: 1


	{
		glBindBuffer(GL_ARRAY_BUFFER, penta_vbo[0]); // VBO Bind
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		// PosLocation			- Location ��ȣ
		// 3					- VerTex Size (x, y, z �Ӽ��� Vec3�̴� 3) 
		// GL_FLOAT, GL_FALSE	- �ڷ����� Normalize ����
		// sizeof(float) * 3	- VerTex ������ ���� ũ�� (�� �������� �޸� ����)
		//			(0�� ����)	- 0 �� ��� �ڵ����� 2��° ����(3) x 3��° ����(float)�� ����
		// 0					- ������ ���� offset (0�̸� ������ ó������ ����)
	}
	{
		glBindBuffer(GL_ARRAY_BUFFER, penta_vbo[1]); // VBO Bind
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	}
	if (penta[2][1] >= -0.5) {
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 5); // ������� ���
	}
	else {
		glPointSize(10);
		//for (int i = 0; i < 5; ++i) {
			//penta[i][0] = 0.5;
			//penta[i][1] = -0.5;
		//}
		glDrawArrays(GL_POINTS, 2, 1); // ������� ���
	}
}

GLvoid bline_to_tri() {
	if (start) {
		bline[2][0] += 0.02;
		bline[2][1] -= 0.08;
		bline[1][0] -= 0.02;
		Re_Buffer();
		if (bline[2][1] <= -0.6) {
			start = false;
		}
	}



	int PosLocation = glGetAttribLocation(shaderProgramID, "in_Position"); //	: 0  Shader�� 'layout (location = 0)' �κ�
	int ColorLocation = glGetAttribLocation(shaderProgramID, "in_Color"); //	: 1


	{
		glBindBuffer(GL_ARRAY_BUFFER, bline_vbo[0]); // VBO Bind
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		// PosLocation			- Location ��ȣ
		// 3					- VerTex Size (x, y, z �Ӽ��� Vec3�̴� 3) 
		// GL_FLOAT, GL_FALSE	- �ڷ����� Normalize ����
		// sizeof(float) * 3	- VerTex ������ ���� ũ�� (�� �������� �޸� ����)
		//			(0�� ����)	- 0 �� ��� �ڵ����� 2��° ����(3) x 3��° ����(float)�� ����
		// 0					- ������ ���� offset (0�̸� ������ ó������ ����)
	}
	{
		glBindBuffer(GL_ARRAY_BUFFER, bline_vbo[1]); // VBO Bind
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	}
	if (bline[1][0] == bline[2][0]) {
		glDrawArrays(GL_LINES, 0, 3); // ������� ���
	}
	else {
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 3); // ������� ���
	}
}


GLvoid btri_to_rec() {
	if (start) {
		btri[2][0] -= 0.05;
		btri[3][0] += 0.05;
		Re_Buffer();
		if (btri[2][0] <= -0.6) {
			start = false;
		}
	}

	int PosLocation = glGetAttribLocation(shaderProgramID, "in_Position"); //	: 0  Shader�� 'layout (location = 0)' �κ�
	int ColorLocation = glGetAttribLocation(shaderProgramID, "in_Color"); //	: 1


	{
		glBindBuffer(GL_ARRAY_BUFFER, btri_vbo[0]); // VBO Bind
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		// PosLocation			- Location ��ȣ
		// 3					- VerTex Size (x, y, z �Ӽ��� Vec3�̴� 3) 
		// GL_FLOAT, GL_FALSE	- �ڷ����� Normalize ����
		// sizeof(float) * 3	- VerTex ������ ���� ũ�� (�� �������� �޸� ����)
		//			(0�� ����)	- 0 �� ��� �ڵ����� 2��° ����(3) x 3��° ����(float)�� ����
		// 0					- ������ ���� offset (0�̸� ������ ó������ ����)
	}
	{
		glBindBuffer(GL_ARRAY_BUFFER, btri_vbo[1]); // VBO Bind
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	}
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // ������� ���
}

GLvoid brec_to_penta() {
	if (start) {
		brec[0][0] -= 0.02;
		brec[0][1] -= 0.02;
		brec[1][0] += 0.01;
		brec[2][1] += 0.02;
		brec[3][0] -= 0.01;
		brec[4][0] += 0.02;
		brec[4][1] -= 0.02;
		Re_Buffer();
		if (brec[2][1] >= 0.8) {
			start = false;
		}
	}

	int PosLocation = glGetAttribLocation(shaderProgramID, "in_Position"); //	: 0  Shader�� 'layout (location = 0)' �κ�
	int ColorLocation = glGetAttribLocation(shaderProgramID, "in_Color"); //	: 1


	{
		glBindBuffer(GL_ARRAY_BUFFER, brec_vbo[0]); // VBO Bind
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		// PosLocation			- Location ��ȣ
		// 3					- VerTex Size (x, y, z �Ӽ��� Vec3�̴� 3) 
		// GL_FLOAT, GL_FALSE	- �ڷ����� Normalize ����
		// sizeof(float) * 3	- VerTex ������ ���� ũ�� (�� �������� �޸� ����)
		//			(0�� ����)	- 0 �� ��� �ڵ����� 2��° ����(3) x 3��° ����(float)�� ����
		// 0					- ������ ���� offset (0�̸� ������ ó������ ����)
	}
	{
		glBindBuffer(GL_ARRAY_BUFFER, brec_vbo[1]); // VBO Bind
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	}
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 5); // ������� ���
}

GLvoid bpenta_to_dot() {
	if (start) {
		bpenta[0][0] += 0.06;
		bpenta[0][1] -= 0.06;
		bpenta[1][0] += 0.03;
		bpenta[1][1] += 0.03;
		bpenta[2][1] -= 0.08;
		bpenta[3][0] -= 0.03;
		bpenta[3][1] += 0.03;
		bpenta[4][0] -= 0.06;
		bpenta[4][1] -= 0.06;
		Re_Buffer();
		if (bpenta[2][1] <= 0) {
			start = false;
		}
	}

	int PosLocation = glGetAttribLocation(shaderProgramID, "in_Position"); //	: 0  Shader�� 'layout (location = 0)' �κ�
	int ColorLocation = glGetAttribLocation(shaderProgramID, "in_Color"); //	: 1


	{
		glBindBuffer(GL_ARRAY_BUFFER, bpenta_vbo[0]); // VBO Bind
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		// PosLocation			- Location ��ȣ
		// 3					- VerTex Size (x, y, z �Ӽ��� Vec3�̴� 3) 
		// GL_FLOAT, GL_FALSE	- �ڷ����� Normalize ����
		// sizeof(float) * 3	- VerTex ������ ���� ũ�� (�� �������� �޸� ����)
		//			(0�� ����)	- 0 �� ��� �ڵ����� 2��° ����(3) x 3��° ����(float)�� ����
		// 0					- ������ ���� offset (0�̸� ������ ó������ ����)
	}
	{
		glBindBuffer(GL_ARRAY_BUFFER, bpenta_vbo[1]); // VBO Bind
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	}
	if (bpenta[2][1] >= 0) {
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 5); // ������� ���
	}
	else {
		glPointSize(10);
		//for (int i = 0; i < 5; ++i) {
			//penta[i][0] = 0.5;
			//penta[i][1] = -0.5;
		//}
		glDrawArrays(GL_POINTS, 2, 1); // ������� ���
	}
}

GLvoid copy_arr() {
	GLfloat line_[3][3]{ {-0.9, 0.3, 0}, {-0.4, 0.8, 0}, {-0.4, 0.8, 0} };
	GLfloat tri_[4][3]{ {0.2, 0.2, 0}, {0.8, 0.2, 0},  {0.5, 0.8, 0}, {0.5, 0.8, 0} };
	GLfloat rec_[5][3]{ {-0.8, -0.2, 0}, {-0.8, -0.8, 0}, {-0.5, -0.2, 0}, {-0.2, -0.8, 0}, {-0.2, -0.2, 0} };
	GLfloat penta_[5][3]{ {0.1, -0.3, 0}, {0.25, -0.8, 0}, {0.5, -0.1, 0}, {0.75, -0.8, 0}, {0.9, -0.3, 0} };

	GLfloat bline_[3][3]{ {-0.4, -0.6, 0}, {0.4, 0.6, 0}, {0.4, 0.6, 0} };

	GLfloat btri_[4][3]{ {-0.6, -0.6, 0}, {0.6, -0.6, 0},  {0.0, 0.8, 0}, {0.0, 0.8, 0} };

	GLfloat brec_[5][3]{ {-0.6, 0.6, 0}, {-0.6, -0.6, 0}, {0.0, 0.6, 0}, {0.6, -0.6, 0}, {0.6, 0.6, 0} };

	GLfloat bpenta_[5][3]{ {-0.6, 0.4, 0}, {-0.4, -0.4, 0}, {0.0, 0.7, 0}, {0.4, -0.4, 0}, {0.6, 0.4, 0} };

	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			line[i][j] = line_[i][j];
			bline[i][j] = bline_[i][j];
		}
	}

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 3; ++j) {
			tri[i][j] = tri_[i][j];
			btri[i][j] = btri_[i][j];
		}
	}

	for (int i = 0; i < 5; ++i) {
		for (int j = 0; j < 3; ++j) {
			rec[i][j] = rec_[i][j];
			brec[i][j] = brec_[i][j];
		}
	}

	for (int i = 0; i < 5; ++i) {
		for (int j = 0; j < 3; ++j) {
			penta[i][j] = penta_[i][j];
			bpenta[i][j] = bpenta_[i][j];
		}
	}
}