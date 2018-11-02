#include "canvas.H"
#include "robot.H"

CCanvas::CCanvas(int x, int y, int w,int h,const char *l)
    :Fl_Box(x, y, w, h, l)
{
    box(FL_FLAT_BOX);
    color(fl_rgb_color(255,255,255));
    m_simulation_running = false;
    m_map_image = NULL;
    m_robot = NULL;
    m_counter = 0;
}

//
// SET
//

void CCanvas::set_addr_robot(CRobot *r)
{
    m_robot = r;
}

void CCanvas::set_cfg_data(const AZ_CONFIG *cfg)
{
	m_cfg = cfg;

	// Canvas refresh reate
    Fl::add_timeout(m_cfg->SCREEN_TIMEOUT, timeout_cb, (void*)this);
}

void CCanvas::set_map_image(Fl_PNG_Image *img)
{
    m_map_image = img;
}

void CCanvas::notify_simulation_status(bool s)
{
    m_simulation_running = s;
}

//
// Private
//

int CCanvas::handle(int e)
{
    if (e == FL_PUSH && m_simulation_running == false) {
        m_robot->az_set_location((Fl::event_x() - this->x()) / m_cfg->SCALE_FACTOR,
            (Fl::event_y() - this->y()) / m_cfg->SCALE_FACTOR, 0);
        m_robot->az_update_all_sensors();
    }
    return(Fl_Box::handle(e));
}

void CCanvas::draw()
{
    Fl_Box::draw();
    
    if (m_map_image)
        m_map_image->draw(0, 25);

    draw_sensor_beam();
    draw_robot();

    // Do not update the text on properties window all the time.
    m_counter = m_counter + 1;
    if (m_counter > 5) {
        m_robot->update_properties_window();
        m_counter = 0;
    }
}

void CCanvas::draw_robot()
{
    // Draw a robot as a circle with a line
    fl_color(FL_BLACK);

    CPose *pose = m_robot->az_get_pose_addr();
    double x = pose->x() + this->x();
    double y = pose->y() + this->y();

    CPoint2D p0(x, y);
    CPoint2D p1(p0.translate(m_cfg->ROBOT_RADIUS, pose->th()));
    fl_line(x, y, p1.x(), p1.y());
    fl_circle(x, y, m_cfg->ROBOT_RADIUS);
}

void CCanvas::draw_sensor_beam()
{
    CSensor *sensor_data = m_robot->az_get_sensor_addr();

    for (int i = 0; i < m_cfg->LIDAR_RAYS; i = i + 1) {
        if (i == 0)
            fl_color(FL_BLUE);
        else
            fl_color(FL_RED);

       // Sensor hit mark
        fl_circle(sensor_data[i].get_hit_point().x() + this->x(),
            sensor_data[i].get_hit_point().y() + this->y(),
            2.0); // Size of sensor hit mark

        // Sensor rays
        //fl_line_style(FL_DASH);
        fl_line(sensor_data[i].get_start_point().x() + this->x(),
            sensor_data[i].get_start_point().y() + this->y(), 
            sensor_data[i].get_hit_point().x() + this->x(),
            sensor_data[i].get_hit_point().y() + this->y());
        //fl_line_style(FL_SOLID);
    }
}

void CCanvas::timeout_cb(void *userdata)
{
    CCanvas *o = (CCanvas*)userdata;
    o->redraw();
    Fl::repeat_timeout(o->m_cfg->SCREEN_TIMEOUT, timeout_cb, userdata);
}
