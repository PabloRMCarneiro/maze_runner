#include <iostream>
#include <thread>
#include <vector>
#include <fstream>
#include <mutex>
#include <chrono>

using namespace std;

struct Maze
{
  vector<vector<char>> maze;
  int numRows;
  int numCols;
};

mutex outputMutex;

bool is_valid(const Maze &maze, int i, int j)
{
  return (i >= 0 && i < maze.numRows && j >= 0 && j < maze.numCols &&
          maze.maze[i][j] != '#' && maze.maze[i][j] != 'o');
}

void print_maze(const Maze &maze, bool isFound)
{
  lock_guard<mutex> lock(outputMutex);
  system("clear");
  for (int i = 0; i < maze.numRows; ++i)
  {
    for (int j = 0; j < maze.numCols; ++j)
    {
      if (isFound && (maze.maze[i][j] == 'o' || maze.maze[i][j] == 's'))
        cout << "\033[1;32m" << maze.maze[i][j] << "\033[0m";
      else if (isFound && maze.maze[i][j] == '#')
        cout << "\033[31m" << maze.maze[i][j] << "\033[0m";
      else
      {
        if (maze.maze[i][j] == 'o')
          cout << "\033[31m" << maze.maze[i][j] << "\033[0m";
        else
          cout << maze.maze[i][j];
      }
    }
    cout << endl;
  }
}

void walk(Maze &maze, int i, int j, bool &exit_found)
{
  if (maze.maze[i][j] == 's')
  {
    print_maze(maze, true);
    exit_found = true;
    return;
  }
  maze.maze[i][j] = 'o';
  print_maze(maze, false);
  this_thread::sleep_for(chrono::milliseconds(100));
  vector<thread> threads;
  int di[] = {-1, 1, 0, 0};
  int dj[] = {0, 0, -1, 1};
  for (int k = 0; k < 4; ++k)
  {
    int ni = i + di[k];
    int nj = j + dj[k];
    if (is_valid(maze, ni, nj))
    {
      threads.emplace_back(walk, ref(maze), ni, nj, ref(exit_found));
    }
  }
  for (auto &t : threads)
  {
    t.join();
  }
}

Maze load_maze(const char *file_name)
{
  Maze maze;
  ifstream file(file_name);
  if (!file.is_open())
  {
    cerr << "Error: Unable to open file" << endl;
    exit(1);
  }
  file >> maze.numRows >> maze.numCols;
  maze.maze.resize(maze.numRows, vector<char>(maze.numCols));
  for (int i = 0; i < maze.numRows; ++i)
  {
    for (int j = 0; j < maze.numCols; ++j)
    {
      file >> maze.maze[i][j];
    }
  }
  file.close();
  return maze;
}

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    cerr << "Usage: " << argv[0] << " <data_file>" << endl;
    return 1;
  }
  Maze maze = load_maze(argv[1]);
  int start_i, start_j;
  for (int i = 0; i < maze.numRows; ++i)
  {
    for (int j = 0; j < maze.numCols; ++j)
    {
      if (maze.maze[i][j] == 'e')
      {
        start_i = i;
        start_j = j;
        break;
      }
    }
  }
  bool exit_found = false;
  walk(maze, start_i, start_j, exit_found);
  exit_found ? cout << "Exit found!" << endl : cout << "Exit not found!" << endl;
  return 0;
}