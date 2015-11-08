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

#ifndef INCLUDED_IEEE802_11_CHECK_ADDR_IMPL_H
#define INCLUDED_IEEE802_11_CHECK_ADDR_IMPL_H

#include <ieee802-11/check_addr.h>

struct mac_header {
	//protocol version, type, subtype, to_ds, from_ds, ...
	uint16_t frame_control;
	uint16_t duration;
	uint8_t addr1[6];
	uint8_t addr2[6];
	uint8_t addr3[6];
	uint16_t seq_nr;
}__attribute__((packed));


namespace gr {
  namespace ieee802_11 {

    class check_addr_impl : public check_addr
    {
     private:
     uint8_t d_src_mac[6];
     bool d_debug;


     public:
      check_addr_impl(std::vector<uint8_t> src_mac, bool debug);
      ~check_addr_impl();

      // Where all the action really happens
     void data_input (pmt::pmt_t msg);
     bool check_mac(std::vector<uint8_t> mac);
     bool equal(uint8_t *header_addr, uint8_t *src_mac_addr);
    };

  } // namespace ieee802-11
} // namespace gr

#endif /* INCLUDED_IEEE802-11_CHECK_ADDR_IMPL_H */

