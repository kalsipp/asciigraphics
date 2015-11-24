#include <iostream>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <chrono>
#include <ios>
#include <fstream>
#include <stdio.h>
#include <unistd.h>
#include "textgraphics.hpp"

//#### Special member functions ####
//#### Constructor
Textgrafs::Textgrafs(){ 
  std::cout << std::unitbuf; //Turns off the buffer
  std::ios_base::sync_with_stdio(false); //Turn off sync with in stream

  struct winsize w; //Get size of window
  ioctl(0, TIOCGWINSZ, &w);
  rows_ = w.ws_row;
  cols_ = w.ws_col;

  //Prepare grid
  std::string s(cols_, ' ');
  grid.resize(rows_,s);
  old_grid.resize(rows_,s);

  timer_ = std::chrono::system_clock::now(); //First timepoint
  
}
//#### Destructor
Textgrafs::~Textgrafs(){
  //Actually don't need one right now
}

//#### Member functions
void Textgrafs::paint(){
  //Used for continuous update
  //Refer to print for manual control
  if(next_tick()){ //Limit framerate
    print(); //Print out the current grid
    //save_old_grid();
  }
  clear_grid(); //This empties the grid. You need to enter what you painted every frame
}


void Textgrafs::cursorpos(int px, int py){
  ++px; //escape is 1 base
  ++py;
  std::cout << "\033[" << py << ";" << px << "H";
}
std::string Textgrafs::cursorpos_str(int px, int py){
  ++px;
  ++py;
  std::string s = "\033[" + std::to_string(px) + ";" + std::to_string(py) + "H";
  return s;
}
void Textgrafs::hide_cursor(){
}
void Textgrafs::clear(){
  std::cout << "\033[2J";
}
void Textgrafs::save_old_grid(){
  for(unsigned int i = 0; i < grid.size() ; ++i){
    if(0!=grid[i].compare(old_grid[i])){
      old_grid[i] = grid[i];
    }
  }
}
void Textgrafs::print(){
    std::string s;
 
    
    cursorpos(0,0);
    for(unsigned int y = 0; y < rows_ ; ++y){ //Generate the full grid
          s+= grid[y];
	  s+= "\n";
    }
    s.pop_back(); //Remove last \n
    //write(1, s.c_str(), s.length()); //works 
    
    std::cout << s;//works
    
    //printf(s.c_str()); //bad
}

void Textgrafs::print_img(std::vector<std::string> & img_ref, int px, int py, int max_size_y, int min_size_y){
  //Dealing with scrap input
  if(img_ref.size() <=0) return; 
  if(img_ref[0].length() <= 0) return;
  //If outside screen skip it
  if(px >= cols_) return;
  if(py >= rows_) return;
  if(px + img_ref[0].length() <= 0) return;
  if(py + img_ref.size() <= 0) return;
  if(max_size_y <= 0) return;
  if(min_size_y < 0) return;
  if(max_size_y <= min_size_y) return;
 //This function prints out an image, meant to deal with ascii escape code formatted images with color. 
  //Don't try to do high framerate animations with this.
  //Cannot truncate overflow in x-axis at the moment due to that every pixel is actually around 11 letters.  
  for(unsigned int i = min_size_y; i < img_ref.size(); ++i){
    cursorpos(px, py+i-min_size_y);
    if(i +py-min_size_y>= rows_) break;
    if(i+py -min_size_y>= max_size_y) break;
    std::cout << img_ref[i];
  }
}
void Textgrafs::clear_grid(){
  //Fills the grid with space
  std::string s(cols_, ' ');
  for(int i = 0; i < rows_ ; ++i){
    grid[i] = s;
  }
}
bool Textgrafs::next_tick(){
  //Calculate next frame
  std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::system_clock::now()-timer_);
  if(debug)add_row(std::to_string(time_span.count()), 0, 0);
  if(time_span.count() > time_between_frames_){
    timer_ = std::chrono::system_clock::now();
    return true;
  }
  return false;
  
}
void Textgrafs::add_row(std::string text, int px, int py){
  //User adds a row of text to the grid. 
  //if outside the screen the text will be truncated (or ignored if nothing is on screen)
  if(py < 0 || py >= rows_){
    return;
  }
  if(px < 0){
    if(abs(px) < text.length()){
      text.erase(0,abs(px));
    }
  }
  if(px >=cols_){
    return;
  }
  if(px + text.length() >= cols_){
    text.erase((cols_-px), text.length());
  }
  if(px >= 0)  grid[py].replace(px, text.length(), text);
  if(px < 0) grid[py].replace(0, text.length(), text);
}
void Textgrafs::add_col(std::string text, int px, int py){
  //User adds a row of text to the grid. 
  //if outside the screen the text will be truncated (or ignored if nothing is on screen)
  if(px < 0 || px >= cols_ || py >= rows_){
    return;
  }
  if(py < 0){
    if(abs(py) < text.length()){
      text.erase(0, abs(py)); 
    }
  }
  if(py + text.length() >= rows_){
    text.erase((rows_-py), text.length());
  }
  std::string s;
  for(unsigned int i = py;  i < py+text.length(); ++i){
    //std::cout << "i " << i << std::endl;
    s.assign(text, i-py, 1);
    add_row(s, px, i);
  }
}
void Textgrafs::add_rect(char letter, int px, int py, int sizex, int sizey){
  //User adds a full rext to the grid. 
  //if outside the screen the text will be truncated (or ignored if nothing is on screen)
  std::string s(sizex, letter);
  for(int i = 0; i < sizey; ++i){
    add_row(s, px, py+i); //Truncation will be handled by add_row
  }
}

void Textgrafs::add_rect_unique(const std::vector<std::string> & shape, int px, int py){
  for(unsigned int i = 0; i<shape.size();++i){
    add_row(shape[i], px, py+i);
  }
}

void Textgrafs::add_border(char letter, int px, int py, int sizex, int sizey){
  std::string obx(sizex-1, letter); //outer border x
  std::string oby(sizey, letter); //outer border y

    //Create outer borders
    add_row(obx, px, py);
    add_row(obx, px, py+sizey-1);
    add_col(oby, px, py);
    add_col(oby, px+sizex-1, py);
}
void Textgrafs::add_ellipse(char letter, int px,int py, int rx, int ry){
  //x^2/a^2 + y^2/b^2 = 1
  //TODO make work
  std::vector<std::string> s;
  std::vector<int> starts (ry, -1);
  std::vector<int> ends (rx, -1);
  for(int y = 0; y < ry; ++y){
    for(int x = 0; x < rx; ++x){
      float rad = (x*x)/(rx*rx) + (y*y)/(ry*ry);
      std::cout << "rad " << rad << std::endl;
      if(starts[y] == -1 && rad <= 1){
	starts[y] = x;
      }
      else if(starts[y] != -1 &&  rad > 1){
	ends[y] = x;
      }
    }
  }
  for(int i = 0; i < ry; ++i){
    std::cout << "ends " << ends[i] << " starts " << starts[i] << std::endl;
    std::string p(ends[i]-starts[i], letter);
    std::cout << "p " << p <<std::endl;
    std::cout << "px + starts[i] " << px+starts[i] <<std::endl;
    std::cout << "py+i " << py+i << std::endl;
    
    add_row(p, px + starts[i], py + i);
  }
}

