#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include "FirebaseESP8266.h"
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
TinyGPSPlus gps;
SoftwareSerial mygps(D2, D1);  // GPS Tx Pin - NodeMCU D2  GPS Rx Pin NodeMCU D1

FirebaseData firebaseData;

const char* ssid = "GPS";                                                                 // Tên mạng Wi-Fi
const char* password = "88888888";                                                        // Mật khẩu mạng Wi-Fi
const char* apSSID = "ESP8266_AP";                                                        // Tên Wi-Fi AP
const char* apPassword = "12345678";                                                      // Mật khẩu Wi-Fi AP
const char* firebaseHost = "https://project-thuctap-144d3-default-rtdb.firebaseio.com/";  // Địa chỉ Firebase
const char* firebaseAuth = "DxL4ExigdpZyTRMDLNmF2QWVaMRj51U7FOQAAEjf";                    // Token xác thực Firebase

ESP8266WebServer server(80);

String checkemail;
String checkemailofAdmin;


String emailadmingps;
String emailusergps;
float Latitude, Longitude;

void setup() {
  Serial.begin(9600);
  mygps.begin(9600);
  Serial.println("GPS TESTING");
  EEPROM.begin(512);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Firebase.begin(firebaseHost, firebaseAuth);

  // Kiểm tra xem đã tồn tại dữ liệu trong EEPROM hay không
  checkemail = readFromEEPROM(0);
  String email = readFromEEPROM(0);
  String name = readFromEEPROM(email.length() + 1);
  String phone = readFromEEPROM(email.length() + name.length() + 2);
  String emailofadmin = readFromEEPROM(email.length() + name.length() + 2 + phone.length() + 1);
  checkemailofAdmin = readFromEEPROM(email.length() + name.length() + 2 + phone.length() + 1);

  emailusergps = readFromEEPROM(0);
  emailadmingps = readFromEEPROM(email.length() + name.length() + 2 + phone.length() + 1);


  if (email.length() > 0 && name.length() > 0 && phone.length() > 0 && emailofadmin.length() > 0) {
    // Nếu dữ liệu tồn tại, không cần tạo Wi-Fi AP và trang web
    Serial.println("Data already exists:");
    Serial.println("Email: " + email);
    Serial.println("Name: " + name);
    Serial.println("Phone: " + phone);
    Serial.println("Email of Admin: " + emailofadmin);
    delay(1000);
  } else {
    // Nếu không có dữ liệu, tạo Wi-Fi AP và trang web

    // Tạo Wi-Fi AP
    WiFi.softAP(apSSID, apPassword);
    Serial.println("Access Point started");
    Serial.print("IP Address: ");
    Serial.println(WiFi.softAPIP());

    // Định nghĩa trang web
    // tạo trang web
    server.on("/", HTTP_GET, handleRoot);
    server.on("/save", HTTP_POST, handleSave);
    server.begin();
    Serial.println("HTTP server started");
  }
  Firebase.setInt(firebaseData, "admin/" + emailadmingps + "/users/" + emailusergps + "/GPS", 1);
}

void loop() {
  server.handleClient();
  deleteEEPROM();
}

void deleteEEPROM() {
  Firebase.getInt(firebaseData, "admin/" + checkemailofAdmin + "/users/" + checkemail + "/deleteEEPROM");
  int checkdelete = firebaseData.intData();
  if (checkdelete == 1) {
    // Xóa toàn bộ EEPROM bằng cách ghi giá trị 0 vào từng địa chỉ
    for (int i = 0; i < 512; i++) {
      EEPROM.write(i, 0);
    }
    EEPROM.commit();
    Serial.println("EEPROM cleared");
    ESP.restart();
  }
}

void handleRoot() {
  String html = "<html><body>";
  html += "<h1>ESP8266 Web Config</h1>";
  html += "<form action='/save' method='POST'>";
  html += "Email of Admin: <input type='text' name='emailofadmin'><br><br>";
  html += "Email: <input type='text' name='email'><br><br>";
  html += "Name: <input type='text' name='name'><br><br>";
  html += "Number Phone: <input type='text' name='phone'><br><br>";
  html += "<input type='submit' value='Save'>";
  html += "</form>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}


String readFromEEPROM(int addr) {
  String data = "";
  char c;
  c = EEPROM.read(addr);
  while (c != '\0') {
    data += c;
    addr++;
    c = EEPROM.read(addr);
  }
  return data;
}

void handleSave() {
  String emailofadmin = server.arg("emailofadmin");
  String email = server.arg("email");
  String name = server.arg("name");
  String phone = server.arg("phone");

  // Loại bỏ các ký tự không mong muốn từ email
  email.replace("@", "");
  email.replace(".", "");

  // Loại bỏ các ký tự không mong muốn từ email
  emailofadmin.replace("@", "");
  emailofadmin.replace(".", "");

  // Lưu thông tin vào EEPROM
  Serial.println("Đang lưu dữ liệu vào EEPROM");
  int addr = 0;
  for (int i = 0; i < email.length(); i++) {
    EEPROM.write(addr, email[i]);
    addr++;
  }
  EEPROM.write(addr, '\0');  // Kết thúc chuỗi
  addr++;

  for (int i = 0; i < name.length(); i++) {
    EEPROM.write(addr, name[i]);
    addr++;
  }
  EEPROM.write(addr, '\0');  // Kết thúc chuỗi
  addr++;

  for (int i = 0; i < phone.length(); i++) {
    EEPROM.write(addr, phone[i]);
    addr++;
  }
  EEPROM.write(addr, '\0');  // Kết thúc chuỗi
  addr++;

  for (int i = 0; i < emailofadmin.length(); i++) {
    EEPROM.write(addr, emailofadmin[i]);
    addr++;
  }
  EEPROM.write(addr, '\0');  // Kết thúc chuỗi
  addr++;

  Serial.println("Lưu thành công!!!");
  EEPROM.commit();

  // Gửi dữ liệu lên Firebase
  Firebase.setString(firebaseData, "admin/" + emailofadmin + "/users/" + email + "/email", email);
  Firebase.setString(firebaseData, "admin/" + emailofadmin + "/users/" + email + "/name", name);
  Firebase.setString(firebaseData, "admin/" + emailofadmin + "/users/" + email + "/phone", phone);
  Firebase.setInt(firebaseData, "admin/" + emailofadmin + "/users/" + email + "/deleteEEPROM", 0);


  // Trả về trang web sau khi lưu dữ liệu
  String response = "Saved. <a href='/'>Exit</a>";
  server.send(200, "text/html", response);
  delay(1000);
  // Tắt Wi-Fi AP sau khi đã lưu xong dữ liệu
  WiFi.softAPdisconnect();
}
