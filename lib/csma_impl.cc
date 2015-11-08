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
#include <gnuradio/block_detail.h>
#include "csma_impl.h"
#include <boost/crc.hpp>
#include <cstdlib>
#include <cmath>
#include <ctime>

#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>

#define PERMS 0600
#define dout d_debug && std::cout



namespace gr {
  namespace ieee802_11 {

    csma::sptr
    csma::make(float threshold, bool debug)
    {
      return gnuradio::get_initial_sptr
        (new csma_impl(threshold, debug));
    }

    /*
     * The private constructor
     */
    csma_impl::csma_impl(float threshold, bool debug)
      : gr::block("csma",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(0, 0, 0))
    {
		message_port_register_out(pmt::mp("out"));
		message_port_register_in(pmt::mp("in"));
		set_msg_handler(pmt::mp("in"),
				boost::bind(&csma_impl::in, this, _1));
		
		d_threshold = threshold;
        d_debug = debug;
		
	}

    /*
     * Our virtual destructor.
     */
    csma_impl::~csma_impl()
    {
    }


    
    void
    csma_impl::in(pmt::pmt_t msg)
    {
	    dout << "==========msg received===========\n";
	    
		// get share memory
		int segmentid;
		double * power;
		segmentid=shm_get(123456,(void**)&power,sizeof(double));
		
		
		if (segmentid<0){printf("Error creating segmentid"); exit(0);};
		
		//parameter declaration
		int max_attempts = 15;
		int cwmin[2] = {3, 15};
		double slot_time = 13.0; //micro-seconds
		double aifs[2] = {58.0, 149.0};
		
		// extract the dictionary for the ac level
		pmt::pmt_t p_dict(pmt::car(msg));
		pmt::pmt_t ac_level = pmt::dict_ref(p_dict, pmt::mp("ac_level"), pmt::PMT_NIL);
		int ac = (int) pmt::to_long(ac_level);
		dout << "ac level: " << ac <<std::endl;

		
		
		//check channel state
		bool okay_to_send = false;
		okay_to_send = channel_state(d_threshold, power); 		
		
		if (okay_to_send) {
			dout<<"channel is free, jumping to send...\n";
			}
		else {
			dout << "channel is BUSY, waiting...\n";
			}
		
		
		int n_attempts = 0;
		double counter;
		int backoff;
		
		
		
		//state machine
		
		while (!okay_to_send)
		{
			dout<<"################# Attempt "<< n_attempts <<std::endl;
			dout << " Waiting AIFS "<<aifs[ac] << " micro secs\n";
			//wait for aifs
			wait_time(aifs[ac]);
			
			
			//slot
			srand(time(NULL));
			backoff = rand() % (int)(cwmin[ac]*pow(2,n_attempts));
			counter = backoff;
			
			dout <<" Backoff: "<< backoff<<std::endl;
			dout <<" Counter: "<<counter<<std::endl;
		
			while (counter>=0) 
			{
				dout << " Waiting for slot time\n";
				wait_time(slot_time);
				counter--;
				dout << "  Counter: "<< counter<<std::endl;

				
				okay_to_send = channel_state(d_threshold, power);
				if (okay_to_send) {
					dout << "   Channel is free\n" ;
					}
				else {
					dout << "   Channel is busy, back to aifs\n";
					n_attempts++;
					if (n_attempts > max_attempts) {
						dout << "Max attempts reached\n";
						return;
						}
				    break;
					}				   
			}
			
			if (okay_to_send) {break;}
		}
		
		//send the msg
		
		message_port_pub(pmt::mp("out"), msg);
		dout << "Msg sent. Waiting for aifs\n";
		//post-tx aifs
		
		wait_time(aifs[ac]);
		
		dout << "==========csma end===========\n\n";

		
	}
	
	void 
	csma_impl::wait_time(double wait_duration)
	{
		clock_t start_time;
		clock_t stop_time;
		
		start_time = clock();
		dout << "evaluating start time"<< start_time<<"\n";
		
		stop_time = clock();
		
		while((stop_time - start_time) <= wait_duration)
		{
		
			stop_time = clock();
		}
		dout << "evaluating final stop time"<<stop_time<<"\n";
	}
	
	bool 
	csma_impl::channel_state(float threshold, double * power)
	{
			dout << "&power is " << * power <<std::endl;
			dout << "&power address is "<< power <<std::endl;
			dout << "&threshold is "<< threshold <<std::endl;
		
		if (*power >= threshold) {return false;}
		return true;
	}
	
	
	int
	csma_impl::shm_get(int key, void **start_ptr, int size)
	{
		int shmid;
		shmid = shmget((key_t) key, size, PERMS | IPC_CREAT);
		(*start_ptr) = (void *) shmat(shmid, (char *) 0, 0);
		return shmid;
	}


	int
	csma_impl::shm_rm(int shmid)
	{
		return shmctl(shmid, IPC_RMID, (struct shmid_ds *) 0);

	}


  } /* namespace ieee802_11 */
} /* namespace gr */

