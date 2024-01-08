#ifndef Display_h
#define Display_h

class Display {
public:
  virtual void writeText(const char* text) = 0;
  virtual ~Display() {}
};
#endif