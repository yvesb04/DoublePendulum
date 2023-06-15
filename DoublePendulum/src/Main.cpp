#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>

constexpr auto PI = 3.14159265358979323846;


void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void HSVToRGB(float h, float s, float v, float& r, float& g, float& b);

//***VARIABLES****
//Initial values are assigned to these variables here.
float theta_1 = 3.0f;
float omega_1 = 0;
float theta_2 = 0.7;
float omega_2 = 0;
float m1 = 2;
float m2 = 2; //masses of pendulums
float L1 = 1;
float L2 = 1; //rod lengths
float g = 9.81; //acceleration due to gravity
float h = 0.025;


float k1, k2, k3, k4, l1, l2, l3, l4;  //variables for Runge-Kutta  

float f_1(float omg) {
	return omg;
}

float g_1(float theta1, float theta2, float omg1, float omg2) {
	return ((m2 * g * sin(theta2) * cos(theta1 - theta2))
		- (m2 * sin(theta1 - theta2) * (L1 * (omg1 * omg1) * cos(theta1 - theta2) + (L2 * (omg2 * omg2))))
		- ((m1 + m2) * g * sin(theta1))) / (L1 * (m1 + (m2 * sin(theta1 - theta2) * sin(theta1 - theta2))));
}

//***PENDULUM 2****

float p1, p2, p3, p4, q1, q2, q3, q4;   //variables for Runge-Kutta

float f_2(float omg) {
	return omg;
}

float g_2(float theta1, float theta2, float omg1, float omg2) {
	return  ((m1 + m2) * (L1 * (omg1 * omg1) * sin(theta1 - theta2)
		- (g * sin(theta2)) + (g * sin(theta1) * cos(theta1 - theta2)))
		+ (m2 * L2 * omg2 * omg2 * sin(theta1 - theta2) * cos(theta1 - theta2))) / (L2 * (m1 + (m2 * sin(theta1 - theta2) * sin(theta1 - theta2))));
}


int main()
{
	//GLFW Initialization and Configuration
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
	//GLFW Window Creation
	GLFWwindow* window = glfwCreateWindow(800, 800, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//GLAD Load OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	std::cout << glGetString(GL_VERSION) << std::endl;

	int flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(glDebugOutput, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}


	// BUILD AND COMPILE SHADER PROGRAM
	// --------------------------------
	Shader ourShader("res/shader.vert", "res/shader.frag");

	struct Vertex {
		float position[3];
		float color[3];
	};

	struct Pendulum {
		Vertex vertices[4] {
			{{ -0.01f, 0.0f, 0.0f },	{1.0f, 1.0f, 1.0f}}, // First vertex position (x, y, z) and color (r, g, b)
			{{	0.01f, 0.0f, 0.0f },	{1.0f, 1.0f, 1.0f}}, // Second vertex position (x, y, z) and color (r, g, b)
			{{  0.01f, 0.4f, 0.0f },	{1.0f, 1.0f, 1.0f}}, // Third vertex position (x, y, z) and color (r, g, b)
			{{ -0.01f, 0.4f, 0.0f },	{1.0f, 1.0f, 1.0f}}, // Fourth vertex position (x, y, z) and color (r, g, b)
		};
		unsigned int indices[6] {
			0, 1, 2, // First triangle (bottom left - top left - top right)
			2, 3, 0  // Second triangle (bottom left - top right - bottom right)
		};
	};

	int const numPendulums = 10;
	Pendulum pendulums[numPendulums]{}; // 4 vertices per rod

	// Fill the vertex data with positions and colors
	// Each triangle has a slightly different color
	// Fill the vertex data with positions and colors

	for (int i = 0; i < numPendulums; i++) {
		float hue = static_cast<float>(i) / numPendulums; // Vary hue from 0.0 to 1.0
		std::cout << hue << std::endl;
		// Convert HSV to RGB
		float r, g, b;
		float width = 2.0f / numPendulums;
		HSVToRGB(hue, 1.0f, 1.0f, r, g, b);

		// Vertices of the triangle with the same color
		pendulums[i].vertices[0].

		vertices[i * 4 + 0].color[0] = r;      // First vertex color (r)
		vertices[i * 4 + 0].color[1] = g;      // First vertex color (g)
		vertices[i * 4 + 0].color[2] = b;      // First vertex color (b)


		vertices[i * 4 + 1].color[0] = r;      // Second vertex color (r)
		vertices[i * 4 + 1].color[1] = g;      // Second vertex color (g)
		vertices[i * 4 + 1].color[2] = b;      // Second vertex color (b)


		vertices[i * 4 + 2].color[0] = r;      // Third vertex color (r)
		vertices[i * 4 + 2].color[1] = g;      // Third vertex color (g)
		vertices[i * 4 + 2].color[2] = b;      // Third vertex color (b)


		vertices[i * 4 + 3].color[0] = r;      // Fourth vertex color (r)
		vertices[i * 4 + 3].color[1] = g;      // Fourth vertex color (g)
		vertices[i * 4 + 3].color[2] = b;      // Fourth vertex color (b)

		unsigned int baseIndex = i * 4;
		indices[i * 6] = baseIndex;
		indices[i * 6 + 1] = baseIndex + 1;
		indices[i * 6 + 2] = baseIndex + 2;
		indices[i * 6 + 3] = baseIndex + 2;
		indices[i * 6 + 4] = baseIndex + 3;
		indices[i * 6 + 5] = baseIndex;

		std::cout << "done!";


	}


	unsigned int VBO, VAO, EBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO); //Bind VAO first, then bind and set VBOs and then configure vertex attributes

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, position)));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, color)));


	ourShader.use();



	//Render Loop
	//------------------------------------
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.14f, 0.15f, 0.18f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//Runge-Kutta 4 starts here.
		k1 = h * f_1(omega_1);
		l1 = h * g_1(theta_1, theta_2, omega_1, omega_2);
		p1 = h * f_2(omega_2);
		q1 = h * g_2(theta_1, theta_2, omega_1, omega_2);

		k2 = h * f_1(omega_1 + (0.5 * l1));
		l2 = h * g_1(theta_1 + (0.5 * k1), theta_2 + (0.5 * p1), omega_1 + (0.5 * l1), omega_2 + (0.5 * q1));
		p2 = h * f_2(omega_2 + (0.5 * q1));
		q2 = h * g_2(theta_1 + (0.5 * k1), theta_2 + (0.5 * p1), omega_1 + (0.5 * l1), omega_2 + (0.5 * q1));

		k3 = h * f_1(omega_1 + (0.5 * l2));
		l3 = h * g_1(theta_1 + (0.5 * k2), theta_2 + (0.5 * p2), omega_1 + (0.5 * l2), omega_2 + (0.5 * q2));
		p3 = h * f_2(omega_2 + (0.5 * q2));
		q3 = h * g_2(theta_1 + (0.5 * k2), theta_2 + (0.5 * p2), omega_1 + (0.5 * l2), omega_2 + (0.5 * q2));

		k4 = h * f_1(omega_1 + l3);
		l4 = h * g_1(theta_1 + k3, theta_2 + p3, omega_1 + l3, omega_2 + q3);
		p4 = h * f_2(omega_2 + q3);
		q4 = h * g_2(theta_1 + k3, theta_2 + p3, omega_1 + l3, omega_2 + q3);

		theta_1 = theta_1 + (k1 + (2 * k2) + (2 * k3) + k4) / 6;
		omega_1 = omega_1 + (l1 + (2 * l2) + (2 * l3) + l4) / 6;
		//Below two lines keep the theta_1 in range of -2PI to 2PI.
		if (theta_1 > 2 * PI) theta_1 = theta_1 - (2 * PI);
		if (theta_1 < -2 * PI) theta_1 = theta_1 + (2 * PI);


		theta_2 = theta_2 + (p1 + (2 * p2) + (2 * p3) + p4) / 6;
		omega_2 = omega_2 + (q1 + (2 * q2) + (2 * q3) + q4) / 6;
		//Below two lines keep the theta_2 in range of -2PI to 2PI.
		if (theta_2 > 2 * PI) theta_2 = theta_2 - (2 * PI);
		if (theta_2 < -2 * PI) theta_2 = theta_2 + (2 * PI);

		pendulum1.angle = theta_1 * 180 / PI; //converts theta_1 from radian to degree

		glBindVertexArray(VAO);
	

		glDrawElements(GL_TRIANGLES, numPendulums * 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	glfwTerminate();
	return 0;                   
}

void HSVToRGB(float h, float s, float v, float& r, float& g, float& b) {
	int i = static_cast<int>(h * 6.0f);
	float f = h * 6.0f - i;
	float p = v * (1.0f - s);
	float q = v * (1.0f - f * s);
	float t = v * (1.0f - (1.0f - f) * s);

	switch (i % 6) {
	case 0: r = v; g = t; b = p; break;
	case 1: r = q; g = v; b = p; break;
	case 2: r = p; g = v; b = t; break;
	case 3: r = p; g = q; b = v; break;
	case 4: r = t; g = p; b = v; break;
	case 5: r = v; g = p; b = q; break;
	}
}

//GLFW Whenever window size changes this callback executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

//Process any input (keys pressed etc)
GLboolean wireframe = GL_FALSE;
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	static bool lock = 0, wf_mode = 0;
	if (!glfwGetKey(window, GLFW_KEY_W)) {
		lock = 0;
	}
	if (glfwGetKey(window, GLFW_KEY_W) && lock == 0) {
		glPolygonMode(GL_FRONT_AND_BACK, (wf_mode = 1 - wf_mode) ? GL_LINE : GL_FILL);
		lock = 1;
	}
}

void APIENTRY glDebugOutput(GLenum source,
	GLenum type,
	unsigned int id,
	GLenum severity,
	GLsizei length,
	const char* message,
	const void* userParam)
{
	// ignore non-significant error/warning codes
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

	std::cout << "---------------" << std::endl;
	std::cout << "Debug message (" << id << "): " << message << std::endl;

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
	case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
	} std::cout << std::endl;

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
	case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
	} std::cout << std::endl;

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
	case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
	case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
	} std::cout << std::endl;
	std::cout << std::endl;
}