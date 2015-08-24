/*
    This file is part of wireworld_systemc
    Copyright (C) 2015  Julien Thevenon ( julien_thevenon at yahoo.fr )

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#include "wireworld_types.h"
#include "top.h"

int sc_main(int argc,char ** argv)
{

  std::vector<wireworld_systemc::wireworld_types::t_coordinates > l_copper_cells;
  l_copper_cells.push_back(wireworld_systemc::wireworld_types::t_coordinates(0,1));
  l_copper_cells.push_back(wireworld_systemc::wireworld_types::t_coordinates(1,2));
  l_copper_cells.push_back(wireworld_systemc::wireworld_types::t_coordinates(1,0));
  l_copper_cells.push_back(wireworld_systemc::wireworld_types::t_coordinates(2,1));
  l_copper_cells.push_back(wireworld_systemc::wireworld_types::t_coordinates(3,1));
  l_copper_cells.push_back(wireworld_systemc::wireworld_types::t_coordinates(4,1));

  std::vector<wireworld_systemc::wireworld_types::t_coordinates > l_queue_cells;
  l_queue_cells.push_back(wireworld_systemc::wireworld_types::t_coordinates(1,0));
  std::vector<wireworld_systemc::wireworld_types::t_coordinates > l_electron_cells;
  l_electron_cells.push_back(wireworld_systemc::wireworld_types::t_coordinates(0,1));


  wireworld_systemc::top l_top("top",l_copper_cells,l_queue_cells,l_electron_cells);
  sc_start();
  return 0;
}
//EOF
