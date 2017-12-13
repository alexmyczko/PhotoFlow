/* 
 */

/*

    Copyright (C) 2014 Ferrero Andrea

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.


 */

/*

    These files are distributed with PhotoFlow - http://aferrero2707.github.io/PhotoFlow/

 */

#ifndef PERSPECTIVE_CONFIG_DIALOG_HH
#define PERSPECTIVE_CONFIG_DIALOG_HH

#include <gtkmm.h>

#include "../operation_config_gui.hh"
#include "../../operations/perspective.hh"


namespace PF {

  class PerspectiveConfigGUI: public OperationConfigGUI
{
  Gtk::VBox controlsBox;

  int active_point_id;

public:
  PerspectiveConfigGUI( Layer* l );

  void do_update();
  bool has_editing_mode() { return true; }

  bool pointer_press_event( int button, double x, double y, int mod_key );
  bool pointer_release_event( int button, double x, double y, int mod_key );
  bool pointer_motion_event( int button, double x, double y, int mod_key );
  bool modify_preview( PixelBuffer& buf_in, PixelBuffer& buf_out,
      float scale, int xoffset, int yoffset );
};

}

#endif
