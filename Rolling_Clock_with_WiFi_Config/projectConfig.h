#define PROJECT_CONFIG_JSON "/project_config.json"

#define PROJECT_TIME_ZONE_LABEL "timezone"
#define PROJECT_TIME_TWENTYFOUR_HOUR "twentyfourHour"
#define PROJECT_TIME_US_DATE "usDate"

class ProjectConfig {
public:
  String timeZone = "Europe/London";
  bool twentyfourHour = false;
  bool usDateFormat = false;
  bool fetchConfigFile() {
    if (SPIFFS.exists(PROJECT_CONFIG_JSON)) {
      Serial.print("Reading config file.");
      File configFile = SPIFFS.open(PROJECT_CONFIG_JSON, "r");
      if (configFile) {
        Serial.println("Opened config file.");
        StaticJsonDocument<1024> json;
        DeserializationError error = deserializeJson(json, configFile);
        serializeJsonPretty(json, Serial);
        if (!error) {
          Serial.println("\nParsed JSON.");
          if (json.containsKey(PROJECT_TIME_ZONE_LABEL)) {
            timeZone = String(json[PROJECT_TIME_ZONE_LABEL].as<String>());
          }
          if (json.containsKey(PROJECT_TIME_TWENTYFOUR_HOUR)) {
            twentyfourHour = json[PROJECT_TIME_TWENTYFOUR_HOUR].as<bool>();
          }
          if (json.containsKey(PROJECT_TIME_US_DATE)) {
            usDateFormat = json[PROJECT_TIME_US_DATE].as<bool>();
          }
          return true;
        }
        else {
          Serial.println("Failed to load json config.");
          return false;
        }
      }
    }
    Serial.println("Config file does not exist.");
    return false;
  }

  bool saveConfigFile() {
    Serial.println(F("Saving config file."));
    StaticJsonDocument<1024> json;
    json[PROJECT_TIME_ZONE_LABEL] = timeZone;
    json[PROJECT_TIME_TWENTYFOUR_HOUR] = twentyfourHour;
    json[PROJECT_TIME_US_DATE] = usDateFormat;

    File configFile = SPIFFS.open(PROJECT_CONFIG_JSON, "w");
    if (!configFile) {
      Serial.println("Failed to open the config file for writing.");
      return false;
    }
    serializeJsonPretty(json, Serial);
    if (serializeJson(json, configFile) == 0) {
      Serial.println(F("Failed to write the JSON config file."));
      return false;
    }
    return true;
  }
};