#include "radar_block.h"

void radar_block(hls::stream<axi_t> &data_out,
                 ap_uint<32> f_clk,
                 float f_start,
                 float f_stop,
                 float t_ramp,
                 float t_silence,
                 ap_uint<1> enable,
                 hls::stream<ap_uint<1>> &ramp_active) {

    #pragma HLS INTERFACE axis port=data_out
    #pragma HLS INTERFACE axis port=ramp_active
    #pragma HLS INTERFACE s_axilite port=f_clk bundle=CTRL_BUS
    #pragma HLS INTERFACE s_axilite port=f_start bundle=CTRL_BUS
    #pragma HLS INTERFACE s_axilite port=f_stop bundle=CTRL_BUS
    #pragma HLS INTERFACE s_axilite port=t_ramp bundle=CTRL_BUS
    #pragma HLS INTERFACE s_axilite port=t_silence bundle=CTRL_BUS
    #pragma HLS INTERFACE s_axilite port=enable bundle=CTRL_BUS
    #pragma HLS INTERFACE ap_ctrl_none port=return

    // Si no está habilitado, sacamos 0's
    if (!enable) {
        ramp_active.write(0);
        data_out.write((axi_t)0);
        return;
    }

    // Calculamos el nº de muestras
    unsigned int num_samples = (unsigned int)(f_clk * t_ramp);
    unsigned int num_silence_samples = (unsigned int)(f_clk * t_silence);

    // Validación
    if(num_samples == 0) {
        ramp_active.write(0);
        data_out.write((axi_t)0);
        return;
    }

    // Al inicio de cada rampa hay que resetear la fase
    ap_uint<64> phase_acc = 0;

    // Generacion de la rampa
    RAMP_LOOP: for (unsigned int i = 0; i < num_samples; i++) {
        #pragma HLS PIPELINE II=1
        #pragma HLS LOOP_TRIPCOUNT max=340000
        
        ramp_active.write(1);

        // Calculamos la frecuencia instantánea
        float freq = f_start + (f_stop - f_start) * i / num_samples;

        // Calculamos el incremento de fase --> (freq * 2^32) / f_clk
        ap_uint<64> phase_inst = (ap_uint<64>)(freq * (1ULL << 32) / f_clk);

        phase_acc += phase_inst;

        axi_t phase_output = (axi_t)(phase_acc >> 32);
        data_out.write(phase_output);
    }

    // TIEMPO DE SILENCIO
    SILENCE_LOOP: for (unsigned int i = 0; i < num_silence_samples; i++) {
        #pragma HLS PIPELINE II=1
        #pragma HLS LOOP_TRIPCOUNT max=340000
        
        ramp_active.write(0);
        
        data_out.write((axi_t)0);
    }
    
    // El algoritmo del bloque termina aquí y se auto-reinicia automáticamente por la FPGA
}