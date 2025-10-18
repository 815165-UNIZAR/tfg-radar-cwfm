#ifndef RADAR_BLOCK_H
#define RADAR_BLOCK_H

#include "ap_int.h"
#include "hls_stream.h"

typedef ap_uint<32> axi_t;

void radar_block(hls::stream<axi_t> &data_out,
                 ap_uint<32> f_clk,
                 float f_start,
                 float f_stop,
                 float t_ramp,
                 float t_silence,
                 ap_uint<1> enable,
                 hls::stream<ap_uint<1>> &ramp_active);

#endif