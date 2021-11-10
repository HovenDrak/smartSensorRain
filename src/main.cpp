#include <Arduino.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ElegantOTA.h>
#include "FS.h"
#include <NTPClient.h>

// Krod add ****************************
#include <Ticker.h>
#define LED_OSC LED_BUILTIN
Ticker timer;
volatile int TICK_EVENT_COUNTER = 1500000;
void ICACHE_RAM_ATTR timerLED_OSC()
{
  timer1_write(TICK_EVENT_COUNTER);
  digitalWrite(LED_OSC, digitalRead(LED_OSC) ^ 1);

  // @TODO LOG TEMPO REAL...
}
// Krod add ****************************

/* horário do esp */
String dia[] = {"Domingo", "Segunda-Feira", "Terça-Feira", "Quarta-Feira", "Quinta-Feira", "Sexta-Feira", "Sábado"};
WiFiUDP udp;
NTPClient ntpClient(udp, "b.ntp.br", -3 * 3600);
/* horário do esp */

/* rede gerada pelo esp */
const char *ssidOFF = "Sensor de Chuva";
const char *passwordOFF = "12345678";
/* rede gerada pelo esp */

/*Mapeamento EEPROM */
const byte POSI_SSID = 0;
const byte POSI_PASSWORD = 40 + POSI_SSID;
const byte POSI_TOTAL = 40 + POSI_PASSWORD;
/*Mapeamento EEPROM */

/* HTTP */
const char *host = "trigger.fullarm.com";
String url = "/api/v1/trigger/2369";
const int httpsPort = 443;
const String token = "d417cb59bf5794971b80480eeb7c73ed7470eaca";
const char fingerprint[] PROGMEM = "CD 30 5E 9A FF D4 8B 62 DC 83 D4 3F D5 A4 CD A4 B2 F5 53 0E";
/* HTTP */

/* pinos usados no esp */
#define sensor 12
#define janela 13
#define botoeira 16
#define LED_WIFI 4
#define LED_JANELA 14
#define LED_CHUVA 5
/* pinos usados no esp */

/* logger esp */
String arquivoPrincipal = "./log.txt";
String arquivoStatus = "./logStatus.txt";
String buf;
/* logger esp */

String bot;

/* pagina home */
const char PAGINA_HTML_HOME[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-br">
<head>
<meta charset="UTF-8">
<title>Sensor de Chuva</title>
<style>
body {
background-color: navy;
text-align: center;
}
.titulo{
font-family: Arial, Helvetica, sans-serif;
color: white;
}
.button {
width: 300px;
height: 50px;
border-radius: 10px;
background-color: grey;
position: relative;
}
.button:hover{
background-color:dimgrey;
color: white;
border: 2px solid black;
}
</style>
</head>
<body>
<h2 class="titulo">Sensor de Chuva</h2>
<h2 class="titulo">Menu</h2>
<a href="/config_wifi"><button class="button">Configurações WIFI</button></a>
<p></p>
<a href="/config_mqtt"><button class="button">Configurações MQTT</button></a>
<p></p>
<a href="/update"><button class="button">Atualização Firmware</button></a>
<p></p>
<a href="/debug"><button class="button">Log de Informações</button></a>
<p></p>
<a href="/teste-botoeira"><button class="button">Teste de Botoeira</button></a>
<p></p>
<div style="width: 300px; height: 2px; background-color: white; position: relative; left: 38.8%;"></div>
<h5 class="titulo" style="width: 200px; position: absolute; left: 47%; bottom: 21.5%;">Hardware FullArm @2021</h5>
</body>
</html>
)rawliteral";
/* pagina home */

/* pagina wifi */
const char PAGINA_HTML_WIFI[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-br">
<head>
<meta charset="UTF-8">
<title>Configurações WIFI </title>
<style>
body {
background-color: navy;
text-align: center;
}
h2 {
font-family: Arial, Helvetica, sans-serif;
color: black;
}
.button {
width: 300px;
height: 50px;
border-radius: 10px;
background-color: grey;
position: relative;
}
.button:hover{
background-color:dimgrey;
color: white;
border: 2px solid black;
}
.buttonEnviar {
width: 210px;
height: 30px;
border-radius: 10px;
background-color: grey;
position: relative;
}
.buttonEnviar:hover{
background-color:dimgrey;
color: white;
border: 2px solid black;
}
.campo {
width: 200px;
height: 25px;
border-radius: 10px;
background-color: whitesmoke;
position: relative;
}
label {
font-family: Arial, Helvetica, sans-serif;
color: black;
}
.titulo{
font-family: Arial, Helvetica, sans-serif;
color: white;
}
</style>
</head>
<body>
<h2 class="titulo">Sensor de Chuva</h2>
<h3 class="titulo">Configuração WIFI</h3>
<form action="/salvar-senha" method="POST">
<div style="position: relative; right: 1.5%;">
<label for="SSID" style="color:white">SSID: </label>
<input type="text" id="SSID" name="SSID" class="campo" placeholder=" Digite o nome da rede WIFI"
required="required" value="" />
</div>
<p></p>
<div style="position: relative; right: 1%;">
<label for="senhaSSID" style="color:white">Senha: </label>
<input type="password" id="senha" name="senhaSSID" class="campo" placeholder=" Digite a senha da rede WIFI"
required="required" value="" />
<input type="checkbox" onclick="show()">
</div>
<p></p>
<div>
<button type="submit" value="Enviar" onclick="enviar()" class="buttonEnviar">Enviar Configurações</button>
</div>
</form>
<p></p>
<a href="/">
<button class="button">Voltar para Home</button>
</a>
<script>
function show() {
var senha = document.getElementById("senha");
if (senha.type === "password") {
senha.type = "text";
} else {
senha.type = "password";
}}
</script>
</body>
</html>
)rawliteral";
/* pagina wifi */

/* pagina mqtt */
const char PAGINA_HTML_MQTT[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-br">
<head>
<meta charset="UTF-8">
<title>MQTT</title>
<style>
body{background-color: navy;text-align: center;}
h2{font-family: Arial, Helvetica, sans-serif;color: black;}
.button{width: 300px;height: 50px;border-radius: 10px;background-color: grey;position: relative;}
</style>
</head>
<body>
<h2>Configurações do MQTT</h2>
<form action = "/salvar-mqtt" method = "POST">
<div>
<label for="host">Host: </label>
<input type="text" id="host" class="campo" placeholder=" Digite o host do MQTT" required="required" value="" />
</div>
<p></p>
<div>
<label for="port">Porta: </label>
<input type="text" id="porta" name="porta" class="campo" placeholder=" Digite a porta do MQTT" required="required" value=""/>
</div>
<p></p>
<div>
<div>
<label for="username">Username: </label>
<input type="text" id="user" name="user" class="campo" placeholder=" Digite o usuário MQTT" required="required" value=""/>
</div>
<p></p>
<div>
<label for="senha">Senha: </label>
<input type="password" id="senha" name="senha" class="campo" placeholder=" Digite a senha do MQTT" required="required" value=""/>
<input type="checkbox" onclick="show()">
</div>
<p></p>
<div>
<button type="submit" value="Enviar" onclick="enviar()">Enviar Configurações</button>
</div>
</form>
<a href="/">
<button class="button">Voltar para Home</button>
</a>
</body>
</html>
)rawliteral";
/* pagina mqtt */

int temp = 0;
int temp2 = 0;

WiFiClient espClient;
WiFiClientSecure httpsClient;
ESP8266WebServer server(80);

void reconnectWiFi();

/* funções EEPROM */
String getEEPROMString(byte offset);
void setEEPROMString(byte offset, String s);
/* funções EEPROM */

/* funções log esp */
void formatFS(void);
void createFile(String arquivoPrincipal, String arquivoStatus);
void deleteFile(String arquivo);
void writeFile(String msg, String arquivo);
void readFile(String arquivo);
void closeFS(void);
void openFS(void);
/* funções log esp */

void setup()
{
  Serial.begin(115200);
  EEPROM.begin(POSI_TOTAL);

  openFS();
  createFile(arquivoPrincipal, arquivoStatus);

  pinMode(sensor, INPUT_PULLUP);
  pinMode(janela, INPUT_PULLUP);
  pinMode(LED_OSC, OUTPUT);
  pinMode(botoeira, OUTPUT);
  digitalWrite(botoeira, LOW);

  pinMode(LED_WIFI, OUTPUT);
  pinMode(LED_JANELA, OUTPUT);
  pinMode(LED_CHUVA, OUTPUT);

  // Krod add ****************************
  timer1_attachInterrupt(timerLED_OSC);
  timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);
  timer1_write(TICK_EVENT_COUNTER);
  // Krod add ****************************

  IPAddress staticIP(172, 19, 1, 255);
  IPAddress gateway(192, 168, 4, 1);
  IPAddress subnet(255, 255, 255, 0);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssidOFF, passwordOFF, 2, 0);
  WiFi.config(staticIP, gateway, subnet);

  Serial.println(getEEPROMString(POSI_SSID) + getEEPROMString(POSI_PASSWORD));

  server.begin();
  Serial.println("Server started");
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  ESP.wdtEnable(17000);

  server.on("/", []()
            { server.send(200, "text/html", PAGINA_HTML_HOME); });

  server.on("/config_wifi", []()
            { server.send(200, "text/html", PAGINA_HTML_WIFI); });

  server.on("/config_mqtt", []()
            { server.send(200, "text/html", PAGINA_HTML_MQTT); });

  server.on("/debug", []()
            {
              buf = "";
              buf += "<!DOCTYPE html>";
              buf += "<html lang=\"pt-br\">";
              buf += "<head>";
              buf += "<meta charset=\"UTF-8\">";
              buf += "<meta http-equiv='refresh' content='5'>";
              buf += "<title>Sensor de Chuva</title>";
              buf += "<style>";
              buf += "body{background-color: navy;text-align: center; align-items: center;}";
              buf += "h2{font-family: Arial, Helvetica, sans-serif;color: white;}";
              buf += ".button{width: 300px;height: 50px;border-radius: 10px;background-color: grey;position: relative;}";
              buf += ".logger{background-color: rgb(38, 70, 80); position:relative; left:30%; width: 40%; height: 400px; overflow: scroll; overflow-x: hidden; align-items: flex-start; border-radius: 15px;border : 2px solid black;}";
              buf += ".logger::-webkit-scrollbar{width: 12px; }";
              buf += ".logger::-webkit-scrollbar-track{background: rgb(38, 70, 80); border-radius: 100px;}";
              buf += ".logger::-webkit-scrollbar-thumb{background-color: dimgray; border-radius: 30px; border: 2px solid black;}";
              buf += ".button:hover{background-color:dimgrey;color: white;border: 2px solid black;}";
              buf += ".titulo{font-family: Arial, Helvetica, sans-serif;color: white;}";
              buf += ".status{width: 24%; height: 200px; background-color: rgb(38, 70, 80); position: absolute; left: 3%; bottom: 45%; border: 2px solid black; border-radius: 10px;}";
              buf += "</style>";
              buf += "</head>";
              buf += "<body>";
              buf += "<h2 class=\"titulo\">Sensor de Chuva</h2>";
              buf += "<h3 class=\"titulo\">Logger de Informações</h3>";
              buf += "<div class=\"logger\" id=\"log\">";
              buf += "<p style=\"padding: 10px; text-align: start; color: white;\">";
              readFile(arquivoPrincipal);
              buf += "</p>";
              buf += "</div>";
              buf += "<p></p>";
              buf += "<div class=\"status\"><h3 class=\"titulo\">Tabela de Status</h3>";
              buf += "<p style=\"padding: 10px; text-align: start; color: white;\">";
              readFile(arquivoStatus);
              buf += "</p>";
              buf += "</div>";
              buf += "<a href=\"/\">";
              buf += "<button class=\"button\">Voltar para Home</button>";
              buf += "</a>";
              buf += "</body>";
              buf += "<script>";
              buf += "var logger = document.querySelector('#log');";
              buf += "logger.scrollTop = logger.scrollHeight - logger.clientHeight;";
              buf += "</script>";
              buf += "</html>";
              server.send(200, "text/html", buf);
            });

  server.on("/teste-botoeira", []()
            {
              bot = "";
              bot += "comando por botoeira...";
              digitalWrite(botoeira, HIGH);
              delay(500);
              digitalWrite(botoeira, LOW);
              writeFile("Janela fechada por BOTOEIRA!!!", arquivoPrincipal);
              server.send(200, "text/html", bot);
            });

  server.on("/reiniciar", []()
            {
              bot = "";
              bot += "reiniciando...";
              writeFile("Reiniciando ESP...", arquivoPrincipal);
              server.send(200, "text/html", bot);
              ESP.restart();
            });

  server.on("/salvar-senha", []()
            {
              String ssid = server.arg(0);
              String password = server.arg(1);

              Serial.println(ssid);
              Serial.println(password);

              WiFi.begin(ssid, password); //Inicia WiFi com os dados preenchidos
              delay(1000);
              Serial.print("Conectando");
              while (WiFi.status() != WL_CONNECTED)
              {
                delay(500);
                Serial.print(".");
              }
              Serial.println();

              Serial.print("Conectado | Endereço IP: ");
              digitalWrite(LED_WIFI, HIGH);
              Serial.println(WiFi.localIP());
              delay(1000);

              setEEPROMString(POSI_SSID, ssid);
              setEEPROMString(POSI_PASSWORD, password);

              EEPROM.commit();

              delay(500);
              server.send(200, "text/html", PAGINA_HTML_WIFI);
            });
  ElegantOTA.begin(&server);
  server.begin();
}

void loop()
{
  ESP.wdtFeed();
  reconnectWiFi();
  httpsClient.setFingerprint(fingerprint);
  httpsClient.setTimeout(15000);
  server.handleClient();

  if ((millis() - temp2) >= 1000)
  {
    temp2 = millis();
    String statusChuva = digitalRead(sensor) ? "sem chuva" : "chovendo";
    String statusJanela = digitalRead(janela) ? "fechada" : "aberta";

    Serial.println("Status da chuva: " + statusChuva + "\nStatus da Janela: " + statusJanela);

    deleteFile(arquivoStatus);

    writeFile("Status Chuva: " + statusChuva, arquivoStatus);
    delay(200);
    writeFile("Status Janela: " + statusJanela, arquivoStatus);
    delay(200);
  }

  if ((millis() - temp) >= 100)
  {
    temp = millis();
    if (WiFi.status() == WL_CONNECTED)
    {
      digitalWrite(LED_WIFI, HIGH);

      if (digitalRead(sensor) == LOW)
      {

        digitalWrite(LED_CHUVA, HIGH);

        if (digitalRead(janela) == LOW)
        {

          digitalWrite(LED_JANELA, HIGH);

          Serial.println("HTTPS Connecting");
          writeFile("Conectando no host HTTPS...", arquivoPrincipal);
          delay(200);
          writeFile("Chuva detectada!!!", arquivoPrincipal);
          delay(200);
          writeFile("Janela aberta!!!", arquivoPrincipal);

          int r = 0;

          while ((!httpsClient.connect(host, httpsPort)) && (r < 30))
          {
            delay(100);
            Serial.print(".");
            r++;
          }
          if (r == 30)
          {
            Serial.println("Connection failed");
            writeFile("Conexão HTTPS falhou!!!", arquivoPrincipal);
          }
          else
          {
            Serial.println("Connected to web");
            writeFile("Conectado com sucesso", arquivoPrincipal);
          }

          writeFile("Enviando request...", arquivoPrincipal);
          delay(200);
          httpsClient.print(String("POST ") + url + " HTTP/1.1\r\n" +
                            "Host: " + host + "\r\n" +
                            "Authorization: Bearer " + token + "\r\n" +
                            "Keep-Alive: timeout=5, max=100" + "\r\n" +
                            "Content-Type: application/json; charset=UTF-8; charset=UTF-8" + "\r\n" +
                            "Connection: close\r\n\r\n");

          Serial.println("request sent");
          delay(200);
          writeFile("Request enviada!!!", arquivoPrincipal);

          while (espClient.connected())
          {
            String line = espClient.readStringUntil('\n');
            if (line == "\r")
            {
              Serial.println("headers received");
              break;
            }
          }
          Serial.println("reply was:");
          Serial.println("==========");
          String line;

          while (espClient.available())
          {
            line = espClient.readStringUntil('\n');
            Serial.println(line);
          }
          Serial.println("==========");
          writeFile("Fechando conexão HTTPS!!!", arquivoPrincipal);
          Serial.println("closing connection");

          writeFile("Esperando janela ser fechada...", arquivoPrincipal);
          delay(7000);

          if (digitalRead(janela) == LOW)
          {
            writeFile("Janela continua aberta...", arquivoPrincipal);
            delay(200);
            digitalWrite(botoeira, HIGH);
            delay(500);
            digitalWrite(botoeira, LOW);
            writeFile("Janela fechada por BOTOEIRA!!!", arquivoPrincipal);
            Serial.println("Comando por botoeira");
            delay(7000);
          }
          else
          {
            digitalWrite(janela, LOW);
            writeFile("Janela fechou por REQUEST!!!", arquivoPrincipal);
            Serial.println("Comando por REQUEST");
          }
        }
        else
        {
          digitalWrite(LED_JANELA, LOW);
        }
      }
      else
      {
        digitalWrite(LED_CHUVA, LOW);
        if (digitalRead(janela) == LOW)
        {
          digitalWrite(LED_JANELA, HIGH);
        }
        else
        {
          digitalWrite(LED_JANELA, LOW);
        }
        return;
      }
    }
    else
    {
      reconnectWiFi();
    }
  }
}

String getEEPROMString(byte offset)
{

  String s = "";
  while (offset < POSI_TOTAL)
  {
    if (EEPROM.read(offset) == 0)
    {
      break;
    }
    s += char(EEPROM.read(offset));
    offset++;
  }
  return s;
}

void setEEPROMString(byte offset, String s)
{

  for (byte b = 0; b <= s.length(); b++)
  {
    EEPROM.write(offset + b, s[b]);
  }
}

void reconnectWiFi(void)
{

  if (WiFi.status() == WL_CONNECTED)
  {
    digitalWrite(LED_WIFI, HIGH);
    return;
  }

  WiFi.begin(getEEPROMString(POSI_SSID), getEEPROMString(POSI_PASSWORD));

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
    Serial.print(".");
    digitalWrite(LED_WIFI, LOW);
  }

  Serial.println();
  Serial.print("Conectado com sucesso na rede ");
  Serial.println(getEEPROMString(POSI_SSID));
  digitalWrite(LED_WIFI, HIGH);
  Serial.println("IP obtido: ");
  Serial.println(WiFi.localIP());
  String mac = WiFi.macAddress();
  Serial.println("MAC: " + mac);
}

void formatFS(void)
{
  SPIFFS.format();
}

void createFile(String arquivoPrincipal, String arquivoStatus)
{
  File wFile;
  File wFile2;

  //Cria o arquivo se ele não existir
  if (SPIFFS.exists("./log.txt"))
  {
    Serial.println("Arquivo principal ja existe!");
  }
  else
  {
    Serial.println("Criando o arquivo principal...");
    wFile = SPIFFS.open(arquivoPrincipal, "w+");

    //Verifica a criação do arquivo
    if (!wFile)
    {
      Serial.println("Erro ao criar arquivo principal!");
    }
    else
    {
      Serial.println("Arquivo principal criado com sucesso!");
    }
    wFile.close();
  }

  if (SPIFFS.exists("./logStatus.txt"))
  {
    Serial.println("Arquivo status ja existe!");
  }
  else
  {
    Serial.println("Criando o arquivo de Status...");
    wFile2 = SPIFFS.open(arquivoStatus, "w+");

    if (!wFile2)
    {
      Serial.println("Erro ao criar arquivo Status!");
    }
    else
    {
      Serial.println("Arquivo Status criado com sucesso!");
    }
    wFile2.close();
  }
}

void deleteFile(String arquivo)
{
  //Remove o arquivo
  if (!SPIFFS.remove(arquivo))
  {
    Serial.println("Erro ao remover arquivo!");
  }
  else
  {
    Serial.println("Arquivo removido com sucesso!");
  }
}

void writeFile(String msg, String arquivo)
{

  //Abre o arquivo para adição (append)
  //Inclue sempre a escrita na ultima linha do arquivo
  File rFile = SPIFFS.open(arquivo, "a+");

  if (!rFile)
  {
    Serial.println("Erro ao abrir arquivo!");
  }
  else
  {
    ntpClient.update();
    String hora = ntpClient.getFormattedTime();
    time_t epoch = ntpClient.getEpochTime();

    struct tm ts;
    char buffer[80];

    time(&epoch);

    ts = *localtime(&epoch);
    strftime(buffer, sizeof(buffer), "%Y/%m/%d ", &ts);
    rFile.printf("\n%s", buffer);
    rFile.print(hora + ": " + msg);
    Serial.println(msg);
  }
  rFile.close();
}

void readFile(String arquivo)
{
  //Faz a leitura do arquivo
  File rFile = SPIFFS.open(arquivo, "r");
  Serial.println("Reading file...");
  while (rFile.available())
  {
    String line = rFile.readStringUntil('\n');
    buf += line;
    buf += "<br/>";
  }
  rFile.close();
}

void closeFS(void)
{
  SPIFFS.end();
}

void openFS(void)
{
  //Abre o sistema de arquivos
  if (!SPIFFS.begin())
  {
    Serial.println("Erro ao abrir o sistema de arquivos");
  }
  else
  {
    Serial.println("Sistema de arquivos aberto com sucesso!");
  }
}