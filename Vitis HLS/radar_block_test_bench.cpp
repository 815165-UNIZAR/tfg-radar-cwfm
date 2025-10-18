#include <iostream>
#include <fstream>
#include <cmath>
#include "radar_block.h"

using namespace std;

int main() {
    // Parámetros inciales
    ap_uint<32> f_clk = 300000000;    // 300 MHz
    float f_start = 1e9;              // 1 GHz
    float f_stop = 3e9;               // 3 GHz
    float t_ramp = 50e-6;             // 50 us
    float t_silence = 100e-6;          // 50 us
    
    unsigned int num_rampas = 5;      // Número de rampas a simular
    
    // Calculo de las muestrass
    unsigned int num_samples = (unsigned int)(f_clk * t_ramp);
    unsigned int num_silence = (unsigned int)(f_clk * t_silence);
    unsigned int samples_per_cycle = num_samples + num_silence; // Muestras por cada ciclo de rampa + silencio
    unsigned int total_samples = samples_per_cycle * num_rampas;
    
    cout << "=== CONFIGURACIÓN ===" << endl;
    cout << "Frecuencia muestreo: " << f_clk/1e6 << " MHz" << endl;
    cout << "Rango frecuencias: " << f_start/1e6 << " - " << f_stop/1e6 << " MHz" << endl;
    cout << "Rampas a simular: " << num_rampas << endl;
    cout << "Muestras/rampa: " << num_samples << endl;
    cout << "Muestras/silencio: " << num_silence << endl;
    cout << "Total muestras: " << total_samples << endl << endl;
    
    // Streams
    hls::stream<axi_t> data_out;
    hls::stream<ap_uint<1>> ramp_active;
    ap_uint<1> enable = 1;
    
    cout << "Simulando " << num_rampas << " rampas (emulando auto-restart)..." << endl;
    
    // Simulamos el autoreset llamando a la función varias veces
    for(unsigned int ciclo = 0; ciclo < num_rampas; ciclo++) {
        radar_block(data_out, f_clk, f_start, f_stop, t_ramp, t_silence, 
                    enable, ramp_active);
        
        if(ciclo % 1 == 0) {
            cout << "  Rampa " << (ciclo+1) << "/" << num_rampas << " completada" << endl;
        }
    }
    
    cout << "✓ Simulación completada" << endl;
    cout << "Procesando datos..." << endl << endl;
    
    // Creamos una matriz d edatos para leerla en MATLAB
    ofstream datos("radar_data_sin_handshake.csv");
    datos << "tiempo,frecuencia,fase_hw,trigger" << endl;
    
    for (unsigned int i = 0; i < total_samples; i++) {
        axi_t phase_out;
        ap_uint<1> ramp_flag;
        
        data_out.read(phase_out);
        ramp_active.read(ramp_flag);
        
        double tiempo = i / (double)f_clk;
        unsigned int muestra_en_rampa = i % samples_per_cycle;
        
        double freq = 0.0;
        if (muestra_en_rampa < num_samples) {
            freq = f_start + (f_stop - f_start) * muestra_en_rampa / (double)num_samples;
        }
        
        double fase_hw_rad = (phase_out.to_uint() * 2.0 * M_PI) / 4294967296.0;
        int trigger = static_cast<int>(ramp_flag);
        
        datos << tiempo << "," << freq << "," << fase_hw_rad << "," << trigger << endl;
        
        if (i % (total_samples/10) == 0) {
            cout << "  Procesando: " << (i*100)/total_samples << "%" << endl;
        }
    }
    
    datos.close();
    
    // Creamos un script en MATLAB para leer los datos y hacer una representación gráfica
    ofstream matlab("plot_radar_sin_handshake.m");
    
    matlab << "% Script de visualización del radar CW-FM" << endl;
    matlab << "% Generado automáticamente por testbench" << endl;
    matlab << "clear; clc; close all;" << endl << endl;
    
    matlab << "% Parámetros del sistema" << endl;
    matlab << "f_clk = " << f_clk << "; % Hz" << endl;
    matlab << "f_start = " << f_start << "; % Hz" << endl;
    matlab << "f_stop = " << f_stop << "; % Hz" << endl;
    matlab << "t_ramp = " << t_ramp << "; % s" << endl;
    matlab << "t_silence = " << t_silence << "; % s" << endl << endl;
    
    matlab << "% Cargar datos" << endl;
    matlab << "fprintf('Cargando datos...\\n');" << endl;
    matlab << "data = readmatrix('radar_data_sin_handshake.csv');" << endl << endl;
    
    matlab << "% Extraer columnas" << endl;
    matlab << "t = data(:,1); % tiempo (s)" << endl;
    matlab << "freq = data(:,2); % frecuencia (Hz)" << endl;
    matlab << "fase_hw = data(:,3); % fase hardware (rad)" << endl;
    matlab << "trigger = data(:,4); % señal trigger" << endl << endl;
    
    matlab << "fprintf('Datos cargados: %d muestras\\n', length(t));" << endl << endl;
    
    matlab << "% Crear figura" << endl;
    matlab << "figure('Position', [100 100 1400 900], 'Name', 'Análisis Radar CW-FM');" << endl << endl;
    
    matlab << "subplot(3,1,1);" << endl;
    matlab << "plot(t*1e6, freq/1e9, 'LineWidth', 1.5);" << endl;
    matlab << "xlabel('Tiempo (\\mus)'); ylabel('Frecuencia (MHz)');" << endl;
    matlab << "title('\\it Frecuencia Instantánea del Chirp');" << endl;
    matlab << "grid on; grid minor;" << endl << endl;
    
    matlab << "subplot(3,1,2);" << endl;
    matlab << "plot(t*1e6, fase_hw, 'LineWidth', 1.5);" << endl;
    matlab << "xlabel('Tiempo (\\mus)'); ylabel('Fase (rad)');" << endl;
    matlab << "title('\\it Fase Acumulada');" << endl;
    matlab << "grid on; grid minor;" << endl << endl;
    
    matlab << "subplot(3,1,3);" << endl;
    matlab << "plot(t*1e6, trigger, 'LineWidth', 2);" << endl;
    matlab << "xlabel('Tiempo (\\mus)'); ylabel('Estado');" << endl;
    matlab << "title('\\it Señal Ramp Active (Trigger)');" << endl;
    matlab << "ylim([-0.2 1.2]); yticks([0 1]); yticklabels({'Silencio','Rampa'});" << endl;
    matlab << "grid on;" << endl << endl;
    
    matlab << "fprintf('\\nEstadísticas:\\n');" << endl;
    matlab << "fprintf(' Duración total: %.2f us\\n', max(t)*1e6);" << endl;
    matlab << "fprintf(' Ancho de banda: %.2f MHz\\n', (f_stop - f_start)/1e6);" << endl;
    matlab << "fprintf(' Tasa de barrido: %.2f MHz/us\\n', (f_stop - f_start)/t_ramp/1e6);" << endl;
    
    matlab.close();
    
    cout << "\n=== ARCHIVOS GENERADOS ===" << endl;
    cout << "  ✓ radar_data.csv - " << total_samples << " muestras" << endl;
    cout << "  ✓ plot_radar.m   - Script MATLAB" << endl;
    cout << "\nEjecuta en MATLAB: plot_radar" << endl;
    cout << "\n=== VERIFICACIÓN ===" << endl;
    cout << "Deberías ver:" << endl;
    cout << "  • Frecuencia: " << num_rampas << " rampas lineales (diente de sierra)" << endl;
    cout << "  • Fase: " << num_rampas << " curvas cuadráticas que resetean a 0" << endl;
    cout << "  • Trigger: " << num_rampas << " pulsos cuadrados alternando rampa/silencio" << endl;
    
    return 0;
}