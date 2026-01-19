#include <LedControl.h>
#include <Encoder.h>
#define ENCODER_USE_INTERRUPTS

// --- Configuración Display (MAX7219) ---
const int PIN_DIN = 48;
const int PIN_CS  = 49;
const int PIN_CLK_DISP = 50;

const int BTN_PIN = 5;

const int DISP_COUNT = 2;

LedControl lc = LedControl(PIN_DIN, PIN_CLK_DISP, PIN_CS, DISP_COUNT);
Encoder knob(2,4);

long value =0;
long oldPosition  = 0;

unsigned long ultimaLimpieza = 0;


void setup() {
  Serial.begin(57600);
  Serial.println("Basic Encoder Test:");

  pinMode(BTN_PIN, INPUT);

  // Configurar Display
  for (int i =0; i< DISP_COUNT; i++) {
    lc.shutdown(i, false);
    lc.setIntensity(i, 1);
    lc.clearDisplay(i);
    lc.setScanLimit(i, 8);
  }
  
  value = 10000;
  showValue(value);
}

void loop() {
  long newPosition = knob.read();
  int buttonState = digitalRead(BTN_PIN);
  long* old = &oldPosition;

  if (buttonState == LOW) {
    value = 10000;
    showValue(value);
  }

  int diff = abs(newPosition - *old);


  if (diff >=4) {
    if (newPosition > *old) {
      value+= 100;
    } else {
      value-= 100;
    }

    if(value < 0 || value > 99999999) {
      value = 0;
    }
    oldPosition = newPosition;
    Serial.println(value);
    showValue(value);
  }

  // --- REFUERZO DE SEGURIDAD ---
  // Cada 5 segundos refrescamos los registros del chip por si hubo ruido
  if (millis() - ultimaLimpieza > 5000) {
    lc.shutdown(0, false);  // Asegura que no se haya ido a dormir solo
    lc.setIntensity(0, 1);   // Asegura que el brillo no suba solo
    ultimaLimpieza = millis();
  }

  delay(50);
}

void showValue(long n) {
  lc.clearDisplay(0);
  lc.clearDisplay(1);
  
  if (n == 0) {
    lc.setDigit(0, 0, 0, false); // Si es cero, mostrar un 0 en la posición inicial
    lc.setDigit(1, 0, 0, false); // Si es cero, mostrar un 0 en la posición inicial
    return;
  }

  int column = 0;
  while (n > 0 && column < 8) {
    byte digito = n % 10;      // Obtenemos el último número (ej: de 123, saca el 3)
    lc.setDigit(0, column, digito, false);
    lc.setDigit(1, column, digito, false);
    n = n / 10;                // Quitamos el último número (ej: de 123, queda 12)
    column++;                 // Pasamos a la siguiente posición del display
  }
}