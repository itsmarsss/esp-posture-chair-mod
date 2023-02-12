#include <Arduino.h>
#include <AsyncTCP.h>
// #include <EasyAsyncTCP.h> for esp8266
#include <driver/rtc_io.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"
#include "time.h"
#include <WiFi.h>
#include <WiFiUdp.h>

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
            margin-bottom: 2rem;
            display: grid;
            place-items: center;
        }

        a {
            text-decoration: none;
        }

        button {
            margin-top: 1rem;
            padding: 12.5px 30px;
            border: 0;
            border-radius: 100px;
            background-color: #21a0f5;
            color: #ffffff;
            text-transform: uppercase;
            letter-spacing: 4px;
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
            width: 500px;
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


        .submit {
            padding: 10px 20px;
            border: none;
            font-size: 17px;
            color: #ffffff;
            border-radius: 7px;
            font-weight: 700;
            transition: 0.5s;
        }

        .submit {
            background: #3399FF;
            box-shadow: 0 0 12px #3399FF;
        }

        .submit:hover {
            box-shadow: 0 0 5px #3399FF,
                0 0 6px #3399FF,
                0 0 13px #3399FF,
                0 0 18px #3399FF;
        }
    </style>
</head>

<body>
    <div><a href="./testmessage">
            <button>
                Test Message
            </button>
        </a>
        <a href="./restart">
            <button style="margin-left: 1rem; margin-right: 1rem;">
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
            <b>SSID: </b><input style="width: 7rem;" type="text" name="ssid">
            <br>
            <b>Password: </b><input style="width: 7rem;" type="password" name="password">
            <hr>
            <b>UDP Address <i>(xxx.xxx.xxx.xxx)</i>: </b><input style="width: 7rem;" type="text" minlength="7"
                maxlength="15" size="15"
                pattern="^((\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.){3}(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$" name="udpAddress">
            <br>
            <b>UDP Port <i>(0 - 65536)</i>: </b><input style="width: 5rem;" type="number" min="0" max="65536"
                name="udpPort">
            <hr>
            <b>GMT Offset (s): </b><input style="width: 5rem;" type="number" name="gmtoffsetsec">
            <br>
            <b>Daylight Offset (s): </b><input style="width: 5rem;" type="number" name="daylightoffsetsec">
            <br>
            <b>Deep Sleep Time Out (ms): </b><input style="width: 6rem;" type="number" name="deeptimeout">
            <hr>
            <b>Lower Back GPIO: </b><input type="number" min="0" max="50" name="lowerbackgpio">
            <br>
            <b>Neck GPIO: </b><input type="number" min="0" max="50" name="neckgpio">
            <br>
            <b>Wake-up GPIO: </b><input type="number" min="0" max="50" name="wakeupgpio">
            <br>
            <button class="submit" type="submit" style="width: 100%; background-color: #2ba8fb;">Submit</button>
        </form>
    </div>
</body>

<script>
    fetch('/raw')
        .then(res => res.json())
        .then(out =>
            setData(out))
        .catch(err => {
            throw err;
            console.log('Error querying json');
            alert('Error getting settings... Please reload');
        });

    function setData(out) {
        document.getElementsByName("ssid")[0].value = out.SSID;
        document.getElementsByName("password")[0].value = out.Password;

        document.getElementsByName("udpAddress")[0].value = out.UDPAddress;
        document.getElementsByName("udpPort")[0].value = out.UDPPort;

        document.getElementsByName("gmtoffsetsec")[0].value = out.GMTOffset;
        document.getElementsByName("daylightoffsetsec")[0].value = out.DaylightOffset;
        document.getElementsByName("deeptimeout")[0].value = out.DeepSleepTimeOut;

        document.getElementsByName("lowerbackgpio")[0].value = out.LowerBackGPIO;
        document.getElementsByName("neckgpio")[0].value = out.NeckGPIO;
        document.getElementsByName("wakeupgpio")[0].value = out.WakeUpGPIO;
    }
</script>

</html>
)rawliteral";

AsyncWebServer server(80);

const char *ssid = "kk66036+";
const char *password = "29372F733EE5";

WiFiUDP udp;
String udpAddress = "192.168.0.12";
int udpPort = 8080;

long deeptimeout = 300000;
int lowerbackgpio = 18;
int neckgpio = 19;
int wakeupgpio = 15;
String logs = "";

const char *ntpServer = "pool.ntp.org";
long gmtOffset_sec = -18000;
long daylightOffset_sec = -18000;

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}

String getLocalTime()
{
  struct tm timeinfo;

  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
    return "Time Unavailable";
  }

  char timeStringBuff[50];
  strftime(timeStringBuff, sizeof(timeStringBuff), "%B-%d-%Y-%H:%M:%S", &timeinfo);

  String asString(timeStringBuff);
  return timeStringBuff;
}

boolean writeLogs()
{
  File file = SPIFFS.open("/" + getLocalTime() + ".txt", FILE_WRITE);

  logs += "~~~ End Of Logs ~~~\n";

  if (file.print(logs))
  {
    logs = "";
    file.close();
    return true;
  }
  else
  {
    file.close();
    return false;
  }
}

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
            {
    Serial.println("Index Requested");

    request->send(200, "text/html", index_html); });

  server.on("/settings", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    Serial.println("Settings Requested");

    request->send(200, "text/html", settings_html); });

  server.on("/testmessage", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    Serial.println("Message Requested");

    request->redirect("/settings");
    udp.beginPacket(udpAddress.c_str(), udpPort);
    uint8_t both[50] = "test";
    udp.write(both, 4);
    udp.endPacket(); });

  server.on("/restart", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    Serial.println("Restart Requested");

    request->redirect("/settings");
    delay(500);
    ESP.restart(); });

  server.on("/deepsleep", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    Serial.println("Deep Sleep Requested"); 
    
    request->redirect(settings_html);
    delay(500);
    esp_deep_sleep_start(); });

  server.on("/putsettings", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    request->redirect("/settings");

    if (request->hasParam("udpAddress"))
    {
      String input = request->getParam("udpAddress")->value();
      udpAddress = input;
    }

    if (request->hasParam("udpPort"))
    {
      String input = request->getParam("udpPort")->value();
      udpPort = atoi(input.c_str());
    }

    if (request->hasParam("gmtoffsetsec"))
    {
      String input = request->getParam("gmtoffsetsec")->value();
      gmtOffset_sec = atoi(input.c_str());
    }

    if (request->hasParam("daylightoffsetsec"))
    {
      String input = request->getParam("daylightoffsetsec")->value();
      daylightOffset_sec = atoi(input.c_str());
    }

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

    pinMode(lowerbackgpio, INPUT_PULLUP);
    pinMode(neckgpio, INPUT_PULLUP);
    pinMode(wakeupgpio, INPUT_PULLUP);

    Serial.println("Settings Update Requested");
    Serial.print("\tUDP Address: ");
    Serial.println(udpAddress);
    Serial.print("\tUDP Port: ");
    Serial.println(udpPort);
    Serial.print("\tGMT Offset: ");
    Serial.println(gmtOffset_sec);
    Serial.print("\tDaylight Offset: ");
    Serial.println(daylightOffset_sec);
    Serial.print("\tDeep Sleep Time Out: ");
    Serial.println(deeptimeout);
    Serial.print("\tLower Back GPIO: ");
    Serial.println(lowerbackgpio);
    Serial.print("\tNeck GPIO: ");
    Serial.println(neckgpio);
    Serial.print("\tWake-up GPIO: ");
    Serial.println(wakeupgpio); });

  server.on("/logs", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    Serial.println("Logs Requested");

    File root = SPIFFS.open("/");
    File file = root.openNextFile();

    String list = "<style>html { font-family: Arial, Helvetica, sans-serif; background: #393939; color: #ffffff; } a { color: #32a8ff } button { padding: 12.5px 30px; margin-top: 1rem; border: 0; border-radius: 100px; background-color: #e30f13; color: #ffffff; font-weight: Bold; transition: all 0.5s; -webkit-transition: all 0.5s; } button:hover { background-color: #c72c17; box-shadow: 0 0 20px #c72c1750; } button:active { background-color: #f5233f; transition: all 0.25s; -webkit-transition: all 0.25s; box-shadow: none; }</style>";
    
    while(file.available()) {
      list += "File: <a href=\"./getfile?filename=";
      list += file.name();
      list += "\">";
      list += file.name();
      list += "</a> | <a href=\"./removefile?filename=";
      list += file.name();
      list += "\"><button>Delete</button></a><br>";
      file = root.openNextFile();
    }

    list += "<h3><b>~~~ End Of Files ~~~</b></h3>";
    request->send(200, "text/html", list); });

  server.on("/getfile", HTTP_GET, [](AsyncWebServerRequest *request)
            { 
    Serial.println("File Download Requested: ");
    Serial.println(request->getParam("filename")->value()); 

    if (request->hasParam("filename")) {
      String file = request->getParam("filename")->value();
      File download = SPIFFS.open("/" + file, "r");

      AsyncWebServerResponse *response = request->beginResponse(200, "text/html", download.readString());
      
      response->addHeader("Content-Type", "text/text");
      response->addHeader("Content-Disposition", "attachment; filename=" + file);
      download.close();

      request->send(response);
    } });

  server.on("/removefile", HTTP_GET, [](AsyncWebServerRequest *request)
            { 
    Serial.println("File Removal Requested: ");
    Serial.println(request->getParam("filename")->value()); 

    if (request->hasParam("filename")) {
      AsyncWebServerResponse *response = request->beginResponse(200, "text/html", "File not found if not removed...");
      
      String file = request->getParam("filename")->value();
      File remove = SPIFFS.open("/" + file, "r");

      if (remove.available())
      {
        SPIFFS.remove("/" + file);
        remove.close();
      }

      request->redirect("./logs");
    } });

  server.on("/raw", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              String rawSettings = "{\n\"SSID\" : \"";
              rawSettings += ssid;
              rawSettings += "\",";

              rawSettings += "\n\"Password\" : \"";
              rawSettings += password;
              rawSettings += "\",";

              rawSettings += "\n\"UDPAddress\" : \"";
              rawSettings += udpAddress;
              rawSettings += "\",";

              rawSettings += "\n\"UDPPort\" : \"";
              rawSettings += udpPort;
              rawSettings += "\",";

              rawSettings += "\n\"GMTOffset\" : \"";
              rawSettings += gmtOffset_sec;
              rawSettings += "\",";

              rawSettings += "\n\"DaylightOffset\" : \"";
              rawSettings += daylightOffset_sec;
              rawSettings += "\",";

              rawSettings += "\n\"DeepSleepTimeOut\" : \"";
              rawSettings += deeptimeout;
              rawSettings += "\",";

              rawSettings += "\n\"LowerBackGPIO\" : \"";
              rawSettings += lowerbackgpio;
              rawSettings += "\",";

              rawSettings += "\n\"NeckGPIO\" : \"";
              rawSettings += neckgpio;
              rawSettings += "\",";

              rawSettings += "\n\"WakeUpGPIO\" : \"";
              rawSettings += wakeupgpio;
              rawSettings += "\"";

              rawSettings += "\n}";

              AsyncWebServerResponse *response = request->beginResponse(200, "json", rawSettings);

              response->addHeader("Access-Control-Allow-Origin", "*");
              response->addHeader("Access-Control-Allow-Methods", "GET, POST, PUT");
              response->addHeader("Access-Control-Allow-Headers", "Content-Type");

              request->send(response); });

  server.onNotFound(notFound);
  server.begin();

  SPIFFS.begin(true);

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  rtc_gpio_pulldown_en((gpio_num_t)wakeupgpio);
  esp_sleep_enable_ext0_wakeup((gpio_num_t)wakeupgpio, RISING);

  Serial.print(getLocalTime());
  Serial.println(": ESP Booted");

  logs += getLocalTime() + ": ESP Booted\n";
}

long lastActivity = millis();

void loop()
{
  Serial.println("Pinged");

  udp.beginPacket(udpAddress.c_str(), udpPort);
  uint8_t ping[50] = "ping";
  udp.write(ping, 4);
  udp.endPacket();

  if (digitalRead(wakeupgpio) == 0)
  {
    lastActivity = millis();
    if (digitalRead(lowerbackgpio) == 1 && digitalRead(neckgpio) == 1)
    {
      Serial.println("\tBoth");

      udp.beginPacket(udpAddress.c_str(), udpPort);
      uint8_t both[50] = "both";
      udp.write(both, 4);
      udp.endPacket();

      logs += getLocalTime() + ": Both\n";
    }
    else if (digitalRead(lowerbackgpio) == 1)
    {
      Serial.println("\tLower");

      udp.beginPacket(udpAddress.c_str(), udpPort);
      uint8_t low[50] = "low";
      udp.write(low, 3);
      udp.endPacket();

      logs += getLocalTime() + ": Back\n";
    }
    else if (digitalRead(neckgpio) == 1)
    {
      Serial.println("\tTop");

      udp.beginPacket(udpAddress.c_str(), udpPort);
      uint8_t top[50] = "top";
      udp.write(top, 3);
      udp.endPacket();

      logs += getLocalTime() + ": Neck\n";
    }
  }
  else
  {
    Serial.println("\tMoved");

    udp.beginPacket(udpAddress.c_str(), udpPort);
    uint8_t top[50] = "move";
    udp.write(top, 4);
    udp.endPacket();
    logs += getLocalTime() + ": Moved\n";
  }

  if (strstr(getLocalTime().c_str(), "11:59:59") != NULL)
  {
    if (!writeLogs())
    {
      logs += "~~~ Write Unsuccessful ~~~\n";
    }
  }

  if (millis() - lastActivity >= deeptimeout)
  {
    Serial.println("\tSleep");

    udp.beginPacket(udpAddress.c_str(), udpPort);
    uint8_t sleep[50] = "sleep";
    udp.write(sleep, 5);
    udp.endPacket();

    logs += getLocalTime() + ": Deep Sleep\n";
    delay(500);
    esp_deep_sleep_start();
  }

  delay(1000);
}
