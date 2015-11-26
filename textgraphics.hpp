#ifndef TEXTGRAPHICS_HPP
#define TEXTGRAPHICS_HPP
#include <vector>
#include <string>
#include <chrono>
#include <climits>
#include "pixel.hpp"

class Textgrafs{
public:
  Textgrafs();
  ~Textgrafs();
  //void add_row(std::std::string text, int px, int py); //Add row of text. Show with print or paint
  //void add_col(std::string text, int px , int py);
  //void add_rect(char letter, int px, int py, int sizex, int sizey);
  void add_border(const Pixel &, int px, int py, int sizex, int sizey);
  //void add_rect_unique(const std::vector<std::string> & shape , int px, int py);
  //void add_ellipse(char letter, int px, int py, int rx, int ry); //Broken
  void add_pixel(const Pixel & , int, int);
  void add_image(std::vector<std::vector<std::string>> & , int, int);
  void print(); //Prints one grid
  //void print_img(std::vector<std::string>&, int px, int py, int max_size_y = INT_MAX, int min_size_y = 0);
  bool next_tick(); //Should be private
  void paint(); //For continuous animation
  void clear_screen(); //Empty screen
  void hide_cursor();
  void cursorpos(int, int); //Set cursorpos on screen
  std::string cursorpos_str(int, int);
  void clear_grid(); 
  int rows_;
  int cols_;
private:

  void save_old_grid();
  std::chrono::system_clock::time_point timer_;
  bool debug = false;
  double time_between_frames_ = 0.005;
  std::vector<std::vector<std::string>> grid;
  std::vector<std::vector<std::string>> old_grid;
};

#endif
