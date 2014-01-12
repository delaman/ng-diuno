#include <SPI.h>
#include <Ethernet.h>
#include <Flash.h>
#include <SD.h>
#include <TinyWebServer.h>
#include "./pin.h"

#define DEBUG 1

boolean file_handler(TinyWebServer& web_server);
boolean index_handler(TinyWebServer& web_server);
boolean pins_handler(TinyWebServer& web_server);
boolean digital_pin_handler(TinyWebServer& web_server);
// String parse_path_string(String pathstring, int size);
void parse_path_string(const char* str, int size, char **messages);
String get_request_data(Client &client);

// HELPERS
void setLedEnabled(int pin, int state);
boolean getPinState(int i);

const char *HOST = "localhost:9000";
Pin** pins;
int numPins;

boolean has_filesystem = true;
// const char *HOST = "fathomless-river-4136.herokuapp.com";
Sd2Card card; 
SdVolume volume;
SdFile root;
SdFile file;

// HARDWARE STATES
// temporary
// int digitalPins[] = { 13, 12, NULL };
// int digitalPinStates[] = { LOW, LOW, NULL };

static uint8_t mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// Don't forget to modify the IP to an available one on your home network
byte ip[] = { 10, 0, 1, 32 };

TinyWebServer::PathHandler handlers[] = {
  {"/pins/" "*", TinyWebServer::POST, &digital_pin_handler},
  {"/pins", TinyWebServer::GET, &pins_handler},
  {"/", TinyWebServer::GET, &index_handler },
  {NULL},
};

boolean file_handler(TinyWebServer& web_server) {
  char* filename = TinyWebServer::get_file_from_path(web_server.get_path());
  Serial << "Reading from filename: " << filename;
  send_file_name(web_server, filename);
  free(filename);
  return true;
}

void send_file_name(TinyWebServer& web_server, const char* filename) {
  if (!filename) {
    web_server.send_error_code(404);
    web_server.end_headers();
    web_server << F("Could not parse URL");
  } else {
    TinyWebServer::MimeType mime_type
      = TinyWebServer::get_mime_type_from_filename(filename);
    web_server.send_error_code(200);
    if (file.open(&root, filename, O_READ)) {
    web_server.send_content_type(mime_type);
    web_server.end_headers();
      Serial << F("Read file "); Serial.println(filename);
      web_server.send_file(file);
      file.close();
    } else {
    web_server.send_content_type("text/plain");
    web_server.end_headers();
    Serial << F("Could not find file: "); Serial.println(filename);
      web_server << F("Could not find file: ") << filename << "\n";
    }
  }
}

/*
boolean index_handler(TinyWebServer& web_server) {
  char* filename = TinyWebServer::get_file_from_path("/INDEX001.HTM");
  send_file_name(web_server, filename);
  free(filename);
  return true;
}
*/

boolean index_handler(TinyWebServer& web_server) {
  web_server << "<html><head><title>Web server</title></head>";
  web_server << "<body>";
  web_server << "<script id=\"appscript\" src=\"http://" << HOST << "/scripts/scripts.js\"></script>";
  web_server << "</body></html>";
  return true;
}

// API
// 

// GET /pins
boolean pins_handler(TinyWebServer& web_server) {
  web_server.send_error_code(200);
  // web_server.send_content_type("application/javascript");
  web_server.end_headers();
  // UpdatePinsState();
  // Client& client = web_server.get_client();
  pinsToString(web_server);
  return true;
}

// POST /dpin/int
boolean digital_pin_handler(TinyWebServer& web_server) {
  // GET PATH
  String pathstring = web_server.get_path();
  int size = count_forward_slashes(pathstring);
  char** parsed = (char**)malloc(sizeof(char *) * size);
  parse_path_string(pathstring.c_str(), size, parsed);

  if (strcmp(parsed[1], "digital") == 0) {
    Serial << "Digital\n";
    // READ DATA
    Client& client = web_server.get_client();
    String data = get_request_data(client);
    // DIGITAL
    Serial << "digital\n" << data << "\n";
  }

  web_server.send_error_code(200);
  web_server.send_content_type("application/javascript");
  web_server.end_headers();

  web_server << "{}";

  for(int j=0; j<size; j++){
    free(parsed[j]);
  }
  free(parsed);
  // String pathstring = web_server.get_path();
  // String pinStr;
  // boolean keep = false;

  // int pin = atoi(pinStr.c_str());
  // boolean pinState = getPinState(pin);
  // Serial << "Pinstate (" << pin << "): " << pinState << "\n";
  // setLedEnabled(pin, !pinState);
  // pinState = !pinState;

  // pinsToString(web_server);

  // web_server << F("{\"pin\":\"");
  // web_server << pin;
  // web_server << F("\",\"state\":");
  // web_server << pinState;
  // web_server << F("}");
  return true;
}

// LED helpers
void setLedEnabled(int pin, int state) {
  // digitalPinStates[pin] = state;
  Serial << "Writing " << pin << " as " << state << "\n";
  // digitalWrite(pin, state);
  // delay(1000);
  // digitalWrite(pin, !state);
}
boolean getPinState(int i) { 
  return true;
}

boolean has_ip_address = false;
const char* headers[] = {"Access-Control-Allow-Origin", NULL};
TinyWebServer web = TinyWebServer(handlers, headers);

const char* ip_to_str(const uint8_t* ipAddr)
{
  static char buf[16];
  sprintf(buf, "%d.%d.%d.%d\0", ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3]);
  return buf;
}

// Setup arduino pins
int SetupArduino(Pin **newPins)
{
  int count = 0;

  newPins = (Pin **)realloc(newPins, sizeof(Pin *) * 11);
  for (int i = 0; i < 11; i++)
  {
    Pin *p = (Pin *)malloc(sizeof(Pin *));
    p->setPin(i+2);
    // p->SetIO(OUTPUT_TYPE);
    // p->SetState(LOW_STATE);
    // p->SetType(DIGITAL);
    newPins[i] = p;
    count++;
  }

  return count;
}

void setup() {

#if DEBUG
Serial.begin(9600);
#endif
  // pinMode(10, OUTPUT); // set the SS pin as an output (necessary!)
  // digitalWrite(10, HIGH); // but turn off the W5100 chip!
  // pinMode(13, OUTPUT);
  // pinMode(12, OUTPUT);

  /*
  char *number;
  PinType type;
  PinSetting setting;
  PinState state;
  */
#if DEBUG
  Serial << F("Setting up pins...\n");
#endif

  numPins = SetupArduino(pins);

#if DEBUG
  for(int i=0; i < numPins; i++){
    pins[i]->Print();
  }
#endif

  // if (!card.init(SPI_FULL_SPEED, 4)) {
  //   Serial << F("card failed\n");
  //   has_filesystem = false;
  // }
  // // initialize a FAT volume
  // if (!volume.init(&card)) {
  //   Serial << F("vol.init failed!\n");
  //   has_filesystem = false;
  // }
  // root.openRoot(volume);
  // root.ls();

#if DEBUG
  Serial << F("Setting up the Ethernet card...\n");
#endif
  Ethernet.begin(mac, ip);

  // Start the web server.
#if DEBUG
  Serial << F("Web server starting...\n");
#endif

  web.begin();

#if DEBUG  
  Serial << F("Free RAM: ") << FreeRam() << "\n";
  Serial << F("Ready to accept HTTP requests.\n\n");
#endif
}

void loop() {
  web.process();
}

void UpdatePinsState() {
  for(int i=0; i<sizeof(pins); i++){
    pins[i]->getState();
  }
}

bool pinsToString(TinyWebServer& web_server) {
  web_server << F("{\"pins\": ");
  web_server << F("{\"digital\":{");
  int len = 3;
  for(int i=0; i<len; i++){
    // digitalPins[i]
    web_server << F("\"");
    web_server << pins[i]->getPin();
    web_server << F("\":\"");
    web_server << pins[i]->getState();
    web_server << F("\"");
    if ((i+1) < len) web_server << F(",");
  }
  web_server << F("}}");
  web_server << F("}");
  return true;
}

/*
*   Count the number of '/'
*   characters in the pathstring
*/
int count_forward_slashes(String pathstring) {
  int p = 0;
  int count = 0;
  while (pathstring[p] != '\0') {
    if (pathstring[p] == '/') count++;
    p++;
  }
  return count;
}

void parse_path_string(const char* cStr, int size, char** messages) { 
  char* str = strdup(cStr);
  char *p;
  int i = 0;

  for(str = strtok_r(str,"/",&p); str; str = strtok_r(NULL, "/", &p))
  {
    int len = strlen(str);
    char *msg = (char *)malloc((len + 1) * sizeof(char)); // sizeof(char) == 1
    strncpy(msg, str, len + 1);
    // char *msg = strdup(str);
    messages[i] = msg;
    i++;
  }
}

String get_request_data(Client &client)
{
  String str;
  if(client.available()) {
    for(char ch = (char)client.read(); ch != NULL && ch != '\n'; ch = (char)client.read()) {
      Serial << ch;
      str += ch;
    }
    Serial << "--------------------------------\n";
  }
  return str;
}
