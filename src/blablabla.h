#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

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
<input type="text" id="SSID" class="campo" placeholder=" Digite o nome da rede WIFI" required="required" value="" />
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
<a href="/home.html">
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

ESP8266WebServer server(80);
WiFiServer servidor(80);

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200);

    IPAddress staticIP(192, 168, 4, 2); // IP set to Static
    IPAddress gateway(192, 168, 4, 1);  // gateway set to Static
    IPAddress subnet(255, 255, 255, 0); // subnet set to Static

    WiFi.mode(WIFI_AP); // Working mode only as Acess Point

    WiFi.softAP("Sensor de Chuva", "12345678", 2, 0);
    WiFi.config(staticIP, gateway, subnet);

    server.begin();
    servidor.begin();

    Serial.println("Server started");
    Serial.println(WiFi.softAPIP());
}

void loop()
{
    WiFiClient client = servidor.available();
    if (!client)
    {
        return;
    }

    // Wait until the client sends some data
    while (!client.available())
    {
        delay(1);
    }

    String req = client.readStringUntil('\r');
    req = req.substring(req.indexOf("/") + 1, req.indexOf("HTTP") - 1);
    Serial.println(req);
    client.flush();

    // Match the request

    if (req.indexOf("/") != -1)
    {
        server.send(200, "text/html", PAGINA_HTML_HOME);
    }
    else if (req.indexOf("/config_wifi") != -1)
    {
        server.send(200, "text/html", PAGINA_HTML_WIFI);
    }
    else if (req.indexOf("/salvar-senha") != -1)
    {
        String ssid = server.arg(0);
        String password = server.arg(1);

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

        server.send(200, "text/plain", PAGINA_HTML_WIFI);
    }
    else
    {
        client.print("Invalid Request");
        client.flush();
        client.stop();
        return;
    }

    client.print("HTTP/1.1 200 OK\n\n");
    client.flush();
}