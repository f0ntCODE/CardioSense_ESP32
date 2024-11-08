/*CRÉDITOS:

 */

/* Fazer testes de display e posicionamento de elementos */

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/* variáveis definidas */

#define LARGURA 128           // largura da tela
#define ALTURA 64             // altura da tela
#define TAXA_SERIAL 115200    // taxa de atualização serial
#define ENDERECO_DISPLAY 0x3c // endereço do display
int PINO = 34; // pino que recebe sinal analógico do sensor

/*  domínio de rede  */

const char *URL_servidor = "http://192.168.15.10:80/api/test/esp/post"; // endereço do servidor

Adafruit_SSD1306 display(LARGURA, ALTURA, &Wire, -1); // instância do objeto do display oled

/**************************** EXECUÇÃO ********************************************/


const unsigned char epd_bitmap_coracao [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x78, 0x3f, 0xfc, 0x3f, 0xfc, 0x7f, 0xfc, 0x3f, 0xfc, 
	0x3f, 0xfc, 0x1f, 0xf8, 0x0f, 0xf0, 0x07, 0xc0, 0x03, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char epd_bitmap_no_wifi [] PROGMEM = {
	0x00, 0x00, 0x08, 0x00, 0x0c, 0x80, 0x06, 0xf8, 0x36, 0x1c, 0x63, 0x06, 0x09, 0xb0, 0x18, 0x98, 
	0x00, 0xc0, 0x06, 0x60, 0x00, 0x20, 0x01, 0xb0, 0x01, 0x80, 0x00, 0x00
};

const unsigned char epd_bitmap_connected [] PROGMEM = {
	0x03, 0xc0, 0x1e, 0x78, 0x30, 0x0c, 0x40, 0x02, 0x07, 0xe0, 0x18, 0x18, 0x10, 0x08, 0x03, 0xc0, 
	0x06, 0x60, 0x00, 0x00, 0x01, 0x80, 0x01, 0x80
};

const unsigned char epd_bitmap_logotipo [] PROGMEM = {
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
	0x00, 0x00, 0x00, 0x00
};

void setup()
{

  Serial.begin(TAXA_SERIAL);
  int bpm = 100;

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
  display.clearDisplay();
  display.drawBitmap(46, 14, epd_bitmap_logotipo, 32, 49, WHITE);      
  display.setCursor(16, 0);     
  display.setTextSize(1);      
  display.setTextColor(WHITE); 
  display.println("Cardio");
  display.setCursor(76, 0);
  display.print("Sense"); // exibir mensagem
  display.setTextWrap(true);
  display.display(); // mostrar no display
  delay(4000);

//apresentação dos dados
 display.clearDisplay();
 display.setCursor(8, 0);
 display.setTextSize(2);
 display.setTextColor(WHITE);
 display.print("BPM");
 display.setCursor(6, 20);
 display.println(bpm);
 display.drawLine(52, 4, 52, 32, WHITE);
 display.setTextSize(1);
 display.setCursor(72, 8);
 display.print("Status: ");
 display.setCursor(62, 22);
 display.println("Conectado");
 display.drawBitmap(18, 40, epd_bitmap_coracao, 16, 16, WHITE);
 display.drawBitmap(80, 40, epd_bitmap_connected, 16, 12, WHITE);
 display.display();


} // setup

void loop()
{


}
