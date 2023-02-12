#include <Arduino.h>
#include <AsyncTCP.h>
// #include <EasyAsyncTCP.h> for esp8266
#include <driver/rtc_io.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"
#include <WiFi.h>
#include <WiFiUdp.h>

const char *ssid = "kk66036+";
const char *password = "29372F733EE5";

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>

<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="icon" href="data:,">
    <style>
        body {
            width: 100vh;
            font: Helvetica;
            background-color: lightgrey;
            margin: 0;
            padding: 0;
            overflow: hidden;
        }

        a {
            text-decoration: none;
        }

        div {
            margin: 0;
            padding: 0;
            width: 100vw;
        }

        button {
            height: 100vh;
            width: 50vw;
            position: absolute;
            font-size: 2rem;
            font-weight: bold;
            border-style: none;
            transition: 300ms;
            color: #eeebe9;
            top: 0%;
        }

        button:hover {
            opacity: 95%;
            font-size: 2.5rem;
            color: #ffffff;
        }
    </style>
</head>

<body>
    <div>
        <a href="./settings">
            <button style="
            background-color: #2c2c2c;
            left: 0%;
            ">Settings</button>
        </a>
        <a href="./logs">
            <button style="
            background-color: #007acc;
            left: 50%;
            ">View Logs</button>
        </a>
    </div>
</body>

</html>
)rawliteral";

const char settings_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>

<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="icon" href="data:,">
    <style>
        html {
            font-family: Arial, Helvetica, sans-serif;
            background: #393939;
            color: #ffffff;
        }

        body {
            margin-top: 2rem;
            display: grid;
            place-items: center;
        }

        a {
            text-decoration: none;
        }

        button {
            padding: 12.5px 30px;
            border: 0;
            border-radius: 100px;
            background-color: #21a0f5;
            color: #ffffff;
            font-weight: Bold;
            transition: all 0.5s;
            -webkit-transition: all 0.5s;
        }

        button:hover {
            background-color: #6fc5ff;
            box-shadow: 0 0 20px #6fc5ff50;
        }

        button:active {
            background-color: #3d94cf;
            transition: all 0.25s;
            -webkit-transition: all 0.25s;
            box-shadow: none;
        }

        input {
            max-width: 190px;
            background-color: #f5f5f5;
            color: #242424;
            padding-bottom: 2px;
            margin-top: .5rem;
            margin-bottom: .5rem;
            min-height: 30px;
            width: 3rem;
            border-radius: 4px;
            outline: none;
            border: none;
            line-height: 1.15;
            box-shadow: 0px 10px 20px -18px;
        }

        input:focus {
            padding-bottom: 0px;
            border-bottom: 2px solid #3d94cf;
            border-radius: 4px 4px 2px 2px;
        }

        input:hover {
            outline: 1px solid lightgrey;
        }

        .cont {
            width: 300px;
            border-radius: 50px;
            background-color: #252525;
            border-width: 2px;
            padding: 2rem;
            padding-top: 0.25rem;
            margin-top: 4rem;
            box-shadow: 15px 15px 30px #202020,
                -15px -15px 30px #555555;
            transition: 200ms;
        }

        h2 {
            margin-bottom: 0;
        }
    </style>
</head>

<body>
    <div><a href="./testmotor">
            <button>
                Test Motor
            </button>
        </a>
        <a href="./restart">
            <button>
                Restart
            </button>
        </a>
        <a href="./deepsleep">
            <button>
                Deep Sleep
            </button>
        </a>
    </div>

    <div class="cont">
        <form action="/putsettings">
            <h2>Settings</h2>
            <br>
            <b>Deep Sleep Time Out (ms): </b><input class="input" type="number" name="deeptimeout">
            <br>
            <b>Lower Back GPIO: </b><input type="number" name="lowerbackgpio">
            <br>
            <b>Neck GPIO: </b><input type="number" name="neckgpio">
            <br>
            <b>Wake-up GPIO: </b><input type="number" name="motorgpio">
            <br>
            <button type="submit" style="width: 100%; background-color: #2ba8fb;">Submit</button>
        </form>
    </div>
</body>

</html>
)rawliteral";

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}

AsyncWebServer server(80);

long deeptimeout = 300000;
int lowerbackgpio = 18;
int neckgpio = 19;
int wakeupgpio = 15;
String logs = "";

void setup()
{
  Serial.begin(115200);

  pinMode(lowerbackgpio, INPUT_PULLUP);
  pinMode(neckgpio, INPUT_PULLUP);
  pinMode(wakeupgpio, INPUT_PULLUP);

  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", index_html); });

  server.on("/settings", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(404, "text/html", settings_html); });

  server.on("/restart", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    request->send_P(200, "text/html", settings_html);
    Serial.println("Restart Requested");
    delay(500);
    ESP.restart(); });

  server.on("/deepsleep", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    request->send_P(200, "text/html", settings_html);
    Serial.println("Deep Sleep Requested");
    delay(500);
    esp_deep_sleep_start(); });

  server.on("/putsettings", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    if (request->hasParam("deeptimeout")) {
      String input = request->getParam("deeptimeout")->value();
      deeptimeout = atoi(input.c_str());
    }
    if (request->hasParam("lowerbackgpio")) {
      String input = request->getParam("lowerbackgpio")->value();
      lowerbackgpio = atoi(input.c_str());
    }
    if (request->hasParam("neckgpio")) {
      String input = request->getParam("neckgpio")->value();
      neckgpio = atoi(input.c_str());
    }
    if (request->hasParam("wakeupgpio")) {
      String input = request->getParam("wakeupgpio")->value();
      wakeupgpio = atoi(input.c_str());
      rtc_gpio_pulldown_en((gpio_num_t) wakeupgpio);
      esp_sleep_enable_ext0_wakeup((gpio_num_t) wakeupgpio, RISING);
    }
    Serial.print("Deep Sleep Time Out: ");
    Serial.println(deeptimeout);
    Serial.print("Lower Back GPIO: ");
    Serial.println(lowerbackgpio);
    Serial.print("Neck GPIO: ");
    Serial.println(neckgpio);
    Serial.print("Wake-up GPIO: ");
    Serial.println(wakeupgpio);

    pinMode(lowerbackgpio, INPUT_PULLUP);
    pinMode(neckgpio, INPUT_PULLUP);
    pinMode(wakeupgpio, INPUT_PULLUP);

    request->send(200, "text/html", settings_html); });

  server.on("/logs", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    File root = SPIFFS.open("/");
    //request->send(404, "text/plain", "Not implemented");
    File file = root.openNextFile();
    while(file) {
      file = root.openNextFile();
      Serial.println(file.name());
    } });

  server.onNotFound(notFound);
  server.begin();

  SPIFFS.begin(true);

  rtc_gpio_pulldown_en((gpio_num_t)wakeupgpio);
  esp_sleep_enable_ext0_wakeup((gpio_num_t)wakeupgpio, RISING);
}

WiFiUDP udp;
const char *udpAddress = "192.168.0.12";
long lastActivity = millis();

void loop()
{
  Serial.println("Pinged");
  udp.beginPacket(udpAddress, 8080);
  uint8_t ping[50] = "ping";
  udp.write(ping, 4);
  udp.endPacket();

  if (digitalRead(wakeupgpio) == 0)
  {
    if (digitalRead(lowerbackgpio) == 1 && digitalRead(neckgpio) == 1)
    {
      Serial.println("\tBoth");
      udp.beginPacket(udpAddress, 8080);
      uint8_t both[50] = "both";
      udp.write(both, 4);
      udp.endPacket();
    }
    else if (digitalRead(lowerbackgpio) == 1)
    {
      Serial.println("\tLower");
      udp.beginPacket(udpAddress, 8080);
      uint8_t low[50] = "low";
      udp.write(low, 3);
      udp.endPacket();
    }
    else if (digitalRead(neckgpio) == 1)
    {
      Serial.println("\tTop");
      udp.beginPacket(udpAddress, 8080);
      uint8_t top[50] = "top";
      udp.write(top, 3);
      udp.endPacket();
    }
  }
  if (millis() - lastActivity >= deeptimeout)
  {
    Serial.println("\tSleep");
    udp.beginPacket(udpAddress, 8080);
    uint8_t sleep[50] = "sleep";
    udp.write(sleep, 5);
    udp.endPacket();
    delay(500);
    esp_deep_sleep_start();
  }

  delay(1000);
}
