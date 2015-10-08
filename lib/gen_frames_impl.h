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

#ifndef INCLUDED_IEEE802_11_GEN_FRAMES_IMPL_H
#define INCLUDED_IEEE802_11_GEN_FRAMES_IMPL_H

#include <ieee802-11/gen_frames.h>

extern const std::complex<double> BPSK_D[2];
extern const std::complex<double> QPSK_D[4];
extern const std::complex<double> QAM16_D[16];
extern const std::complex<double> QAM64_D[64];

struct mac_header {
	//protocol version, type, subtype, to_ds, from_ds, ...
	uint16_t frame_control;
	uint16_t duration;
	uint8_t addr1[6];
	uint8_t addr2[6];
	uint8_t addr3[6];
	uint16_t seq_nr;
}__attribute__((packed));

struct mac_ack_header {
	uint16_t frame_control;
	uint16_t duration;
	uint8_t addr1[6];
}__attribute__((packed));

namespace gr {
  namespace ieee802_11 {

    class gen_frames_impl : public gen_frames
    {
     private:
     
//variable declaration
	uint16_t d_seq_nr;
	uint8_t d_src_mac[6];
	uint8_t d_dst_mac[6];
	uint8_t d_bss_mac[6];
	uint8_t d_psdu[1528];
	float d_frame_type;
	long d_ac;
	uint16_t d_duration;
	

     public:
      gen_frames_impl(float frame_type, long ac, std::uint16_t duration, std::vector<uint8_t> src_mac, std::vector<uint8_t> dst_mac, std::vector<uint8_t> bss_mac);
      ~gen_frames_impl();


      void data_input (pmt::pmt_t msg);
      bool check_mac(std::vector<uint8_t> mac);
      void generate_mac_data_frame(const char *msdu, int msdu_size, uint16_t duration, char **psdu, int *psdu_size);
      void generate_mac_ack_frame(uint16_t duration, char **psdu, int *psdu_size);
      void generate_mac_management_frame();

    };

  } // namespace ieee802-11
} // namespace gr

#endif /* INCLUDED_IEEE802-11_GEN_FRAMES_IMPL_H */

