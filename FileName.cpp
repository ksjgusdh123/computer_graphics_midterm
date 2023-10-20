#define _CRT_SECURE_NO_WARNINGS //--- 프로그램 맨 앞에 선언할 것
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <gl/glew.h> // 필요한 헤더파일 include
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
//--- 필요한 변수 선언
GLint width, height;
GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader; //--- 버텍스 세이더 객체
GLuint fragmentShader; //--- 프래그먼트 세이더 객체

std::random_device rd;
std::mt19937 dre(rd());
std::uniform_real_distribution<float> uid{ 0.1, 0.4 };

//--- 메인 함수
void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Example1");
	//--- GLEW 초기화하기
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



//--- 다시그리기 콜백 함수
GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{
	glViewport(0, 0, w, h);
}

GLint result;
GLchar errorLog[512];

void make_vertexShaders()
{
	GLchar* vertexSource;
	vertexSource = filetobuf("예시vertex.glsl");
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
	fragmentSource = filetobuf("예시fragment.glsl");
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

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	GLfloat rColor = 1.0, gColor = 1.0, bColor = 1.0;
	//--- 변경된 배경색 설정
	glClearColor(rColor, gColor, bColor, 1.0f);
	//glClearColor(1.0, 1.0, 1.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//--- 렌더링 파이프라인에 세이더 불러오기
	glUseProgram(shaderProgramID);
	//--- 사용할 VAO 불러오기
	glBindVertexArray(vao);
	//--- 삼각형 그리기
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



	glutSwapBuffers(); //--- 화면에 출력하기
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
	glutPostRedisplay(); //--- 배경색이 바뀔 때마다 출력 콜백 함수를 호출하여 화면을 refresh 한다
}


GLvoid Timer_event(int value) {






	glutPostRedisplay(); //--- 배경색이 바뀔 때마다 출력 콜백 함수를 호출하여 화면을 refresh 한다
	glutTimerFunc(100, Timer_event, 4);
}


GLvoid InitBuffer()
{
	glGenVertexArrays(1, &vao); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(vao); //--- VAO를 바인드하기
	glGenBuffers(2, vbo); //--- 2개의 VBO를 지정하고 할당하기
	//--- 1번째 VBO를 활성화하여 바인드하고, 버텍스 속성 (좌표값)을 저장
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	//--- 변수 diamond 에서 버텍스 데이터 값을 버퍼에 복사한다.
	//--- triShape 배열의 사이즈: 9 * float
	glBufferData(GL_ARRAY_BUFFER, sizeof(line_background), line_background, GL_STATIC_DRAW);
	//--- 좌표값을 attribute 인덱스 0번에 명시한다: 버텍스 당 3* float
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	//--- attribute 인덱스 0번을 사용가능하게 함
	glEnableVertexAttribArray(0);
	//--- 2번째 VBO를 활성화 하여 바인드 하고, 버텍스 속성 (색상)을 저장
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	//--- 변수 colors에서 버텍스 색상을 복사한다.
	//--- colors 배열의 사이즈: 9 *float 
	glBufferData(GL_ARRAY_BUFFER, sizeof(line_background), back_colors, GL_STATIC_DRAW);
	//--- 색상값을 attribute 인덱스 1번에 명시한다: 버텍스 당 3*float 
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	//--- attribute 인덱스 1번을 사용 가능하게 함.
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
	//--- 변수 diamond 에서 버텍스 데이터 값을 버퍼에 복사한다.
	//--- triShape 배열의 사이즈: 9 * float
	glBufferData(GL_ARRAY_BUFFER, sizeof(line_background), line_background, GL_STATIC_DRAW);
	//--- 좌표값을 attribute 인덱스 0번에 명시한다: 버텍스 당 3* float
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	//--- attribute 인덱스 0번을 사용가능하게 함
	glEnableVertexAttribArray(0);
	//--- 2번째 VBO를 활성화 하여 바인드 하고, 버텍스 속성 (색상)을 저장
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	//--- 변수 colors에서 버텍스 색상을 복사한다.
	//--- colors 배열의 사이즈: 9 *float 
	glBufferData(GL_ARRAY_BUFFER, sizeof(line_background), back_colors, GL_STATIC_DRAW);
	//--- 색상값을 attribute 인덱스 1번에 명시한다: 버텍스 당 3*float 
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	//--- attribute 인덱스 1번을 사용 가능하게 함.
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
	int PosLocation = glGetAttribLocation(shaderProgramID, "in_Position"); //	: 0  Shader의 'layout (location = 0)' 부분
	int ColorLocation = glGetAttribLocation(shaderProgramID, "in_Color"); //	: 1


	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]); // VBO Bind
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		// PosLocation			- Location 번호
		// 3					- VerTex Size (x, y, z 속성의 Vec3이니 3) 
		// GL_FLOAT, GL_FALSE	- 자료형과 Normalize 여부
		// sizeof(float) * 3	- VerTex 마다의 공백 크기 (한 정점마다 메모리 간격)
		//			(0과 같음)	- 0 일 경우 자동으로 2번째 인자(3) x 3번째 인자(float)로 설정
		// 0					- 데이터 시작 offset (0이면 데이터 처음부터 시작)
	}
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]); // VBO Bind
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	}
	glDrawArrays(GL_LINES, 0, 4); // 설정대로 출력
}

GLvoid line_to_tri() {
	if (start) {
		line[2][0] += 0.04;
		line[2][1] -= 0.083;
		line[1][0] -= 0.025;
	}



	int PosLocation = glGetAttribLocation(shaderProgramID, "in_Position"); //	: 0  Shader의 'layout (location = 0)' 부분
	int ColorLocation = glGetAttribLocation(shaderProgramID, "in_Color"); //	: 1


	{
		glBindBuffer(GL_ARRAY_BUFFER, line_vbo[0]); // VBO Bind
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		// PosLocation			- Location 번호
		// 3					- VerTex Size (x, y, z 속성의 Vec3이니 3) 
		// GL_FLOAT, GL_FALSE	- 자료형과 Normalize 여부
		// sizeof(float) * 3	- VerTex 마다의 공백 크기 (한 정점마다 메모리 간격)
		//			(0과 같음)	- 0 일 경우 자동으로 2번째 인자(3) x 3번째 인자(float)로 설정
		// 0					- 데이터 시작 offset (0이면 데이터 처음부터 시작)
	}
	{
		glBindBuffer(GL_ARRAY_BUFFER, line_vbo[1]); // VBO Bind
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	}
	if (line[1][0] == line[2][0]) {
		glDrawArrays(GL_LINES, 0, 3); // 설정대로 출력
	}
	else {
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 3); // 설정대로 출력
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

	int PosLocation = glGetAttribLocation(shaderProgramID, "in_Position"); //	: 0  Shader의 'layout (location = 0)' 부분
	int ColorLocation = glGetAttribLocation(shaderProgramID, "in_Color"); //	: 1


	{
		glBindBuffer(GL_ARRAY_BUFFER, tri_vbo[0]); // VBO Bind
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		// PosLocation			- Location 번호
		// 3					- VerTex Size (x, y, z 속성의 Vec3이니 3) 
		// GL_FLOAT, GL_FALSE	- 자료형과 Normalize 여부
		// sizeof(float) * 3	- VerTex 마다의 공백 크기 (한 정점마다 메모리 간격)
		//			(0과 같음)	- 0 일 경우 자동으로 2번째 인자(3) x 3번째 인자(float)로 설정
		// 0					- 데이터 시작 offset (0이면 데이터 처음부터 시작)
	}
	{
		glBindBuffer(GL_ARRAY_BUFFER, tri_vbo[1]); // VBO Bind
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	}
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // 설정대로 출력
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

	int PosLocation = glGetAttribLocation(shaderProgramID, "in_Position"); //	: 0  Shader의 'layout (location = 0)' 부분
	int ColorLocation = glGetAttribLocation(shaderProgramID, "in_Color"); //	: 1


	{
		glBindBuffer(GL_ARRAY_BUFFER, rec_vbo[0]); // VBO Bind
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		// PosLocation			- Location 번호
		// 3					- VerTex Size (x, y, z 속성의 Vec3이니 3) 
		// GL_FLOAT, GL_FALSE	- 자료형과 Normalize 여부
		// sizeof(float) * 3	- VerTex 마다의 공백 크기 (한 정점마다 메모리 간격)
		//			(0과 같음)	- 0 일 경우 자동으로 2번째 인자(3) x 3번째 인자(float)로 설정
		// 0					- 데이터 시작 offset (0이면 데이터 처음부터 시작)
	}
	{
		glBindBuffer(GL_ARRAY_BUFFER, rec_vbo[1]); // VBO Bind
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	}
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 5); // 설정대로 출력
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

	int PosLocation = glGetAttribLocation(shaderProgramID, "in_Position"); //	: 0  Shader의 'layout (location = 0)' 부분
	int ColorLocation = glGetAttribLocation(shaderProgramID, "in_Color"); //	: 1


	{
		glBindBuffer(GL_ARRAY_BUFFER, penta_vbo[0]); // VBO Bind
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		// PosLocation			- Location 번호
		// 3					- VerTex Size (x, y, z 속성의 Vec3이니 3) 
		// GL_FLOAT, GL_FALSE	- 자료형과 Normalize 여부
		// sizeof(float) * 3	- VerTex 마다의 공백 크기 (한 정점마다 메모리 간격)
		//			(0과 같음)	- 0 일 경우 자동으로 2번째 인자(3) x 3번째 인자(float)로 설정
		// 0					- 데이터 시작 offset (0이면 데이터 처음부터 시작)
	}
	{
		glBindBuffer(GL_ARRAY_BUFFER, penta_vbo[1]); // VBO Bind
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	}
	if (penta[2][1] >= -0.5) {
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 5); // 설정대로 출력
	}
	else {
		glPointSize(10);
		//for (int i = 0; i < 5; ++i) {
			//penta[i][0] = 0.5;
			//penta[i][1] = -0.5;
		//}
		glDrawArrays(GL_POINTS, 2, 1); // 설정대로 출력
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



	int PosLocation = glGetAttribLocation(shaderProgramID, "in_Position"); //	: 0  Shader의 'layout (location = 0)' 부분
	int ColorLocation = glGetAttribLocation(shaderProgramID, "in_Color"); //	: 1


	{
		glBindBuffer(GL_ARRAY_BUFFER, bline_vbo[0]); // VBO Bind
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		// PosLocation			- Location 번호
		// 3					- VerTex Size (x, y, z 속성의 Vec3이니 3) 
		// GL_FLOAT, GL_FALSE	- 자료형과 Normalize 여부
		// sizeof(float) * 3	- VerTex 마다의 공백 크기 (한 정점마다 메모리 간격)
		//			(0과 같음)	- 0 일 경우 자동으로 2번째 인자(3) x 3번째 인자(float)로 설정
		// 0					- 데이터 시작 offset (0이면 데이터 처음부터 시작)
	}
	{
		glBindBuffer(GL_ARRAY_BUFFER, bline_vbo[1]); // VBO Bind
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	}
	if (bline[1][0] == bline[2][0]) {
		glDrawArrays(GL_LINES, 0, 3); // 설정대로 출력
	}
	else {
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 3); // 설정대로 출력
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

	int PosLocation = glGetAttribLocation(shaderProgramID, "in_Position"); //	: 0  Shader의 'layout (location = 0)' 부분
	int ColorLocation = glGetAttribLocation(shaderProgramID, "in_Color"); //	: 1


	{
		glBindBuffer(GL_ARRAY_BUFFER, btri_vbo[0]); // VBO Bind
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		// PosLocation			- Location 번호
		// 3					- VerTex Size (x, y, z 속성의 Vec3이니 3) 
		// GL_FLOAT, GL_FALSE	- 자료형과 Normalize 여부
		// sizeof(float) * 3	- VerTex 마다의 공백 크기 (한 정점마다 메모리 간격)
		//			(0과 같음)	- 0 일 경우 자동으로 2번째 인자(3) x 3번째 인자(float)로 설정
		// 0					- 데이터 시작 offset (0이면 데이터 처음부터 시작)
	}
	{
		glBindBuffer(GL_ARRAY_BUFFER, btri_vbo[1]); // VBO Bind
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	}
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // 설정대로 출력
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

	int PosLocation = glGetAttribLocation(shaderProgramID, "in_Position"); //	: 0  Shader의 'layout (location = 0)' 부분
	int ColorLocation = glGetAttribLocation(shaderProgramID, "in_Color"); //	: 1


	{
		glBindBuffer(GL_ARRAY_BUFFER, brec_vbo[0]); // VBO Bind
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		// PosLocation			- Location 번호
		// 3					- VerTex Size (x, y, z 속성의 Vec3이니 3) 
		// GL_FLOAT, GL_FALSE	- 자료형과 Normalize 여부
		// sizeof(float) * 3	- VerTex 마다의 공백 크기 (한 정점마다 메모리 간격)
		//			(0과 같음)	- 0 일 경우 자동으로 2번째 인자(3) x 3번째 인자(float)로 설정
		// 0					- 데이터 시작 offset (0이면 데이터 처음부터 시작)
	}
	{
		glBindBuffer(GL_ARRAY_BUFFER, brec_vbo[1]); // VBO Bind
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	}
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 5); // 설정대로 출력
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

	int PosLocation = glGetAttribLocation(shaderProgramID, "in_Position"); //	: 0  Shader의 'layout (location = 0)' 부분
	int ColorLocation = glGetAttribLocation(shaderProgramID, "in_Color"); //	: 1


	{
		glBindBuffer(GL_ARRAY_BUFFER, bpenta_vbo[0]); // VBO Bind
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		// PosLocation			- Location 번호
		// 3					- VerTex Size (x, y, z 속성의 Vec3이니 3) 
		// GL_FLOAT, GL_FALSE	- 자료형과 Normalize 여부
		// sizeof(float) * 3	- VerTex 마다의 공백 크기 (한 정점마다 메모리 간격)
		//			(0과 같음)	- 0 일 경우 자동으로 2번째 인자(3) x 3번째 인자(float)로 설정
		// 0					- 데이터 시작 offset (0이면 데이터 처음부터 시작)
	}
	{
		glBindBuffer(GL_ARRAY_BUFFER, bpenta_vbo[1]); // VBO Bind
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	}
	if (bpenta[2][1] >= 0) {
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 5); // 설정대로 출력
	}
	else {
		glPointSize(10);
		//for (int i = 0; i < 5; ++i) {
			//penta[i][0] = 0.5;
			//penta[i][1] = -0.5;
		//}
		glDrawArrays(GL_POINTS, 2, 1); // 설정대로 출력
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