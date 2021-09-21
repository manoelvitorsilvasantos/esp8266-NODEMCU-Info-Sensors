/*
   ESP8266 LLMNR responder sample
   Copyright (C) 2017 Stephen Warren <swarren@wwwdotorg.org>

   Based on:
   ESP8266 Multicast DNS (port of CC3000 Multicast DNS library)
   Version 1.1
   Copyright (c) 2013 Tony DiCola (tony@tonydicola.com)
   ESP8266 port (c) 2015 Ivan Grokhotkov (ivan@esp8266.com)
   MDNS-SD Support 2015 Hristo Gochkov
   Extended MDNS-SD support 2016 Lars Englund (lars.englund@gmail.com)

   License (MIT license):

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.

*/
/*
   This is an example of an HTTP server that is accessible via http://esp8266/
   (or perhaps http://esp8266.local/) thanks to the LLMNR responder.

   Instructions:
   - Update WiFi SSID and password as necessary.
   - Flash the sketch to the ESP8266 board.
   - Windows:
     - No additional software is necessary.
     - Point your browser to http://esp8266/, you should see a response. In most
       cases, it is important that you manually type the "http://" to force the
       browser to search for a hostname to connect to, rather than perform a web
       search.
     - Alternatively, run the following command from the command prompt:
       ping esp8266
   - Linux:
     - To validate LLMNR, install the systemd-resolve utility.
     - Execute the following command:
       systemd-resolve -4 -p llmnr esp8266
     - It may be possible to configure your system to use LLMNR for all name
       lookups. However, that is beyond the scope of this description.

*/

#include <ESP8266WiFi.h>
#include <ESP8266LLMNR.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include "config.h"


ESP8266WebServer web_server(8081);

void handle_http_not_found() {
  web_server.send(404, "text/plain", "Not Found");
}

void handle_http_root() {
  float amostra = (analogRead(0)*3.3/1024);
  float res = (amostra*100)/3.3;
  String body = "<!DOCTYPE html><html lang=pt-br><head><meta charset=utf-8><meta http-equiv='refresh' content='200'><meta name=viewport content='width=device-width, initial-scale=1.0'><title>Esp8266 WebServer</title><style>*{box-sizing:border-box;margin:0;padding:0}html{max-width:400px;margin:0 auto}body{background:skyblue;font-family:Arial}.App{background:#fff;box-shadow:2px 3px 3px 3px gray;border:1px solid #fff;border-radius:5px;padding:10px;margin:10px}.App-title{text-align:left;padding-left:10px;padding:10px}#action{background:#000;border-radius:5px;padding:5px;color:#fff;text-decoration:none;position:relative;font-weight:500;margin-left:10px}.dt{color:black;margin-bottom:10px;padding:10px}.temp{background:linear-gradient(180deg,#74937f,#4b5d51);border:1px solid gray;box-shadow:1px 1px 1px 1px gray;border-radius:5px;color:#b8c5bb;padding:10px}</style></head><body><div class=App><h1 class=App-title> IOT - ESP8266</h1><br/><hr><br><div class=dt><b><p class=temp>Temperatura %d </p></b><br><b><p class=temp>Humildade %f</p></b><br><b>";
         body += "<p class=temp>Dia ";
         body += res;
         body +="%</p></b><br><br></div></div></body></html>";
  web_server.send(200, "text/html", body);
}

void setup(void) {
  Serial.begin(115200);

  // Connect to WiFi network
  WiFi.mode(WIFI_AP_STA);

  WiFi.softAP(AP_SSID, AP_PASS);
  WiFi.softAPConfig(ip_local, gateway, subnet);
  
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  WiFi.config(wifi_ip, wifi_gateway, wifi_subnet);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println(AP_SSID);
  Serial.print("IP Local: ");
  Serial.println(WiFi.softAPIP());

  // Start LLMNR responder
  LLMNR.begin("esp8266");
  Serial.println("LLMNR responder started");
  // Start HTTP server
  web_server.onNotFound(handle_http_not_found);
  web_server.on("/", handle_http_root);
  web_server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  web_server.handleClient();
}
