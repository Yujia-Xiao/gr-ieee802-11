/* -*- c++ -*- */
/* 
 * Copyright 2015 <+YOU OR YOUR COMPANY+>.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_IEEE802_11_CSMA_IMPL_H
#define INCLUDED_IEEE802_11_CSMA_IMPL_H

#include <ieee802-11/csma.h>
#include <fstream>

namespace gr {
  namespace ieee802_11 {

    class csma_impl : public csma
    {
     private:
      float d_threshold;
      bool d_debug;
      std::ofstream fp;

     public:
      csma_impl(float threshold, bool debug);
      ~csma_impl();

      // Where all the action really happens
      void in(pmt::pmt_t msg);
      void wait_time(double wait_duration);
      bool channel_state(float threshold, double * power);
      int shm_get(int key, void **start_ptr, int size);
      int shm_rm(int shmid);

    };

  } // namespace ieee802-11
} // namespace gr

#endif /* INCLUDED_IEEE802-11_CSMA_IMPL_H */

