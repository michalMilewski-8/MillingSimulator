#include <stdio.h>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <math.h>
#include <vector>

//#include "Shader.h"
//#include "Camera.h"
//#include "Cursor.h"
//#include "Torus.h"
#include "Point.h"
//#include "BezierC0.h"
//#include "BezierC2.h"
//#include "BezierInterpol.h"
//#include "BezierFlakeC0.h"
//#include "BezierFlakeC2.h"
//#include "TriangularGregoryPatch.h"
//#include "Virtual.h"
//#include "Virtual.h"
//#include "Intersection.h"
#include "Line.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Dependencies/include/rapidxml-1.13/rapidxml.hpp"
#include "Dependencies/include/rapidxml-1.13/rapidxml_print.hpp"
#include "Dependencies/include/rapidxml-1.13/rapidxml_utils.hpp"
#include "./Dependencies/include/ImGuiFileDialog-Lib_Only/ImGuiFileDialog.h"

#define DEFAULT_WIDTH 1280
#define DEFAULT_HEIGHT 720
#define EPS 0.1
#define PRECISION 1.0f
using namespace rapidxml;
using namespace std;

glm::vec3 cameraPos, cameraFront, cameraUp, lookAt, moving_up;
unsigned int width_, height_;

int e = 0;
glm::mat4 projection, view, model, mvp;
glm::mat4 projection_i, view_i, model_i, mvp_i;
glm::vec2 mousePosOld, angle;
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
bool stereoscopic = false;
bool plain = true;
float ipd = 0.01f;
float d = 0.1f;
float near = 1.0f;
float far = 800.0f;
bool serch_for_intersections_using_cursor = false;
float distance_d = 1.0f;
float speed = 10.0f; // speed of milling tool in milimeters per second;
float size_x = 180;
float size_y = 180;
float size_z = 17;
float drill_size = 0.25f;
int divisions_x = 400;
int divisions_y = 400;
bool drill = false;
bool show_path = true;

int number_of_divisions = 20;
Camera cam;
Shader ourShader;
//std::unique_ptr<Cursor> cursor, center;

std::vector<std::shared_ptr<Object>> objects_list = {};
std::unique_ptr<Point> milling_tool;

void draw_scene();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void transform_screen_coordinates_to_world(glm::vec3& world_coordinates_end, glm::vec3& world_coordinates_start, float x_pos, float y_pos);
void create_gui();
void drill_simulation();

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	GLFWwindow* window = glfwCreateWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, "SimpleCAD", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();
	const char* glsl_version = "#version 330";
	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// build and compile our shader program
	// ------------------------------------
	ourShader = Shader("shader.vs", "shader.fs"); // you can name your shader files however you like
	Shader gridShader = Shader("shader_grid.vs", "shader_grid.fs");

	model = glm::mat4(1.0f);
	view = glm::mat4(1.0f);

	cameraPos = { 0,0,250 };
	cameraFront = { 0,0,-1 };
	lookAt = { 0,0,0 };
	cameraUp = { 0,1,0 };

	angle = { -90.0f, 0.0f };
	width_ = DEFAULT_WIDTH;
	height_ = DEFAULT_HEIGHT;

	cam = Camera(cameraPos, cameraFront, cameraUp);
	cam.SetPerspective(glm::radians(45.0f), DEFAULT_WIDTH / (float)DEFAULT_HEIGHT, near, far);

	glEnable(GL_DEPTH_TEST);

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CW);

	milling_tool = std::make_unique<Point>(glm::vec3(0, 0, 0), ourShader);
	// render loop
	while (!glfwWindowShouldClose(window))
	{
		// cleaning frame
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		projection = cam.GetProjectionMatrix();
		projection_i = glm::inverse(projection);
		view = cam.GetViewMatrix();
		view_i = glm::inverse(view);

		mvp = projection * view;

		processInput(window);
		create_gui();

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		GLint data;
		glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &data);

		draw_scene();

		// Render dear imgui into screen
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// check and call events and swap the buffers
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	// cleanup stuff
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	objects_list.clear();
	ourShader.deleteShader();
	return 0;
}

void draw_scene() {
	for (auto& ob : objects_list) {
		ob->DrawObject(mvp);
	}

	if (!!milling_tool) {
		milling_tool->SetViewPos(cam.getPos());
		milling_tool->DrawObject(mvp);
		if (show_path && !!milling_tool->current_path)
			milling_tool->current_path->DrawObject(mvp);

		drill_simulation();
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	cam.SetPerspective(glm::radians(45.0f), width / (float)height, near, far);
	width_ = width;
	height_ = height;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		for (auto& obj : objects_list) {
			if (obj->selected)
			{
				obj->UnSelect();
			}
			for (auto& virt : obj->GetVirtualObjects()) {
				if (virt->selected)
				{
					virt->UnSelect();
				}
			}
		}
	}
}

void transform_screen_coordinates_to_world(glm::vec3& world_coordinates_end, glm::vec3& world_coordinates_start, float x_pos, float y_pos) {
	glm::vec4 NDC_ray_start(
		((float)x_pos / (float)width_ - 0.5f) * 2.0f,
		-((float)y_pos / (float)height_ - 0.5f) * 2.0f,
		-1.0f,
		1.0f
	);
	glm::vec4 NDC_ray_end(
		((float)x_pos / (float)width_ - 0.5f) * 2.0f,
		-((float)y_pos / (float)height_ - 0.5f) * 2.0f,
		0.0f,
		1.0f
	);

	glm::vec4 lRayStart_camera = projection_i * NDC_ray_start;
	lRayStart_camera /= -lRayStart_camera.w;
	glm::vec4 lRayStart_world = view_i * lRayStart_camera;
	lRayStart_world /= lRayStart_world.w;
	glm::vec4 lRayEnd_camera = projection_i * NDC_ray_end;
	lRayEnd_camera /= -lRayEnd_camera.w;
	glm::vec4 lRayEnd_world = view_i * lRayEnd_camera;
	lRayEnd_world /= lRayEnd_world.w;


	world_coordinates_end = lRayEnd_world;
	world_coordinates_start = lRayStart_world;

}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (ImGui::GetIO().WantCaptureMouse)
		return;
	glm::vec2 mousePos = { xpos,ypos };
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
	{
		glm::vec2 diff = (mousePosOld - mousePos) * PRECISION;
		float cameraSpeed = 5.0f * deltaTime;
		float radius;
		diff *= cameraSpeed;

		glm::vec3 right_movement = cam.GetRightVector() * -diff.x;
		glm::vec3 up_movement = cam.GetUpVector() * diff.y;
		glm::vec3 angle2 = lookAt - (cameraPos + right_movement + up_movement);

		auto rotation = Object::RotationBetweenVectors(lookAt - cameraPos, angle2);
		auto roation = glm::toMat4(rotation);
		//glm::vec3 odn = center->GetPosition();
		//if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		//	switch (e) {
		//	case 0:
		//		odn = center->GetPosition();
		//		break;
		//	case 1:
		//		odn = cursor->GetPosition();
		//		break;
		//	default:
		//		return;
		//		break;
		//	}
		//	for (auto& obj : objects_list) {
		//		if (obj->selected)
		//		{
		//			obj->RotateObject(rotation);
		//			glm::vec4 pos = { obj->GetPosition() - odn, 0.0f };
		//			obj->MoveObjectTo(odn + static_cast<glm::vec3>(roation * pos));
		//		}
		//		for (auto& virt : obj->GetVirtualObjects()) {
		//			if (virt->selected)
		//			{
		//				glm::vec4 pos2 = { virt->GetPosition() - odn, 0.0f };
		//				virt->MoveObjectTo(odn + static_cast<glm::vec3>(roation * pos2));
		//			}
		//		}
		//	}
		//}

		//else {
		angle += diff;
		if (angle.y > 90.0f) angle.y = 90.0f - EPS;
		if (angle.y < -90.0f) angle.y = -90.0f + EPS;
		if (angle.x > 180.0f) angle.x = -180.0f + EPS;
		if (angle.x < -180.0f) angle.x = 180.0f - EPS;
		radius = glm::length(cameraPos - lookAt);

		cameraPos.x = lookAt.x + radius * glm::cos(glm::radians(angle.y)) * glm::cos(glm::radians(angle.x));
		cameraPos.z = lookAt.z + radius * -glm::cos(glm::radians(angle.y)) * glm::sin(glm::radians(angle.x));
		cameraPos.y = lookAt.y + radius * glm::sin(glm::radians(-angle.y));

		cameraFront = glm::normalize(lookAt - cameraPos);
		cam.LookAt(cameraPos, cameraFront, cameraUp);
		//}
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		glm::vec2 diff = (mousePosOld - mousePos) * PRECISION;
		float cameraSpeed = 10.0f * deltaTime;

		glm::vec2 movement = diff * cameraSpeed;

		glm::vec3 right_movement = cam.GetRightVector() * movement.x;
		glm::vec3 up_movement = cam.GetUpVector() * -movement.y;
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
			for (auto& obj : objects_list) {
				if (obj->selected)
					obj->MoveObject(-right_movement + -up_movement);
				for (auto& virt : obj->GetVirtualObjects()) {
					if (virt->selected)
						virt->MoveObject(-right_movement + -up_movement);
				}
			}
		}
		else {
			cameraPos += right_movement + up_movement;
			lookAt += right_movement + up_movement;

			cam.LookAt(cameraPos, cameraFront, cameraUp);
		}
	}
	/*if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT == GLFW_PRESS))
	{
		glm::vec2 diff = (mousePosOld - mousePos) * PRECISION;
		float cameraSpeed = 0.2f * deltaTime;

		glm::vec2 movement = diff * cameraSpeed;

		glm::vec3 right_movement = cam.GetRightVector() * -movement.x;
		glm::vec3 up_movement = cam.GetUpVector() * movement.y;
		cursor->MoveObject(right_movement + up_movement);
	}*/

	mousePosOld = mousePos;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (ImGui::GetIO().WantCaptureMouse)
		return;
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		float minLength = std::numeric_limits<float>::max();

		Object* closest = nullptr;

		glm::vec3 lRayEnd_world;
		glm::vec3 lRayStart_world;
		double x_pos, y_pos;
		glfwGetCursorPos(window, &x_pos, &y_pos);

		//char buf[256];
		//sprintf_s(buf, "x: %f y: %f", x_pos, y_pos);
		//glfwSetWindowTitle(window, buf);

		transform_screen_coordinates_to_world(lRayEnd_world, lRayStart_world, x_pos, y_pos);

		glm::vec3 lRayDir_world(lRayEnd_world - lRayStart_world);
		lRayDir_world = glm::normalize(lRayDir_world);

		for (auto& obj : objects_list) {
			if (dynamic_cast<Point*>(obj.get())) {
				glm::vec3 point = obj->GetPosition();
				glm::vec3 toPoint(point - glm::vec3(lRayStart_world));
				glm::vec3 crossp = glm::cross(lRayDir_world, toPoint);
				float length = glm::length(crossp);
				if (length < minLength && length < 0.1f)
				{
					minLength = length;
					closest = obj.get();
				}
			}
			for (auto& virt : obj->GetVirtualObjects()) {
				auto vr_point = std::dynamic_pointer_cast<Point>(virt);
				if (vr_point) {
					glm::vec3 point2 = vr_point->GetPosition();
					glm::vec3 toPoint2(point2 - glm::vec3(lRayStart_world));
					glm::vec3 crossp2 = glm::cross(lRayDir_world, toPoint2);
					float length = glm::length(crossp2);
					if (length < minLength && length < 0.1f)
					{
						minLength = length;
						closest = vr_point.get();
					}
				}
			}
		}
		if (closest) {
			closest->Select();
		}
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (ImGui::GetIO().WantCaptureMouse)
		return;

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		float precision = 0.01f;

		float movement = 1.0f + yoffset * precision;
		if (movement <= 0.0f)
			movement = 0.1f;
		for (auto& obj : objects_list) {
			if (obj->selected)
				obj->ResizeObject({ movement,movement,movement });
		}
	}
	else {
		float precision = 0.01f;

		float movement = 1.0f - yoffset * precision;
		if (movement <= 0.0f)
			movement = 0.1f;
		//TODO: przerobiæ to na coœ ³adniejszego;
		//cam.ScaleWorld({ movement,movement,movement });
		cameraFront = glm::normalize(lookAt - cameraPos);
		float dist = glm::length(lookAt - cameraPos);
		cameraPos = lookAt - (cameraFront * dist * movement);
		cam.LookAt(cameraPos, cameraFront, cameraUp);
	}

	//cam.Zoom(yoffset * precision);

}

void main_menu() {
	// Menu Bar
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Menu##main"))
		{
			if (ImGui::MenuItem("Open##main", "Ctrl+O")) {
				ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".*", "..\\Paths1\\.");
			}
			if (ImGui::MenuItem("Save##main", "Ctrl+O")) {
				xml_document<char> document;
				xml_node<>* scene = document.allocate_node(node_element, "Scene");
				for (auto& obj : objects_list) {
					obj->Serialize(document, scene);
				}
				document.append_node(scene);

				ofstream myfile;
				myfile.open("example.txt");
				myfile << document;
				myfile.close();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
	// display
	if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
	{
		// action if OK
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			std::string file_to_open = ImGuiFileDialog::Instance()->GetFilePathName();

			std::ifstream infile(file_to_open);
			std::string line;
			float x_prop = 0.0, y_prop = 0.0, z_prop = 0.0;
			milling_tool->current_path = std::make_unique<Line>(ourShader);
			while (std::getline(infile, line))
			{
				float x = 0.0, y = 0.0, z = 0.0;
				int old_num = 0;
				int N_num, G_num;
				char N, G, X, Y, Z;
#pragma warning(suppress : 4996)
				int result = sscanf(line.c_str(), "%c%i%c%i%c%f%c%f%c%f",
					&N, &N_num, &G, &G_num, &X, &x, &Y, &y, &Z, &z);
				if (result < 5 && result % 2 == 1) continue;

				if (X == 'X') x_prop = x;
				else if (X == 'Y') y_prop = x;
				else if (X == 'Z') z_prop = x;

				if (result > 6 && Y == 'Y') y_prop = y;
				else if (result > 6 && Y == 'Z') z_prop = y;

				if (result > 8 && Z == 'Z') z_prop = z;
				//save coordinates;
				glm::vec3 point_in_path = { x_prop , y_prop,z_prop };
				milling_tool->current_path->AddPoint(point_in_path);
			}
			milling_tool->current_path->Reverse();
			milling_tool->MoveObjectTo(milling_tool->current_path->GetFirstPoint());
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}
}

void create_gui() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	//ImGui::ShowDemoWindow();
	bool open;
	ImGuiWindowFlags flags = 0;
	flags |= ImGuiWindowFlags_MenuBar;
	ImGui::Begin("Main Menu##uu", &open, flags);
	main_menu();
	ImGui::Checkbox("Drill", &drill);

	if (ImGui::Button("Drill All")) {
		milling_tool->DrillAll();
	}


	ImGui::InputFloat("Drilling speed", &speed);
	ImGui::InputFloat("Drill size", &drill_size);
	if (ImGui::Button("Set Drill size")) {
		milling_tool->UpdateDrillSize(drill_size);
	}

	ImGui::Checkbox("Show milling path", &show_path);

	ImGui::InputFloat("Size x", &size_x);
	ImGui::InputFloat("Size y", &size_y);
	ImGui::InputFloat("Size z", &size_z);

	ImGui::InputInt("Divisions x", &divisions_x);
	ImGui::InputInt("Divisions y", &divisions_y);
	if (ImGui::Button("Create Material")) {
		milling_tool->InitializeDrilledMaterial({ size_x,size_y,size_z }, { divisions_x,divisions_y });
	}

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();
}

void drill_simulation()
{
	if (!drill) return;
	milling_tool->MoveTool(deltaTime, speed);
}
