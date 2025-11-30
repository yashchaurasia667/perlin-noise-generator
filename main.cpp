#include <cstdlib>
#include <cmath>
#include <ctime>
#include <iostream>

const unsigned int SCR_WIDTH = 800, CELL_WIDTH = 100, CELLS = SCR_WIDTH / CELL_WIDTH, TRANSLATOR = SCR_WIDTH / CELLS;
int vertex_angles[CELLS][CELLS];
float color_map[SCR_WIDTH][SCR_WIDTH];

typedef struct
{
  float x, y;
} vector2;

int generateRandomAngles(int x, int y)
{
  return std::rand() % 360;
}

vector2 translatePixelToCoordinate(int x, int y)
{
  vector2 coord;
  coord.x = x / TRANSLATOR;
  coord.y = y / TRANSLATOR;
  return coord;
}

// float interpolateCubic(vector2 c1, vector2 c2, vector2 c3)
// {
//   t =
// }

float perlin(int x, int y)
{
  vector2 coords = translatePixelToCoordinate(x, y);
  // get bounding vectors
  int x0 = floor(coords.x) - 1;
  int y0 = floor(coords.y) - 1;
  int x1 = x0 + 1;
  int y1 = y0 + 1;

  // normalize the point to between 0 & 1 and calculate the dot products
  float dot_prod[4];
  vector2 normalized = {x0 - x, y0 - y};
  dot_prod[0] = normalized.x * cos(vertex_angles[x0][y0]) + normalized.y * sin(vertex_angles[x0][y0]);
  normalized = {(float)x1 - x, (float)y0 - y};
  dot_prod[1] = normalized.x * cos(vertex_angles[x1][y0]) + normalized.y * sin(vertex_angles[x1][y0]);
  normalized = {(float)x1 - x, (float)y1 - y};
  dot_prod[2] = normalized.x * cos(vertex_angles[x1][y1]) + normalized.y * sin(vertex_angles[x1][y1]);
  normalized = {(float)x0 - x, (float)y1 - y};
  dot_prod[3] = normalized.x * cos(vertex_angles[x0][y1]) + normalized.y * sin(vertex_angles[x0][y1]);

  // linearly interpolate bottom 2 dot products (d0, d1) and the top 2 dot products
  float percent = x - x0;
  float bot_interp = (1.0 - percent) * dot_prod[0] + (percent * dot_prod[1]);
  float top_interp = (1.0 - percent) * dot_prod[2] + (percent * dot_prod[3]);

  // interpolate the result from previous step
  percent = y - y0;
  float final_interp = (1.0 - percent) * bot_interp + (percent * top_interp);

  return final_interp;
}

int main()
{
  std::srand(std::time(0));

  for (int i = 0; i < CELLS; i++)
    for (int j = 0; j < CELLS; j++)
      vertex_angles[i][j] = generateRandomAngles(i, j);

  // print the randomly generated vectors
  for (int i = 0; i < CELLS; i++)
  {
    for (int j = 0; j < CELLS; j++)
      std::cout << vertex_angles[i][j] << " ";
    std::cout << std::endl;
  }

  // run the perlin noise function for every pixel
  for (int i = 0; i < SCR_WIDTH; i++)
  {
    for (int j = 0; j < SCR_WIDTH; j++)
    {
      color_map[i][j] = perlin(i, j);
    }
  }

  for (int i = 0; i < SCR_WIDTH; i++)
  {
    for (int j = 0; j < SCR_WIDTH; j++)
      std::cout << color_map[i][j] << " ";
    std::cout << std::endl;
  }

  return 0;
}