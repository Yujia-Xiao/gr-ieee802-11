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
#include "channel_power_impl.h"

#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>

#define PERMS 0600
#define dout d_debug && std::cout

namespace gr {
  namespace ieee802_11 {

    channel_power::sptr
    channel_power::make(int n_samples, bool debug)
    {
      return gnuradio::get_initial_sptr
        (new channel_power_impl(n_samples, debug));
    }

    /*
     * The private constructor
     */
    channel_power_impl::channel_power_impl(int n_samples, bool debug)
      : gr::block("channel_power",
              gr::io_signature::make(1, 1, sizeof(float)),
              gr::io_signature::make(1, 1, sizeof(float)))
    {
		d_n_samples = n_samples;
		d_debug = debug;
		}

    /*
     * Our virtual destructor.
     */
    channel_power_impl::~channel_power_impl()
    {
    }

    void
    channel_power_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        ninput_items_required[0] = noutput_items * d_n_samples;
    }

    int
    channel_power_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        const float *in = (const float *) input_items[0];
        float *out = (float *) output_items[0];

        int segmentid;
		double * power;
		segmentid=shm_get(123456,(void**)&power,sizeof(double));
		if (segmentid<0){printf("Error creating segmentid"); exit(0);};
        
        
        // Do <+signal processing+>
        float x;
        long double count = 0.0;
        for(int i=0;i<noutput_items;i++)
        {
			x=0;
			for (int j=i;j<i+d_n_samples;j++)
			{
				x = x + in[j] * in[j];
			}
			out[i] = x/d_n_samples;
			*power = out[i];
			
			
		}
        
        dout << "[channel_power]Power evaluated to be " << *power << std::endl;
        dout << "noutput_items is " << noutput_items << std::endl;
        
        
        
        

        
        // Tell runtime system how many input items we consumed on
        // each input stream.
        consume_each (noutput_items);

        // Tell runtime system how many output items we produced.
        return noutput_items;
    }

	int
	channel_power_impl::shm_get(int key, void **start_ptr, int size)
	{
		int shmid;
		shmid = shmget((key_t) key, size, PERMS | IPC_CREAT);
		(*start_ptr) = (void *) shmat(shmid, (char *) 0, 0);
		return shmid;
	}


	int
	channel_power_impl::shm_rm(int shmid)
	{
		return shmctl(shmid, IPC_RMID, (struct shmid_ds *) 0);

	}

  } /* namespace ieee802-11 */
} /* namespace gr */

