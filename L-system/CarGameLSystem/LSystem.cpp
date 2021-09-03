
#include "GL/glew.h"
#include "glm/glm.hpp"
#include "../BaseCodes/BasicShapeObjs.h"
#include "../BaseCodes/Mesh.h"
#include <iostream>
#include <string>
#include <stack>

static float g_d = 0.05f;
static float g_delta = glm::radians(30.f);
static std::string g_initial_str = "X";

static bool leaf_color_ck = false;
static bool night_mode;
//노랑색 꽃 !!
//바닥 텍스처 바꾸기
// 오리 찾기



std::string Rule(char in)
{
	std::string out;

	if (in == 'X') out = "F[+X][-X-[+X++_fL]][F][\nX\n\nf][/X/f]";// [\n^X]";
	else if (in == 'F') out = "FF";
	else                  out = in;

	return out;
}


std::string Reproduce(std::string input)
{
	std::string output;

	for (unsigned int i = 0; i < input.size(); i++)
	{
		output = output + Rule(input[i]);
	}

	return output;
}



void CreateLSystemTree(int iteration, Mesh &out_mash)
{
	glm::mat4 Fs(1);

	// Element 1
	Mesh trunk = glm::scale(glm::vec3(0.01f, g_d, 0.01f))
		* glm::translate(glm::vec3(0.f, 0.5f, 0.f))
		* g_cylinder_mesh;
	trunk.SetColor(0.35f, 0.2f, 0.08f, 1);

	// leaf
	Mesh leaf = glm::rotate(glm::pi<float>() / 2.f, glm::vec3(0.f, 1.f, 0.f))
		* glm::rotate(-glm::pi<float>() / 6.f, glm::vec3(1.f, 0.f, 0.f))
		* glm::scale(glm::vec3(0.03f / 1.5, 0.001f / 1.5, 0.08f / 1.5)) //0.03f, 0.001f, 0.08f
		* glm::translate(glm::vec3(0.f, 0.f, 1.f))//* glm::translate(glm::vec3(0.f, 0.f, 1.f))
		* g_cylinder_mesh;
	leaf.SetColor(0.f, 1.f, 0.f, 1.f);

	Mesh flower[15];
	

	for (int i = 0; i < 15; i++)
	{
		/*flower= glm::rotate(glm::pi<float>() / 2.f * (i+1), glm::vec3(0.f, 1.f, 0.f))
			* glm::rotate(-glm::pi<float>() / 6.f, glm::vec3(1.f, 0.f, 0.f))
			//* glm::scale(glm::vec3(0.03f/2, 0.001f/2, 0.08f/2)) //0.03f, 0.001f, 0.08f
			* glm::scale(glm::vec3(0.05f, 0.002f, 0.08f))
			* glm::translate(glm::vec3(0.f, 1.f, 1.f))//* glm::translate(glm::vec3(0.f, 0.f, 1.f))
			* g_cylinder_mesh;
			*/
		if (i < 5)
		{
			flower[i] = glm::translate(glm::vec3(0.f, -0.03f, 0.f)) //확인용
				*glm::rotate(glm::pi<float>() / 2.5f * (i + 1), glm::vec3(0.f, 1.f, 0.f))
				* glm::rotate(-glm::pi<float>() / 6.f, glm::vec3(1.f, 0.f, 0.f))
				//* glm::scale(glm::vec3(0.03f/2, 0.001f/2, 0.08f/2)) //0.03f, 0.001f, 0.08f
				* glm::scale(glm::vec3(0.05f / 2, 0.002f / 2, 0.08f / 2))
				* glm::translate(glm::vec3(0.f, 0.f, 1.f))//* glm::translate(glm::vec3(0.f, 0.f, 1.f))
				* g_cylinder_mesh;
		}
		else if(i<10 && i>5)
		{
			flower[i] = glm::rotate(2.f, glm::vec3(0.f, 1.f, 0.f))
				* glm::translate(glm::vec3(0.f, -0.02f, 0.f)) //확인용
				* glm::rotate(glm::pi<float>() / 2.5f * (i + 1), glm::vec3(0.f, 1.f, 0.f))
				* glm::rotate(-glm::pi<float>() / 5.f, glm::vec3(1.f, 0.f, 0.f))
				//* glm::scale(glm::vec3(0.03f/2, 0.001f/2, 0.08f/2)) //0.03f, 0.001f, 0.08f
				
				* glm::scale(glm::vec3(0.05f / 3, 0.002f / 3, 0.08f / 3))
				* glm::translate(glm::vec3(0.f, 0.f, 1.f))//* glm::translate(glm::vec3(0.f, 0.f, 1.f))
				* g_cylinder_mesh;
		}
		else
		{
			flower[i] = glm::rotate(4.f, glm::vec3(0.f, 1.f, 0.f))
				* glm::translate(glm::vec3(0.f, -0.017f, 0.f)) //확인용
				* glm::rotate(glm::pi<float>() / 2.5f * (i + 1), glm::vec3(0.f, 1.f, 0.f))
				* glm::rotate(-glm::pi<float>() / 4.f, glm::vec3(1.f, 0.f, 0.f))
				* glm::scale(glm::vec3(0.05f / 4, 0.002f / 4, 0.08f / 4))
				* glm::translate(glm::vec3(0.f, 0.f, 1.f))//* glm::translate(glm::vec3(0.f, 0.f, 1.f))
				* g_cylinder_mesh;
		}

		//flower_white[i] = flower[i];
	
			flower[i].SetColor(1.f, 0.2f, 0.2f, 1.f);
	
			//flower_white[i].SetColor(1.f, 1.f, 1.f, 1.f);
		//	Fs += Fs * flower;
	}

	Mesh flower_in = glm::translate(glm::vec3(0.f, -0.005f, 0.f)) //확인용
		//*glm::rotate(glm::pi<float>() / 2.5f * (i + 1), glm::vec3(0.f, 1.f, 0.f))
		//* glm::rotate(-glm::pi<float>() / 6.f, glm::vec3(1.f, 0.f, 0.f))
		//* glm::scale(glm::vec3(0.03f/2, 0.001f/2, 0.08f/2)) //0.03f, 0.001f, 0.08f
		* glm::scale(glm::vec3(0.007f, 0.005f, 0.007f))
		* glm::translate(glm::vec3(0.f, 0.f, 1.f))//* glm::translate(glm::vec3(0.f, 0.f, 1.f))
		* g_cylinder_mesh;

	flower_in.SetColor(0.8f, 0.6f, 0.2f, 1.f);
	//out_mash += flower_in;

	//Fs += Fs * flower_in;




	// String Reproduction
	std::string str = g_initial_str;

	for (int i = 0; i < iteration; i++)
		str = Reproduce(str);

	//cnt를 추가할꺼임 계속 피면 안되니께 ㅠ

	int cnt = 0;
	int cnt_ck = 0;

	std::stack<glm::mat4> stack_T;
	glm::mat4 T(1);
	for (unsigned int i = 0; i < str.size(); i++)
	{
		//cnt++;
		if (str[i] == 'F')
		{
			out_mash += T * trunk;
			T = T * glm::translate(glm::vec3(0, g_d, 0));


		}
		else if (str[i] == 'f')
		{
			out_mash += T * leaf;
			//out_mash += T * flower; //확인
			T = T * glm::translate(glm::vec3(0, g_d, 0));
			//	if (cnt / 3 == 0)
				/*{
					for (int i = 0; i < 5; i++)
					{
						out_mash += T * flower[i];

					}
					out_mash += T* flower_in;
				}
				*/


		}
		else if (str[i] == '+')
		{
			glRotated(g_delta, 1, 0, 0);
			T = T * glm::rotate(g_delta, glm::vec3(1, 0, 0));
		}
		else if (str[i] == '-')
		{
			glRotated(-g_delta, 1, 0, 0);
			T = T * glm::rotate(-g_delta, glm::vec3(1, 0, 0));
		}
		else if (str[i] == '^')
		{
			glRotated(g_delta, 0, 1, 0);
			T = T * glm::rotate(g_delta, glm::vec3(0, 1, 0));
		}
		else if (str[i] == '&')
		{
			glRotated(-g_delta, 0, 1, 0);
			T = T * glm::rotate(-g_delta, glm::vec3(0, 1, 0));
		}
		else if (str[i] == '\n')
		{
			glRotated(g_delta, 0, 0, 1);
			T = T * glm::rotate(g_delta, glm::vec3(0, 0, 1));
		}
		else if (str[i] == '/')
		{
			glRotated(-g_delta, 0, 0, 1);
			T = T * glm::rotate(-g_delta, glm::vec3(0, 0, 1));
		}
		else if (str[i] == 'L')
		{
			if (cnt % 6 == 0 && i > 16)
			{
				cnt_ck++;
				for (int k = 0;k < 15; k++)
				{
					if (cnt_ck % 2 == 0)
					{
						if (k < 5)
							flower[k].SetColor(1.f, 0.2f, 0.2f, 1.f);
						else if(k>5 && k<10)
							flower[k].SetColor(1.f, 0.2f, 0.1f, 0.8f);
						else
							flower[k].SetColor(1.f, 0.2f, 0.1f, 0.7f);
					}
					else
					{
						if (k < 5)
							flower[k].SetColor(1.f, 1.f, 1.f, 1.f);
						else if (k > 5 && k < 10)
							flower[k].SetColor(0.9f, 0.9f, 0.9f, 0.8f);
						else
							flower[k].SetColor(0.9f, 0.9f, 0.9f, 0.75f);
						
					}

						out_mash += T * flower[k];
				}
				out_mash += T * flower_in;
			}
			cnt++;
		}
		else if (str[i] == '_') //change color
		{
			if (leaf_color_ck)
			{
				leaf.SetColor(0.2f, 0.6f, 0.f, 0.8f);
				leaf_color_ck = false;
			}
			else
			{
				leaf.SetColor(0.f, 1.f, 0.f, 1.f);
				leaf_color_ck = true;
			}

			//leaf.SetColor(0.2f, 0.5f, 0.f, 1.f);
			out_mash += T * leaf;
			T = T * glm::translate(glm::vec3(0, g_d, 0));
		}
		else if (str[i] == '[')
		{
			stack_T.push(T);
		}
		else if (str[i] == ']')
		{
			T = stack_T.top();
			stack_T.pop();
		}

	}
}