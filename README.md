# MiniVision

MiniVision es un sistema embebido diseñado para capturar y visualizar imágenes utilizando una Raspberry Pi Pico y una cámara Arducam Mini. Este proyecto tiene como objetivo implementar un driver en MicroPython, capaz de interactuar con la cámara a través del bus SPI, capturar una imagen y enviarla codificada en formato base64 mediante el puerto serial USB a un PC.

En el PC, una aplicación o script recibe la imagen base64, la decodifica y la muestra en pantalla, completando así el flujo de captura, transmisión y visualización. El entorno de desarrollo utilizado es Thonny IDE, lo que facilita la programación y depuración del sistema embebido.

![image](https://github.com/user-attachments/assets/38b868d8-6530-4406-86c6-19f0920182bf)


## Requisitos Funcionales

### Inicialización del Módulo de Cámara:

El driver debe detectar y configurar la cámara al arrancar el sistema:

●      Al encender la Pico, el driver lee la identificación del sensor (ID del fabricante y modelo) vía interfaz (I²C/SPI) y verifica que coincide con el esperado.

●      Si la identificación es correcta, configura los registros internos del sensor (resolución, formato de píxel, exposición inicial).

●      Si la identificación falla, devuelve un código de error y deja el bus libre para reintentos.

### Manejo de Frames de la Cámara:

Permitir la captura de un frame completo de la cámara en memoria RAM del Pico.

●      Al invocar una función, el driver activa la señal de disparo (VSYNC) y recibe datos de píxel hasta completar el frame.

●      El buffer destino se rellena sin overflow.

●      La función devuelve el tamaño real de datos recibidos y un código de estado (OK o ERROR_TIMEOUT).

### Comunicación con Host:

El driver debe permitir al microcontrolador leer el buffer de imagen vía SPI.

●      API: read_frame_spi(buffer, length).

●      La transferencia respeta la velocidad máxima del bus.

●      Se implementa CRC16 sobre el bloque de datos; si la verificación falla, devuelve ERROR_CRC.

### Gestión de Energía:

Implementar modos de bajo consumo para el sensor y la Pico cuando no hay captura activa.

●      API: enter_sleep_mode() y wake_from_sleep().

●      En modo sleep, la corriente total (Pico + sensor) debe reducirse en ≥60 %.

●      Al despertar, el sensor recupera su última configuración sin re‑inicialización completa.

## Requisitos no funcionales

### Rendimiento:

El sistema debe capturar una imagen de resolución mínima (160x120) y enviarla al PC en un tiempo menor a 10 segundos usando MicroPython y el puerto serial.

### Disponibilidad:

El sistema debe estar listo para recibir comandos desde el PC a través del puerto serial en menos de 5 segundos después de energizarse.

### Fiabilidad:

El sistema debe capturar y transferir imágenes correctamente al PC en al menos 90% de los intentos durante pruebas normales.

### Mantenibilidad:

El código en MicroPython debe estar estructurado en funciones bien separadas  y debe incluir comentarios que faciliten futuras modificaciones.

### Usabilidad:

El sistema debe aceptar comandos simples por consola serial en Thonny, como snap, y responder con mensajes legibles como Imagen capturada - Enviando....

### Consumo de energía:

Todo el sistema debe poder alimentarse exclusivamente por el puerto USB de la PC, sin requerir alimentación externa adicional.

### Interoperabilidad:

El sistema debe funcionar correctamente con el monitor serial integrado de Thonny IDE, sin requerir software externo para visualizar los datos enviados.

### Tiempos de respuesta:

El sistema debe responder al comando de captura (snap) con un mensaje de confirmación (OK, Imagen capturada, etc.) en menos de 1 segundo.

## Escenario de pruebas

El sistema está compuesto por una cámara Arducam Mini conectada a una Raspberry Pi Pico mediante el bus SPI. La Raspberry Pi Pico captura la imagen y la envía en formato base64 a través del puerto serial USB a un PC. En el PC, un programa recibe los datos, los decodifica y los muestra en pantalla. Durante la prueba final, se mostrará en vivo cómo el sistema capta una imagen del entorno real y la reproduce en la pantalla del PC, demostrando que la transmisión de datos, el procesamiento y la visualización están funcionando correctamente.

![ChatGPT Image 6 abr 2025, 07_12_35 p m](https://github.com/user-attachments/assets/eeb756c0-dea1-480f-9f47-2aea51ae2dd8)

## Costos

| **Componente/Recurso**     | **Costo (COP)** |
|------------------------|-------------|
| Raspberry Pi Pico      | $23,000      |
| Cámara Arducam Mini    | $110,000     |
| Cables Dupont x10      | $2,500       |
| **Total**              | **$135,500** |


