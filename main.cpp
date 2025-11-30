#include <iostream>
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

// vector2 randomGradient(int x, int y)
// {
//   const unsigned w = 8 * sizeof(unsigned);
//   const unsigned s = w / 2;
//   unsigned a = x, b = y;
//   a *= 3284157443;

//   b ^= a << s | a >> w - s;
//   b *= 1911520717;

//   a ^= b << s | b >> w - s;
//   a *= 2048419325;
//   float random = a * (3.14159565 / ~(~0u >> 1));

//   vector2 v;
//   v.x = sin(random);
//   v.y = cos(random);

//   return v;
// }

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

  float n0 = dotGridGradient(x0, y0, x, y);
  float n1 = dotGridGradient(x1, y0, x, y);
  float ix0 = interpolate(n0, n1, nx);

  n0 = dotGridGradient(x1, y1, x, y);
  n1 = dotGridGradient(x0, y1, x, y);
  float ix1 = interpolate(n0, n1, nx);

  float value = interpolate(ix0, ix1, ny);
  return value;
}

int main()
{
  std::srand(std::time(0));

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
      // std::cout << noise << std::endl;
      pixels[i][j] = noise;
    }
  }
  std::cout << std::endl;

  for (int i = 0; i < SCR_WIDTH; i++)
  {
    for (int j = 0; j < SCR_WIDTH; j++)
      std::cout << pixels[i][j] << " ";
    std::cout << std::endl;
  }

  return 0;
}