# Radar CW-FM en Banda C para Detección de Drones

**Trabajo Fin de Grado**  
Ingeniería de Tecnologías y Servicios de Telecomunicación  
Universidad de Zaragoza, 2026

---

## 👨‍🎓 Autor
Daniel Colás Sánchez

## 👨‍🏫 Directores
- Íñigo Salinas Áriz
- Carlos Heras Vila

---

## 📖 Descripción

Este repositorio contiene el código fuente desarrollado para el TFG sobre el diseño e implementación de un radar de onda continua con modulación en frecuencia (CW-FM) operando en banda C. El sistema está diseñado como herramienta de investigación aplicable a estudios de detección y caracterización de drones mediante análisis de firma micro-Doppler.

---

## 📁 Estructura del repositorio
```
tfg-radar-cwfm/
├── vitis_hls/
│   ├── radar_block.h
│   ├── radar_block.cpp
│   └── testbench.cpp
└── matlab/
    ├── plot_radar_sin_handshake.m
    ├── radar_data_sin_handshake.csv
```

---

## 🛠️ Tecnologías utilizadas

- **Vitis HLS 2023.x**: Síntesis de alto nivel para FPGA
- **Xilinx Zynq UltraScale+ MPSoC (RFSoC)**: Plataforma hardware
- **MATLAB R2020a+**: Análisis y visualización de señales

---

## 🚀 Cómo usar este código

### Simulación en Vitis HLS

1. Abrir Vitis HLS
2. Crear nuevo proyecto
3. Añadir `radar_block.cpp` como código fuente
4. Añadir `testbench.cpp` como test bench
5. Ejecutar C Simulation

### Visualización en MATLAB

1. Ejecutar el test bench en Vitis HLS (genera `radar_data_sin_handshake.csv`)
2. Abrir MATLAB
3. Navegar a la carpeta `matlab/`
4. Ejecutar: `plot_radar_sin_handshake`

---

## 📄 Documentación completa

La memoria completa del TFG, incluyendo fundamento teórico, diseño del sistema y resultados experimentales, está disponible en el repositorio institucional de la Universidad de Zaragoza.

---

## 📧 Contacto

Para consultas sobre este proyecto: 815165@unizar.es

---

## 📜 Licencia

Este proyecto está bajo licencia MIT. Consulta el archivo `LICENSE` para más detalles.
```
