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
#include "check_addr_impl.h"

namespace gr {
  namespace ieee802_11 {

    check_addr::sptr
    check_addr::make(std::vector<uint8_t> src_mac)
    {
      return gnuradio::get_initial_sptr
        (new check_addr_impl(src_mac));
    }

    /*
     * The private constructor
     */
    check_addr_impl::check_addr_impl(std::vector<uint8_t> src_mac)
      : gr::block("check_addr",
              gr::io_signature::make(0,0,0),
              gr::io_signature::make(0,0,0))
    {
		message_port_register_out(pmt::mp("out"));
		message_port_register_in(pmt::mp("data input"));
		set_msg_handler(pmt::mp("data input"),
				boost::bind(&check_addr_impl::data_input, this, _1));
		
		if (!check_mac(src_mac)) throw std::invalid_argument("wrong mac address size");
		
		for (int i = 0; i < 6; i++)
		{
			d_src_mac[i] = src_mac[i];
		}	
	}

    /*
     * Our virtual destructor.
     */
    check_addr_impl::~check_addr_impl()
    {
    }

    void
    check_addr_impl::data_input (pmt::pmt_t msg)
    {
		if (pmt::is_eof_object(msg)) {
			detail().get() -> set_done(true);
			return;
		} else if (pmt::is_symbol(msg)) {
			return;
		}
		
		pmt::pmt_t msg1 = pmt::cdr(msg);
		int data_len = pmt::blob_length(msg1);
		mac_header *h = (mac_header*)pmt::blob_data(msg1);
		
		if (h->addr1 == d_src_mac) {
			message_port_pub(pmt::mp("out"), msg);
			
		}
	}
	
	bool
	check_addr_impl::check_mac(std::vector<uint8_t> mac)
	{
		if (mac.size() != 6) return false;
		return true;
	}
	
	

  } /* namespace ieee802-11 */
} /* namespace gr */

