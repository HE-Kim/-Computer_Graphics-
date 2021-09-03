


#include "GL/glew.h"
#include "GL/freeglut.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "CarGameLSystemApp.h"
#include "../BaseCodes/Camera.h"
#include "../BaseCodes/GroundObj2.h"
#include "../BaseCodes/InitShader.h"
#include "../BaseCodes/BasicShapeObjs.h"
#include "CarModel.h"
#include "TreeModelL.h"


#include <iostream>

// Window and User Interface
static bool g_left_button_pushed;
static bool g_right_button_pushed;
static int g_last_mouse_x;
static int g_last_mouse_y;

extern GLuint g_window_w;
extern GLuint g_window_h;

//////////////////////////////////////////////////////////////////////
// Camera 
//////////////////////////////////////////////////////////////////////
static Camera g_camera;
static int g_camera_mode = 0;
glm::vec3 g_camera_pos = { 0.f, 0.f, 0.f };
glm::vec3 g_camera_pos_f = { 0.f, 0.f, 0.f };

extern bool night_mode = false;


//////////////////////////////////////////////////////////////////////
//// Define Shader Programs
//////////////////////////////////////////////////////////////////////
GLuint s_program_id;



//////////////////////////////////////////////////////////////////////
//// Animation Parameters
//////////////////////////////////////////////////////////////////////
float g_elaped_time_s = 0.f;	// 
float g_time_s = 0.f;
void Timer(int value);

//
static const float FPS = 60.f;

//////////////////////////////////////////////////////////////////////
//// Car Position, Rotation, Velocity_
//// 자동차 제어 변수들.
//////////////////////////////////////////////////////////////////////
glm::vec3 g_car_poisition(0.f, 0.f, 0.f); //위치
float g_car_speed = 0;			          // 속도 (초당 이동 거리)
float g_car_rotation_y = 0;		          // 현재 방향 (y축 회전)
float g_car_angular_speed = 0;	          // 회전 속도 (각속도 - 초당 회전 각)



//////////////////////////////////////////////////////////////////////////
float ck_z = 0.f;
float ck_y = 0.f;
float ck_time = 0.f;


glm::mat4 view_matrix;
/**
InitOpenGL: 프로그램 초기 값 설정을 위해 최초 한 번 호출되는 함수. (main 함수 참고)
OpenGL에 관련한 초기 값과 프로그램에 필요한 다른 초기 값을 설정한다.
예를들어, VAO와 VBO를 여기서 생성할 수 있다.
*/
void InitOpenGL()
{
	s_program_id = CreateFromFiles("../Shaders/v_shader.glsl", "../Shaders/f_shader.glsl");
	glUseProgram(s_program_id);


	glViewport(0, 0, (GLsizei)g_window_w, (GLsizei)g_window_h);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);


	// Initial State of Camera
	// 카메라 초기 위치 설정한다.
	g_camera.lookAt(glm::vec3(3.f, 2.f, 3.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));

	// basic meshes
	InitBasicShapeObjs();

	// Tree
	InitTreeModel();

	// Car
	InitCarModel();

	// 바닥 격자 VAO 생성
	InitGround2();

	// Timer 함수 호출. // 추가
	g_elaped_time_s = 0.f;
	glutTimerFunc((unsigned int)(1000.f / FPS), Timer, (int)(1000.f / FPS));
}







/**
ClearOpenGLResource: 프로그램이 끝나기 메모리 해제를 위해 한 번 호출되는 함수. (main 함수 참고)
프로그램에서 사용한 메모리를 여기에서 해제할 수 있다.
예를들어, VAO와 VBO를 여기서 지울 수 있다.
*/
void ClearOpenGLResource()
{
	// Delete (VAO, VBO)
	DeleteBasicShapeObjs();
	DeleteTreeModel();
	DeleteCarModel();
	DeleteGround2();
}



/**
Timer: 지정된 시간 후에 자동으로 호출되는 callback 함수.
ref: https://www.opengl.org/resources/libraries/glut/spec3/node64.html#SECTION000819000000000000000
*/
void Timer(int value)
{
	// Timer 호출 시간 간격을 누적하여, 최초 Timer가 호출된 후부터 현재까지 흘러간 계산한다.
	g_elaped_time_s += value / 1000.f;


	// Turn
	g_car_rotation_y += g_car_angular_speed;

	// Calculate Velocity // 카메라가 자동차를 따라댕겨야해! !! !  
	glm::vec3 speed_v = glm::vec3(0.f, 0.f, g_car_speed);
	glm::vec3 velocity = glm::rotateY(speed_v, g_car_rotation_y);	// speed_v 를 y축을 기준으로 g_car_rotation_y 만큼 회전한다.

																	// Move
	g_car_poisition += velocity;


	// glutPostRedisplay는 가능한 빠른 시간 안에 전체 그림을 다시 그릴 것을 시스템에 요청한다.
	// 결과적으로 Display() 함수가 호출 된다.
	glutPostRedisplay();

	// 1/60 초 후에 Timer 함수가 다시 호출되로록 한다.
	// Timer 함수 가 동일한 시간 간격으로 반복 호출되게하여,
	// 애니메이션 효과를 표현할 수 있다
	glutTimerFunc((unsigned int)(1000 / 60), Timer, (1000 / FPS)); //FPS=60! // 추가
}



/**
Display: 윈도우 화면이 업데이트 될 필요가 있을 때 호출되는 callback 함수.

윈도우 상에 최종 결과를 렌더링 하는 코드는 이 함수 내에 구현해야한다.
원도우가 처음 열릴 때, 윈도우 크기가 바뀔 때, 다른 윈도우에 의해 화면의 일부
또는 전체가 가려졌다가 다시 나타날 때 등 시스템이 해당 윈도우 내의 그림에 대한
업데이트가 필요하다고 판단하는 경우 자동으로 호출된다.
강제 호출이 필요한 경우에는 glutPostRedisplay() 함수를 호출하면된다.

이 함수는 불시에 빈번하게 호출된다는 것을 명심하고, 윈도우 상태 변화와 무관한
1회성 코드는 가능한한 이 함수 밖에 구현해야한다. 특히 메모리 할당, VAO, VBO 생성
등의 하드웨어 점유를 시도하는 코드는 특별한 이유가 없다면 절대 이 함수에 포함시키면
안된다. 예를 들어, 메시 모델을 정의하고 VAO, VBO를 설정하는 부분은 최초 1회만
실행하면되므로 main() 함수 등 외부에 구현해야한다. 정의된 메시 모델을 프레임 버퍼에
그리도록 지시하는 코드만 이 함수에 구현하면 된다.

만일, 이 함수 내에서 동적 메모리 할당을 해야하는 경우가 있다면 해당 메모리는 반드시
이 함수가 끝나기 전에 해제 해야한다.

ref: https://www.opengl.org/resources/libraries/glut/spec3/node46.html#SECTION00081000000000000000
*/
void Display()
{
	// 전체 화면을 지운다.
	// glClear는 Display 함수 가장 윗 부분에서 한 번만 호출되어야한다.
	glClearColor(0.5f, 0.75f, 0.9f, 0.7f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



	// Vertex shader 의 matrix 변수들의 location을 받아온다.
	int m_proj_loc = glGetUniformLocation(s_program_id, "proj_matrix");
	int m_view_loc = glGetUniformLocation(s_program_id, "view_matrix");
	int m_model_loc = glGetUniformLocation(s_program_id, "model_matrix");
	int m_shadow_loc = glGetUniformLocation(s_program_id, "shadow_matrix");

	glm::mat4 projection_matrix;
	//glm::mat4 view_matrix;

	/*
	1일때 일반
	2일때 핸들쪽(1인칭)
	3일때 뒷쪽(3인칭)
	4일때 부드럽게 줌인
	5일때 부드럽게 줌아웃

	움직이지 않아두 ㄱㅊ
	*/

	{
		// 빛의 개수를 Shader 변수 num_of_lights 로 넘겨준다.
		int num_of_lights_loc = glGetUniformLocation(s_program_id, "num_of_lights");
		glUniform1i(num_of_lights_loc, 3);
	}



	if (g_camera_mode == 1) //2번 눌렀을 때임!
	{
		// g_camera_pos={ 0.05f, 0.6f, -0.15f }; //값은 임의의 값을 준 것 // 확인해보고 수정해보셈
		//g_camera_pos = { 0.f, 0.55f, 0.f };

		// Projection Transform Matrix 설정.
		projection_matrix = glm::perspective(glm::radians(90.f), (float)g_window_w / g_window_h, 0.01f, 10000.f);
		glUniformMatrix4fv(m_proj_loc, 1, GL_FALSE, glm::value_ptr(projection_matrix));

		// Camera Transform Matrix 설정.
		//glm::vec3 num_2 = glm::rotateY(g_camera_pos + glm::vec3(0.f, 0.f, 1.f), g_car_rotation_y) + g_car_poisition;

		//g_camera_pos_f = glm::rotateY(g_camera_pos, g_car_rotation_y) + g_car_poisition;
		glm::vec3 num_2 = glm::rotateY(glm::vec3(0.f, 0.6f, -0.15f) + glm::vec3(0.f, 0.f, 1.f), g_car_rotation_y) + g_car_poisition;

		g_camera_pos_f = glm::rotateY(glm::vec3(0.f, 0.6f, -0.15f), g_car_rotation_y) + g_car_poisition;

		view_matrix = glm::lookAt(g_camera_pos_f, num_2, glm::vec3(0.f, 1.f, 0.f));
		glUniformMatrix4fv(m_view_loc, 1, GL_FALSE, glm::value_ptr(view_matrix));
		/*-----------------------------------------------------------------------------------------*/
		if (!night_mode)
		{
			// 빛의 종류 설정(0: Directionl Light, 1 : Point Light, 2 : Spot Light), fshader_MultiLights.glsl 참고.
			int type_loc = glGetUniformLocation(s_program_id, "lights[0].type");
			glUniform1i(type_loc, 0);

			// 빛의 방향 설정.
			glm::vec3 dir(-1.f, -1.f, 0.f);
			dir = glm::normalize(dir);

			//glm::vec3 light_dir__;
			//light_dir = glm::normalize(light_dir);


			// Apply Camera Matrices.
			////// *** 현재 카메라 방향을 고려하기 위해 view transform 적용  ***
			//  light_dir는 방향을 나타내는 벡터이므로 이동(Translation)변환은 무시되도록 한다. (네 번째 요소 0.f으로 셋팅)
			dir = glm::vec3(view_matrix * glm::vec4(dir, 0.f));
			dir[2] = -0.8f; //이 값을 고정해서 밝아졌긴 함 ㅇㅇ
			dir[0] = -0.05f;

			//두줄 추가
			int dir_loc = glGetUniformLocation(s_program_id, "lights[0].dir");
			glUniform3f(dir_loc, dir[0], dir[1], dir[2]);

			// 빛의 세기 설정.
			int intensity_loc = glGetUniformLocation(s_program_id, "lights[0].intensity");
			glUniform3f(intensity_loc, 1.f, 1.f, 1.f);
		}
		/*-----------------------------------------------------------------------------------------*/


	}
	else if (g_camera_mode == 2) //3을 눌렀을때
	{
		projection_matrix = glm::perspective(glm::radians(45.f), (float)g_window_w / g_window_h, 0.01f, 10000.f);
		glUniformMatrix4fv(m_proj_loc, 1, GL_FALSE, glm::value_ptr(projection_matrix));

		//
		// 살짝 나춰야함 그러면 rotate를 해줘야해~!
		glm::vec3 num_2 = glm::rotateY(glm::vec3(0.f, 0.f, 0.f) + glm::vec3(0.f, 0.f, 1.f), g_car_rotation_y) + g_car_poisition;

		g_camera_pos_f = glm::rotateY(glm::vec3(0.f, 2.8f, -3.2f), g_car_rotation_y) + g_car_poisition;

		view_matrix = glm::lookAt(g_camera_pos_f, num_2, glm::vec3(0.f, 1.f, 0.f));
		glUniformMatrix4fv(m_view_loc, 1, GL_FALSE, glm::value_ptr(view_matrix));

		/*----------------------빛-----------------------*/

		if (!night_mode)
		{
			int type_loc = glGetUniformLocation(s_program_id, "lights[0].type");
			glUniform1i(type_loc, 0);

			// 빛의 방향 설정.
			glm::vec3 dir(-1.f, -1.f, 0.f);
			dir = glm::normalize(dir);

			// Apply Camera Matrices.
			////// *** 현재 카메라 방향을 고려하기 위해 view transform 적용  ***
			//  light_dir는 방향을 나타내는 벡터이므로 이동(Translation)변환은 무시되도록 한다. (네 번째 요소 0.f으로 셋팅)
			dir = glm::vec3(view_matrix * glm::vec4(dir, 0.f));
			//std::cout << "view_matrix: " << glm::to_string(view_matrix) << std::endl;

			dir[2] = -0.9f; //이 값을 고정해서 밝아졌긴 함 ㅇㅇ
			dir[0] = -0.f;
			dir[1] = -0.02f;




			int dir_loc = glGetUniformLocation(s_program_id, "lights[0].dir");
			glUniform3f(dir_loc, dir[0], dir[1], dir[2]);

			// 빛의 세기 설정.
			int intensity_loc = glGetUniformLocation(s_program_id, "lights[0].intensity");
			glUniform3f(intensity_loc, 1.f, 1.f, 1.f);
		}
	}
	else if (g_camera_mode == 3) //4을 눌렀을때 //줌인 // 2번(1)을 누르고 4(3)을 누르면 실행 xx! 
	{
		ck_time = g_time_s / 3.5f;
		if (g_time_s / 3.5f + 45 >= 90.f)
		{
			ck_time = 90.f;
			//std::cout << "ck_time! ! !" << std::endl;
		}

		ck_y = 2.8f - g_time_s / 74;
		if (2.8f - g_time_s / 74 <= 0.6f)
		{
			ck_y = 0.6f;
			//std::cout << "ck_y! ! !" << std::endl;
		}

		ck_z = -3.2f + (g_time_s / 52);
		if (-3.2f + (g_time_s / 52) >= -0.15f)
		{
			ck_z = -0.15f;
			//std::cout << "ck_z! ! !" << std::endl;
		}


		projection_matrix = glm::perspective(glm::radians(45.f) + glm::radians(ck_time), (float)g_window_w / g_window_h, 0.01f, 10000.f);
		glUniformMatrix4fv(m_proj_loc, 1, GL_FALSE, glm::value_ptr(projection_matrix));
		// 3->2로 갈꺼야! 
		glm::vec3 num_2 = glm::rotateY(glm::vec3(0.f, 0.f, 0.f) + glm::vec3(0.f, 0.f, 1.f), g_car_rotation_y) + g_car_poisition;


		g_camera_pos_f = glm::rotateY(glm::vec3(0.f, ck_y, ck_z), g_car_rotation_y) + g_car_poisition;

		/*
				if (g_time_s > 10 )//&& g_time_s<15)
				{
					g_camera_pos_f = glm::rotateY(glm::vec3(0.f, 0.6f, -0.15f), g_car_rotation_y) + g_car_poisition;
				}
				else if(g_time_s<=10)
					g_camera_pos_f = glm::rotateY(glm::vec3(0.f, 2.8f, -3.2f), g_car_rotation_y) + g_car_poisition;
				else if (g_time_s >= 15)
					g_camera_pos_f = glm::rotateY(glm::vec3(0.f, 1.0f, -3.2f), g_car_rotation_y) + g_car_poisition;
				else if (g_time_s >= 30)
					g_camera_pos_f = glm::rotateY(glm::vec3(0.f, 1.0f, -3.2f), g_car_rotation_y) + g_car_poisition;
					*/
		view_matrix = glm::lookAt(g_camera_pos_f, num_2, glm::vec3(0.f, 1.f, 0.f));

		//view_matrix[1] -= g_time_s / 60;


//g_time_s에 계속 더해줘야해..  자연스럽게 하려고? 'ㅁ' 

		if (g_time_s >= 155)// || view_matrix[1] <= 0.6f)
		{
			g_time_s = 0.f;
			g_camera_mode = 1;
		}
		else
			g_time_s += 2.2f;


		glUniformMatrix4fv(m_view_loc, 1, GL_FALSE, glm::value_ptr(view_matrix));


	}
	else if (g_camera_mode == 4) //5을 눌렀을때 //줌아웃 // 3번(2)을 누르고 5(4)을 누르면 실행 xx! 
	{
		ck_time = g_time_s / 3.5f;
		if (90.f - g_time_s / 3.5f <= 45.f)
		{
			ck_time = 45.f;
			std::cout << "ck_time! ! !" << std::endl;
		}

		ck_y = 0.6f + g_time_s / 74;
		if (0.6f + g_time_s / 74 >= 2.8f)
		{
			ck_y = 2.8f;
			std::cout << "ck_y! ! !" << std::endl;
		}

		ck_z = -0.15f - (g_time_s / 52);
		if (-0.15 - (g_time_s / 52) <= -3.2f)
		{
			ck_z = -3.2f;
			std::cout << "ck_z! ! !" << std::endl;
		}


		projection_matrix = glm::perspective(glm::radians(90.f) - glm::radians(ck_time), (float)g_window_w / g_window_h, 0.01f, 10000.f);
		glUniformMatrix4fv(m_proj_loc, 1, GL_FALSE, glm::value_ptr(projection_matrix));
		// 3->2로 갈꺼야! 
		glm::vec3 num_2 = glm::rotateY(glm::vec3(0.f, 0.f, 0.f) + glm::vec3(0.f, 0.f, 1.f), g_car_rotation_y) + g_car_poisition;

		//여기서부터 천처늬 생각해보자!

		g_camera_pos_f = glm::rotateY(glm::vec3(0.f, ck_y, ck_z), g_car_rotation_y) + g_car_poisition;


		view_matrix = glm::lookAt(g_camera_pos_f, num_2, glm::vec3(0.f, 1.f, 0.f));



		if (g_time_s >= 155)// || view_matrix[1] <= 0.6f)
		{
			g_time_s = 0.f;
			g_camera_mode = 2;
			//g_time_s += 2.2f;

		}
		else
			g_time_s += 2.2f;


		glUniformMatrix4fv(m_view_loc, 1, GL_FALSE, glm::value_ptr(view_matrix));

		if (!night_mode)
		{
			int type_loc = glGetUniformLocation(s_program_id, "lights[0].type");
			glUniform1i(type_loc, 0);

			// 빛의 방향 설정.
			glm::vec3 dir(-1.f, -1.f, 0.f);
			dir = glm::normalize(dir);

			// Apply Camera Matrices.
			////// *** 현재 카메라 방향을 고려하기 위해 view transform 적용  ***
			//  light_dir는 방향을 나타내는 벡터이므로 이동(Translation)변환은 무시되도록 한다. (네 번째 요소 0.f으로 셋팅)
			dir = glm::vec3(view_matrix * glm::vec4(dir, 0.f));
			//std::cout << "view_matrix: " << glm::to_string(view_matrix) << std::endl;

			dir[2] = -0.9f; //이 값을 고정해서 밝아졌긴 함 ㅇㅇ
			dir[0] = -0.f;
			dir[1] = -0.02f;

			int dir_loc = glGetUniformLocation(s_program_id, "lights[0].dir");
			glUniform3f(dir_loc, dir[0], dir[1], dir[2]);

			// 빛의 세기 설정.
			int intensity_loc = glGetUniformLocation(s_program_id, "lights[0].intensity");
			glUniform3f(intensity_loc, 1.f, 1.f, 1.f);
		}
	}
	else //카메라 모드가 1이 아닐때 'ㅁ'
	{
		glm::mat4 projection_matrix = glm::perspective(glm::radians(45.f), (float)g_window_w / g_window_h, 0.01f, 10000.f);
		glUniformMatrix4fv(m_proj_loc, 1, GL_FALSE, glm::value_ptr(projection_matrix));

		// Camera Transform Matrix 설정.
		glUniformMatrix4fv(m_view_loc, 1, GL_FALSE, glm::value_ptr(g_camera.GetGLViewMatrix()));

		// 빛의 종류 설정 (0: Directionl Light, 1: Point Light, 2: Spot Light), fshader_MultiLights.glsl 참고.
		int type_loc = glGetUniformLocation(s_program_id, "lights[0].type");
		glUniform1i(type_loc, 0);

		glm::vec3 dir(-1.f, -1.f, 0.f);
		dir = glm::normalize(dir);

		// Apply Camera Matrices.
		////// *** 현재 카메라 방향을 고려하기 위해 view transform 적용  ***
		//  light_dir는 방향을 나타내는 벡터이므로 이동(Translation)변환은 무시되도록 한다. (네 번째 요소 0.f으로 셋팅)
		dir = glm::vec3(view_matrix * glm::vec4(dir, 0.f));
		//std::cout << "view_matrix: " << glm::to_string(view_matrix) << std::endl;

		dir[2] = -0.9f; //이 값을 고정해서 밝아졌긴 함 ㅇㅇ
		dir[0] = -0.f;
		dir[1] = -0.02f;



		int dir_loc = glGetUniformLocation(s_program_id, "lights[0].dir");
		glUniform3f(dir_loc, dir[0], dir[1], dir[2]);

		// 빛의 세기 설정.
		int intensity_loc = glGetUniformLocation(s_program_id, "lights[0].intensity");
		glUniform3f(intensity_loc, 1.f, 1.f, 1.f);
	
	}



	if (night_mode)
	{
		glClearColor(0.01f, 0.01f, 0.03f, 0.001f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		if (g_camera_mode == 0)
		{// Camera Transform Matrix 설정.
			view_matrix = g_camera.GetGLViewMatrix();
			glUniformMatrix4fv(m_view_loc, 1, GL_FALSE, glm::value_ptr(view_matrix));
		}


		// Lights, 빛 설정.
		{
			// 빛의 개수를 Shader 변수 num_of_lights 로 넘겨준다.
			int num_of_lights_loc = glGetUniformLocation(s_program_id, "num_of_lights");
			glUniform1i(num_of_lights_loc, 30);

			// Directional Light 설정.
			{
				// 빛의 종류 설정 (0: Directionl Light, 1: Point Light, 2: Spot Light), fshader_MultiLights.glsl 참고.
				int type_loc = glGetUniformLocation(s_program_id, "lights[0].type");
				glUniform1i(type_loc, 0);



				// 빛의 방향 설정.
				glm::vec3 dir(-1.f, -1.f, 0.f);
				dir = glm::normalize(dir);

				////// *** 현재 카메라 방향을 고려하기 위해 view transform 적용  ***
				//  dir는 방향을 나타내는 벡터이므로 이동(Translation)변환은 무시되도록 한다. (네 번째 요소 0.f으로 셋팅)
				dir = glm::vec3(view_matrix * glm::vec4(dir, 0.f));

				int dir_loc = glGetUniformLocation(s_program_id, "lights[0].dir");
				glUniform3f(dir_loc, dir[0], dir[1], dir[2]);

				// 빛의 세기 설정.
				int intensity_loc = glGetUniformLocation(s_program_id, "lights[0].intensity");
				glUniform3f(intensity_loc, 0.3f, 0.3f, 0.3f);
			}
			/////////////// Spot Light 설정.//왼
			{
				// 빛의 종류 설정 (0: Directionl Light, 1: Point Light, 2: Spot Light), fshader_MultiLights.glsl 참고.
				int type_loc = glGetUniformLocation(s_program_id, "lights[1].type");
				glUniform1i(type_loc, 2);

				// 빛이 출발하는 위치(광원) 설정.
				//glm::vec3 pos = g_car_poisition + glm::rotateY(glm::vec3(0.17f, 0.4f, 0.36f), g_car_rotation_y);
				glm::vec3 pos = g_car_poisition + glm::rotateY(glm::vec3(0.2f, 0.5f, 0.4f), g_car_rotation_y);
				// Apply Camera Matrices
				////// *** 현재 카메라 방향을 고려하기 위해 view transform 적용  ***
				//  이때 pos는 위치를 나타내는 포인트이므로 이동(Translation)변환이 적용되도록 한다. (네 번째 요소 1.f으로 셋팅)
				pos = glm::vec3(view_matrix * glm::vec4(pos, 1.f));

				int pos_loc = glGetUniformLocation(s_program_id, "lights[1].position");
				glUniform3f(pos_loc, pos[0], pos[1], pos[2]);


				// 빛의 방향 설정.
				glm::vec3 dir(0.f, -0.5f, 1.f);
				dir = glm::rotateY(dir, g_car_rotation_y);
				dir = glm::normalize(dir);

				////// *** 현재 카메라 방향을 고려하기 위해 view transform 적용  ***
				//  dir는 방향을 나타내는 벡터이므로 이동(Translation)변환은 무시되도록 한다. (네 번째 요소 0.f으로 셋팅)
				dir = glm::vec3(view_matrix * glm::vec4(dir, 0.f));

				int dir_loc = glGetUniformLocation(s_program_id, "lights[1].dir");
				glUniform3f(dir_loc, dir[0], dir[1], dir[2]);



				// 빛의 세기 설정
				int intensity_loc = glGetUniformLocation(s_program_id, "lights[1].intensity");
				glUniform3f(intensity_loc, 1.f, 1.f, 1.f);

				// 빛의 퍼짐 정도 설정.
				int light_cos_cutoff_loc = glGetUniformLocation(s_program_id, "lights[1].cos_cutoff");
				glUniform1f(light_cos_cutoff_loc, cos(26.f / 180.f * glm::pi<float>()));
			}
			// 오른쪽 Headlight
			{
				// 빛의 종류 설정 (0: Directionl Light, 1: Point Light, 2: Spot Light), fshader_MultiLights.glsl 참고.
				int type_loc = glGetUniformLocation(s_program_id, "lights[2].type");
				glUniform1i(type_loc, 2);

				// 빛이 출발하는 위치(광원) 설정.
				glm::vec3 pos = g_car_poisition + glm::rotateY(glm::vec3(-0.2f, 0.5f, 0.4f), g_car_rotation_y);
				// Apply Camera Matrices
				////// *** 현재 카메라 방향을 고려하기 위해 view transform 적용  ***
				//  이때 pos는 위치를 나타내는 포인트이므로 이동(Translation)변환이 적용되도록 한다. (네 번째 요소 1.f으로 셋팅)
				pos = glm::vec3(view_matrix * glm::vec4(pos, 1.f));

				int pos_loc = glGetUniformLocation(s_program_id, "lights[2].position");
				glUniform3f(pos_loc, pos[0], pos[1], pos[2]);


				// 빛의 방향 설정.
				glm::vec3 dir(0.f, -0.5f, 1.f);
				dir = glm::rotateY(dir, g_car_rotation_y);
				dir = glm::normalize(dir);

				////// *** 현재 카메라 방향을 고려하기 위해 view transform 적용  ***
				//  dir는 방향을 나타내는 벡터이므로 이동(Translation)변환은 무시되도록 한다. (네 번째 요소 0.f으로 셋팅)
				dir = glm::vec3(view_matrix * glm::vec4(dir, 0.f));

				int dir_loc = glGetUniformLocation(s_program_id, "lights[2].dir");
				glUniform3f(dir_loc, dir[0], dir[1], dir[2]);



				// 빛의 세기 설정
				int intensity_loc = glGetUniformLocation(s_program_id, "lights[2].intensity");
				glUniform3f(intensity_loc, 1.f, 1.f, 1.f);

				// 빛의 퍼짐 정도 설정.
				int light_cos_cutoff_loc = glGetUniformLocation(s_program_id, "lights[2].cos_cutoff");
				glUniform1f(light_cos_cutoff_loc, cos(26.f / 180.f * glm::pi<float>()));
			}
			//반디
			{
				// 빛의 종류 설정 (0: Directionl Light, 1: Point Light, 2: Spot Light), fshader_MultiLights.glsl 참고.
				int type_loc = glGetUniformLocation(s_program_id, "lights[3].type");
				glUniform1i(type_loc, 1);


				// 빛이 출발하는 위치(광원) 설정.
				// 시간에 따라 위치가 변하도록 함.
				glm::vec3 pos = glm::vec3(2.f, 0.f, 0.f)*(cos(g_elaped_time_s) / 2, 0.1f, sin(g_elaped_time_s) / 2);
				//glm::vec3 pos = glm::translate(glm::vec3(2.f - 5.f, 0.f,2.f - 5.f));
				// Apply Camera Matrices
				////// *** 현재 카메라 방향을 고려하기 위해 view transform 적용  ***
				//  이때 pos는 위치를 나타내는 포인트이므로 이동(Translation)변환이 적용되도록 한다. (네 번째 요소 1.f으로 셋팅)
				pos = glm::vec3(glm::translate(glm::vec3(3.f, 0.1f, 3.f))*view_matrix * glm::vec4(pos, 1.f));

				int pos_loc = glGetUniformLocation(s_program_id, "lights[3].position");
				glUniform3f(pos_loc, pos[0], pos[1], pos[2]);


				// 빛의 세기 설정.
				int intensity_loc = glGetUniformLocation(s_program_id, "lights[3].intensity");
				glUniform3f(intensity_loc, 1.f, 1.f, 1.f);


			}
			// 반디 2
			{
				// 빛의 종류 설정 (0: Directionl Light, 1: Point Light, 2: Spot Light), fshader_MultiLights.glsl 참고.
				int type_loc = glGetUniformLocation(s_program_id, "lights[4].type");
				glUniform1i(type_loc, 1);


				// 빛이 출발하는 위치(광원) 설정.
				// 시간에 따라 위치가 변하도록 함.
				glm::vec3 pos(cos(g_elaped_time_s), 0.1f, sin(g_elaped_time_s));

				// Apply Camera Matrices
				////// *** 현재 카메라 방향을 고려하기 위해 view transform 적용  ***
				//  이때 pos는 위치를 나타내는 포인트이므로 이동(Translation)변환이 적용되도록 한다. (네 번째 요소 1.f으로 셋팅)
				pos = glm::vec3(glm::translate(glm::vec3(2.f - 5.f, 0.f, 2.f - 5.f))*view_matrix * glm::vec4(pos, 1.f));

				int pos_loc = glGetUniformLocation(s_program_id, "lights[4].position");
				glUniform3f(pos_loc, pos[0], pos[1], pos[2]);


				// 빛의 세기 설정.
				int intensity_loc = glGetUniformLocation(s_program_id, "lights[4].intensity");
				glUniform3f(intensity_loc, 1.f, 1.f, 1.f);


			}


		}

	}
	else
	{

		{
			// 빛의 종류 설정 (0: Directionl Light, 1: Point Light, 2: Spot Light), fshader_MultiLights.glsl 참고.
			int type_loc = glGetUniformLocation(s_program_id, "lights[1].type");
			glUniform1i(type_loc, 2);

			// 빛이 출발하는 위치(광원) 설정.
			//glm::vec3 pos = g_car_poisition + glm::rotateY(glm::vec3(0.17f, 0.4f, 0.36f), g_car_rotation_y);
			glm::vec3 pos = g_car_poisition + glm::rotateY(glm::vec3(0.2f, 0.5f, 0.4f), g_car_rotation_y);
			// Apply Camera Matrices
			////// *** 현재 카메라 방향을 고려하기 위해 view transform 적용  ***
			//  이때 pos는 위치를 나타내는 포인트이므로 이동(Translation)변환이 적용되도록 한다. (네 번째 요소 1.f으로 셋팅)
			pos = glm::vec3(view_matrix * glm::vec4(pos, 1.f));

			int pos_loc = glGetUniformLocation(s_program_id, "lights[1].position");
			glUniform3f(pos_loc, pos[0], pos[1], pos[2]);


			// 빛의 방향 설정.
			glm::vec3 dir(0.f, -0.5f, 1.f);
			dir = glm::rotateY(dir, g_car_rotation_y);
			dir = glm::normalize(dir);

			////// *** 현재 카메라 방향을 고려하기 위해 view transform 적용  ***
			//  dir는 방향을 나타내는 벡터이므로 이동(Translation)변환은 무시되도록 한다. (네 번째 요소 0.f으로 셋팅)
			dir = glm::vec3(view_matrix * glm::vec4(dir, 0.f));

			int dir_loc = glGetUniformLocation(s_program_id, "lights[1].dir");
			glUniform3f(dir_loc, dir[0], dir[1], dir[2]);



			// 빛의 세기 설정
			int intensity_loc = glGetUniformLocation(s_program_id, "lights[1].intensity");
			glUniform3f(intensity_loc, 0.f, 0.f, 0.f);

			// 빛의 퍼짐 정도 설정.
			int light_cos_cutoff_loc = glGetUniformLocation(s_program_id, "lights[1].cos_cutoff");
			glUniform1f(light_cos_cutoff_loc, cos(26.f / 180.f * glm::pi<float>()));
		}
		// 오른쪽 Headlight
		{
			// 빛의 종류 설정 (0: Directionl Light, 1: Point Light, 2: Spot Light), fshader_MultiLights.glsl 참고.
			int type_loc = glGetUniformLocation(s_program_id, "lights[2].type");
			glUniform1i(type_loc, 2);

			// 빛이 출발하는 위치(광원) 설정.
			glm::vec3 pos = g_car_poisition + glm::rotateY(glm::vec3(-0.2f, 0.5f, 0.4f), g_car_rotation_y);
			// Apply Camera Matrices
			////// *** 현재 카메라 방향을 고려하기 위해 view transform 적용  ***
			//  이때 pos는 위치를 나타내는 포인트이므로 이동(Translation)변환이 적용되도록 한다. (네 번째 요소 1.f으로 셋팅)
			pos = glm::vec3(view_matrix * glm::vec4(pos, 1.f));

			int pos_loc = glGetUniformLocation(s_program_id, "lights[2].position");
			glUniform3f(pos_loc, pos[0], pos[1], pos[2]);


			// 빛의 방향 설정.
			glm::vec3 dir(0.f, -0.5f, 1.f);
			dir = glm::rotateY(dir, g_car_rotation_y);
			dir = glm::normalize(dir);

			////// *** 현재 카메라 방향을 고려하기 위해 view transform 적용  ***
			//  dir는 방향을 나타내는 벡터이므로 이동(Translation)변환은 무시되도록 한다. (네 번째 요소 0.f으로 셋팅)
			dir = glm::vec3(view_matrix * glm::vec4(dir, 0.f));

			int dir_loc = glGetUniformLocation(s_program_id, "lights[2].dir");
			glUniform3f(dir_loc, dir[0], dir[1], dir[2]);



			// 빛의 세기 설정
			int intensity_loc = glGetUniformLocation(s_program_id, "lights[2].intensity");
			glUniform3f(intensity_loc, 0.f, 0.f, 0.f);

			// 빛의 퍼짐 정도 설정.
			int light_cos_cutoff_loc = glGetUniformLocation(s_program_id, "lights[2].cos_cutoff");



			//glUniform1f(light_cos_cutoff_loc, cos(26.f / 180.f * glm::pi<float>()));


		}

	}

	{
	glm::vec3 light_dir(-1.f, -1.f, -1.f);
	light_dir = glm::normalize(light_dir);

	// fragment shader 로 보내는 light_dir 은 view_matrix 를 미리 적용해서 보낸다.
	glm::vec3 m_light_dir = glm::vec3(view_matrix * glm::vec4(light_dir, 0.f));
	int light_dir_loc = glGetUniformLocation(s_program_id, "light_dir");
	glUniform3f(light_dir_loc, m_light_dir[0], m_light_dir[1], m_light_dir[2]);

	// Shadow Projection Matrix
	glm::mat4 shadow_matrix = glm::mat4(1.f);
	shadow_matrix[1][0] = light_dir.x;
	shadow_matrix[1][1] = 0.f;
	shadow_matrix[3][1] = 0.001f;
	shadow_matrix[1][2] = light_dir.z;

	glUniformMatrix4fv(m_shadow_loc, 1, GL_FALSE, glm::value_ptr(shadow_matrix));

	}
	glUniform1i(glGetUniformLocation(s_program_id, "p_shadow_mode"), false);

	// 바닥 격자
	{// Ground를 위한 Phong Shading 관련 변수 값을 설정한다.
		int shininess_loc = glGetUniformLocation(s_program_id, "shininess_n");
		glUniform1f(shininess_loc, 50.f);

		int K_s_loc = glGetUniformLocation(s_program_id, "K_s");
		glUniform3f(K_s_loc, 0.3f, 0.3f, 0.3f);



		// 카메라 변환 행렬을 설정한다.
		glm::mat4 model_T(1.f); // 단위행렬
		glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(model_T));

		// 그린다.
		DrawGround2();
	}

	// Moving Car
	{
		int shininess_loc = glGetUniformLocation(s_program_id, "shininess_n");
		glUniform1f(shininess_loc, 50.f);

		int K_s_loc = glGetUniformLocation(s_program_id, "K_s");
		glUniform3f(K_s_loc, 0.3f, 0.3f, 0.3f);
		
		glm::mat4 car_T = glm::translate(g_car_poisition) * glm::rotate(g_car_rotation_y, glm::vec3(0.f, 1.f, 0.f));
		glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(car_T));
		DrawCarModel();
	}

	// 나무
	// 나무

/*	for (int i = 0; i <= 5; i++)
	{
		for (int j = 0; j <= 5; j++)
		{
			int shininess_loc = glGetUniformLocation(s_program_id, "shininess_n");
			glUniform1f(shininess_loc, 50.f);

			int K_s_loc = glGetUniformLocation(s_program_id, "K_s");
			glUniform3f(K_s_loc, 0.3f, 0.3f, 0.3f);

			glm::mat4 model_T;
			model_T = glm::translate(glm::vec3(i * 2.f - 5.f, 0.f, j * 2.f - 5.f));
			glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(model_T));
			DrawTreeModel();
		}
	}*/
	// 나무
	
	{
		//DeleteTreeModel();
		//InitTreeModel();
		for (int i = 0; i <= 5; i++)
			for (int j = 0; j <= 5; j++)
			{
				glm::mat4 tree_T;
				tree_T = glm::translate(glm::vec3(i*2.f - 5.f, 0.f, j*2.f - 5.f));
				glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(tree_T));

				DrawTreeModel((i + j) % NUM_TREES);
			}
	}
	///////////////////////////////////////////////////////////////////그림자///////////////////////////////////////////////////////

	glUniform1i(glGetUniformLocation(s_program_id, "p_shadow_mode"), true);

	// Moving Car
	{
		/*glm::mat4 car_T = glm::translate(g_car_poisition) * glm::rotate(g_car_rotation_y, glm::vec3(0.f, 1.f, 0.f));
		glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(car_T));
		DrawCarModel();
		*/
	
		glm::mat4 car_T = glm::translate(g_car_poisition) * glm::rotate(g_car_rotation_y, glm::vec3(0.f, 1.f, 0.f));
		glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(car_T));
		DrawCarModel();
	}
	
	// 나무
	for (int i = 0; i <= 5; i++)
	{
		for (int j = 0; j <= 5; j++)
		{
			glm::mat4 model_T;
			model_T = glm::translate(glm::vec3(i * 2.f - 5.f, 0.f, j * 2.f - 5.f));
			glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(model_T));
			DrawTreeModel((i + j) % NUM_TREES);
		}
	}
	
	// flipping the double buffers
	// glutSwapBuffers는 항상 Display 함수 가장 아래 부분에서 한 번만 호출되어야한다.
	glutSwapBuffers();
}





/**
Reshape: 윈도우의 크기가 조정될 때마다 자동으로 호출되는 callback 함수.

@param w, h는 각각 조정된 윈도우의 가로 크기와 세로 크기 (픽셀 단위).
ref: https://www.opengl.org/resources/libraries/glut/spec3/node48.html#SECTION00083000000000000000
*/
void Reshape(int w, int h)
{
	//  w : window width   h : window height
	g_window_w = w;
	g_window_h = h;

	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glutPostRedisplay();
}

/**
Keyboard: 키보드 입력이 있을 때마다 자동으로 호출되는 함수.
@param key는 눌려진 키보드의 문자값.
@param x,y는 현재 마우스 포인터의 좌표값.
ref: https://www.opengl.org/resources/libraries/glut/spec3/node49.html#SECTION00084000000000000000

*/
void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 's':
	case 'S':
		g_car_speed = -0.01f;		// 후진 속도 설정 
		glutPostRedisplay();
		break;

	case 'w':
	case 'W':
		g_car_speed = 0.01f;		// 전진 속도 설정
		glutPostRedisplay(); // 그림 업데이트 
		break;

	case 'a':
	case 'A':
		g_car_angular_speed = glm::radians(1.f);		// 좌회전 각속도 설정
		glutPostRedisplay();
		break;

	case 'd':
	case 'D':
		g_car_angular_speed = -1 * glm::radians(1.f);		//  우회전 각속도 설정
		glutPostRedisplay();
		break;

	case '1':
		g_camera_mode = 0;
		glutPostRedisplay();
		break;

	case '2':
		g_camera_mode = 1;
		glutPostRedisplay();
		break;
	case '3':
		g_camera_mode = 2;
		glutPostRedisplay();
		break;
	case '4':
		if (g_camera_mode == 1)// || g_camera_mode == 3)
		{
			g_camera_mode = 1;
		}
		else if (g_camera_mode == 3)
		{

		}
		else if (g_camera_mode == 4)
		{
			g_camera_mode = 3;
			g_time_s -= 10.f;
		}
		else
		{

			g_time_s = 0.f;
			g_camera_mode = 3;
			//g_camera_pos_f = g_camera_pos;
		}

		glutPostRedisplay();
		break;
	case '5':
		if (g_camera_mode == 2)//|| g_camera_mode == 4)
		{
			g_camera_mode = 2;
		}
		else if (g_camera_mode == 4)
		{

		}
		else if (g_camera_mode == 3)
		{
			g_camera_mode = 4;
			g_time_s -= 10.f;
		}
		else
		{
			g_time_s = 0.f;
			g_camera_mode = 4; // 5를 눌렀을때
		}

		glutPostRedisplay();
		break;

		/*case 'n':
		case 'N':
			night_mode = true;
			//g_camera_mode = 5;
			//g_car_angular_speed = -1 * glm::radians(1.f);		//  우회전 각속도 설정
			glutPostRedisplay();
			break;*/
	}
	if (key == 'n' || key == 'N')
	{
		if (night_mode == true)
		{
			night_mode = false;
			glutPostRedisplay();
		}
		else
		{
			night_mode = true;
			//g_camera_mode = 5;
			//g_car_angular_speed = -1 * glm::radians(1.f);		//  우회전 각속도 설정
			glutPostRedisplay();
			//break; 
		}
	}
}

/**
KeyboardUp: 눌려졌던 키가 놓여질 때마다 자동으로 호출되는 함수.
@param key는 해당 키보드의 문자값.
@param x,y는 현재 마우스 포인터의 좌표값.
ref: https://www.opengl.org/resources/libraries/glut/spec3/node49.html#SECTION00084000000000000000

*/
void KeyboardUp(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 's':
	case 'S':
		g_car_speed = 0.f;		// 후진 속도 설정
		glutPostRedisplay();
		break;

	case 'w':
	case 'W':
		g_car_speed = 0.f;		// 전진 속도 설정
		glutPostRedisplay();
		break;

	case 'a':
	case 'A':
		g_car_angular_speed = 0.f;		// 좌회전 각속도 설정
		glutPostRedisplay();
		break;

	case 'd':
	case 'D':
		g_car_angular_speed = 0.f;		//  우회전 각속도 설정
		glutPostRedisplay();
		break;

	}

}



/**
Mouse: 마우스 버튼이 입력될 때마다 자동으로 호출되는 함수.
파라메터의 의미는 다음과 같다.
@param button: 사용된 버튼의 종류
  GLUT_LEFT_BUTTON - 왼쪽 버튼
  GLUT_RIGHT_BUTTON - 오른쪽 버튼
  GLUT_MIDDLE_BUTTON - 가운데 버튼 (휠이 눌러졌을 때)
  3 - 마우스 휠 (휠이 위로 돌아 갔음).
  4 - 마우스 휠 (휠이 아래로 돌아 갔음).
@param state: 조작 상태
  GLUT_DOWN - 눌러 졌음
  GLUT_UP - 놓여졌음
@param x,y: 조작이 일어났을 때, 마우스 포인터의 좌표값.
*/
void Mouse(int button, int state, int x, int y)
{
	float mouse_xd = (float)x / g_window_w;
	float mouse_yd = 1 - (float)y / g_window_h;
	float last_mouse_xd = (float)g_last_mouse_x / g_window_w;
	float last_mouse_yd = 1 - (float)g_last_mouse_y / g_window_h;


	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		g_left_button_pushed = true;

	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
		g_left_button_pushed = false;

	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
		g_right_button_pushed = true;

	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
		g_right_button_pushed = false;
	else if (button == 3)
	{
		g_camera.inputMouse(Camera::IN_TRANS_Z, 0, -1, 0.01f);
		glutPostRedisplay();
	}
	else if (button == 4)
	{
		g_camera.inputMouse(Camera::IN_TRANS_Z, 0, 1, 0.01f);
		glutPostRedisplay();
	}

	g_last_mouse_x = x;
	g_last_mouse_y = y;
}





/**
MouseMotion: 마우스 포인터가 움직일 때마다 자동으로 호출되는 함수.
@prarm x,y는 현재 마우스 포인터의 좌표값을 나타낸다.
*/
void MouseMotion(int x, int y)
{
	float mouse_xd = (float)x / g_window_w;
	float mouse_yd = 1 - (float)y / g_window_h;
	float last_mouse_xd = (float)g_last_mouse_x / g_window_w;
	float last_mouse_yd = 1 - (float)g_last_mouse_y / g_window_h;

	if (g_left_button_pushed)
	{
		g_camera.inputMouse(Camera::IN_ROTATION_Y_UP, last_mouse_xd, last_mouse_yd, mouse_xd, mouse_yd);
		glutPostRedisplay();
	}
	else if (g_right_button_pushed)
	{
		g_camera.inputMouse(Camera::IN_TRANS, last_mouse_xd, last_mouse_yd, mouse_xd, mouse_yd, 0.01f);
		glutPostRedisplay();
	}

	g_last_mouse_x = x;
	g_last_mouse_y = y;
}