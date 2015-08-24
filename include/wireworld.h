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

#ifndef WIREWORLD_H
#define WIREWORLD_H

#include "systemc.h"
#include "cell_factory.h"
#include <vector>
#include <set>
#include <map>

namespace wireworld_systemc
{
  class wireworld: public sc_module
  {
  public:
    SC_HAS_PROCESS(wireworld);
    inline wireworld(sc_module_name p_name,
                     const std::vector<wireworld_types::t_coordinates > & p_copper_cells,
                     const std::vector<wireworld_types::t_coordinates > & p_queue_cells,
                     const std::vector<wireworld_types::t_coordinates > & p_electron_cells);
    inline ~wireworld(void);
    sc_in<bool> m_clk;
  private:
    inline void clk_management(void);
    

    typedef std::map<wireworld_types::t_coordinates,std::vector<wireworld_types::t_coordinates>> t_neighbours;
    inline void instanciate_cells(const std::vector<wireworld_types::t_coordinates > & p_cells,
                                  const wireworld_types::t_cell_state & p_state,
                                  const t_neighbours & p_neighbours);

    typedef std::map<wireworld_types::t_coordinates,std::pair<cell_base*,sc_signal<bool>*>> t_cell_map;
    t_cell_map m_cells;
    sc_signal<bool> m_clk_sig;
    sc_trace_file *m_trace_file;
    unsigned int m_nb_electron;
    unsigned int m_nb_queue;
    sc_signal<uint32_t> m_nb_electron_sig;
  };

  //----------------------------------------------------------------------------
  void wireworld::clk_management(void)
  {
    m_clk_sig.write(m_clk.read());
    if(!m_clk_sig.read())
      {
        std::cout << m_nb_electron << "E\t" << m_nb_queue << "Q" << std::endl ;
        m_nb_electron_sig.write(m_nb_electron);
        if(m_nb_electron || m_nb_queue)
          {
            m_nb_queue = m_nb_electron;
            m_nb_electron = 0;
          }
        else
          {
            sc_stop();
          }
      }
  }

  //----------------------------------------------------------------------------
  wireworld::wireworld(sc_module_name p_name,
                       const std::vector<wireworld_types::t_coordinates > & p_copper_cells,
                       const std::vector<wireworld_types::t_coordinates > & p_queue_cells,
                       const std::vector<wireworld_types::t_coordinates > & p_electron_cells
                       ):
    sc_module(p_name),
    m_clk("clk_in"),
    m_clk_sig("clk"),
    m_trace_file(nullptr),
    m_nb_electron_sig("nb_electron")
    {
      m_trace_file = sc_create_vcd_trace_file("trace");
      SC_METHOD(clk_management);
      dont_initialize();
      sensitive << m_clk;

      sc_trace(m_trace_file,m_clk_sig,m_clk_sig.name());
      sc_trace(m_trace_file,m_nb_electron_sig,m_nb_electron_sig.name());

      m_nb_queue = p_queue_cells.size();
      m_nb_electron = p_electron_cells.size();

      // Insert copper cells in a set to have a kind of spatial access
      std::set<wireworld_types::t_coordinates> l_cells;
      for(auto l_iter: p_copper_cells)
        {
          l_cells.insert(std::set<wireworld_types::t_coordinates>::value_type(l_iter));
        }

      // Cout number of neighbours
      t_neighbours l_neighbours;
      for(auto l_iter:l_cells)
        {
          t_neighbours::iterator l_neighbour_iter = l_neighbours.insert(t_neighbours::value_type(l_iter,std::vector<wireworld_types::t_coordinates>())).first;

          uint32_t l_x = l_iter.first;
          uint32_t l_y = l_iter.second;

          for(int l_y_index = -1 ; l_y_index < 2 ; ++l_y_index)
            {
              for(int l_x_index = -1 ; l_x_index < 2 ; ++l_x_index)
                {
                  int l_rel_x = l_x + l_x_index;
                  int l_rel_y = l_y + l_y_index;
                  if((l_x_index || l_y_index) && l_cells.end() != l_cells.find(wireworld_types::t_coordinates(l_rel_x,l_rel_y)))
                    {
                      l_neighbour_iter->second.push_back(wireworld_types::t_coordinates(l_rel_x,l_rel_y));
                    }
                }
            }
        }

      // Instanciate cells
      instanciate_cells(p_electron_cells,wireworld_types::t_cell_state::ELECTRON,l_neighbours);
      instanciate_cells(p_queue_cells,wireworld_types::t_cell_state::QUEUE,l_neighbours);
      instanciate_cells(p_copper_cells,wireworld_types::t_cell_state::COPPER,l_neighbours);

      // Bind cells
      for(auto l_iter: p_copper_cells)
        {
          // Find cell
          t_cell_map::iterator l_cell_iter = m_cells.find(l_iter);
          assert(m_cells.end() != l_cell_iter);

          // Bind unique clock signal and electron signals
          l_cell_iter->second.first->bind_clk(m_clk_sig);
          l_cell_iter->second.first->bind_electron(*(l_cell_iter->second.second));
          sc_trace(m_trace_file,*(l_cell_iter->second.second),l_cell_iter->second.second->name());

          // Search for neighbours coordinates
          t_neighbours::const_iterator l_neighbour_list_iter = l_neighbours.find(l_iter);
          assert(l_neighbours.end() != l_neighbour_list_iter);

          // Iterate on neighbours
          unsigned int l_index = 0;
          for(auto l_neighbour_iter:l_neighbour_list_iter->second)
            {
              t_cell_map::iterator l_neighbour_cell_iter = m_cells.find(l_neighbour_iter);
              assert(m_cells.end() != l_neighbour_cell_iter);
              
              // Bind neighbour
              l_cell_iter->second.first->bind_neighbour(*(l_neighbour_cell_iter->second.second),l_index);
              ++l_index;
            }
        }
    }

  //----------------------------------------------------------------------------
  void wireworld::instanciate_cells(const std::vector<wireworld_types::t_coordinates > & p_cells,
                                    const wireworld_types::t_cell_state & p_state,
                                    const wireworld::t_neighbours & p_neighbours)
  {
    for(auto l_iter:p_cells)
      {
        if(m_cells.end() == m_cells.find(l_iter))
          {
            std::stringstream l_stream;
            l_stream << l_iter.first << "_" << l_iter.second;
            std::string l_name = l_stream.str();
            std::string l_cell_name = "cell_" + l_name;
            t_neighbours::const_iterator l_neighbour_iter = p_neighbours.find(l_iter);
            assert(p_neighbours.end() != l_neighbour_iter);
            m_cells.insert(t_cell_map::value_type(
                                                  l_iter,
                                                  std::pair<cell_base*,sc_signal<bool>*>(cell_factory::create(l_cell_name.c_str(),
                                                                                                              l_neighbour_iter->second,
                                                                                                              p_state,
                                                                                                              l_iter.first,
                                                                                                              l_iter.second,
                                                                                                              m_nb_electron
                                                                                                              ),
                                                                                         new sc_signal<bool>(l_name.c_str(),
                                                                                                             wireworld_types::t_cell_state::ELECTRON == p_state
                                                                                                             )
                                                                                         )
                                                  )
                           );
          }
      }
  }

  //----------------------------------------------------------------------------
  wireworld::~wireworld(void)
  {
    sc_close_vcd_trace_file(m_trace_file);
    for(auto l_iter:m_cells)
      {
        delete l_iter.second.first;
        delete l_iter.second.second;
      }
  }
}
#endif // WIREWORLD_H
//EOF
