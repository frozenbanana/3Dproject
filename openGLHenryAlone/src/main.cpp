#include <iostream>
#include <GL\glew.h>
#include "Display.h"
#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"
#include "Texture.h"
#include "Camera.h"

#define WIDTH 800
#define HEIGHT 600

int main(int argc, char ** argv)
{
	Display display(WIDTH, HEIGHT, "Hello World");

	Mesh mesh2("./objects/monkey.obj");

	Shader shader("basicShader");
	Texture texture("./textures/bricks.jpg");
	Camera camera(glm::vec3(0, 0, -3), 70.f, (float)WIDTH / (float)HEIGHT, 0.01f, 1000.f);
	Transform transform;

	float counter = 0.f;
	while (!display.IsClosed())
	{
		display.Clear(1.f, 0.9f, 0.1f, 1.0f);
		transform.GetRot().y = counter;

		shader.Bind();
		texture.Bind(0);
		shader.Update(transform, camera);
		mesh2.Draw();

		display.Update();
		counter += 0.001f;
	}
	return 0;
}