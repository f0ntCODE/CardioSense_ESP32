/*CRÉDITOS:

  -> https://www.chatgpt.org;
  -> https://www.youtube.com;
  -> https://www.how2electronics.com;

 */

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

/* variáveis definidas */

#define LARGURA 128           // largura da tela
#define ALTURA 64             // altura da tela
#define TAXA_SERIAL 115200    // taxa de atualização serial
#define ENDERECO_DISPLAY 0x3c // endereço do display

/****ELEMENTOS DO DISPLAY****/
const unsigned char coracao[] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x78, 0x3f, 0xfc, 0x3f, 0xfc, 0x7f, 0xfc, 0x3f, 0xfc,
    0x3f, 0xfc, 0x1f, 0xf8, 0x0f, 0xf0, 0x07, 0xc0, 0x03, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};

const unsigned char sem_wifi[] PROGMEM = {
    0x00, 0x00, 0x08, 0x00, 0x0c, 0x80, 0x06, 0xf8, 0x36, 0x1c, 0x63, 0x06, 0x09, 0xb0, 0x18, 0x98,
    0x00, 0xc0, 0x06, 0x60, 0x00, 0x20, 0x01, 0xb0, 0x01, 0x80, 0x00, 0x00};

const unsigned char conectado[] PROGMEM = {
    0x03, 0xc0, 0x1e, 0x78, 0x30, 0x0c, 0x40, 0x02, 0x07, 0xe0, 0x18, 0x18, 0x10, 0x08, 0x03, 0xc0,
    0x06, 0x60, 0x00, 0x00, 0x01, 0x80, 0x01, 0x80};

const unsigned char logotipo[] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0x00, 0x03, 0xff, 0xff, 0x00,
    0x03, 0xff, 0xff, 0x00, 0x03, 0xff, 0xff, 0x80, 0x03, 0x80, 0x07, 0x80, 0x03, 0x80, 0x07, 0x80,
    0x03, 0x80, 0x03, 0x80, 0x07, 0xff, 0xff, 0x80, 0x07, 0xff, 0xff, 0xc0, 0x0f, 0xff, 0xff, 0xe0,
    0x1f, 0xff, 0xff, 0xf0, 0x3c, 0x00, 0x00, 0xf0, 0x3c, 0x00, 0x00, 0x70, 0x3c, 0x00, 0x00, 0x70,
    0x3c, 0x00, 0x00, 0x78, 0x3c, 0x00, 0x00, 0x7e, 0x3c, 0x00, 0x00, 0x7e, 0x3c, 0x00, 0x10, 0x7e,
    0x3c, 0x00, 0x28, 0x78, 0x3c, 0x00, 0x28, 0x70, 0x3c, 0x08, 0x28, 0x70, 0x3c, 0x18, 0x29, 0x70,
    0x3c, 0x1a, 0x6b, 0x70, 0x3f, 0xef, 0x4e, 0x70, 0x3c, 0x0d, 0x44, 0x70, 0x3c, 0x01, 0x80, 0x70,
    0x3c, 0x01, 0x80, 0x70, 0x3c, 0x01, 0x80, 0x70, 0x3c, 0x00, 0x00, 0x70, 0x3c, 0x00, 0x00, 0x70,
    0x3c, 0x00, 0x00, 0x70, 0x3c, 0x00, 0x00, 0x70, 0x3c, 0x00, 0x00, 0x70, 0x3c, 0x00, 0x00, 0x70,
    0x1f, 0xff, 0xff, 0xf0, 0x1f, 0xff, 0xff, 0xe0, 0x0f, 0xff, 0xff, 0xc0, 0x07, 0xff, 0xff, 0x80,
    0x03, 0x80, 0x03, 0x80, 0x03, 0x80, 0x07, 0x80, 0x03, 0x80, 0x07, 0x80, 0x03, 0xc0, 0x07, 0x80,
    0x03, 0xff, 0xff, 0x80, 0x03, 0xff, 0xff, 0x00, 0x01, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00};

/**** variáveis globais ****/
/*  sensor  */
const int LIMITE_PICO = 2070; // define o limite de sinal analógico para considerar um batimento. AJUSTAR PARA CALIBRAÇÃO

boolean statusContagem;
int batida, bpm = 0;
unsigned long intervalo;
unsigned long tempoAntigo = 0;
unsigned long intervaloEnvio = 15000;

int PINO = 34; // pino que recebe sinal analógico do sensor

/*  domínio de rede  */

const char *URL_servidor = "http://192.168.15.10:80/api/esp/data/receive"; // endereço do servidor

Adafruit_SSD1306 display(LARGURA, ALTURA, &Wire, -1); // instância do objeto do display oled

/**************************** FUNÇÕES *********************************************/

void conectar()
{

  WiFiManager wfMan;

  if (!wfMan.autoConnect("CardioSense-ESP", "cardiosense123"))
  {

    Serial.println("Conexão: desconectado");
  }
  else
  {

    Serial.println("Conexão: OK");
  }
}

/* enviar dados ao servidor */
void enviarDados(const char *sensor, float batimento)
{

  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;
    http.begin(URL_servidor);                           // iniciar conexão com o servidor
    http.addHeader("Content-Type", "application/json"); // cabeçalho json
    Serial.println("\t Cabeçalho: OK\n");
    /*criar JSON*/
    JsonDocument doc;
    doc["bpm"] = sensor; // enviar as variáveis às chaves do JSON
    doc["valor"] = batimento;
    String requisicaoCorpo;
    serializeJson(doc, requisicaoCorpo);
    Serial.println("Enviando os dados: " + requisicaoCorpo + "\n"); // depuração
    int respostaHttp = http.POST(requisicaoCorpo);                  // obter resposta do protocolo http
    Serial.println("\t Obtendo resposta do servidor...\n");
    if (respostaHttp > 0)
    {
      String respostaServidor = http.getString();
      Serial.println("\t Resposta do servidor:" + respostaServidor);
    }
    else
    {
      Serial.println("\t Erro ao enviar os dados: " + respostaHttp);
    }
    http.end(); // finalizar a conexão http
  }
  else
  {
    Serial.println("\t Wifi nao conectado");
    display.setCursor(0, 62);
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.println("WiFi desconectado...");
    display.display();
  }
} // enviarDados

/* cosiderar um pico de batimento */

int BPM()
{

  int valorSensor = analogRead(PINO);
  analogSetAttenuation(ADC_11db);

  unsigned long tempoAtual = millis();

  /* será enviada para o plotter */
  Serial.println(valorSensor);

  Serial.print(">Variacao: ");
  Serial.println(valorSensor);

  /*******************************/
  if (statusContagem == 0)
  {

    if (valorSensor >= LIMITE_PICO)
    {
      Serial.println(valorSensor);
      statusContagem = 1;
      batida++;
      Serial.println("Batida detectada");
      Serial.print("\tBatida: ");
      Serial.println(batida);
    }
  }
  else
  {

    if (valorSensor < LIMITE_PICO)
    {
      statusContagem = 0;
    }
  }

  if (millis() - intervalo >= 21000)
  {

    bpm = batida * 4;
    batida = 0;
    Serial.print("BPM: ");
    Serial.println(bpm);
    intervalo = millis();

    if (WiFi.isConnected())
    {

      enviarDados("Cardiaco", bpm);
    }
  }

  else
  {

    Serial.println("Não foi possível enviar os dados para a API.");
  }
  return bpm;
}

/**************************** EXECUÇÃO ********************************************/
void setup()
{

  Serial.begin(TAXA_SERIAL);

  /* leitura do sensor */

  Serial.println("Iniciando sistema...");
  delay(2000);
  /* iniciar display */

  if (!display.begin(SSD1306_SWITCHCAPVCC, ENDERECO_DISPLAY))
  {
    Serial.println("\tDisplay: falhou");
    while (true)
      ;
  }

  Serial.println("\tDisplay: OK");

  // exibir mensagem inicial
   display.clearDisplay();
   display.drawBitmap(46, 14, logotipo, 32, 49, WHITE);
   display.setCursor(16, 0);
   display.setTextSize(1);
   display.setTextColor(WHITE);
   display.println("Cardio");
   display.setCursor(76, 0);
   display.print("Sense"); // exibir mensagem
   display.setTextWrap(true);
   display.display(); // mostrar no display
   delay(4000);

   display.clearDisplay();
   display.setCursor(32, 0);
   display.setTextSize(2);
   display.setTextColor(SSD1306_WHITE);
   display.printf("AVISO");
   display.setCursor(4, 16);
   display.setTextSize(1);
   display.setTextWrap(true);
   display.println("Certifique-se de se  conectar a rede para gravar no site \n oficial da \nCardioSense.");
   display.display();
   delay(6000);

  conectar();

} // setup

void loop()
{
  display.clearDisplay();

  Serial.println("Host: " + WiFi.localIP().toString());
  Serial.println("SSID: " + WiFi.SSID());
  Serial.println("Status: " + WiFi.status());

  /* ALTERAÇÃO DE ÍCONES */
  if (!WiFi.isConnected())
  {
    display.setCursor(54, 22);
    display.println("Desconectado");
    display.drawBitmap(80, 40, sem_wifi, 16, 12, WHITE);
  }
  else
  {
    display.setCursor(64, 22);
    display.println("Conectado");
    display.drawBitmap(80, 40, conectado, 16, 12, WHITE);
  }

  display.setCursor(8, 0);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.print("BPM");
  display.setCursor(6, 20);
  display.println(BPM());
  display.drawLine(48, 4, 48, 32, WHITE);
  display.setTextSize(1);
  display.setCursor(72, 8);
  display.print("Status: ");
  display.drawBitmap(18, 40, coracao, 16, 16, WHITE);
  display.display();

  delay(125);
}