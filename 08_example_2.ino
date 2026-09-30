// Pin assignment
#define PIN_LED   9     // LED (must support PWM)
#define PIN_TRIG 12     // Ultrasonic sensor TRIGGER
#define PIN_ECHO 13     // Ultrasonic sensor ECHO

// Parameters
#define SND_VEL 346.0       // sound velocity at 24°C (m/s)
#define INTERVAL 25         // sampling interval (ms)
#define PULSE_DURATION 10   // ultrasound pulse duration (µs)
#define DIST_MIN 100.0      // minimum distance (mm)
#define DIST_MAX 300.0      // maximum distance (mm)

#define TIMEOUT ((INTERVAL / 2) * 1000.0)  // max echo waiting time (µs)
#define SCALE (0.001 * 0.5 * SND_VEL)      // convert duration → distance (mm)

unsigned long last_sampling_time = 0;      // last measurement time

void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);
  
  Serial.begin(57600);
}

void loop() {
  // wait for next sampling interval
  if (millis() < (last_sampling_time + INTERVAL))
    return;

  float distance = USS_measure(PIN_TRIG, PIN_ECHO); // measure distance
  int duty = 255; // default: OFF (active low)

  if (distance >= DIST_MIN && distance <= DIST_MAX) {
    if (distance == 150 || distance == 250) {
      // Force 50% brightness
      duty = 128;
    } else if (distance <= 200) {
      // From 100 → 200 mm: brighter
      duty = map(distance, 100, 200, 255, 0);
    } else {
      // From 200 → 300 mm: dimmer
      duty = map(distance, 200, 300, 0, 255);
    }
  }

  // Write brightness (PWM, active low)
  analogWrite(PIN_LED, duty);

  // === Serial Plotter output (numeric only) ===
  Serial.print("Min:");        Serial.print(DIST_MIN);
  Serial.print(",Distance:");  Serial.print(distance);
  Serial.print(",Duty:");      Serial.print(duty);
  Serial.print(",Max:");       Serial.print(DIST_MAX);
  Serial.println("");

  // update last sampling time
  last_sampling_time += INTERVAL;
}

// === Ultrasonic measurement function ===
// Returns distance in mm
float USS_measure(int TRIG, int ECHO) {
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);

  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE; // mm
}
