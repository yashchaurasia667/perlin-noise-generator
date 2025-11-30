#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <shader.h>
#include <random>
#include <cmath>
#include <ctime>

const unsigned int SCR_WIDTH = 800, CELL_WIDTH = 100, CELLS = SCR_WIDTH / CELL_WIDTH, TRANSLATOR = SCR_WIDTH / CELLS;
typedef struct
{
  float x, y;
} vector2;
// #define M_PI 3.14159265

vector2 vertex_angles[CELLS][CELLS];
float pixels[SCR_WIDTH][SCR_WIDTH];

void draw(unsigned char *rgb, unsigned int side)
{
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  GLFWwindow *window = glfwCreateWindow(side, side, "perlin noise", NULL, NULL);
  if (window == NULL)
  {
    std::cout << "Failed to create a glfw window" << std::endl;
    return;
  }

  glfwMakeContextCurrent(window);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return;
  }

  Shader shader("../shaders/vertex.glsl", "../shaders/fragment.glsl");

  float quadVerts[] = {
      // positions    // texcoords
      -1.f, 1.f, 0.f, 1.f,
      -1.f, -1.f, 0.f, 0.f,
      1.f, -1.f, 1.f, 0.f,

      -1.f, 1.f, 0.f, 1.f,
      1.f, -1.f, 1.f, 0.f,
      1.f, 1.f, 1.f, 1.f};

  GLuint VAO, VBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  GLuint tex;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, side, side, 0, GL_RGB, GL_UNSIGNED_BYTE, rgb);

  shader.use();
  shader.setInt("tex", 0); // IMPORTANT

  // Render loop
  while (!glfwWindowShouldClose(window))
  {
    glClear(GL_COLOR_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, tex);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glfwSwapBuffers(window); // IMPORTANT
    glfwPollEvents();        // IMPORTANT
  }

  glDeleteTextures(1, &tex);
  glDeleteBuffers(1, &VBO);
  glDeleteVertexArrays(1, &VAO);
}

vector2 generateRandomAngles(int x, int y)
{
  static std::mt19937 rng(std::random_device{}());
  static std::uniform_real_distribution<float> dist(0.0f, 2.0f * M_PI);
  float angle = dist(rng);
  return vector2{(float)sin(angle), (float)cos(angle)};
}

vector2 translatePixelToCoordinate(int x, int y)
{
  vector2 coord;
  coord.x = (float)x / (float)TRANSLATOR;
  coord.y = (float)y / (float)TRANSLATOR;
  return coord;
}

float dotGridGradient(int ix, int iy, float x, float y)
{
  vector2 gradient = vertex_angles[ix][iy];
  float dx = x - (float)ix;
  float dy = y - (float)iy;
  return (dx * gradient.x + dy * gradient.y);
}

float interpolate(float a0, float a1, float w)
{
  return (a1 - a0) * (3.0f - w * 2.0f) * w * w + a0;
}

float perlin(float x, float y)
{
  int x0 = (int)x;
  int y0 = (int)y;
  int x1 = x0 + 1;
  int y1 = y0 + 1;

  float nx = x - (float)x0;
  float ny = y - (float)y0;

  float n00 = dotGridGradient(x0, y0, x, y);
  float n10 = dotGridGradient(x1, y0, x, y);
  float ix0 = interpolate(n00, n10, nx);

  float n01 = dotGridGradient(x0, y1, x, y);
  float n11 = dotGridGradient(x1, y1, x, y);
  float ix1 = interpolate(n01, n11, nx);

  float value = interpolate(ix0, ix1, ny);
  return value;
}

int main()
{
  std::srand(glfwGetTime());

  for (int i = 0; i < CELLS; i++)
    for (int j = 0; j < CELLS; j++)
      vertex_angles[i][j] = generateRandomAngles(i, j);

  // for (int i = 40; i < 800; i += 57)
  // {
  //   vector2 coords = translatePixelToCoordinate(i, i);
  //   float noise = perlin(coords.x, coords.y);
  //   std::cout << noise << std::endl;
  // }

  // run the perlin noise function for every pixel
  for (int i = 0; i < SCR_WIDTH; i++)
  {
    for (int j = 0; j < SCR_WIDTH; j++)
    {
      vector2 coords = translatePixelToCoordinate(i, j);
      float noise = perlin(coords.x, coords.y);
      noise += 1.0f;
      noise /= 2.0f;
      // std::cout << noise << std::endl;
      pixels[i][j] = noise;
    }
  }
  std::cout << std::endl;
  unsigned char *rgb = new unsigned char[SCR_WIDTH * SCR_WIDTH * 3];
  for (int y = 0; y < SCR_WIDTH; y++)
  {
    for (int x = 0; x < SCR_WIDTH; x++)
    {
      float v = pixels[y][x]; // 0.0 → 1.0
      unsigned char c = (unsigned char)(v * 255.0f);

      int idx = (y * SCR_WIDTH + x) * 3;
      rgb[idx + 0] = c; // R
      rgb[idx + 1] = c; // G
      rgb[idx + 2] = c; // B
    }
  }

  draw(rgb, SCR_WIDTH);
  return 0;
}