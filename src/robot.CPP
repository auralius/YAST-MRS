#include "robot.H"


///////////////////////////////////////////////////////////////////////////////
// PROCESS
///////////////////////////////////////////////////////////////////////////////

CRobot::CRobot(CSimulationWindow *w)
{
    m_window = w;
    m_speed_l = 0.0;
    m_speed_r = 0.0;
    m_pose0 = m_pose; // Initial previous robot pose
    m_time_step = 0.02; // Default time step
    m_step_num = 0;
    m_global_flag = true; // Global flag, this class is running
    m_first_time_flag = true;
}

CRobot::~CRobot(void)
{
    // Clear the global flag, telling that this class has been destroyed
    m_global_flag = false;

    fprintf(stdout, "Cleaning memory [CRobot]\n");
    fflush(stdout);

	delete [] m_sensor_data;
}

int CRobot::az_read_config_file(const char *fn)
{
    // Read configuration file
    CConfig c;
    int ret = c.read_config_file(fn);
    if (ret == -1 ) {
    	fprintf(stderr, "Error loading config file: %s.\nFile not found!\n" \
    			"Using default configuration.\n" , fn);
    	fflush(stderr);

    }

    // Store to memory
    m_cfg.SCALE_FACTOR = c.get_scale_factor();
    m_cfg.SCREEN_TIMEOUT = c.get_time_out();
    m_cfg.ROBOT_DIAMETER = c.get_robot_diameter();
    m_cfg.ROBOT_RADIUS = c.get_robot_radius();
    m_cfg.GRID_MAP_H = c.get_grid_map_h();
    m_cfg.GRID_MAP_W = c.get_grid_map_h();
    m_cfg.LIDAR_STDEV = c.get_lidar_stdev();
    m_cfg.LIDAR_START_ANGLE = c.get_lidar_start_angle();
    m_cfg.LIDAR_SWEEP_ANGLE = c.get_lidar_sweep_angle();
    m_cfg.LIDAR_RAYS  = c.get_lidar_ray_num();
    m_cfg.LIDAR_MAX = c.get_lidar_max_dist();

	// Iniatialize simulation based on loaded configuration
    init_sim();

	return ret;
}

void CRobot::az_step()
{
    m_step_num++;

    // Robot's kinematic
    // Make it fast: compute--store--use--reuse!
    double plus_factor = m_speed_r + m_speed_l;
    double minus_factor = m_speed_l - m_speed_r;
    double half_plus_factor = plus_factor / 2;
    double a = m_cfg.ROBOT_RADIUS * plus_factor / minus_factor;
    double b = m_pose.th() + minus_factor * m_time_step / m_cfg.ROBOT_DIAMETER;
    double cos_theta = cos(m_pose.th());
    double sin_theta = sin(m_pose.th());

    // Division by zero, robot moves on a straight line
    if (std::abs(minus_factor) > 0.001) {
        m_pose.set_th(b);
        m_pose.set_x(m_pose.x() + a * (sin(b) - sin_theta));
        m_pose.set_y(m_pose.y() - a * (cos(b) - cos_theta));
    }

    else {
        m_pose.set_x(m_pose.x() + half_plus_factor * cos_theta * m_time_step);
        m_pose.set_y(m_pose.y() + half_plus_factor * sin_theta * m_time_step);
    }

    // update sensor value
    az_update_all_sensors();
}

void CRobot::az_log_sensor(const char *fn)
{
    std::ofstream f;
    f.open(fn, std::ios::app);

    if (f.is_open()) {
        for (int i = 0; i < m_cfg.LIDAR_RAYS; i++) {
            f << m_sensor_data[i].get_value() << "\t";
        }
        f << "\n";
    }
    f.close();
}

////////////////////////////////////////////////////////////////////////////////
// SET
////////////////////////////////////////////////////////////////////////////////

void CRobot::az_set_location(double x, double y, double th)
{
    m_pose = CPose(x * m_cfg.SCALE_FACTOR, y * m_cfg.SCALE_FACTOR, th);
}

void CRobot::az_set_lspeed(double s)
{
    m_speed_l = s * m_cfg.SCALE_FACTOR;
}

void CRobot::az_set_rspeed(double s)
{
    m_speed_r = s * m_cfg.SCALE_FACTOR;
}

void CRobot::az_set_stop()
{
    m_speed_l = 0.0;
    m_speed_r = 0.0;
}

void CRobot::az_set_time_step(double t)
{
    m_time_step = t;
}

////////////////////////////////////////////////////////////////////////////////
// GET
////////////////////////////////////////////////////////////////////////////////

double CRobot::az_get_sensor_data(double th)
{
    // Outside of range
    if (th < m_cfg.LIDAR_START_ANGLE ||
        th > m_cfg.LIDAR_START_ANGLE + m_cfg.LIDAR_SWEEP_ANGLE)
        return 0;

    // Given theta, compute sensor index
    int index = (th - m_cfg.LIDAR_START_ANGLE) / m_cfg.LIDAR_SWEEP_ANGLE * (m_cfg.LIDAR_RAYS -1);
    return m_sensor_data[index].get_value() / m_cfg.SCALE_FACTOR;
}

double CRobot::az_get_lspeed()
{
    return m_speed_l / m_cfg.SCALE_FACTOR;
}

double CRobot::az_get_rspeed()
{
    return m_speed_r / m_cfg.SCALE_FACTOR;
}

double CRobot::az_get_pos_x()
{
    return m_pose.x() / m_cfg.SCALE_FACTOR;
}

double CRobot::az_get_pos_x_prev()
{
    return m_pose0.x() / m_cfg.SCALE_FACTOR;
}

double CRobot::az_get_pos_y()
{
    return m_pose.y() / m_cfg.SCALE_FACTOR;
}

double CRobot::az_get_pos_y_prev()
{
    return m_pose0.y() / m_cfg.SCALE_FACTOR;
}

double CRobot::az_get_angle()
{
    return m_pose.th();
}

double CRobot::az_get_time_step()
{
    return m_time_step;
}

int CRobot::az_get_step_number()
{
    return m_step_num;
}

////////////////////////////////////////////////////////////////////////////////
// Calculation
////////////////////////////////////////////////////////////////////////////////

int CRobot::az_calc_index(int x, int y)
{
    return x + y * m_cfg.GRID_MAP_W;
}

////////////////////////////////////////////////////////////////////////////////
// Give access to some of the robot data
////////////////////////////////////////////////////////////////////////////////

CPose * CRobot::az_get_pose_addr()
{
    return &m_pose;
}

CSensor * CRobot::az_get_sensor_addr()
{
    return m_sensor_data;
}

///////////////////////////////////////////////////////////////////////////////
// PRIVATE MEMBERS
///////////////////////////////////////////////////////////////////////////////

void CRobot::init_sim()
{
	// Set m_cfg of CSimulationWindow to point m_cfg of CRobot
	m_window->set_cfg_data(&m_cfg);

	// Create necessary arrays
	m_sensor_data = new CSensor[m_cfg.LIDAR_RAYS];

    // Tell the canvas information about the robot so it can be drawn
    m_window->get_canvas()->set_addr_robot(this);

	// Initial position: center
	az_set_location(1, 1, 0.0);

    boost::thread sim(simulation_thread, (void*) this);
}

void CRobot::simulation_thread(void *param)
{
    CRobot *o = (CRobot*)param;
    o->simulation_thread_worker();
}

void CRobot::simulation_thread_worker()
{
    while (m_global_flag) {
        if (m_window->get_simulation_flag() == true) {
            // Do these at first time of the iteration only:
            //  record robot initial pose
            //  initialize sensor value
            if (m_first_time_flag == true) {
                    m_pose0 = m_pose;
                    m_step_num = 0;
                    az_update_all_sensors();

                    m_first_time_flag = false;
            } // end if
            az_sim_fn();
        } // end if

        else
            m_first_time_flag = true;

#		ifdef WIN32
			Sleep(m_window->get_simulation_speed());
#		else
			usleep(m_window->get_simulation_speed());
#		endif
    } // end while
}

void CRobot::az_update_all_sensors()
{
    const double step = m_cfg.LIDAR_SWEEP_ANGLE / m_cfg.LIDAR_RAYS;
    CPoint2D po(0, 0); // po = laser ray start point
    CPoint2D pt(0,0); // pt = laser ray farthest end point

    CPoint2D temp(0, 0); // po = laser ray start point

    if (m_window->get_binary_map())
        for (int i = 0; i < m_cfg.LIDAR_RAYS; i++)
            m_sensor_data[i].set_binary_map(m_window->get_map_width(),
                m_window->get_map_height(),
                m_window->get_binary_map());

    // For every sensor
    for (int i = 0; i < m_cfg.LIDAR_RAYS; i++) {
        temp = CPoint2D(m_pose.x() + m_cfg.ROBOT_RADIUS, m_pose.y());
        po = temp.rotate_about(m_pose,
            m_pose.th() + step * i + m_cfg.LIDAR_START_ANGLE);

        temp = CPoint2D(m_pose.x() + m_cfg.ROBOT_RADIUS + m_cfg.LIDAR_MAX, m_pose.y());
        pt = temp.rotate_about(m_pose,
            m_pose.th() + step * i + m_cfg.LIDAR_START_ANGLE);

        // Input data to sensor and update its value
        m_sensor_data[i].set_start_point(po);
        m_sensor_data[i].set_end_point(pt);
        m_sensor_data[i].update_value();
    }

    string s = "Number of sensor rays = " + std::to_string(m_cfg.LIDAR_RAYS) + '\n' + '\n';
    for (int i = 0; i < m_cfg.LIDAR_RAYS; i++) {
        s = s + "Ray-" + std::to_string(i) + " = " + std::to_string(m_sensor_data[i].get_value()) + '\n';
    }
    
    m_window->set_text_propoerties_window(s.c_str());
}