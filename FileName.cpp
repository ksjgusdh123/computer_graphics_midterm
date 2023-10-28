#define _CRT_SECURE_NO_WARNINGS //--- 프로그램 맨 앞에 선언할 것
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <gl/glew.h> // 필요한 헤더파일 include
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
// 랜덤 엔진
std::random_device rd;
std::mt19937 dre(rd());
std::uniform_real_distribution<float> urd{ 0.4, 1 };
std::uniform_real_distribution<float> urd_color{ 0.1, 1 };
std::uniform_int_distribution<int> uid{ 3, 6 };


//--- 필요한 변수 선언
GLint width, height;
GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader; //--- 버텍스 세이더 객체
GLuint fragmentShader; //--- 프래그먼트 세이더 객체
GLuint vao;

class PLANE {
	GLfloat p[6][3];
	GLfloat color[3];
	GLuint vbo[2];
	glm::mat4 TR;

	float x_move;
	float y_move;
	float x_pos;

	int dir;
	int state;

public:
	GLvoid re_init() {
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
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 6); // 설정대로 출력
			break;
		case 4:
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 6); // 설정대로 출력
			break;
		case 5:
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 6); // 설정대로 출력
			break;
		case 6:
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 6); // 설정대로 출력
			break;
		}
	}

	GLvoid draw() {
		int PosLocation = glGetAttribLocation(shaderProgramID, "positionAttribute"); //	: 0  Shader의 'layout (location = 0)' 부분
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]); // VBO Bind
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(PosLocation);

		Transform();
		unsigned int colorLocation = glGetUniformLocation(shaderProgramID, "colorAttribute");
		glUniform3fv(colorLocation, 1, color); // 예시 색상
		pick_draw();
	}

	GLvoid Transform() {
		unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "transform"); //--- 버텍스 세이더에서모델 변환 위치 가져오기
		TR = glm::translate(TR, glm::vec3(x_move, y_move, 0.0));
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR)); //--- modelTransform 변수에 변환 값 적용하기
	}

	GLvoid update() {
		if (x_pos >= 10) {
			y_move -= 0.01;
			if (dir > 0) {
				x_move -= 0.0001;
			}
			else {
				x_move += 0.0001;
			}
		}
		else {
			y_move -= 0.01;
			if (dir > 0) {
				x_move -= 0.01;
			}
			else {
				x_move += 0.01;
			}
			x_pos += 1;
		}
	}
};


//--- 메인 함수
void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(700, 700);
	glutCreateWindow("Example1");
	//--- GLEW 초기화하기
	glewInit();
	glewExperimental = GL_TRUE;
	make_shaderProgram();
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

GLvoid Timer_event(int value) {
	p.re_init();
	manage.push_back(p);
	glutPostRedisplay(); //--- 배경색이 바뀔 때마다 출력 콜백 함수를 호출하여 화면을 refresh 한다
	glutTimerFunc(100, Timer_event, 4);
}


GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	glClearColor(rColor, gColor, bColor, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramID);
	glBindVertexArray(vao);
	//--- 삼각형 그리기

	for (int i = 0; i < manage.size(); ++i) {
		manage.at(i).draw();
		manage.at(i).update();
	}
	glutSwapBuffers(); //--- 화면에 출력하기
}

GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{
	glViewport(0, 0, w, h);
}

GLvoid Mouse_Click(int button, int state, int x, int y) {

}

GLvoid Mouse_Move(int x, int y)
{
	ConvertXY_OPENGL(x, y);

	glutPostRedisplay(); //--- 배경색이 바뀔 때마다 출력 콜백 함수를 호출하여 화면을 refresh 한다



}






GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	}
	glutPostRedisplay(); //--- 배경색이 바뀔 때마다 출력 콜백 함수를 호출하여 화면을 refresh 한다
}



GLvoid ConvertXY_OPENGL(int x, int y)
{
	int w = 800;
	int h = 600;

	ox = (float)(x - (float)w / 2.0) * (float)(1.0 / (float)(w / 2.0));
	oy = -(float)(y - (float)h / 2.0) * (float)(1.0 / (float)(h / 2.0));

}



void make_vertexShaders()
{
	GLchar* vertexSource;
	vertexSource = filetobuf("vertex.glsl");
	//--- 버텍스 세이더 객체 만들기
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//--- 세이더 코드를 세이더 객체에 넣기
	glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);
	//--- 버텍스 세이더 컴파일하기
	glCompileShader(vertexShader);
	//--- 컴파일이 제대로 되지 않은 경우: 에러 체크
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}

void make_fragmentShaders()
{
	GLchar* fragmentSource;
	fragmentSource = filetobuf("fragment.glsl");
	//--- 프래그먼트 세이더 객체 만들기
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	//--- 세이더 코드를 세이더 객체에 넣기
	glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);
	//--- 프래그먼트 세이더 컴파일
	glCompileShader(fragmentShader);
	//--- 컴파일이 제대로 되지 않은 경우: 컴파일 에러 체크
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: fragment shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}

GLvoid make_shaderProgram()
{
	GLuint shaderID;
	make_vertexShaders(); //--- 버텍스 세이더 만들기
	make_fragmentShaders(); //--- 프래그먼트 세이더 만들기
	//-- shader Program
	shaderProgramID = glCreateProgram();
	glAttachShader(shaderProgramID, vertexShader);
	glAttachShader(shaderProgramID, fragmentShader);
	glLinkProgram(shaderProgramID);
	//--- 세이더 삭제하기
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	//--- Shader Program 사용하기
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