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
const int LIMITE_PICO = 1980; // define o limite de sinal analógico para considerar um batimento. AJUSTAR PARA CALIBRAÇÃO VALORES VÁLIDOS: entre 1985 a 2100
boolean statusContagem;
int batida, bpm = 0;
unsigned long intervalo;

int PINO = 34; // pino que recebe sinal analógico do sensor

/*  domínio de rede  */

const char *URL_servidor = "http://192.168.xx.xx/php/testePhp/para_projeto/getESP.php"; // rota para se conectar à API coletora

Adafruit_SSD1306 display(LARGURA, ALTURA, &Wire, -1); // instância do objeto do display oled

/**************************** FUNÇÕES *********************************************/

/* enviar dados ao servidor */
void enviarDados(const char* sensor, float batimento)
{
  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;

    http.begin(URL_servidor);                           // iniciar conexão com o servidor
    http.addHeader("Content-Type", "application/json"); // cabeçalho json

    /*criar JSON*/
    JsonDocument doc;
    doc["sensor"] = sensor; // enviar as variáveis às chaves do JSON
    doc["valor"] = batimento;
    String requisicaoCorpo;
    serializeJson(doc, requisicaoCorpo);

    Serial.println("Enviando os dados: " + requisicaoCorpo); // depuração

    int respostaHttp = http.POST(requisicaoCorpo); // obter resposta do protocolo http

    if (respostaHttp > 0)
    {
      String respostaServidor = http.getString();
      Serial.println("Resposta do servidor: " + respostaServidor);
    }
    else
    {
      Serial.println("Erro ao enviar os dados: " + respostaHttp);
    }
    http.end(); // finalizar a conexão http
  }
  else
  {
    Serial.println("Wifi nao conectado");

    display.clearDisplay();
    display.setCursor(0, 0);
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

  Serial.println(valorSensor);
  Serial.print(">Variacao: "); 
  Serial.println(valorSensor);

  if (statusContagem == 0)
  {
    if (valorSensor >= LIMITE_PICO)
    {
      Serial.println(valorSensor);
      statusContagem = 1;
      batida++;
      Serial.println("Batida detectada");
      Serial.print("Batida: ");
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

  if (millis() - intervalo >= 15000)
  {

    bpm = batida * 4;
    batida = 0;
    Serial.print("BPM: ");
    Serial.println(bpm);
    intervalo = millis();

   display.clearDisplay();
   display.setCursor(0, 0);
   display.setTextSize(2);
   display.setTextWrap(true);
   display.setTextColor(SSD1306_WHITE);
   display.print("BPM: ");
   display.setCursor(64, 32);
   display.setTextSize(2.5);
   display.println(bpm);
   display.display();
  }

  //enviarDados("Cardiaco", bpm);

  return bpm;
}

/**************************** EXECUÇÃO ********************************************/
void setup()
{
  Serial.begin(TAXA_SERIAL);

  /* leitura do sensor */
  //analogReadResolution(12);       // resolução da leitura analógica para 12 bits

  delay(1500);

  WiFiManager wfMan;

  // wfMan.resetSettings();

  /* iniciar display */

  if (!display.begin(SSD1306_SWITCHCAPVCC, ENDERECO_DISPLAY))
  {
    Serial.println("Falha de inicializacao do display");
    while (true);
  }

  Serial.println("Display: OK");

  /* exibir mensagem inicial */
  display.clearDisplay();              // limpar display
  display.setCursor(0, 0);             // definir posição do conteúdo (x, y)
  display.setTextSize(1.5);              // tamanho do texto
  display.setTextColor(SSD1306_WHITE); // cor do texto
  display.setTextWrap(true);
  display.println("Cardio");
  display.setCursor(32, 16);
  display.print("Sense"); // exibir mensagem      
  display.setCursor(0, 21);
  display.println("Precisao em cada batida");
  display.display();                // mostrar no display
  delay(4000);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.print("BPM: ");

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.println("Aguardando conexao com wifi...");
  display.display();

  /* conectando ao wifi */

  // wfMan.resetSettings();

  if (!wfMan.autoConnect("CardioSense", "CardioSense123"))
  {
    Serial.println("Falha ao se conectar ao WiFi...");

    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.println("Falha de conexao com o WiFi");
    display.display();
    ESP.restart();
    delay(1500);
  }

  Serial.println("Conexao: OK");

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.printf("Conectado!");
  display.display();
  delay(2000);

} // setup

void loop()
{

  BPM();
  int bpm = BPM();

  display.println(bpm);
  
  delay(150);
}
