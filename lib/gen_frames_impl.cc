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
#include "gen_frames_impl.h"
#include <string>
#include <boost/crc.hpp>


namespace gr {
  namespace ieee802_11 {

    gen_frames::sptr
    gen_frames::make(float frame_type, long ac, std::uint16_t duration, std::vector<uint8_t> src_mac, std::vector<uint8_t> dst_mac, std::vector<uint8_t> bss_mac)
    {
      return gnuradio::get_initial_sptr
        (new gen_frames_impl(frame_type, ac, duration, src_mac, dst_mac, bss_mac));
    }

    /*
     * The private constructor
     */
    gen_frames_impl::gen_frames_impl(float frame_type, long ac, std::uint16_t duration, std::vector<uint8_t> src_mac, std::vector<uint8_t> dst_mac, std::vector<uint8_t> bss_mac)
      : gr::block("gen_frames",
              gr::io_signature::make(0,0,0),
              gr::io_signature::make(0,0,0)),
		d_seq_nr(0)
    {
		
	message_port_register_out(pmt::mp("out"));
        
        message_port_register_in(pmt::mp("data input"));
        set_msg_handler(pmt::mp("data input"),
		boost::bind(&gen_frames_impl::data_input, this, _1));
		
	if(!check_mac(src_mac)) throw std::invalid_argument("wrong mac address size");
	if(!check_mac(dst_mac)) throw std::invalid_argument("wrong mac address size");
	if(!check_mac(bss_mac)) throw std::invalid_argument("wrong mac address size");
		
	for(int i = 0; i < 6; i++) {
		d_src_mac[i] = src_mac[i];
		d_dst_mac[i] = dst_mac[i];
		d_bss_mac[i] = bss_mac[i];
		}
	d_frame_type = frame_type;
    d_ac = ac;
    d_duration = duration;
    
    
    }
    
    

    /*
     * Our virtual destructor.
     */
    gen_frames_impl::~gen_frames_impl()
    {
    }


   

    void
    gen_frames_impl::data_input (pmt::pmt_t msg)
    {
		size_t       msg_len;
		const char   *msdu;

		// dict
		pmt::pmt_t dict = pmt::make_dict();

		if(pmt::is_eof_object(msg)) {
			message_port_pub(pmt::mp("out"), pmt::PMT_EOF);
			detail().get()->set_done(true);
			return;

		} else if(pmt::is_symbol(msg)) {

			std::string  str;
			str = pmt::symbol_to_string(msg);
			msg_len = str.length();
			msdu = str.data();

		} else if(pmt::is_pair(msg)) {

			dict = pmt::car(msg);
			msg_len = pmt::blob_length(pmt::cdr(msg));
			msdu = reinterpret_cast<const char *>(pmt::blob_data(pmt::cdr(msg)));

		} else {
			throw std::invalid_argument("OFDM MAC expects PDUs or strings");
                return;
		}

		if(msg_len > 1500) {
			throw std::invalid_argument("Frame too large (> 1500)");
		}

		// make MAC frame
		int    psdu_length;
		char   *psdu;


		
		if (d_frame_type == 0)   
	        {
		  // data frame
		  generate_mac_data_frame(msdu, msg_len, d_duration, &psdu, &psdu_length);
	        }
		else if (d_frame_type == 1)
		  {
		    // management frame
		    generate_mac_management_frame();
		  }
		else if (d_frame_type == 2)
		  {
		    // ack frame
		    generate_mac_ack_frame(d_duration, &psdu, &psdu_length);

		  }


		dict = pmt::dict_add(dict, pmt::mp("crc_included"), pmt::PMT_T);
		dict = pmt::dict_add(dict, pmt::mp("ac_level"),  pmt::mp(d_ac));


		// check if csma is enabled
		//pmt::pmt_t pmt_cw = pmt::dict_ref(dict, pmt::mp("cw_min"), pmt::PMT_NIL);
		//if(pmt_cw != pmt::PMT_NIL) {
			//uint64_t cw = pmt::to_long(pmt_cw);
			//pmt::pmt_t backoffs = pmt::make_u64vector(2, 0);
			//pmt::u64vector_set(backoffs, 0, rand() % cw);
			//pmt::u64vector_set(backoffs, 1, rand() % cw);

			//dict = pmt::dict_add(dict, pmt::mp("backoffs"), backoffs);
			//dict = pmt::dict_add(dict, pmt::mp("csma"), pmt::PMT_T);
		//}


		// blob
		pmt::pmt_t mac = pmt::make_blob(psdu, psdu_length);

		// pdu
		message_port_pub(pmt::mp("out"), pmt::cons(dict, mac));
	}
	
	
	bool 
	gen_frames_impl::check_mac (std::vector<uint8_t> mac) 
	{
		if(mac.size() != 6) return false;
		return true;
	}

	void
	gen_frames_impl::generate_mac_data_frame(const char *msdu, int msdu_size, uint16_t duration, char **psdu, int *psdu_size) 
	{

		// mac header
		mac_header header;
		header.frame_control = 0x0008;
		header.duration = duration;

		for(int i = 0; i < 6; i++) {
			header.addr1[i] = d_dst_mac[i];
			header.addr2[i] = d_src_mac[i];
			header.addr3[i] = d_bss_mac[i];
		}

		header.seq_nr = 0;
		for (int i = 0; i < 12; i++) {
			if(d_seq_nr & (1 << i)) {
				header.seq_nr |=  (1 << (i + 4));
			}
		}
		header.seq_nr = htole16(header.seq_nr);
		d_seq_nr++;

		//header size is 24, plus 4 for FCS means 28 bytes
		*psdu_size = 28 + msdu_size;
		*psdu = (char *) calloc(*psdu_size, sizeof(char));

		//copy mac header into psdu
		std::memcpy(*psdu, &header, 24);
		//copy msdu into psdu
		memcpy(*psdu + 24, msdu, msdu_size);
		//compute and store fcs
		boost::crc_32_type result;
		result.process_bytes(*psdu, msdu_size + 24);

		unsigned int fcs = result.checksum();
		memcpy(*psdu + msdu_size + 24, &fcs, sizeof(unsigned int));
	}
    
        void
        gen_frames_impl::generate_mac_management_frame()
        {
		// for future work
        } 


        void
	gen_frames_impl::generate_mac_ack_frame(uint16_t duration, char **psdu, int *psdu_size)
	{

		// mac header
		mac_ack_header header;
		header.frame_control = 0x00D4;
		header.duration = duration;

		for(int i = 0; i < 6; i++) {
			header.addr1[i] = d_dst_mac[i];
			
		}

		//header size is 10, plus 4 for FCS means 14 bytes
		*psdu_size = 14 ;
		*psdu = (char *) calloc(*psdu_size, sizeof(char));

		//copy mac header into psdu
		std::memcpy(*psdu, &header, 10);
		//compute and store fcs
		boost::crc_32_type result;
		result.process_bytes(*psdu,10);

		unsigned int fcs = result.checksum();
		memcpy(*psdu + 10, &fcs, sizeof(unsigned int));
	}


	
  } /* namespace ieee802-11 */
} /* namespace gr */

