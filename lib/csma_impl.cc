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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "csma_impl.h"
#include <boost/crc.hpp>
#include <cstdlib>
#include <cmath>


namespace gr {
  namespace ieee802_11 {

    csma::sptr
    csma::make(float threshold, float signal_power)
    {
      return gnuradio::get_initial_sptr
        (new csma_impl(threshold, signal_power));
    }

    /*
     * The private constructor
     */
    csma_impl::csma_impl(float threshold, float signal_power)
      : gr::block("csma",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(0, 0, 0))
    {
		message_port_register_out(pmt::mp("out"));
		message_port_register_in(pmt::mp("in"));
		set_msg_handler(pmt::mp("in"),
				boost::bind(&csma_impl::in, this, _1));
		
		d_threshold = threshold;
		d_signal_power = signal_power;
		
	}

    /*
     * Our virtual destructor.
     */
    csma_impl::~csma_impl()
    {
    }

    //void
    //csma_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    //{
        //* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
    //}

    //int
    //csma_impl::general_work (int noutput_items,
                       //gr_vector_int &ninput_items,
                       //gr_vector_const_void_star &input_items,
                       //gr_vector_void_star &output_items)
    //{
        //const <+ITYPE*> *in = (const <+ITYPE*> *) input_items[0];
        //<+OTYPE*> *out = (<+OTYPE*> *) output_items[0];

        //// Do <+signal processing+>
        //// Tell runtime system how many input items we consumed on
        //// each input stream.
        //consume_each (noutput_items);

        //// Tell runtime system how many output items we produced.
        //return noutput_items;
    //}
    
    void
    csma_impl::in(pmt::pmt_t msg)
    {
		//parameter declaration
		double max_attempts = 4.0;
		double cwmin[2] = {3.0, 15.0};
		double slot_time = 13.0; //micro-seconds
		double aifs[2] = {58.0, 149.0};
		
		// extract the dictionary for the ac level
		pmt::pmt_t p_dict(pmt::car(msg));
		pmt::pmt_t ac_level = pmt::pmt_dict_ref(p_dict, pmt::pmt_symbol("ac_level"), pmt::PMT_NIL);
		int ac = (int) pmt::pmt_to_long(ac_level);
		
		//check channel state
		bool okay_to_send = false;
		okay_to_send = channel_state(d_threshold); 							// need to deal with
		
		double n_attempts = 0;
		double counter;
		int backoff;
		
		
		
		//state machine
		
		while (!okay_to_send)
		{
			//wait for aifs
			wait_time(aifs[ac]);
			
			
			//slot
			backoff = (int) (rand() % (cwmin[ac]*pow(2,n_attempts)));
			counter = backoff;
			
			while (counter>0) 
			{
				wait_time(slot_time);
				
				counter--;
				okay_to_send = channel_state(d_threshold);
				if (!okay_to_send)
				{
					wait_time(aifs[ac]);
				}
			}
			
			okay_to_send = channel_state(d_threshold);
			n_attempts++;
			if (n_attempts > max_attempts) {return;}
			
		}
		
		//send the msg
		message_port_pub(pmt::mp("out"), msg);
		
		//post-tx aifs
		wait_time(aifs[ac]);
		
		
	}
	
	void 
	csma_impl::wait_time(double wait_duration)
	{
		time_t start_time;
		time_t stop_time;
		
		time(&start_time);
		time(&stop_time);
		while((stop_time - start_time)/1000000 <= wait_duration)
		{
			time(&stop_time);
		}
	}
	
	bool 
	csma_impl::channel_state(float threshold)
	{
		if (d_signal_power >= threshold) {return true;}
	}

  } /* namespace ieee802_11 */
} /* namespace gr */

