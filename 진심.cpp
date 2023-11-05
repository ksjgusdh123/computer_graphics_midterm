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
std::uniform_int_distribution<int> uid{ 3, 4 };


//--- �ʿ��� ���� ����
GLint width, height;
GLuint shaderProgramID; //--- ���̴� ���α׷� �̸�
GLuint vertexShader; //--- ���ؽ� ���̴� ��ü
GLuint fragmentShader; //--- �����׸�Ʈ ���̴� ��ü
GLuint vao, linevbo[2];

bool Isline = false;
GLfloat line[2][3]{ };
GLfloat linecolor[3]{ };


class PLANE {
	GLfloat p[6][3];
	GLfloat color[3];
	GLuint vbo[2];
	glm::mat4 TR;

	float x_move;
	float y_move;
	float x_pos;
	float y_pos;

	float slice_start[2];
	float slice_end[2];
	float slice_sx;
	float slice_ex;
	float slice_sy;
	float slice_ey;
	int slice_num;
	int slice_line;
	int slice_area[4];
	int dir;
	int state;
	int slice_except;

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
		slice_num = 0;
		delete_plane = false;
		TR = glm::mat4(1.0f);
		for (int i = 0; i < 3; ++i) {
			color[i] = urd_color(dre);
		}
		state = 4;// uid(dre);
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
			p[1][0] = p[0][0] + 0.2;
			p[1][1] = p[0][1];
			p[2][0] = p[0][0];
			p[2][1] = p[0][1] + 0.2;
			p[3][0] = p[0][0] + 0.2;
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

	GLvoid re_initBuffer() {
		glBindVertexArray(vao);

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
		if (Isline)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		switch (state) {
		case 3:
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 3); // ������� ���
			break;
		case 4:
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // ������� ���
			break;
		case 5:
			glDrawArrays(GL_TRIANGLE_FAN, 0, 5); // ������� ���
			break;
		case 6:
			glDrawArrays(GL_TRIANGLE_FAN, 0, 6); // ������� ���
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
		show();
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
			y_move -= 0.005;
			y_pos -= 0.005;
			if (x_pos >= 1) {
				if (dir > 0) {
					x_move -= 0.01;
				}
				else {
					x_move += 0.01;
				}
			}
			else {
				if (dir > 0) {
					x_move -= 0.01;
				}
				else {
					x_move += 0.01;
				}
				x_pos += 0.01;
			}

			if (y_pos < -1) {
				delete_plane = true;
			}
		}
		draw();
	}

	bool get_delete() { return delete_plane; }

	bool crash_check() {
		std::vector<float> x;
		std::vector<float> y;
		switch (state) {
		case 3:
			for (int i = 0; i < 3; ++i) {
				for (float t = 0; t <= 1; t += 0.005) {
					float x1 = (1 - t) * (p[i][0] + x_move) + t * (p[(i + 1) % 3][0] + x_move);
					float y1 = (1 - t) * (p[i][1] + y_move) + t * (p[(i + 1) % 3][1] + y_move);
					float max = std::max(line[0][0], line[1][0]);
					float min = std::min(line[0][0], line[1][0]);
					if (x1 >= min && x1 <= max) {
						float ly = (line[1][1] - line[0][1]) / (line[1][0] - line[0][0]) * (x1 - line[0][0]) + line[0][1];
						if (ly >= y1 - 0.01 && ly <= y1 + 0.01) {
							slice_num++;
							if (slice_num == 1) {
								slice_area[0] = i;
								slice_sx = x1;
								slice_sy = y1;
							}
							else if (slice_num == 2) {
								slice_area[1] = i;
								slice_ex = x1;
								slice_ey = y1;
							}
							else if (slice_num == 3) {
								slice_area[2] = i;
								slice_ex = x1;
								slice_ey = y1;
							}
							break;
						}
					}
				}
			}
			if (slice_num == 2 || slice_num == 3) {
				return true;
			}
			slice_num = 0;
			return false;
			break;
		case 4:
			x.push_back(p[0][0] + x_move);
			x.push_back(p[1][0] + x_move);
			x.push_back(p[3][0] + x_move);
			x.push_back(p[2][0] + x_move);
			y.push_back(p[0][1] + y_move);
			y.push_back(p[1][1] + y_move);
			y.push_back(p[3][1] + y_move);
			y.push_back(p[2][1] + y_move);

			for (int i = 0; i < 4; ++i) {
				for (float t = 0; t <= 1; t += 0.005) {
					float x1 = (1 - t) * (x.at(i)) + t * (x.at((i + 1) % 4));
					float y1 = (1 - t) * (y.at(i)) + t * (y.at((i + 1) % 4));

					float max = std::max(line[0][0], line[1][0]);
					float min = std::min(line[0][0], line[1][0]);
					if (x1 >= min && x1 <= max) {
						float ly = (line[1][1] - line[0][1]) / (line[1][0] - line[0][0]) * (x1 - line[0][0]) + line[0][1];
						if (ly >= y1 - 0.01 && ly <= y1 + 0.01) {
							slice_num++;
							if (slice_num == 1) {
								slice_start[0] = x1;
								slice_start[1] = y1;
								slice_sx = x1;
								slice_sy = y1;
								slice_area[0] = i;
							}
							else if (slice_num == 3 || slice_num == 4 || slice_num == 2) {
								slice_end[0] = x1;
								slice_end[1] = y1;
							}
							break;
						}
					}
				}
			}
			if (slice_num == 2 || slice_num == 3 || slice_num == 4) {
				return true;
			}
			slice_num = 0;
			return false;
		case 5:
			/*	x.push_back(p[0][0] + x_move);
				x.push_back(p[1][0] + x_move);
				x.push_back(p[3][0] + x_move);
				x.push_back(p[4][0] + x_move);
				x.push_back(p[2][0] + x_move);
				y.push_back(p[0][1] + y_move);
				y.push_back(p[1][1] + y_move);
				y.push_back(p[3][1] + y_move);
				y.push_back(p[4][1] + y_move);
				y.push_back(p[2][1] + y_move);

				for (int i = 0; i < 4; ++i) {
					for (float t = 0; t <= 1; t += 0.005) {
						float x1 = (1 - t) * (x.at(i)) + t * (x.at((i + 1) % 4));
						float y1 = (1 - t) * (y.at(i)) + t * (y.at((i + 1) % 4));

						float max = std::max(line[0][0], line[1][0]);
						float min = std::min(line[0][0], line[1][0]);
						if (x1 >= min && x1 <= max) {
							float ly = (line[1][1] - line[0][1]) / (line[1][0] - line[0][0]) * (x1 - line[0][0]) + line[0][1];
							if (ly >= y1 - 0.01 && ly <= y1 + 0.01) {
								slice_num++;
								if (slice_num == 1) {
									slice_sx = x1;
									slice_sy = y1;
									slice_area[0] = i;
								}
								else if (slice_num == 2) {
									slice_ex = x1;
									slice_ey = y1;
									slice_area[1] = i;
									if (slice_area[0] == 0) {
										if (i == 1)
											slice_except = 1;
										if (i == 4)
											slice_except = 0;
									}
									else if (slice_area[0] == 1) {
										if(i == 2)
											slice_except = 3;
									}
								}
								break;
							}
						}
					}
				}
				if (slice_num == 2) {
					return true;
				}
				slice_num = 0;*/
			return false;
		case 6:
			return false;
		}

	}

	PLANE& seperate() {
		float p_sx = slice_sx - x_move;
		float p_sy = slice_sy - y_move;
		float p_ex = slice_ex - x_move;
		float p_ey = slice_ey - y_move;
		PLANE temp = *this;

		std::vector<float> x;
		std::vector<float> y;
		std::vector<float>EL_x;
		std::vector<float>EL_y;
		std::vector<float>EL_temp_x;
		std::vector<float>EL_temp_y;
		bool EL_input = true;

		switch (state) {
		case 3:
			if (slice_num == 2) {
				if (slice_area[0] == 0) {
					if (slice_area[1] == 1) {
						p[0][0] -= 0.05;
						p[1][0] = p_sx - 0.05; p[1][1] = p_sy;
						p[2][0] = temp.p[2][0] - 0.05; p[2][1] = temp.p[2][1];
						p[3][0] = p_ex - 0.05; p[3][1] = p_ey;
						for (int i = 4; i < 6; ++i) {
							p[i][0] = p[3][0];
							p[i][1] = p[3][1];
						}
						slice_num = 0;
						slice_line = 0;
						if (dir > 0)
							dir = -1;
						else
							dir = 1;
						state = 4;
						re_initBuffer();

						temp.p[0][0] = p_sx + 0.05; temp.p[0][1] = p_sy;
						temp.p[1][0] += 0.05;
						temp.p[2][0] = p_ex + 0.05; temp.p[2][1] = p_ey;
						for (int i = 3; i < 6; ++i) {
							temp.p[i][0] = temp.p[2][0];
							temp.p[i][1] = temp.p[2][1];
						}
						temp.slice_num = 0;
						temp.slice_line = 0;
						temp.state = 3;
						std::cout << "����" << std::endl;
						return temp;
					}
					else if (slice_area[1] == 2) {
						p[0][0] = p_sx - 0.05; p[0][1] = p_sy;
						p[1][0] -= 0.05;
						p[2][0] = p_ex - 0.05; p[2][1] = p_ey;
						p[3][0] = temp.p[2][0] - 0.05; p[3][1] = temp.p[2][1];
						for (int i = 4; i < 6; ++i) {
							p[i][0] = p[3][0];
							p[i][1] = p[3][1];
						}
						slice_num = 0;
						slice_line = 0;
						if (dir > 0)
							dir = -1;
						else
							dir = 1;
						state = 4;
						re_initBuffer();

						temp.p[0][0] += 0.05;
						temp.p[1][0] = p_sx + 0.05; temp.p[0][1] = p_sy;
						temp.p[2][0] = p_ex + 0.05; temp.p[2][1] = p_ey;
						for (int i = 3; i < 6; ++i) {
							temp.p[i][0] = temp.p[2][0];
							temp.p[i][1] = temp.p[2][1];
						}
						temp.slice_num = 0;
						temp.slice_line = 0;
						temp.state = 3;
						std::cout << "����" << std::endl;
						return temp;
					}
				}
				else if (slice_area[0] == 1) {
					if (slice_area[1] == 2) {
						p[0][0] -= 0.05;
						p[1][0] -= 0.05;
						p[2][0] = p_ex - 0.05; p[2][1] = p_ey;
						p[3][0] = p_sx - 0.05; p[3][1] = p_sy;
						for (int i = 4; i < 6; ++i) {
							p[i][0] = p[3][0];
							p[i][1] = p[3][1];
						}
						slice_num = 0;
						slice_line = 0;
						if (dir > 0)
							dir = -1;
						else
							dir = 1;
						state = 4;
						re_initBuffer();

						temp.p[0][0] = p_ex + 0.05; temp.p[0][1] = p_ey;
						temp.p[1][0] = p_sx + 0.05; temp.p[1][1] = p_sy;
						temp.p[2][0] += 0.05;
						for (int i = 3; i < 6; ++i) {
							temp.p[i][0] = temp.p[2][0];
							temp.p[i][1] = temp.p[2][1];
						}
						temp.slice_num = 0;
						temp.slice_line = 0;
						temp.state = 3;
						std::cout << "����" << std::endl;
						return temp;
					}
				}
			}
			else if (slice_num == 3) {

			}
			break;

		case 4:
			x.push_back(p[0][0] + x_move);
			x.push_back(p[1][0] + x_move);
			x.push_back(p[3][0] + x_move);
			x.push_back(p[2][0] + x_move);
			y.push_back(p[0][1] + y_move);
			y.push_back(p[1][1] + y_move);
			y.push_back(p[3][1] + y_move);
			y.push_back(p[2][1] + y_move);

			for (int i = 0; i < 4; ++i) {
				if (EL_input) {
					EL_x.push_back(x.at(i) - x_move);
					EL_y.push_back(y.at(i) - y_move);
				}
				else {
					EL_temp_x.push_back(x.at(i) - x_move);
					EL_temp_y.push_back(y.at(i) - y_move);
				}
				for (float t = 0; t <= 1; t += 0.005) {
					float x1 = (1 - t) * (x.at(i)) + t * (x.at((i + 1) % 4));
					float y1 = (1 - t) * (y.at(i)) + t * (y.at((i + 1) % 4));
					if (EL_input == true && slice_start[0] >= x1 - 0.01 && slice_start[0] <= x1 + 0.01 && slice_start[1] >= y1 - 0.01 && slice_start[1] <= y1 + 0.01) {
						EL_x.push_back(slice_start[0] - x_move);
						EL_y.push_back(slice_start[1] - y_move);
						EL_temp_x.push_back(slice_start[0] - x_move);
						EL_temp_y.push_back(slice_start[1] - y_move);
						EL_input = false;
						break;
					}
					else if (EL_input == false && slice_end[0] >= x1 - 0.01 && slice_end[0] <= x1 + 0.01 && slice_end[1] >= y1 - 0.01 && slice_end[1] <= y1 + 0.01) {
						EL_x.push_back(slice_end[0] - x_move);
						EL_y.push_back(slice_end[1] - y_move);
						EL_temp_x.push_back(slice_end[0] - x_move);
						EL_temp_y.push_back(slice_end[1] - y_move);
						EL_input = true;
						break;
					}
				}
			}

			for (int i = 0; i < EL_x.size(); ++i) {
				if (EL_x.size() == 4) {
					if (i == 2) {
						p[3][0] = EL_x.at(i) - 0.05;
						p[3][1] = EL_y.at(i);
					}
					else if (i == 3) {
						p[2][0] = EL_x.at(i) - 0.05;
						p[2][1] = EL_y.at(i);
					}
					else {
						p[i][0] = EL_x.at(i) - 0.05;
						p[i][1] = EL_y.at(i);
					}
				}
				else {
					p[i][0] = EL_x.at(i) - 0.05;
					p[i][1] = EL_y.at(i);
				}
			}
			slice_num = 0;
			state = EL_x.size();
			re_initBuffer();

			for (int i = 0; i < EL_temp_x.size(); ++i) {
				if (EL_temp_x.size() == 4) {
					if (i == 2) {
						temp.p[3][0] = EL_temp_x.at(i) + 0.05;
						temp.p[3][1] = EL_temp_y.at(i);
					}
					else if (i == 3) {
						temp.p[2][0] = EL_temp_x.at(i) + 0.05;
						temp.p[2][1] = EL_temp_y.at(i);
					}
					else {
						temp.p[i][0] = EL_temp_x.at(i) + 0.05;
						temp.p[i][1] = EL_temp_y.at(i);
					}
				}
				else {
					temp.p[i][0] = EL_temp_x.at(i) + 0.05;
					temp.p[i][1] = EL_temp_y.at(i);
				}
			}
			temp.slice_num = 0;
			temp.state = EL_temp_x.size();
			return temp;

			break;
		case 5:
			/*if (slice_num == 2) {
				for (int i = 0; i <= 5; ++i) {
					if (slice_except == 1 || slice_except == 3) {
						if (slice_except == i) {
							p[i][0] = p_sx - 0.05;
							p[i][1] = p_sy;
						}
						else if (slice_except + 2 == i) {
							p[i][0] = p_ex - 0.05;
							p[i][1] = p_ey;
						}
						else {
							if (i == 5) {
								p[5][0] = temp.p[3][0] - 0.05;
								p[5][1] = temp.p[3][1];
							}
							else {
								p[i][0] = temp.p[i][0] - 0.05;
								p[i][1] = temp.p[i][1];
							}
						}
					}
					else if (slice_except == 0) {
						if (i == 5) {
							p[i][0] = p_sx - 0.05;
							p[i][1] = p_sy;
						}
						else if (i == 4) {
							p[i][0] = p_ex - 0.05;
							p[i][1] = p_ey;
						}
						else {
							p[i][0] = temp.p[5 - 1 - i][0] - 0.05;
							p[i][1] = temp.p[5 - 1 - i][1];
						}
					}
				}

				slice_num = 0;
				slice_line = 0;

				if (dir > 0)
					dir = -1;
				else
					dir = 1;
				re_initBuffer();

				temp.p[0][0] = p_sx + 0.05; temp.p[0][1] = p_sy;
				temp.p[1][0] = temp.p[slice_except][0] + 0.05; temp.p[1][1] = temp.p[slice_except][1];
				temp.p[2][0] = p_ex + 0.05; temp.p[2][1] = p_ey;
				for (int i = 3; i < 6; ++i) {
					temp.p[i][0] = temp.p[2][0];
					temp.p[i][1] = temp.p[2][1];
				}
				temp.slice_num = 0;
				temp.slice_line = 0;
				temp.state = 3;
				std::cout << "����" << std::endl;
				return temp;
			}*/
			break;
		}
	}

	void show() {
		std::cout << "slice_num: " << slice_num << std::endl;
	}
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
PLANE basket{ 1 };
int draw_count = 0;



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
			if (manage.size() < 2) {
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
		line[1][0] = end_x;
		line[1][1] = end_y;
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		end_x = ox;
		end_y = oy;
		line[1][0] = end_x;
		line[1][1] = end_y;
		int num = manage.size();
		for (int i = 1; i < num; ++i) {
			if (manage.at(i).crash_check()) {
				p = manage.at(i).seperate();
				p.initBuffer();
				manage.push_back(p);
			}
		}
		click = false;
		line[1][0] = line[0][0];
		line[1][1] = line[0][1];

		start_x = end_x = 100;
		start_y = end_y = 100;
		re_init();

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
	case 'l':
	case 'L':
		Isline = true;
		break;
	case 'f':
	case 'F':
		Isline = false;
		break;

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