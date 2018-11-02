/**
 *  @file   simulation_window.H
 *  @brief  Contains class for simulation window
 *  @author Auralius Manurung <manurunga@yandex.com>
 *  @date   2/19/2011
 */

#ifndef SIMULATION_WINDOW_H_
#define SIMULATION_WINDOW_H_

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/fl_ask.H>

#include "configure.H"
#include "config.H"
#include "canvas.H"
#include "world.H"
#include "properties_window.h"


class CSimulationWindow : public Fl_Double_Window
{
public:
    /**
     * Constructor.
     * @param w Window width
     * @param h Window height
     * @param l String label
     */
    CSimulationWindow(int w,int h,const char *l);

    /**
     * Destructor
     */
    ~CSimulationWindow(void);

    /**
     * Get simulation flag, wether it is running or not.
     * @return Simulation flag
     */
    bool get_simulation_flag();

    /**
     * Get simulation speed.
     * @return Simulation speed: Fast, Medium, Slow
     */
    int get_simulation_speed();

    /**
     * Get binary version of loaded map.
	 * @return Map as an array of boolean
     */
    const bool *get_binary_map();

    /**
     * Get map area size (length x width)
     * @return Area size
     */
    int get_area();

    /**
     * Get map height, provided by m_world.
     * @return Map height (still in pixels)
     */
    int get_map_height();

    /**
     * Get map width, provided by m_world.
     * @return Map width (still in pixels)
     */
    int get_map_width();

    /**
     * Get pointer to canvas.
     * @return Pointer to m_canvas
     */
    CCanvas *get_canvas();

    /**
    * Get pointer to properties window.
    * @return Pointer to m_properties_window
    */
    CPropertiesWindow *get_properties_window();

    /**
     * Set configuration
     * @param cfg Configuration to set
     */
	void set_cfg_data(const AZ_CONFIG *cfg);

    void set_text_propoerties_window(const char *t);

private:
    /// Build menu.
    void build_menu();

    /// Stop the simulation
    void stop_simulation();

    /// Call back when one of the menu items clicked
    static void menu_cb(Fl_Widget*w, void *data);

    /// Menu call back worker.
    void menu_cb_worker();

    /// Callback when close [x] button on the window is clicked
    static void window_cb(Fl_Widget *widget, void *data);

    /// Worker callback function when close [x] button on the window is clicked
    void window_cb_worker();

    /// Flag for simulation status.
    bool m_simulation_running;

    /// Simulation speed.
    int m_simulation_speed;

    /// Map area size
    int m_area;

    /// CWorld is needed to process map file.
    CWorld *m_world;

    /// CCanvas is needed for drawing.
    CCanvas *m_canvas;

    /// Menu bar
    Fl_Menu_Bar *m_menu;

    /// Configuraton
	const AZ_CONFIG *m_cfg;

    CPropertiesWindow *m_properties_window;
};

#endif // SIMULATION_WINDOW_H_
