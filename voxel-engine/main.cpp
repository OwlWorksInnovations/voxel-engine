#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

const char* vertexShaderSource = "#version 330 core\n"
	"layout (location = 0) in vec3 aPos;\n"
	"void main()\n"
	"{\n"
	"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
	"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
	"out vec4 FragColor;\n"
	"void main()\n"
	"{\n"
	"	FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);"
	"}\0";

const char* fragmentShaderSource2 = "#version 330 core\n"
	"out vec4 FragColor;\n"
	"void main()\n"
	"{\n"
	"	FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);"
	"}\0";

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void process_input(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

int main() {
	// Initialize glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create window
	GLFWwindow* window = glfwCreateWindow(800, 600, "Voxel Engine", NULL, NULL);
	if (window == NULL) {
		std::cout << "Window failed to create! :(" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD! :(" << std::endl;
		return -1;
	}

	// Set up viewport and handle resize
	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Set up shaders
	// Vertex shader
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// Check if vertex shader compiled
	int  vertexSuccess;
	char vertexInfoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexSuccess);
	if (!vertexSuccess)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, vertexInfoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED :(\n" << vertexInfoLog << std::endl;
	}
	// Fragment shader
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// Check if fragment shader compiled
	int  fragmentSuccess;
	char fragmentInfoLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragmentSuccess);
	if (!fragmentSuccess)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, fragmentInfoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED :(\n" << fragmentInfoLog << std::endl;
	}
	unsigned int fragmentShader2;
	fragmentShader2 = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader2, 1, &fragmentShaderSource2, NULL);
	glCompileShader(fragmentShader2);
	// Check if fragment shader compiled
	int  fragmentSuccess2;
	char fragmentInfoLog2[512];
	glGetShaderiv(fragmentShader2, GL_COMPILE_STATUS, &fragmentSuccess2);
	if (!fragmentSuccess2)
	{
		glGetShaderInfoLog(fragmentShader2, 512, NULL, fragmentInfoLog2);
		std::cout << "ERROR::SHADER::FRAGMENT2::COMPILATION_FAILED :(\n" << fragmentInfoLog2 << std::endl;
	}

	// Shader program (links all shaders)
	unsigned int shaderProgram;
	int success;
	char infoLog[512];
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR:SHADER:SHADERPROGRAM::LINKING_FAILED :(\n" << infoLog << std::endl;
	}
	glDeleteShader(fragmentShader);

	unsigned int shaderProgram2;
	int success2;
	char infoLog2[512];
	shaderProgram2 = glCreateProgram();
	glAttachShader(shaderProgram2, vertexShader);
	glAttachShader(shaderProgram2, fragmentShader2);
	glLinkProgram(shaderProgram2);
	glGetProgramiv(shaderProgram2, GL_LINK_STATUS, &success2);
	if (!success2) {
		glGetProgramInfoLog(shaderProgram2, 512, NULL, infoLog2);
		std::cout << "ERROR:SHADER:SHADERPROGRAM::LINKING_FAILED :(\n" << infoLog2 << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader2);

	// Triangle
	float square1Vertices[] = {
		-0.25f,  0.25f, 0.0f,  // top right
		-0.25f, -0.25f, 0.0f,  // bottom right
		-0.75f,  0.25f, 0.0f,  // top left 
		-0.75f, -0.25f, 0.0f,  // bottom left
	};

	float square2Vertices[] = {
		0.75f, 0.25f, 0.0f,  // top right
		0.75f, -0.25f, 0.0f,  // bottom right
		0.25f, 0.25f, 0.0f,  // top left 
		0.25f, -0.25f, 0.0f,  // bottom left
	};

	unsigned int square1Indices[] = {
		0, 1, 3,
		0, 2, 3,
	};

	unsigned int square2Indices[] = {
		0, 1, 3,
		0, 2, 3,
	};

	// Set up VBO (Vertex Buffer Object), VAO (Vertex Array Object), EBO (Element Buffer Object)
	unsigned int VBO1, VBO2, VAO1, VAO2, EBO1, EBO2;
	glGenVertexArrays(1, &VAO1);
	glGenVertexArrays(1, &VAO2);
	glGenBuffers(1, &VBO1);
	glGenBuffers(1, &VBO2);
	glGenBuffers(1, &EBO1);
	glGenBuffers(1, &EBO2);

	glBindVertexArray(VAO1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(square1Vertices), square1Vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(square1Indices), square1Indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(VAO2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(square2Vertices), square2Vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(square2Indices), square2Indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Render loop
	while (!glfwWindowShouldClose(window)) {
		// Input
		process_input(window);
		// Rendering starts here
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Draw triangle
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO1);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glUseProgram(shaderProgram2);
		glBindVertexArray(VAO2);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// Rendering ends here
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Cleanup and exit
	glDeleteVertexArrays(1, &VAO1);
	glDeleteVertexArrays(1, &VAO2);
	glDeleteBuffers(1, &VBO1);
	glDeleteBuffers(1, &VBO2);
	glDeleteProgram(shaderProgram);
	glDeleteProgram(shaderProgram2);
	glfwTerminate();
	return 0;
}