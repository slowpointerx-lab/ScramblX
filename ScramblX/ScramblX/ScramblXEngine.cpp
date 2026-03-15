#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#include <iostream>

#include "Renderer/ShaderClass.h"
#include "Renderer/Model.h"
#include "Core/Camera.h"

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

Camera camera(glm::vec3(0.0f, 1.0f, 5.0f));
float lastX = SCR_WIDTH / 2, lastY = SCR_HEIGHT / 2;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = ypos - lastY;

	lastX = xpos;
	lastY = ypos;

	camera.PocessMouseMovement(xoffset, yoffset);
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.ProcessKeybord(FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.ProcessKeybord(BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.ProcessKeybord(LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.ProcessKeybord(RIGHT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		camera.ProcessKeybord(UP, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		camera.ProcessKeybord(DOWN, deltaTime);
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 8); // 4 samples is the industry standard

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "ScramblX", NULL, NULL);

	if (!window) {
		std::cout << "Failed to create window\n" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD\n";
		return -1;
	}

	glEnable(GL_MULTISAMPLE);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shader shader("Assets/Shaders/model.vert", "Assets/Shaders/model.frag");

	Model ferarri("Assets/Models/ferrari-laferrari-wwwvecarzcom/source/ferrari_laferrari.glb");
	Model koenigsegg("Assets/Models/2020-koenigsegg-jesko/source/Final_Model/Final_Model.fbx");
	Model lionHead("Assets/Models/lion_head_4k/lion_head_4k.gltf");

	glDepthFunc(GL_LESS);

	while (!glfwWindowShouldClose(window)) {
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_CULL_FACE);

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		shader.use();

		shader.setVec3("viewPos", camera.Position);
		shader.setVec3("lightPos", glm::vec3(10.0f * sin(glfwGetTime()), 0.0f, 10.0f * cos(glfwGetTime())));
		shader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(
			glm::radians(camera.Zoom),
			(float)SCR_WIDTH / SCR_HEIGHT,
			0.1f,
			100000.0f
		);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);

		glm::mat4 car1pos = glm::mat4(1.0f);
		car1pos = glm::translate(car1pos, glm::vec3(2.0f, 0.0f, 0.0f));
		car1pos = glm::rotate(car1pos, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		koenigsegg.Draw(shader.ID, car1pos);

		glm::mat4 car2pos = glm::mat4(1.0f);
		car2pos = glm::translate(car2pos, glm::vec3(-2.0f, 0.0f, 0.0f));
		ferarri.Draw(shader.ID, car2pos);

		glm::mat4 lionpos = glm::mat4(1.0f);
		lionpos = glm::translate(lionpos, glm::vec3(0.0f, 2.0f, 0.0f));
		lionHead.Draw(shader.ID, lionpos);


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
}