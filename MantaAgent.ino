/*
 * MantaAgent.ino
 * AMB82-Mini reference firmware for AI Plush Manta.
 *
 * Hardware-specific camera, LED, servo and audio adapters remain TODOs.
 * Timer logic is local and non-blocking; it does not depend on AI availability.
 */
#include <WiFi.h>

char ssid[] = "YOUR_WIFI_SSID";
char pass[] = "YOUR_WIFI_PASSWORD";
WiFiServer server(8080);

enum Emotion { EMOTION_HAPPY, EMOTION_CURIOUS, EMOTION_THINKING, EMOTION_SLEEPY, EMOTION_FOCUS };
enum TimerState { TIMER_IDLE, TIMER_RUNNING, TIMER_PAUSED, TIMER_COMPLETED };
Emotion currentEmotion = EMOTION_HAPPY;
TimerState timerState = TIMER_IDLE;
uint32_t timerDeadline = 0;
uint32_t pausedRemainingMs = 0;
bool completionNotified = false;

void sendJson(WiFiClient &client, int code, const String &json);
String getRequestTarget(const String &requestLine);
String getQueryValue(const String &target, const String &key);
String runVision();
void setEmotion(Emotion e);
bool playSound(const String &soundId);
void nodHead();
void shakeHead();
void waveHead();
void stopAllMotion();
void updateTimer();
uint32_t timerRemainingMs();

void setup() {
  Serial.begin(115200);
  delay(500);
  // TODO: initialize camera, LED, bounded motion, audio and microSD adapters.
  WiFi.begin(ssid, pass);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print('.'); }
  Serial.println();
  Serial.print("Manta IP: "); Serial.println(WiFi.localIP());
  server.begin();
  Serial.println("Manta Agent API listening on port 8080");
}

void loop() {
  updateTimer();
  WiFiClient client = server.available();
  if (!client) return;
  client.setTimeout(1000);
  String requestLine = client.readStringUntil('\r');
  client.readStringUntil('\n');
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r" || line.length() == 0) break;
  }
  if (requestLine.startsWith("OPTIONS ")) {
    client.println("HTTP/1.1 204 No Content");
    client.println("Access-Control-Allow-Origin: *");
    client.println("Access-Control-Allow-Methods: GET, OPTIONS");
    client.println("Access-Control-Allow-Headers: Content-Type");
    client.println("Connection: close\r\n");
    client.stop(); return;
  }
  if (!requestLine.startsWith("GET ")) {
    sendJson(client, 405, "{\"ok\":false,\"error\":\"method_not_allowed\"}");
    client.stop(); return;
  }

  String target = getRequestTarget(requestLine);
  int q = target.indexOf('?');
  String path = q >= 0 ? target.substring(0, q) : target;

  if (path == "/status") {
    String body = "{\"ok\":true,\"device\":\"manta\",\"timer\":{\"state\":\"" +
      timerStateName() + "\",\"remainingSeconds\":" + String(timerRemainingMs()/1000) + "}}";
    sendJson(client, 200, body);
  } else if (path == "/look") {
    sendJson(client, 200, runVision());
  } else if (path == "/nod") {
    nodHead(); sendJson(client, 200, "{\"ok\":true,\"action\":\"nod\"}");
  } else if (path == "/shake") {
    shakeHead(); sendJson(client, 200, "{\"ok\":true,\"action\":\"shake\"}");
  } else if (path == "/wave") {
    waveHead(); sendJson(client, 200, "{\"ok\":true,\"action\":\"wave\"}");
  } else if (path.startsWith("/emotion/")) {
    String value = path.substring(9);
    if (value == "happy") setEmotion(EMOTION_HAPPY);
    else if (value == "curious") setEmotion(EMOTION_CURIOUS);
    else if (value == "thinking") setEmotion(EMOTION_THINKING);
    else if (value == "sleepy") setEmotion(EMOTION_SLEEPY);
    else if (value == "focus") setEmotion(EMOTION_FOCUS);
    else { sendJson(client, 400, "{\"ok\":false,\"error\":\"unknown_emotion\"}"); client.stop(); return; }
    sendJson(client, 200, "{\"ok\":true,\"emotion\":\"" + value + "\"}");
  } else if (path.startsWith("/play/")) {
    String sound = path.substring(6);
    if (playSound(sound)) sendJson(client, 200, "{\"ok\":true,\"action\":\"play\"}");
    else sendJson(client, 400, "{\"ok\":false,\"error\":\"unknown_sound\"}");
  } else if (path == "/stop") {
    stopAllMotion(); sendJson(client, 200, "{\"ok\":true,\"action\":\"stop\"}");
  } else if (path == "/timer/start") {
    long minutes = getQueryValue(target, "minutes").toInt();
    if (minutes < 1 || minutes > 180) {
      sendJson(client, 400, "{\"ok\":false,\"error\":\"minutes_must_be_1_to_180\"}");
    } else {
      pausedRemainingMs = (uint32_t)minutes * 60000UL;
      timerDeadline = millis() + pausedRemainingMs;
      timerState = TIMER_RUNNING; completionNotified = false;
      setEmotion(EMOTION_FOCUS);
      sendJson(client, 200, timerJson());
    }
  } else if (path == "/timer/pause") {
    if (timerState == TIMER_RUNNING) {
      pausedRemainingMs = timerRemainingMs(); timerState = TIMER_PAUSED;
      sendJson(client, 200, timerJson());
    } else sendJson(client, 409, "{\"ok\":false,\"error\":\"timer_not_running\"}");
  } else if (path == "/timer/resume") {
    if (timerState == TIMER_PAUSED) {
      timerDeadline = millis() + pausedRemainingMs; timerState = TIMER_RUNNING;
      sendJson(client, 200, timerJson());
    } else sendJson(client, 409, "{\"ok\":false,\"error\":\"timer_not_paused\"}");
  } else if (path == "/timer/stop") {
    timerState = TIMER_IDLE; pausedRemainingMs = 0; completionNotified = false;
    setEmotion(EMOTION_HAPPY); sendJson(client, 200, timerJson());
  } else if (path == "/timer/status") {
    sendJson(client, 200, timerJson());
  } else {
    sendJson(client, 404, "{\"ok\":false,\"error\":\"unknown_command\"}");
  }
  delay(1); client.stop();
}

String getRequestTarget(const String &line) {
  int a = line.indexOf(' '), b = line.indexOf(' ', a + 1);
  if (a < 0 || b < 0) return "/";
  return line.substring(a + 1, b);
}
String getQueryValue(const String &target, const String &key) {
  int q = target.indexOf('?'); if (q < 0) return "";
  String query = target.substring(q + 1), token = key + "=";
  int p = query.indexOf(token); if (p < 0) return "";
  int start = p + token.length(), end = query.indexOf('&', start);
  return end < 0 ? query.substring(start) : query.substring(start, end);
}
void sendJson(WiFiClient &c, int code, const String &json) {
  const char *label = code == 200 ? "OK" : code == 400 ? "Bad Request" :
    code == 404 ? "Not Found" : code == 405 ? "Method Not Allowed" : "Conflict";
  c.print("HTTP/1.1 "); c.print(code); c.print(' '); c.println(label);
  c.println("Content-Type: application/json; charset=utf-8");
  c.println("Access-Control-Allow-Origin: *"); c.println("Cache-Control: no-store");
  c.println("Connection: close"); c.print("Content-Length: "); c.println(json.length());
  c.println(); c.print(json);
}

uint32_t timerRemainingMs() {
  if (timerState == TIMER_PAUSED) return pausedRemainingMs;
  if (timerState != TIMER_RUNNING) return 0;
  int32_t diff = (int32_t)(timerDeadline - millis());
  return diff > 0 ? (uint32_t)diff : 0;
}
String timerStateName() {
  switch (timerState) {
    case TIMER_RUNNING: return "running";
    case TIMER_PAUSED: return "paused";
    case TIMER_COMPLETED: return "completed";
    default: return "idle";
  }
}
String timerJson() {
  return "{\"ok\":true,\"timer\":{\"state\":\"" + timerStateName() +
    "\",\"remainingSeconds\":" + String(timerRemainingMs()/1000) + "}}";
}
void updateTimer() {
  if (timerState == TIMER_RUNNING && timerRemainingMs() == 0) {
    timerState = TIMER_COMPLETED; pausedRemainingMs = 0;
    if (!completionNotified) {
      completionNotified = true;
      setEmotion(EMOTION_HAPPY);
      playSound("timer_done");
    }
  }
}

// Hardware adapters: replace TODOs with APIs for the selected AMB82 board/core.
String runVision() { return "{\"ok\":true,\"objects\":[],\"note\":\"camera_adapter_todo\"}"; }
void nodHead() { /* TODO: bounded gentle gesture */ }
void shakeHead() { /* TODO: bounded gentle gesture */ }
void waveHead() { /* TODO: bounded gentle fin gesture */ }
void setEmotion(Emotion e) { currentEmotion = e; /* TODO: map to LED pattern */ }
bool playSound(const String &id) {
  if (id == "hello" || id == "giggle" || id == "goodbye" || id == "timer_done") {
    /* TODO: play whitelisted file from microSD */ return true;
  }
  return false;
}
void stopAllMotion() { /* TODO: move actuator subsystem to safe idle */ }
