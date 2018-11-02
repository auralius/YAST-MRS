/**
 *  @file   canvas.H
 *  @brief  Contains class for drawing
 *  @author Auralius Manurung <manurunga@yandex.com>
 *  @date   2/19/2011
 */

#ifndef CANVAS_H_
#define CANVAS_H_

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/fl_draw.H>
#include <FL/Fl_PNG_Image.H>

#include "configure.H"
#include "pose.H"
#include "sensor.H"

class CRobot; // Forward declaration

class CCanvas:public Fl_Box
{
public:
   /**
	* Constructor.
	* @param x Canvas left corner x position
	* @param y Canvas left corner y position
	* @param w Canvas width
	* @param h Canvas height
	* @param l Text, this ext will be printed in the center of the canvas
	*/
    CCanvas(int x, int y, int w, int h, const char *l);

    /**
    * Set pointer m_pose to point to m_pose in CRobot.
    * @param r Pointer to CRobot
    */
    void set_addr_robot(CRobot *r);

	/**
     * Set pointer m_cfg to point to m_cfg in CRobot.
     * @param p Pointer to m_cfg in CRobot
     */
	void set_cfg_data(const AZ_CONFIG *cfg);

    /**
    * Set pointer m_cfg to point to m_cfg in CRobot.
    * @param p Pointer to m_cfg in CRobot
    */
    void set_map_image(Fl_PNG_Image *img);

    /**
     * Notify if simulation is running, this is done by CSimulationWindow.
     * @param s Flag for simulation
     */
    void notify_simulation_status(bool s);

private:
    /// Handle mouse click event to change robot position.
    int handle(int e) ;

    /// Callback function when redraw() is called.
    void draw();

    /// Timer callback, used to call redraw().
    static void timeout_cb(void *userdata);

    /// Draw the robot.
    void draw_robot();

    /// Draw sensor hitmark and grid occupancy
    void draw_sensor_beam();

    /// If simulation is running, mouse click event should be disabled.
    bool m_simulation_running;

    /// Robot, map, and simulation configuration.
    const AZ_CONFIG	*m_cfg;

    /// Map to draw, a PNG image file.
    Fl_PNG_Image *m_map_image;

	/**
     * Below are the data needed for drawing purpose
     * Robot current position (x, y and angular position), also incluiding
	 * sensor,grid occupancy map and noisy odometry data. All data are
	 * pointers which point to the real data at class CRobot
	 * (sharing same data).
	 */
    CRobot *m_robot;

    /// Counter to update properties window
    int m_counter;
};

#endif // CANVAS_H_
