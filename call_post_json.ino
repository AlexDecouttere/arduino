#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>
#include <Keypad.h>

int relai = 11;
const byte LIGNE = 4;    // 4 lignes
const byte COLONNE = 4;  // 4 colonnes
char touche;
String input_password;

//Déclaration des touches du clavier
char hexaKeys[LIGNE][COLONNE] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '0', 'F', 'E', 'D' }
};


//Affectation des broches du clavier
byte ligne_pin[LIGNE] = { 2, 3, 4, 5 };
byte colonne_pin[COLONNE] = { 6, 7, 8, 9 };
Keypad clavier = Keypad(makeKeymap(hexaKeys), ligne_pin, colonne_pin, LIGNE, COLONNE);


const char* ssid = "Alex";
const char* password = "Zg5shalex";

//Your Domain name with URL path or IP address with path
const char* host = "locbuy-dwlpupmbfa-ew.a.run.app";
const char* hostAuth = "locbuy-auth-dwlpupmbfa-ew.a.run.app";
const String data = "{\"client\":\"Aiolia\"}";
const char* pathAuth = "/auth/token";
const char* path = "/sites";
const char* serverName = "httpbin.org";

String token;

WiFiClient wifi;
WiFiSSLClient clientSSL;

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.begin(9600);
  input_password.reserve(32);
  if (clientSSL.connect(hostAuth, 443)) {
    Serial.println("connected to token service");
    clientSSL.print(String("POST ") + pathAuth + " HTTP/1.1\r\n" + "Host: " + hostAuth + "\r\n" +
                    //"Connection: close\r\n" +
                    "Content-Type: application/json\r\n" + "Content-Length: " + data.length() + "\r\n" + "\r\n" + data + "\n");

    delay(100);
    String tokenResponse = "";
    while (clientSSL.connected()) {
      if (clientSSL.available()) {
        // read an incoming byte from the server and print it to serial monitor:
        char c = clientSSL.read();
        tokenResponse = tokenResponse + c;
      } else {
        clientSSL.stop();
      }
    }
    String tokenTemp = tokenResponse.substring(tokenResponse.length() - 156);
    tokenTemp.remove(tokenTemp.length() - 1);
    tokenTemp.remove(tokenTemp.length() - 1);
    tokenTemp.remove(tokenTemp.length() - 1);
    token = tokenTemp;
    Serial.println(token);
  }
  pinMode(relai, OUTPUT);
}



void loop() {
  digitalWrite(relai, LOW);
  String response = "";
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, LOW);
    char key = clavier.getKey();
    if (key) {
      delay(250);
      if (key == 'E') {
        input_password = "";
        Serial.println("Input has been reset.");  // reset the input password
      } else if (key == 'D') {
        digitalWrite(LED_BUILTIN, HIGH);
        if (clientSSL.connect(host, 443)) {
          Serial.println("connected to gcp");

          clientSSL.println("GET /sites HTTP/1.1");
          clientSSL.println("Host: locbuy-dwlpupmbfa-ew.a.run.app");
          clientSSL.println("Authorization: " + token);
          clientSSL.println();
          delay(1000);
          String tokenResponse = "";
          while (clientSSL.connected()) {
            if (clientSSL.available()) {
              char c = clientSSL.read();
              tokenResponse = tokenResponse + c;
            } else {
              clientSSL.stop();
            }
          }
          Serial.println();
          for (int t=0; t<866; t++)
            {
              tokenResponse.remove(tokenResponse.length() - 1);
            }
          response = tokenResponse;
          Serial.println("response: "+response);
        }
        if(response = "HTTP/1.1 200 OK"){
          digitalWrite(relai, HIGH);
          Serial.println("casier: ouvert");
          delay(5000);
        }
        Serial.println("casier: ferme");


        input_password = "";
      } else {
        input_password += key;
        Serial.println("Current input: " + input_password);
      }
    }
  }
}
