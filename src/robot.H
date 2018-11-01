/**
 *  @file   azolla.H
 *  @brief  Contains all available commands to the robot.
 *  @author Auralius Manurung <manurunga@yandex.com>
 *  @date   2/19/2011
 */

#ifndef AZOLLA_H_
#define AZOOLA_H_

#include "configure.H"
#include "simulation_window.H"

#include <boost/thread.hpp>

#include <iostream>
#include <fstream>

class CRobot
{
public:

    // Process

    /**
     * Constructor
     */
    CRobot(CSimulationWindow *w);

	/**
     * Destructor
     */
    ~CRobot(void);

    /**
     * Read configuration file.
     * @param fn Configuration file name
     * @return 0 if it is successfull, else return -1
     */
    int az_read_config_file(const char *fn);

    /**
     * Update robot pose and sensor reading.
     */
    virtual void az_step();

    /**
     * Simulation function defined by user.
     * This function will be excecuted continously while the
     * simulation is running.
     */
    virtual void az_sim_fn() = 0;

    /**
     * Write sensor data to disk.
     * @param fn  File name
     */
    void az_log_sensor(const char *fn);

    // Set

    /**
     * Set robot location and angular position.
     * @param x Position in x coordinate
     * @param y Position in y coordinate
     * @param th Angular position
     */
    void az_set_location(double x, double y, double th);

    /**
     * Set robot left wheel speed.
     * @param s Desired left wheel speed
     */
    void az_set_lspeed(double s);

    /**
     * Set robot right wheel speed.
     * @param s Desired right wheel speed
     */
    void az_set_rspeed(double s);

    /**
     * Set both wheel's speed to zero (stop the robot).
     */
    void az_set_stop();

    /**
     * Set time step for simulaiton.
     * @param t Desired time step
     */
    void az_set_time_step(double t);

    // Get

    /**
     * Get measured distance by i-th laser.
     * @param i Index
     * @return Measured distance
     */
    double az_get_sensor_data(double th);

    /**
     * Get left wheel speef.
     * @return Left wheel speed
     */
    double az_get_lspeed();

    /**
     * Get right wheel speef.
     * @return Right wheel speed
     */
    double az_get_rspeed();

    /**
     * Get robot position in x axis.
     * @return Robot position in x
     */
    double az_get_pos_x();

    /**
     * Get robot previous position in x axis.
     * @return Robot position in x
     */
    double az_get_pos_x_prev();

    /**
     * Get robot position in y axis.
     * @return Robot position in y
     */
    double az_get_pos_y();

    /**
     * Get robot previous position in y axis.
     * @return Robot position in y
     */
    double az_get_pos_y_prev();

    /**
     * Get robot angular position (radians).
     * @return Robot angular position
     */
    double az_get_angle();

    /**
     * Get simulation time step.
     * @return Simulation time step
     */
    double az_get_time_step();

    /**
     * Get simulation step numbers.
     * @return Step numbers
     */
     int az_get_step_number();

    // Calculation

    /**
     * We are using pseudo-multi-dimension array in which we use 1D array
     * to representate 2D array. The index can be computed as:
     * array2d[x][y] = array1d[x + y * array_width].
     * @return 1D array index
     */
    int az_calc_index(int x, int y);

    /**
    * Get the address of the m_pose.
    * @return address of the m_pose
    */
    CPose *az_get_pose_addr();

    /**
    * Get the address of the m_sensor_data.
    * @return address of the m_sensor_data
    */
    CSensor *az_get_sensor_addr();

    /**
    * Update the values of all sensors
    */
    void az_update_all_sensors();

private:
    /// Prepare everything needed for simulation.
    void init_sim();

    /// Static function to call simulation_thread_worker.
    static void simulation_thread(void* param);

    /// A thread to handle simulation, this thread executes az_sim_fn.
    void simulation_thread_worker();

    /// Provide acces to simulation window.
    CSimulationWindow *m_window;

    /// Left wheel angular speed.
    double m_speed_l;

    /// Right wheel angular speed.
    double m_speed_r;

    /// Robot current pose.
    CPose m_pose;

    /// Robot previous pose.
    CPose m_pose0;

    /// Simulation time step.
    double m_time_step;

    /// Number of simulation steps that already happened so far.
    int m_step_num;

    /// Flag to see if this class has been loaded.
    bool m_global_flag;

    /// First time iteration of the simulation loop.
    bool m_first_time_flag;

    /// Sensor reading data.
    CSensor *m_sensor_data;

    /// Robot, map, and simulation configuration.
    AZ_CONFIG m_cfg;
};

#endif // AZOLLA_H_
