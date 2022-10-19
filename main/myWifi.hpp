String sendGET(String serverPath) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    http.begin(client, serverPath.c_str());

    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      // Serial.print("HTTP Response code: ");
      // Serial.println(httpResponseCode);
      String payload = http.getString();
      // Serial.println(payload);
      return payload;
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  }
  return "error";
}