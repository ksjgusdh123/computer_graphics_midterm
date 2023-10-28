#define _CRT_SECURE_NO_WARNINGS //--- ���α׷� �� �տ� ������ ��
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <gl/glew.h> // �ʿ��� ������� include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <random>
#include <vector>
#include <math.h>
#include <gl\glm\glm\glm.hpp>
#include <gl/glm/glm/ext.hpp>
#include <gl/glm/glm/gtc/matrix_transform.hpp>
char* filetobuf(const char* file);
void make_vertexShaders();
void make_fragmentShaders();
GLvoid make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLvoid Mouse_Click(int button, int state, int x, int y);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Mouse_Move(int x, int y);
GLvoid Timer_event(int value);
GLvoid ConvertXY_OPENGL(int x, int y);
GLvoid re_init();
GLvoid init();
// ���� ����
std::random_device rd;
std::mt19937 dre(rd());
std::uniform_real_distribution<float> urd{ 0.4, 1 };
std::uniform_real_distribution<float> urd_color{ 0.1, 1 };
std::uniform_int_distribution<int> uid{ 3, 6 };


//--- �ʿ��� ���� ����
GLint width, height;
GLuint shaderProgramID; //--- ���̴� ���α׷� �̸�
GLuint vertexShader; //--- ���ؽ� ���̴� ��ü
GLuint fragmentShader; //--- �����׸�Ʈ ���̴� ��ü
GLuint vao, linevbo[2];

class PLANE {
	GLfloat p[6][3];
	GLfloat color[3];
	GLuint vbo[2];
	glm::mat4 TR;

	float x_move;
	float y_move;
	float x_pos;
	float y_pos;

	int dir;
	int state;

	bool delete_plane;
	bool basket;
public:
	PLANE() {}
	PLANE(int i) {
		p[0][0] = -0.3;
		p[0][1] = -0.8;
		p[1][0] = 0.3;
		p[1][1] = -0.8;
		p[2][0] = -0.3;
		p[2][1] = -0.7;
		p[3][0] = 0.3;
		p[3][1] = -0.7;
		for (int i = 4; i < 6; ++i) {
			for (int j = 0; j < 3; ++j) {
				p[i][j] = p[3][j];
			}
		}
		color[0] = 1;
		color[1] = 0;
		color[2] = 0;
		TR = glm::mat4(1.0f);
		basket = true;
		state = 4;
		dir = 1;
	}
	GLvoid re_init() {
		delete_plane = false;
		TR = glm::mat4(1.0f);
		for (int i = 0; i < 3; ++i) {
			color[i] = urd_color(dre);
		}
		state = uid(dre);
		p[0][1] = urd(dre);
		int n = uid(dre);
		const float START = 1.2;
		if (n % 2 == 0) {
			p[0][0] = START;
			dir = 1;
		}
		else {
			p[0][0] = -START;
			dir = -1;
		}
		switch (state) {
		case 3:
			p[1][0] = p[0][0] + 0.2;
			p[1][1] = p[0][1];
			p[2][0] = p[0][0] + 0.2;
			p[2][1] = p[0][1] + 0.2;
			y_pos = p[2][1];
			for (int i = 3; i < 6; ++i) {
				for (int j = 0; j < 3; ++j) {
					p[i][j] = p[2][j];
				}
			}
			break;
		case 4:
			p[1][0] = p[0][0] - 0.2;
			p[1][1] = p[0][1];
			p[2][0] = p[0][0];
			p[2][1] = p[0][1] + 0.2;
			p[3][0] = p[0][0] - 0.2;
			p[3][1] = p[0][1] + 0.2;
			y_pos = p[2][1];
			for (int i = 4; i < 6; ++i) {
				for (int j = 0; j < 3; ++j) {
					p[i][j] = p[3][j];
				}
			}
			break;
		case 5:
			p[1][0] = p[0][0] + 0.07;
			p[1][1] = p[0][1] - 0.1;
			p[2][0] = p[0][0] + 0.15;
			p[2][1] = p[0][1] + 0.07;
			p[3][0] = p[0][0] + 0.23;
			p[3][1] = p[0][1] - 0.1;
			p[4][0] = p[0][0] + 0.3;
			p[4][1] = p[0][1];
			y_pos = p[2][1];
			for (int i = 5; i < 6; ++i) {
				for (int j = 0; j < 3; ++j) {
					p[i][j] = p[4][j];
				}
			}
			break;
		case 6:
			p[1][0] = p[0][0] + 0.05;
			p[1][1] = p[0][1] + 0.1;
			p[2][0] = p[0][0] + 0.05;
			p[2][1] = p[0][1] - 0.1;
			p[3][0] = p[0][0] + 0.18;
			p[3][1] = p[0][1] + 0.1;
			p[4][0] = p[0][0] + 0.18;
			p[4][1] = p[0][1] - 0.1;
			p[5][0] = p[0][0] + 0.23;
			p[5][1] = p[0][1];
			y_pos = p[1][1];
			break;
		}
		initBuffer();
	}

	GLvoid initBuffer() {
		glBindVertexArray(vao);

		glGenBuffers(2, vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(p), p, GL_STATIC_DRAW);

		GLint lineAttribute = glGetAttribLocation(shaderProgramID, "positionAttribute");
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glVertexAttribPointer(lineAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(lineAttribute);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);

		GLint line_Attribute = glGetAttribLocation(shaderProgramID, "colorAttribute");
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glVertexAttribPointer(line_Attribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(line_Attribute);
	}

	GLvoid pick_draw() {
		switch (state) {
		case 3:
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 6); // ������� ���
			break;
		case 4:
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 6); // ������� ���
			break;
		case 5:
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 6); // ������� ���
			break;
		case 6:
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 6); // ������� ���
			break;
		}
	}

	GLvoid draw() {
		int PosLocation = glGetAttribLocation(shaderProgramID, "positionAttribute"); //	: 0  Shader�� 'layout (location = 0)' �κ�
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]); // VBO Bind
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(PosLocation);

		Transform();
		unsigned int colorLocation = glGetUniformLocation(shaderProgramID, "colorAttribute");
		glUniform3fv(colorLocation, 1, color); // ���� ����
		pick_draw();
	}

	GLvoid Transform() {
		unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "transform"); //--- ���ؽ� ���̴������� ��ȯ ��ġ ��������
		TR = glm::mat4(1.0f);
		TR = glm::translate(TR, glm::vec3(x_move, y_move, 0.0));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR)); //--- modelTransform ������ ��ȯ �� �����ϱ�
	}

	GLvoid update() {
		if (basket) {
			if (dir >= 1 && p[1][0] + x_move >= 0.9) {
				dir = -1;
			}
			else if (dir <= -1 && p[0][0] + x_move <= -0.9) {
				dir = 1;
			}

			if (dir >= 1) {
				x_move += 0.05;
			}
			else {
				x_move -= 0.05;
			}
		}
		else {
			y_move -= 0.1;
			y_pos -= 0.1;
			if (x_pos >= 1) {
				if (dir > 0) {
					x_move -= 0.05;
				}
				else {
					x_move += 0.05;
				}
			}
			else {
				if (dir > 0) {
					x_move -= 0.1;
				}
				else {
					x_move += 0.1;
				}
				x_pos += 0.1;
			}

			if (y_pos < -1) {
				delete_plane = true;
			}
		}
		draw();
	}

	bool get_delete() {return delete_plane;}
};


//--- ���� �Լ�
void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ����
{
	//--- ������ �����ϱ�
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(700, 700);
	glutCreateWindow("Example1");
	//--- GLEW �ʱ�ȭ�ϱ�
	glewInit();
	glewExperimental = GL_TRUE;
	make_shaderProgram();
	init();
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glutDisplayFunc(drawScene);
	glutMouseFunc(Mouse_Click);
	glutTimerFunc(100, Timer_event, 1);
	glutKeyboardFunc(Keyboard);
	glutMotionFunc(Mouse_Move);
	glutReshapeFunc(Reshape);
	glutMainLoop();
}


GLfloat rColor = 0.50, gColor = 0.50, bColor = 1.0;
float ox, oy;
std::vector<PLANE> manage;
PLANE p{};
PLANE basket{1};
int draw_count = 0;

GLfloat line[2][3]{ };
GLfloat linecolor[3]{ };

float start_x;
float start_y;
float end_x;
float end_y;
bool click = false;

GLvoid Timer_event(int value) {
	if (manage.size() == 0) {
		manage.push_back(basket);
		manage[0].initBuffer();
	}
	else {
		if (draw_count >= 10) {
			draw_count = 0;
			if (manage.size() < 10) {
				p.re_init();
				manage.push_back(p);
				std::cout << manage.size() - 1 << std::endl;
			}
		}
	}
	draw_count++;
	glutPostRedisplay(); //--- ������ �ٲ� ������ ��� �ݹ� �Լ��� ȣ���Ͽ� ȭ���� refresh �Ѵ�
	glutTimerFunc(100, Timer_event, 4);
}


GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
{
	glClearColor(rColor, gColor, bColor, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramID);
	glBindVertexArray(vao);
	//--- �ﰢ�� �׸���
	for (int i = 0; i < manage.size(); ++i) {
		manage.at(i).update();
		if (manage.at(i).get_delete()) {
			manage.erase(manage.begin() + i);
			std::cout << "����" << std::endl;
			i--;
		}
	}

	if (click) {
		int PosLocation = glGetAttribLocation(shaderProgramID, "positionAttribute"); //	: 0  Shader�� 'layout (location = 0)' �κ�
		glBindBuffer(GL_ARRAY_BUFFER, linevbo[0]); // VBO Bind
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(PosLocation);

		unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "transform"); //--- ���ؽ� ���̴������� ��ȯ ��ġ ��������
		glm::mat4 TR = glm::mat4(1.0f);
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR));

		unsigned int colorLocation = glGetUniformLocation(shaderProgramID, "colorAttribute");
		glUniform3fv(colorLocation, 1, linecolor); // ���� ����
		glDrawArrays(GL_LINES, 0, 2); // ������� ���
	}

	glutSwapBuffers(); //--- ȭ�鿡 ����ϱ�
}

GLvoid Mouse_Click(int button, int state, int x, int y) {
	ConvertXY_OPENGL(x, y);
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		start_x = ox;
		start_y = oy;
		end_x = ox;
		end_y = oy;
		click = true;
		line[0][0] = start_x;
		line[0][1] = start_y;
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		click = false;
		start_x = end_x;
		start_y = end_y;
	}
}

GLvoid Mouse_Move(int x, int y)
{
	if (click) {
		ConvertXY_OPENGL(x, y);
		end_x = ox;
		end_y = oy;
		line[1][0] = end_x;
		line[1][1] = end_y;
		re_init();
	}
}


GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ�
{
	glViewport(0, 0, w, h);
}



GLvoid init() {
	glBindVertexArray(vao);

	glGenBuffers(2, linevbo);
	glBindBuffer(GL_ARRAY_BUFFER, linevbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);

	GLint lineAttribute = glGetAttribLocation(shaderProgramID, "positionAttribute");
	glBindBuffer(GL_ARRAY_BUFFER, linevbo[0]);
	glVertexAttribPointer(lineAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(lineAttribute);

	glBindBuffer(GL_ARRAY_BUFFER, linevbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(linecolor), linecolor, GL_STATIC_DRAW);

	GLint line_Attribute = glGetAttribLocation(shaderProgramID, "colorAttribute");
	glBindBuffer(GL_ARRAY_BUFFER, linevbo[1]);
	glVertexAttribPointer(line_Attribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(line_Attribute);

}

GLvoid re_init() {
	glBindBuffer(GL_ARRAY_BUFFER, linevbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);

	GLint lineAttribute = glGetAttribLocation(shaderProgramID, "positionAttribute");
	glBindBuffer(GL_ARRAY_BUFFER, linevbo[0]);
	glVertexAttribPointer(lineAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(lineAttribute);

	glBindBuffer(GL_ARRAY_BUFFER, linevbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(linecolor), linecolor, GL_STATIC_DRAW);

	GLint line_Attribute = glGetAttribLocation(shaderProgramID, "colorAttribute");
	glBindBuffer(GL_ARRAY_BUFFER, linevbo[1]);
	glVertexAttribPointer(line_Attribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(line_Attribute);
}



GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	}
	glutPostRedisplay(); //--- ������ �ٲ� ������ ��� �ݹ� �Լ��� ȣ���Ͽ� ȭ���� refresh �Ѵ�
}



GLvoid ConvertXY_OPENGL(int x, int y)
{
	int w = 700;
	int h = 700;

	ox = (float)(x - (float)w / 2.0) * (float)(1.0 / (float)(w / 2.0));
	oy = -(float)(y - (float)h / 2.0) * (float)(1.0 / (float)(h / 2.0));

}



void make_vertexShaders()
{
	GLchar* vertexSource;
	vertexSource = filetobuf("vertex.glsl");
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
	fragmentSource = filetobuf("fragment.glsl");
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