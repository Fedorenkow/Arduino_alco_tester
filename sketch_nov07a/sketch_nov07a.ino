#include <LcdKeypad.h>
#include <LiquidCrystal.h>

#define Sober 500   // Максимальне значення, яке ми вважаємо "тверезим" (500 мг/л)
#define Drunk 2000   // Мінімальне значення, яке ми вважаємо "п'яним" (2000 мг/л)
#define MaxSensorValue 1000  // Максимальне значення датчика (1000 мг/л)
#define ledPin 6

float sensorValue;  // Змінна для збереження значення датчика

// Ініціалізуємо LCD екран
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// Крива калібрування (приклад)
float calibrationCurve[] = {50, 130, 240, 290, 429, 550, 700, 1000};  // Значення на датчику (мг/л)
float promilleValues[] = {0, 0, 0.47, 0.9, 2.0, 2.30, 3, 4};  // Відповідні концентрації в промілле (‰)

void setup() {
  Serial.begin(9600); // Встановлюємо швидкість послідовного порту на 9600
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // Ініціалізуємо LCD екран
  lcd.begin(16, 2);

  lcd.print("MQ3 Heating Up!");
  delay(20000); // Дозволяємо MQ3 прогрітися
  lcd.clear();
}

void loop() {
  sensorValue = analogRead(A1); // Читаємо аналогове значення з піна A1 (мг/л)
  float promille = mapFloat(sensorValue, calibrationCurve, promilleValues, 8);
  // Визначаємо стан
  if (promille<0.2) {
    lcd.clear();
    lcd.print("Sober: ");
    lcd.print(promille, 2); // Виводимо з однією десятковою знаком
  } else if (promille <1) {
    lcd.clear();
    lcd.print("Ok: ");
    lcd.print(promille, 2); // Виводимо з однією десятковою знаком
  } else {
    lcd.clear();
    lcd.print("Drunk: ");
    lcd.print(promille, 2); // Виводимо з однією десятковою знаком
  }

  // Перетворюємо значення з датчика в промілле (‰) за допомогою кривої калібрування
 
  
  Serial.print("Значення датчика (мг/л): ");
  Serial.print(sensorValue);
  Serial.print("  |  Концентрація алкоголю (‰): ");
  Serial.println(promille, 2); // Виводимо з однією десятковою знаком

  unsigned int outputValue = map(sensorValue, 0, MaxSensorValue, 0, 255);
  if (sensorValue > 700) {
    analogWrite(ledPin, outputValue); // Генеруємо сигнал ШИМ
  } else {
    digitalWrite(ledPin, LOW);
  }

  delay(2000); // Чекаємо 2 секунди для наступного вимірювання
}

// Функція для перетворення значення в новий діапазон за допомогою кривої калібрування
float mapFloat(float x, float *x_values, float *y_values, int size) {
  if (x <= x_values[0]) return y_values[0];
  if (x >= x_values[size - 1]) return y_values[size - 1];
  for (int i = 1; i < size; i++) {
    if (x <= x_values[i]) {
      float t = (x - x_values[i - 1]) / (x_values[i] - x_values[i - 1]);
      return y_values[i - 1] + t * (y_values[i] - y_values[i - 1]);
    }
  }
  return 0; // Не має повертатися сюди
}
