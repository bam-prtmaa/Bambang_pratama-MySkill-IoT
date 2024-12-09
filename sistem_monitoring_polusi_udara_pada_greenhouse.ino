#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_NAME "Final Project"
#define BLYNK_AUTH_TOKEN "Grmze305pW3VVLN_Ao7mVUqyuWzkVWIf" 

enum State {
  Aman,
  Siaga,
  Bahaya
};

State state=Aman;

int sensor = 36;
char auth[] = "your_auth_token";
char ssid[] = "Your_ssid_wifi";
char pass[] = "your_password_wifi";
long before = 0;
long before2= 0;

int merah = 18;
int hijau = 15;
int buzzer = 13;  // Pin untuk buzzer
int relay = 21;
bool isNotified = false;

BlynkTimer timer;

void setcolor(int merahh, int hijauu) {
  digitalWrite(merah, merahh);
  digitalWrite(hijau, hijauu);
}

void statemachine() {
  switch(state) {
    case Aman:
      Blynk.virtualWrite(V1, "Aman");
      Blynk.logEvent("Normal", "Kadar CO2 Normal");
      setcolor(0, 255); // Hijau (Aman)
      noTone(buzzer); // Matikan buzzer
      digitalWrite(relay, HIGH);
      isNotified = false; // Reset flag saat kondisi kembali aman
      break;
    
    case Siaga:
      Blynk.virtualWrite(V1, "Siaga");
      setcolor(255, 255); // Kuning (Hati-hati)
      noTone(buzzer); // Matikan buzzer
      digitalWrite(relay, HIGH);
      isNotified = false; // Reset flag jika kondisi masih di bawah bahaya
      break;

    case Bahaya:
      Blynk.virtualWrite(V1, "Bahaya");
      setcolor(255, 0); // Merah (Bahaya)
      tone(buzzer, 1000, 3000); // Aktifkan buzzer
      digitalWrite(relay, LOW);

      if (!isNotified) { // Kirim notifikasi hanya jika belum terkirim
        Blynk.logEvent("Bahaya");
        isNotified = true; // Tandai bahwa notifikasi telah dikirim
      }
      break;
  }
}

void sendsensor(){
  statemachine();
  long now = millis();
  int co2now[10];                               //int array for co2 readings                              //int for raw value of co2
  int ppm = 0;                               //int for calculated ppm
  int gabung = 0; 

  for(int x=0; x<10; x++){
    if(now-before2 >=500){
    co2now[x]=analogRead(sensor);
    }
  }

  for(int x=0; x<10; x++){
    gabung=gabung+co2now[x];
  }
  ppm = gabung/10; 

  if(now - before > 1000) {
    before = now;
    Blynk.virtualWrite(V0, ppm);
    Serial.print("co2:"); 
    Serial.println(ppm);
  }
    if(ppm < 700) {
      state=Aman;
    }
    else if(ppm >= 700 && ppm < 1000) {
      state=Siaga;
    }
    else if(ppm >= 1000) {
      state=Bahaya;
    }
  }


void setup() {
  Serial.begin(115200); // Inisialisasi komunikasi serial
  Serial.println("Connecting to Blynk...");
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 8080); // Hubungkan ke Blynk
  pinMode(merah, OUTPUT);
  pinMode(hijau, OUTPUT);
  pinMode(buzzer, OUTPUT); // Atur pin buzzer sebagai output
  pinMode(relay, OUTPUT);
  pinMode(sensor, INPUT);
  timer.setInterval(1000L, sendsensor);
}

void loop() {
  Blynk.run(); // Pastikan koneksi ke server Blynk tetap berjalan
  sendsensor(); // Panggil fungsi kirim data sensor
}
