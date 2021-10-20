#include <Arduino.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

/* rede gerada caso não tiver wifi */
const char *ssidOFF = "Sensor de Chuva";
const char *passwordOFF = "12345678";
/* rede gerada caso não tiver wifi */

const PROGMEM char SSID[] = "";
const PROGMEM char PASSWORD[] = "";

/* pagina home */
const char PAGINA_HTML_HOME[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-br">
<head>
<meta charset="UTF-8">
<title>Sensor de Chuva</title>
<style>
body{background-color: navy;text-align: center;}
h2{font-family: Arial, Helvetica, sans-serif;color: black;}
.button{width: 300px;height: 50px;border-radius: 10px;background-color: grey;position: relative;}
</style>
</head>
<body>
<h2>Configurações Sensor de Chuva</h2>
<a href="/config_wifi"><button class="button">Configurações WIFI</button></a>
<p></p>
<a href="/config_mqtt"><button class="button">Configurações MQTT</button></a>
<p></p>
<a href="/update"><button class="button">Atualização Firmware</button></a>
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
.campo{
width: 200px;
height: 25px;
border-radius: 10px;
background-color: whitesmoke;
position: relative;
}
label{
font-family: Arial, Helvetica, sans-serif;
color: black;
}
</style>
</head>
<body>
<h2>Configurações WIFI</h2>
<form action = "/salvar-senha" method = "POST">
<div>
<label for="SSID">SSID: </label>
<input type="text" id="SSID" name="SSID" class="campo" placeholder=" Digite o nome da rede WIFI" required="required" value="" />
</div>
<p></p>
<div>
<label for="senhaSSID">Senha: </label>
<input type="password" id="senha" name="senhaSSID" class="campo" placeholder=" Digite a senha da rede WIFI" required="required" value=""/>
<input type="checkbox" onclick="show()">
</div>
<p></p>
<div>
<button type="submit" value="Enviar" onclick="enviar()">Enviar Configurações</button>
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
}
}
</script>
</body>
</html>
)rawliteral";
/* pagina wifi */

int temp = 0;

ESP8266WebServer server(80);



void setup()
{
  Serial.begin(115200);
  EEPROM.begin(16);

  IPAddress staticIP(172, 19, 1, 255);
  IPAddress gateway(192, 168, 4, 1);
  IPAddress subnet(255, 255, 255, 0);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssidOFF, passwordOFF, 2, 0);
  WiFi.config(staticIP, gateway, subnet);

  server.begin();
  Serial.println("Server started");
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", [](){ server.send(200, "text/html", PAGINA_HTML_HOME); });

  server.on("/config_wifi", [](){ server.send(200, "text/html", PAGINA_HTML_WIFI); });

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
    Serial.println(WiFi.localIP());
    delay(1000);

    uint8_t ssid8 = atoi(ssid.substring(1,ssid.length()).c_str());
    uint8_t passowrd8 = atoi(password.substring(1, password.length()).c_str());

    EEPROM.write(0, ssid8);
    EEPROM.write(1, passowrd8);
    
    

    server.send(200, "text/html", PAGINA_HTML_WIFI);
  });

  
}

void loop()
{
  server.handleClient();

  if ((millis() - temp) >= 5000){
    temp = millis();


  }
}
