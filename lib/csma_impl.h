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

namespace gr {
  namespace ieee802_11 {

    class csma_impl : public csma
    {
     private:
      float d_threshold;
      float d_signal_power;

     public:
      csma_impl(float threshold, float signal_power);
      ~csma_impl();

     
      //void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      //int general_work(int noutput_items,
	//	       gr_vector_int &ninput_items,
	//	       gr_vector_const_void_star &input_items,
	//	       gr_vector_void_star &output_items);
    //};


	void in(pmt::pmt_t msg);
	void wait_time(double wait_duration);
	bool channel_state(float threshold);
    };
  } // namespace ieee802_11
} // namespace gr

#endif /* INCLUDED_IEEE802_11_CSMA_IMPL_H */
