#include "properties_window.h"
#include "robot.H"


CPropertiesWindow::CPropertiesWindow(int w, int h, const char *l)
    :Fl_Window(0, 0, w, h, l)
{
    m_robot == NULL;
    m_disp = new Fl_Text_Display(20, 20, 640 - 40, 480 - 40, "Robot properties");
    m_tbuff = new Fl_Text_Buffer();      // text buffer
    m_disp->buffer(m_tbuff);
}


CPropertiesWindow::~CPropertiesWindow(void)
{

}

void CPropertiesWindow::set_robot_addr(CRobot *r)
{
    m_robot = r;
}

void CPropertiesWindow::set_disp(const char *t)
{
    m_tbuff->text(t);
}

void CPropertiesWindow::clean_memory()
{

    delete [] m_disp;
}