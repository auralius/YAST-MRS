//
// auralius (manurung.auralius@gmail.com)
//

#include "robot.H"

class CMyRobot : public CRobot
{
public:
    CMyRobot(CSimulationWindow *w)
    :CRobot(w)
    {
        az_read_config_file("../../src/robot.CFG");
    }

    virtual void az_sim_fn()
    {
        az_step();

        double front = az_get_sensor_data(0);
        if (front > 0.1) {
            az_set_lspeed(0.1);
            az_set_rspeed(0.1);
        }
        else
            az_set_stop();
    }
};

///////////////////////////////////////////////////////////////////////////////
// MAIN
///////////////////////////////////////////////////////////////////////////////

INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PSTR lpCmdLine, INT nCmdShow)
{
    CSimulationWindow win(600, 600,"Yet Another Simple 2D - Mobile Robot Simulator");
    //win.color(FL_GRAY);
    win.end();
    win.show();

    CMyRobot robot(&win);

    return(Fl::run());
}
