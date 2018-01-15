#include <SD.h>
#include <SPI.h>
#include <Ethernet.h>
#include <elapsedMillis.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 200);
EthernetServer server(80);
EthernetClient client;
elapsedMillis kompressorTimer;

#define RELAY_CH1 5
#define RELAY_CH2 6
#define RELAY_CH3 7
#define RELAY_CH4 8

File webFile;

const int chipSelect = 4;
String readString;
int reading;
int badTemp = 0;

void setup() {

  analogReference(INTERNAL1V1); // Use built-in 1.1V reference

  pinMode(RELAY_CH1, OUTPUT);
  digitalWrite(RELAY_CH1, LOW);  // switch off Relay1
  pinMode(RELAY_CH2, OUTPUT);
  digitalWrite(RELAY_CH2, LOW);  // switch off Relay2
  pinMode(RELAY_CH3, OUTPUT);
  digitalWrite(RELAY_CH3, LOW);  // switch off Relay3
  pinMode(RELAY_CH4, OUTPUT);
  digitalWrite(RELAY_CH4, LOW);  // switch off Relay4

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}


void loop() {

  if (kompressorTimer >= 300000)
  {
    stannaKompressor();
  }
  
  // listen for incoming clients
  client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();

        if (readString.length() < 300) {
          //store characters to string
          readString += c;      //Serial.print(c);
        }
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html;charset=UTF-8");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          //client.println("Refresh: 5");  // refresh the page automatically every 5 sec

          htmlKod(); // Render html
          break;
        }

        if (c == '\r') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }

    if (readString.indexOf("?startaKompressor") > 0) //checks for pushed button
    {
      startaKompressor(); // Activates relays and starts countdown timer
    }

    // give the web browser time to receive the data
    delay(1);
    readString = "";
    // close the connection:
    client.stop();

    Serial.println("client disconnected");

  } //If client END
}   //void loop END

//float tempKoll(int tempPin){
//  reading = analogRead(tempPin);
//  float tempC = reading / 9.31;
//  return tempC;
//  }

void startaKompressor() {
  digitalWrite(RELAY_CH1, HIGH);
  digitalWrite(RELAY_CH2, HIGH);
  digitalWrite(RELAY_CH3, HIGH);
  digitalWrite(RELAY_CH4, HIGH);
  kompressorTimer = 0;
  client.println("<meta http-equiv=\"refresh\" content=\"5;url=http://192.168.1.200/\"/>" );    
}

void stannaKompressor() {
  digitalWrite(RELAY_CH1, LOW);
  digitalWrite(RELAY_CH2, LOW);
  digitalWrite(RELAY_CH3, LOW);
  digitalWrite(RELAY_CH4, LOW);
}
void htmlKod() {
  webFile = SD.open("index.htm");        // open web page file
  if (webFile) {
    while (webFile.available()) {
      client.write(webFile.read()); // send web page to client
    }
    client.print("<br />");
//    client.println("<h1>Badtemp </h1>" + String(tempKoll(badTemp)) + " grader");
    client.println("<br />");
    client.println("<br />");
    client.print("<footer>Copyright &copy; Tinny Gidensköld</footer>");

    webFile.close();
  }
}
