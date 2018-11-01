#pragma once

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Text_Display.H>

class CRobot; // Forward declaration

class CPropertiesWindow : public Fl_Window
{
public:
    CPropertiesWindow(int w, int h, const char *l);
    ~CPropertiesWindow(void);
    void set_robot_addr(CRobot *r);
    void set_disp(const char *t);
    void clean_memory();

private:
    Fl_Text_Display *m_disp;
    Fl_Text_Buffer *m_tbuff;

    CRobot *m_robot;
};

