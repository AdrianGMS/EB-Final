#include <files.hpp>
#include <model.hpp>
#include <cam.hpp>

const u32 SCR_WIDTH = 1280;
const u32 SCR_HEIGHT = 720;
const f32 ASPECT = (f32)SCR_WIDTH / (f32)SCR_HEIGHT;

Cam* cam;

f32  deltaTime = 0.0f;
f32  lastFrame = 0.0f;
bool wireframe = false;
glm::vec3 PosAvion(0.0f, 0.0f, -2.0f);

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
		PosAvion.y += 0.08f;
	}
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
		PosAvion.y -= 0.08f;
	}
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
		PosAvion.z += 0.08f;
	}
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
		PosAvion.x += 0.08f;
	}
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
		PosAvion.z -= 0.08f;
	}
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
		PosAvion.x -= 0.08f;
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		cam->processKeyboard(FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		cam->processKeyboard(LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		cam->processKeyboard(BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		cam->processKeyboard(RIGHT, deltaTime);
	}
}
void key_callback(GLFWwindow*, int key, int, int act, int) {
	wireframe ^= key == GLFW_KEY_E && act == GLFW_PRESS;
}
void mouse_callback(GLFWwindow* window, f64 xpos, f64 ypos) {
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		cam->movePov(xpos, ypos);
	}
	else {
		cam->stopPov();
	}
}
void scroll_callback(GLFWwindow*, f64, f64 yoffset) {
	cam->processScroll((f32)yoffset);
}

i32 main() {
	GLFWwindow* window = glutilInit(3, 3, SCR_WIDTH, SCR_HEIGHT, "Cubito");
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	cam = new Cam();
	//PATH de texturas y objetos
	Files* files = new Files("bin", "resources/textures", "resources/objects");
	
	//Ruta de shaders
	Shader* objShader = new Shader(files, "shader.vert", "shader.frag");
	Shader* rockShader = new Shader(files, "shader.vert", "shader.frag");

	//Ruta de objetos
	Model* airplane = new Model(files, "airplane2/piper_pa18.obj");
	Model* asteroid = new Model(files, "rock/rock.obj");

	u32 amount = 300;
	glm::mat4* models = new glm::mat4[amount];
	srand(glfwGetTime());
	f32 radius = 3.5f;
	f32 offset = 2.5f;
	for (u32 i = 0; i < amount; ++i) {
		glm::mat4 model = glm::mat4(1.0f);
		f32 angle = (f32)i / (f32)amount * 360.0f;
		f32 displacement = (rand() % (i32)(2 * offset * 100)) / 100.0f - offset;
		f32 x = sin(angle) * radius + displacement;
		displacement = (rand() % (i32)(2 * offset * 100)) / 100.0f - offset;
		f32 y = displacement * 0.4f;
		displacement = (rand() % (i32)(2 * offset * 100)) / 100.0f - offset;
		f32 z = cos(angle) * radius + displacement;
		model = glm::translate(model, { x,y,z });

		f32 scale = (rand() % 10) / 100 + 0.05;
		model = glm::scale(model, glm::vec3(scale));

		f32 rotAngle = (rand() % 360);
		model = glm::rotate(model, rotAngle, { 0.4,0.6,0.8 });

		models[i] = model;
		//std::cout << models[i] << std::endl;
	}

	glm::vec3 lightPos = glm::vec3(1.0f);
	glm::vec3 lightColor = glm::vec3(1.0f);

	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window)) {
		f32 currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(window);
		glClearColor(0.1f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);

		objShader->use();
		lightPos.x = 2.0f * (cos(currentFrame) - sin(currentFrame));
		lightPos.z = 2.0f * (cos(currentFrame) + sin(currentFrame));
		objShader->setVec3("xyz", lightPos);
		objShader->setVec3("xyzColor", lightColor);
		objShader->setVec3("xyzView", cam->pos);
		glm::mat4 proj = glm::perspective(cam->zoom, ASPECT, 0.1f, 100.0f);
		objShader->setMat4("proj", proj);
		objShader->setMat4("view", cam->getViewM4());

		glm::mat4 model = glm::mat4(1.0f);
		model = translate(model, PosAvion);
		model = glm::scale(model, glm::vec3(0.08f));
		objShader->setMat4("model", model);
		airplane->Draw(objShader);

	
		rockShader->use();
		rockShader->setVec3("xyz", lightPos);
		rockShader->setVec3("xyzColor", lightColor);
		rockShader->setVec3("xyzView", cam->pos);
		rockShader->setMat4("proj", proj);
		rockShader->setMat4("view", cam->getViewM4());
		for (u32 i = 0; i < amount; ++i) {
			models[i] = translate(models[i], glm::vec3(0.07f, 0.0f, 0.09f));
			models[i] = glm::rotate(models[i], 0.05f, glm::vec3(0.0f, (2.0f + (sin(currentFrame)/2) * 2), 0.0f));
			rockShader->setMat4("model", models[i]);
			asteroid->Draw(rockShader);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	delete cam;
	delete objShader;
	delete airplane;
	delete asteroid;

	return 0;
}
