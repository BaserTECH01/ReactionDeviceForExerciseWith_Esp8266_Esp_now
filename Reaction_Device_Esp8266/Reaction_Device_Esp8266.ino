//antrenman reaksiyon cihazı 

// butonun bir pini gnd ye  diğeri aşağıdaki belirlenen pine bağlanacak 
// ledin bir pini gnd ye  diğeri aşağıdaki belirlenen pine bağlanacak ledin yönüne dikkat ediniz 


#include <espnow.h> // esp now iletişim protokolü için kullanılan kütüphane

#include <ESP8266WiFi.h> // esp8266 için kerekli olan kütüphane

int buttonPin = D3; //Butonu ve bağlı olduğu pin için tanımlanan değişken

int ledPin =  D4;   //Led ve bağlı olduğu pin için tanımlanan değişken

int randNumber;  //Random sayı için tanımlanan değişken

int ledstate = 1; //led durumunu kaydetmek için tanımlanan değişken // sadece bir board da 1 olarak belirlenmeli diğerlerinde 0

uint8_t ESP_NOW_RECEIVER1[] = { 0xBC, 0xFF, 0x4D, 0x19, 0x57, 0x09 }; // diğer boardun mac adresi
uint8_t ESP_NOW_RECEIVER2[] = { 0xE8, 0xDB, 0x84, 0x9B, 0x08, 0x4A }; // diğer boardun mac adresi


//iletişimde kullanılacak veri yapısı
typedef struct struct_message { 
    int state=1;

} struct_message;

struct_message myData; //giden data için 
struct_message incomData; //gelen data için 


//diğer boardan mesaj geldiğinde çalışacak fonksiyon
void onReceive(uint8_t *mac_addr, uint8_t *data, uint8_t len) {

    //mesajı gönderen boardın mac adresini seri ekrana yazdırmak için 
    Serial.printf("received: %3u from %02x:%02x:%02x:%02x:%02x:%02x\n",
        (uint8_t) *data,
        mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]
    );

//gelen mesajı gelen mesaj yapısına kaydetmek için
memcpy(&incomData, data, sizeof(incomData));


  //led durumu için tanımlanan değişkeni gelen mesajdaki yaoıya göre değiştirmek için
   ledstate=incomData.state;
  delay(50);
  
  //ledi led durumu değişkeni ile durmunu güncellemek için
  digitalWrite(ledPin, ledstate);
  delay(50);

}

//veri gittiğinde çalışacak fonksiyon
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("\r\nLast Packet Send Status:\t");
  if (sendStatus == 0){
    //mesaj gönderiminin başarılı veya başarısız mı olduğunu kontrol etmek için
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
}


void setup() {
  
    Serial.begin(9600);
    
    WiFi.mode(WIFI_STA);//wifi modunu için
    
    WiFi.disconnect();//wifi bağlantısı varsa koparmak için

    //esp now iletişimini başlatmak için
    if (esp_now_init() != 0) {
      Serial.println("Error initializing ESP-NOW");
      return;
    }


    
    // mesaj geldiğinde ve gittiğinde çalışacak fonksiyonları belirtmek için
  esp_now_register_recv_cb(onReceive);  
  esp_now_register_send_cb(OnDataSent);
  
  //esp now rolü belirlemek için
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  
  //diğer boardlar ile eşleşmek için
  esp_now_add_peer(ESP_NOW_RECEIVER1, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
  esp_now_add_peer(ESP_NOW_RECEIVER2, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);

  
  //ledin bağlı olduğu pini çıkış olarak ayarlamak için
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledstate);
  
  //butonun bağlı olduğu durumu giriş olarak ayarlamak için
  pinMode(buttonPin, INPUT_PULLUP);

}

void loop() {
//random sayı değişkenini 0 ve 2 arsında üretilen sayı ile güncellemek için  
randNumber = random(0, 2);

//eğer butona basılmışsa ve led durumu açıksa ve random sayı sıfırsa çalışacak kod
if(digitalRead(buttonPin)==0&&ledstate==1&&randNumber==0){
  
 //diğer boarrdalaradan birine giden data yapısını göndermek için 
 
esp_now_send(ESP_NOW_RECEIVER1, (uint8_t *) &myData, sizeof(myData));

//led durumunu kapalı olarak ayarlamak için
ledstate=0;
digitalWrite(ledPin, ledstate);
}

//eğer butona basılmışsa ve led durumu açıksa ve random sayı birse çalışacak kod
if(digitalRead(buttonPin)==0&&ledstate==1&&randNumber==1){
  
  //diğer boarrdalaradan birine giden data yapısını göndermek için 
esp_now_send(ESP_NOW_RECEIVER2, (uint8_t *) &myData, sizeof(myData));

//led durumunu kapalı olarak ayarlamak için
ledstate=0;
digitalWrite(ledPin, ledstate);
}
}




   
