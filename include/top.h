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

#ifndef TOP_H
#define TOP_H

#include "systemc.h"
#include "wireworld.h"

namespace wireworld_systemc
{
  class top: public sc_module
  {
  public:
    top(sc_module_name p_name,
        const std::vector<std::pair<uint32_t,uint32_t> > & p_copper_cells,
        const std::vector<std::pair<uint32_t,uint32_t> > & p_queue_cells,
        const std::vector<std::pair<uint32_t,uint32_t> > & p_electron_cells);
  private:
    sc_clock m_clk;
    wireworld m_wireworld;
  };

  //----------------------------------------------------------------------------
  top::top(sc_module_name p_name,
           const std::vector<std::pair<uint32_t,uint32_t> > & p_copper_cells,
           const std::vector<std::pair<uint32_t,uint32_t> > & p_queue_cells,
           const std::vector<std::pair<uint32_t,uint32_t> > & p_electron_cells):
    sc_module(p_name),
    m_clk("clk",10.0,SC_NS,0.5,5.0,SC_NS,true),
    m_wireworld("wireworld",p_copper_cells,p_queue_cells,p_electron_cells)
    {
      m_wireworld.m_clk(m_clk);
    }
}
#endif // TOP_H
//EOF
