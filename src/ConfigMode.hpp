#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <DNSServer.h>

ESP8266WebServer server(WIFI_AP_CONFIG_PORT);
ESP8266HTTPUpdateServer httpUpdater;
DNSServer dnsServer;
const byte DNS_PORT = 53;

String config_form = R"html(
<!DOCTYPE HTML><html>
<style>label,input{margin:9px;padding:3px;font-size:1em;display:inline-block;width:15em;}
body{text-align:center;font: italic small-caps bold 1.2em Georgia,sans-serif;background-color:lightblue;}
b{animation: blinker 1s linear infinite;} @keyframes blinker{50%{opacity: 0;}}</style>
<form method='get' action='config'>
<br/><p>{name}</p>
<b>{error}</b><br/>
<p><label>WiFi setup</label><br/>
<label>SSID: </label><input type="text" name="ssid" length=16 required="required" value="{ssid}"><br/>
<label>Password:  </label><input type="text" name="pass" length=16 value="{pass}"></p>
<p><label>MQTT setup</label><br/>
<label>Host: </label><input type="text" name="host" length=32 placeholder="Name or IP.." required="required" value="{host}"><br/>
<label>Port: </label><input type="number" name="port" min="1" max="65535" value="{port}"><br/>
<label>User:  </label><input type="text" name="user" length=16 value="{user}"><br/>
<label>Password:  </label><input type="text" name="mqttPass" length=16 value="{mqttPass}"><br/>
<label>Retain:  </label><input type="checkbox" name="retain"></p>
<p><label>MQTT topics</label><br/>
<label>Feed: </label><input type="text" name="data" length="20" required="required" value="{data}"><br/>
<label>Init: </label><input type="text" name="online" length="20" value="{online}"><br/>
<label>Will: </label><input type="text" name="offline" length="20" value="{offline}"></p>
<input type='submit' value="Apply"><br>
<p>{vendor}<br>{fw} firmware<br/>{hw} hardware</p>
</form>
</html>
)html";

void esperRestart() {
  ESP.restart();
}

void esperConfigMode()
{
  EsperState::set(MODE_CONFIGURING);

  WiFi.mode(WIFI_OFF);
  delay(100);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(WIFI_AP_IP, WIFI_AP_IP, WIFI_AP_Subnet);
  WiFi.softAP(PRODUCT_WIFI_SSID);
  delay(500);
  IPAddress myIP = WiFi.softAPIP();
  E_DEBUG("AP SSID: %s", PRODUCT_WIFI_SSID);
  E_DEBUG("AP IP: %i.%i.%i.%i", myIP[0], myIP[1], myIP[2], myIP[3]);

  if (myIP == (uint32_t)0) {
    EsperState::set(MODE_ERROR);
    return;
  }

  // Set up DNS Server
  dnsServer.setTTL(300); // Time-to-live 300s
  dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure); // Return code for non-accessible domains
  dnsServer.start(DNS_PORT, BOARD_CONFIG_AP_URL, WiFi.softAPIP());
  E_DEBUG("AP URL: %s ",BOARD_CONFIG_AP_URL);

  httpUpdater.setup(&server);

  server.on("/", []() {
    config_form.replace("{name}", BOARD_NAME);
    config_form.replace("{vendor}", BOARD_VENDOR);
    config_form.replace("{fw}", BOARD_FIRMWARE_VERSION);
    config_form.replace("{hw}", BOARD_HARDWARE_VERSION);
    config_form.replace("{error}", EsperErrors.message);
    config_form.replace("{ssid}", EsperConfig.wifiSSID);
    config_form.replace("{pass}", EsperConfig.wifiPass);
    config_form.replace("{host}", EsperConfig.mqttHost);
    config_form.replace("{port}", String(EsperConfig.mqttPort));
    config_form.replace("{user}", EsperConfig.mqttUser);
    config_form.replace("{mqttPass}", EsperConfig.mqttPass);
    config_form.replace("{retain}", EsperConfig.mqttRetain ? "1" : "0");
    config_form.replace("{data}", EsperConfig.mqttFeedTopic);
    config_form.replace("{offline}", EsperConfig.mqttOfflineTopic);
    config_form.replace("{online}", EsperConfig.mqttOnlineTopic);

    server.send(200, "text/html", config_form);
  });
  server.on("/config", []() {
    String ssid = server.arg("ssid");
    String pass = server.arg("pass");
    String host  = server.arg("host");
    String port  = server.arg("port");
    String user  = server.arg("user");
    String mqttPass = server.arg("mqttPass");
    String retain = server.arg("retain");
    String data = server.arg("data");
    String online = server.arg("offline");
    String offline = server.arg("online");

    String content;
    unsigned statusCode;

    E_DEBUG("WiFi SSID: %s Pass: %s", ssid.c_str(), pass.c_str());
    E_DEBUG("MQTT: %s:%s@%s:%i (Retain: %s)",
      user.c_str(), mqttPass.c_str(), host.c_str(), port.c_str(), retain ? "yes" : "no");
    E_DEBUG(data.c_str()); E_DEBUG(online.c_str()); E_DEBUG(offline.c_str());

    if (ssid.length() > 0 && host.length() > 0 && data.length() > 0) {

      EsperConfig.flagConfig = false;
      CopyString(ssid, EsperConfig.wifiSSID);
      if (pass.length()) CopyString(pass, EsperConfig.wifiPass);
      CopyString(host, EsperConfig.mqttHost);
      if (port.length()) EsperConfig.mqttPort = port.toInt();
      if (user.length()) CopyString(user,  EsperConfig.mqttUser);
      if (mqttPass.length()) CopyString(mqttPass,  EsperConfig.mqttPass);
      EsperConfig.mqttRetain = (retain == "1" ? true : false);
      if (data.length()) CopyString(data,  EsperConfig.mqttFeedTopic);
      if (online.length()) CopyString(offline,  EsperConfig.mqttOfflineTopic);
      if (offline.length()) CopyString(offline,  EsperConfig.mqttOnlineTopic);

      content = R"json({"status":"ok","msg":"Configuration saved"})json";
      statusCode = 200;
      server.send(statusCode, "application/json", content);

      EsperState::set(MODE_SWITCH_TO_STA);

    } else {
      E_DEBUG("Configuration invalid");
      content = R"json({"status":"error","msg":"Configuration invalid"})json";
      server.send(404, "application/json", content);
    }
  });
  server.on("/info.json", []() {
    char buff[256];
    snprintf(buff, sizeof(buff),
      R"json({"board":"%s","vendor":"%s","fw":"%s","hw":"%s"})json",
      BOARD_NAME,
      BOARD_VENDOR,
      BOARD_FIRMWARE_VERSION,
      BOARD_HARDWARE_VERSION
    );
    server.send(200, "application/json", buff);
  });
  server.on("/reset", []() {
    EsperState::set(MODE_RESET_CONFIG);
    server.send(200, "application/json", R"json({"status":"ok","msg":"Configuration reset"})json");
  });
  server.on("/reboot", []() {
    esperRestart();
  });

  server.begin();

  while (EsperState::is(MODE_WAIT_CONFIG) || EsperState::is(MODE_CONFIGURING)) {
    dnsServer.processNextRequest();
    server.handleClient();
    if (EsperState::is(MODE_WAIT_CONFIG) && WiFi.softAPgetStationNum() > 0) {
      EsperState::set(MODE_CONFIGURING);
    } else if (EsperState::is(MODE_CONFIGURING) && WiFi.softAPgetStationNum() == 0) {
      EsperState::set(MODE_WAIT_CONFIG);
    }
  }

  server.stop();
}

void esperConnectNet() {
  EsperState::set(MODE_CONNECTING_NET);
  E_DEBUG("Connecting to WiFi: %s:%s", EsperConfig.wifiSSID, EsperConfig.wifiPass);

  WiFi.mode(WIFI_STA);
  if (!WiFi.begin(EsperConfig.wifiSSID, EsperConfig.wifiPass)) {
    flashError("WIFI_HAWDWARE_FAILED");
    return;
  }

  unsigned long timeoutMs = millis() + WIFI_NET_CONNECT_TIMEOUT;
  while ((timeoutMs > millis()) && (WiFi.status() != WL_CONNECTED))
  {
    delay(100);
    if (!EsperState::is(MODE_CONNECTING_NET)) {
      WiFi.disconnect();
      return;
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    EsperState::set(MODE_CONNECTING_MQTT);
  } else {
    E_DEBUG("WiFi timeout");
    flashError("WIFI_TIMEOUT");
    EsperState::set(MODE_ERROR);
  }
}

void esperConnectMqtt() {
  EsperState::set(MODE_CONNECTING_MQTT);

  if (mqtt.connected()) {
    E_DEBUG("Disconnecting MQTT");
    mqtt.disconnect();
  }

  E_DEBUG("Connecting MQTT %s:%s@%s:%i Retain=%s",
    EsperConfig.mqttUser, EsperConfig.mqttPass,
    EsperConfig.mqttHost, EsperConfig.mqttPort,
    EsperConfig.mqttRetain ? "yes" : "no");

  E_DEBUG("MQTT feed: %s", EsperConfig.mqttFeedTopic);
  E_DEBUG("MQTT online: %s", EsperConfig.mqttOnlineTopic);
  E_DEBUG("MQTT offline: %s", EsperConfig.mqttOfflineTopic);
  E_DEBUG("MQTT echo: %s => %s", ECHO_TOPIC, OHCE_TOPIC);

  mqtt.setServer(EsperConfig.mqttHost, EsperConfig.mqttPort);

  mqtt.connect(EsperId.c_str(), EsperConfig.mqttUser, EsperConfig.mqttPass,
    EsperConfig.mqttOfflineTopic, MQTTQOS1, EsperConfig.mqttRetain, EsperId.c_str());

  if (mqtt.state() == MQTT_CONNECTED) {
    EsperState::set(MODE_RUNNING);

    mqtt.publish(EsperConfig.mqttOnlineTopic, EsperId.c_str());

    mqtt.subscribe(OTA_TOPIC);
    mqtt.subscribe((String(OTA_TOPIC) + "/" + EsperId.get()).c_str());
    mqtt.subscribe(ECHO_TOPIC);
    mqtt.subscribe((String(ECHO_TOPIC) + "/" + EsperId.get()).c_str());
    mqtt.subscribe((String(JS_TOPIC) + "/" + EsperId.get()).c_str());

    if (!EsperConfig.flagConfig) {
      EsperConfig.flagConfig = true;
      config_save();
      E_DEBUG("Configuration stored to flash.");
    }
  } else {
    flashError(mqtt.stateName());
    E_DEBUG("MQTT connection failed - %s", mqtt.stateName());
    EsperState::set(MODE_ERROR);
  }
}

void esperRun() {

  if (!WiFi.isConnected()) {
    E_DEBUG("WiFi lost...");
    EsperState::set(MODE_CONNECTING_NET);
    return;
  }

  if (!mqtt.connected()) {
    E_DEBUG("MQTT lost..");
    EsperState::set(MODE_CONNECTING_MQTT);
    return;
  }

  mqtt.loop();
}

void esperSwitchToSTA() {
  EsperState::set(MODE_SWITCH_TO_STA);

  E_DEBUG("Switching to STA...");

  WiFi.mode(WIFI_OFF);
  delay(1000);
  WiFi.mode(WIFI_STA);

  EsperState::set(MODE_CONNECTING_NET);
}

void esperError() {
  EsperState::set(MODE_ERROR);

  unsigned long timeoutMs = millis() + 10000;
  while (timeoutMs > millis() || g_buttonPressed)
  {
    delay(10);
    if (!EsperState::is(MODE_ERROR)) {
      return;
    }
  }
  E_DEBUG("Restarting after error.");
  delay(10);

  esperRestart();
}
