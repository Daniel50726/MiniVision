# MiniVision

MiniVision es un sistema embebido diseñado para capturar y visualizar imágenes utilizando una Raspberry Pi Pico y una cámara OV7670. Este proyecto implementa un driver en C, capaz de interactuar con la cámara a través I2C para configuración y GPIO paralelos para datos de image, capturar una imagen y enviarla a una pantalla LCD integrada usando SPI para envío rápido de datos de píxeles.

<img width="1024" height="1536" alt="ChatGPT Image 12 jul 2025, 01_38_53 a m" src="https://github.com/user-attachments/assets/2e68d416-8e43-471b-a5fc-16265466416f" />

## Requisitos Funcionales

###  Procesamiento de datos
- Se inicializa la cámara y se configura el formato de captura RGB565.
- Se procesa la imagen en memoria, almacenando los datos de los píxeles capturados.
- Se realiza el procesamiento de los datos para ser visualizados correctamente en la pantalla.

###  Comunicación
- Se establece comunicación I2C con la cámara OV7670 para configurarla mediante sus registros.
- Se utiliza comunicación SPI para enviar los datos de imagen a la pantalla LCD SSD1283A.

###  Control
- Se utilizan GPIOs para manejar el encendido y apagado de la cámara y la pantalla.
- Se controla el inicio de la captura de imagen a través de un botón físico.

###  Interfaz de usuario
- La interfaz se basa en un solo botón físico que el usuario presiona para capturar una imagen.
- El resultado se muestra en pantalla de forma inmediata, permitiendo una interacción clara y sencilla.

###  Seguridad en el manejo de recursos
- Se libera el canal de DMA después de cada transferencia.
- Se limpian y desactivan las interrupciones una vez que cumplen su función.
- Se evitan condiciones de carrera o corrupción de memoria.

###  Tiempos de respuesta
- La captura y visualización de imagen se realiza en tiempos adecuados gracias al uso de DMA y SPI.
- No hay bloqueos visibles para el usuario durante el proceso.

### Cumplimiento de estándares
- Se utilizan los protocolos estándar I2C y SPI respetando sus especificaciones.
- El desarrollo se basa en el SDK oficial de la Raspberry Pi Pico, garantizando compatibilidad con la plataforma.

---

## Requisitos no funcionales

### Rendimiento
El sistema procesa imágenes a una resolución de 80x60 píxeles, mostrando cada captura en pantalla con baja latencia. Aunque no se cuenta con métricas formales de rendimiento, la respuesta visual es inmediata para el usuario final, lo que indica un buen desempeño en tiempo real para tareas básicas.

### Disponibilidad  
El sistema está diseñado para funcionar continuamente mientras la Raspberry Pi Pico esté encendida. No se han implementado mecanismos explícitos de manejo de fallos o reinicio automático.

### Fiabilidad 
El sistema opera de forma consistente para la captura y visualización de imágenes. Aunque no se dispone de pruebas formales de fiabilidad, la arquitectura es sencilla.

### Mantenibilidad

El código fuente está organizado de manera modular, con archivos separados por funciones específicas (pantalla, cámara, lógica principal). Esta estructura facilita su lectura, comprensión y futuras modificaciones. Sin embargo, no se cuenta con documentación formal de mantenimiento ni soporte para actualizaciones automáticas.

### Interoperabilidad
El sistema utiliza protocolos estándar de comunicación: I2C para la cámara y SPI para la pantalla. Esto garantiza cierta compatibilidad a nivel de hardware. No obstante, no se implementan APIs externas ni interacción directa con otros sistemas.

### Usabilidad 
El sistema es extremadamente sencillo de usar. Basta con conectar la fuente de alimentación, y el sistema queda listo para capturar imágenes presionando un solo botón físico. No requiere interfaz gráfica ni capacitación previa, lo que garantiza una experiencia de usuario intuitiva.

## Escenario de pruebas

En la prueba final, se mostró en vivo cómo el sistema capta una imagen del entorno real y la reproduce en la pantalla de la LCD, demostrando que la transmisión de datos, el procesamiento y la visualización están funcionando. Se evidencio cómo a pesar de la baja resolucion que se consiguió en el prototipo se hace el correcto funcionamiento de la pantalla LCD.

![WhatsApp Image 2025-07-12 at 1 46 39 AM](https://github.com/user-attachments/assets/feb096e6-5017-45d1-be15-d5772676e93f)

## Costos

| **Componente/Recurso**     | **Costo (COP)** |
|------------------------|-------------|
| Raspberry Pi Pico      | $33,000      |
| Cámara OV7670          | $23,000      |
| Pantalla SSD1283A      | $75,000      |
| Cables Dupont x10      | $5,000       |
| **Total**              | **$136,000** |

## Costos producción en masa

| **Componente/Recurso**     | **Costo (COP)** |
|------------------------|-------------|
| Raspberry Pi Pico      | $23,000      |
| Cámara OV7670          | $23,000      |
| Pantalla SSD1283A      | $75,000      |
| Case en impresión 3D   | $10,000      |
| Pulsador industrial    | $10,000      |
| Regulador de Voltaje   | $2,000       |
| Cables conectores      | $5,000       |
| Porta Pila             | $2,500       |
| **Total**              | **$153,500** |


