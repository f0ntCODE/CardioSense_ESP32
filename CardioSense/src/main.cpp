/*CRÉDITOS:

  -> https://www.chatgpt.org;
  -> https://www.youtube.com/;

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

const char *URL_servidor = "http://192.168.15.10:80/api/test/esp/post"; // endereço do servidor

Adafruit_SSD1306 display(LARGURA, ALTURA, &Wire, -1); // instância do objeto do display oled

/*********** ÍCONE **********/

const unsigned char IconeWifiNaoConectado[] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x18, 0x3C, 0x7E, 0x7E, 0x66, 0x66, 0x3C, 0x18,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x81, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x81,
    0x00, 0x00, 0x00, 0x00};

/**************************** FUNÇÕES *********************************************/

void conectar()
{

  WiFiManager wfMan;

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.println("Aguardando conexao com wifi...");
  display.display();

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
void enviarDados(const char* sensor, float batimento)
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

    // display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(2);
    display.setTextWrap(true);
    display.setTextColor(SSD1306_WHITE);
    display.print("BPM: ");
    display.setCursor(16, 24);
    display.setTextSize(2);
    display.println(bpm);
    //display.drawLine(0, 12, 64, 12, WHITE);
    display.display();

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
  WiFiManager wf;

  // wf.resetSettings(); // descomentar somente quando estiver em fase de desenvolvimento

  /* leitura do sensor */

  delay(1000);
  /* iniciar display */

  if (!display.begin(SSD1306_SWITCHCAPVCC, ENDERECO_DISPLAY))
  {
    Serial.println("\tDisplay: falhou");
    while (true)
      ;
  }

  Serial.println("\tDisplay: OK");

  /* exibir mensagem inicial */
  display.clearDisplay();      // limpar display
  display.setCursor(0, 0);     // definir posição do conteúdo (x, y)
  display.setTextSize(2);      // tamanho do texto
  display.setTextColor(WHITE); // cor do texto
  display.println("Cardio");
  display.setCursor(32, 16);
  display.setTextColor(WHITE);
  display.print("Sense"); // exibir mensagem
  display.drawLine(0, 34, 128, 34, WHITE);
  display.setTextSize(1);
  display.setTextWrap(true);
  display.setCursor(16, 38);
  display.println("Precisao em cada");
  display.setCursor(44, 48);
  display.println("batida");
  display.display(); // mostrar no display
  delay(4000);

  /* conectando ao wifi */
  //conectar();

  /*if (WiFi.isConnected())
  {

    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.printf("Conectado!");
    display.display();
    delay(1000);
  }

  else
  {

    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setTextWrap(true);
    display.printf("Nao foi possivel se conectar a rede. Tente novamente");
    display.display();
    //ESP.restart();
    delay(2000);
  }*/

} // setup

void loop()
{
  display.clearDisplay();



  Serial.println("Host: " + WiFi.localIP().toString());
  Serial.println("SSID: " + WiFi.SSID());
  Serial.println("Status: " + WiFi.status());

  /*if(!WiFi.isConnected()){
    display.drawBitmap(56, 24, IconeWifiNaoConectado, 16, 16, WHITE); //precisa de correção
    display.display();
  }*/

  BPM();



  delay(125);
}
