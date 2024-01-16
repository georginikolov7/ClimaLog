#ifndef Display_h
#define Display_h

class Display {
public:
  virtual void writeText(const char* text) = 0;
  virtual void writeAt(int row, const char* text) = 0;
  virtual void resetDisplay() = 0;
  virtual ~Display() {}
};
#endif