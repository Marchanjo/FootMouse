/*
  FootMouseLeonardo.ino - Code to implement a mouse to be controlled by the feet, 
  use Arduino Leonardo compatible board (ATmega32u), have one thumb joystick to control the 
  mouse cursor movement, one thumb joystick to scroll up and down (same function 
  of wheel mouse), one button for the mouse button 1 and a button for the mouse 
  button 2.
	
  License: CC BY-SA 4.0: Creative Commons Attribution-ShareAlike 4.0 International 
  License. Feel free to use and abuse this code however you'd like. If you find it 
  useful please attribute, and SHARE-ALIKE! 
  
  Created 27 Oct 2014
  by Marcelo Archnajo Jose (marcelo_archanjo@yahoo.com.br)
   
  Distributed as-is; no warranty is given.
 */





//Baseado no MyMouseBTRNJoy10.ino que foi usado nos testes do doutorado, mas este código visa  a Arduino Leaonardo mouse com fio (sem o Bluetooth) 

//curva 1 do código (que é a curva 4 da documentação) está com o dead zone maior 45 ao inves de 32. 
//Deadzone maior ainda agora 80 (parece que ojoystick esta deteriorando) de maior que 45 (valor usado nos testes da tese) mesmo com o MyMouseBTRNJoy10.ino ocorre deslocamento para a esquerda


//esta funcionando e parece que a resposta é melhor do que com o Bluetooth, pode ser que o throughput aumente
//26/05 o  comentário acima envolvia o código sem delay(20); com o delay o comportamento ficou muito parecido com o LCS Bluetooth
//faça o debug do flash tem vezes que um left ve acompanhado do right (talvez do retorno para o centro)


//inicio tabela usada com o Bluetooth
//0x80 128 Teclado Cursor up
//0x82 130 Teclado Cursor down

//0x88 136  Teclado Page up
//0x89 137  Teclado Page down



//0x8A	138	Mouse buttons up
//0x8B	139	Mouse up (10px)
//0x8C	140	Mouse right (10px)
//0x8D	141	Mouse down (10px)
//0x8E	142	Mouse left (10px)
//0x8F	143	Mouse button 1 (first clear with 138)
//0x90	144	Mouse button 2 (first clear with 138)
//0x91	145	Mouse button 3 (first clear with 138)
//fim tabela usada com o Bluetooth




//joy vcc=5v vml
const int VERT = 1; //joy analog 
const int HORIZ = 0; //joy analog
const int VERT2 = 2; //joy analog
const int SEL = 15; //joy digital vrd
const int BT1 = 4;
const int BT2 = 3;



//test single joy//const int VERT2 = 3; //joy analog bco
//test single joy//const int HORIZ2 = 4; //joy analog cnz
//test single joy//const int SEL2 = 6; //joy digital mrn
int vertical, horizontal, vertical2, select;//joy
int lastVertical, lastHorizontal;
int delayTime=20;
//int delayTimeFlash; //usado para definir o que significa um flash (movimento rápido em uma direção), variável relacionada delayTime
//test single joy//int vertical2, horizontal2, select2;//joy2

//int vert;
//const static uint8_t bt_txdPin = 0;
//const static uint8_t bt_rxdPin = 1;
//const static uint8_t bt_dtrPin = 6;//4;
//const static uint8_t bt_rstPin = 5;

//00:11:12:06:03:59 linvor 1234 meu Bluetooth da China (sem trocar firmware não serve como mouse

int x,y;
//test single joy//int x2,y2;
int wheel;
boolean button1,button2,button3,button1Last,button2Last;
boolean button1ClickLock;
boolean deadZonePass;

boolean mouseRefresh;
void mouseClear(void);
void mouseStep(void);
void mouseStep2(void);
void readData(void);
int joystickSteperMouse0(int value);
int joystickSteperMouse1(int value);
int joystickSteperMouse2(int value);
int curva=1;//Curva 4 a mais lenta e melhor
unsigned long currentTime, lastTime;
int difTime,difHor,difVert;
int velHor,velVert;
//bool repairing = true;


void setup()
  {
  pinMode(SEL,INPUT);            //joy set pin to input
  digitalWrite(SEL, HIGH);       // turn on pullup resistors
  
  pinMode(BT1, INPUT);           // set pin to input
  digitalWrite(BT1, HIGH);       // turn on pullup resistors
  
  pinMode(BT2, INPUT);           // set pin to input
  digitalWrite(BT2, HIGH);       // turn on pullup resistors
 
  Serial.begin(9600);
  
  
  wheel=0;  
  button1=false;//inicia o botão1 como aberto
  button2=false;//inicia o botão2 como aberto
  button3=false;//inicia o botão3 como aberto
  button1ClickLock=false;
  deadZonePass=false;
  currentTime = millis();//incialização das váriaveis de tempo
  lastTime=currentTime;//incialização das váriaveis de tempo
  //delayTimeFlash=delayTime+5;//incialização da váriavel que define o flash
  }
 

 
void loop()
  {
  lastVertical=vertical;
  lastHorizontal=horizontal;
  vertical = analogRead(VERT); //joy will be 0-1023 
  horizontal = analogRead(HORIZ); //joy will be 0-1023
  vertical2 = analogRead(VERT2); //joy will be 0-1023
  
  x=joystickSteperMouse1(horizontal); y=joystickSteperMouse1(vertical); //Curva4    
      
      
  mouseRefresh=false;
  if((x!=0)||(y!=0))  mouseRefresh=true;
  //else Mouse.release(MOUSE_ALL);// to avoid continuous click 
  

  //Evita multiplos acionamentos
  //botão1 acionado
  
  button1Last=button1;
  button2Last=button2;
  
  if (digitalRead(BT1)==0) button1=true; 
    else button1=false;
  
  if (digitalRead(BT2)==0) button2=true; 
    else button2=false;
    
  if((button1!=button1Last)||(button2!=button2Last)) 
    mouseRefresh=true;
    
  button3=false;
  
 /*   
    
  if( ((digitalRead(SEL)==0)&&(button1==false)) || ((digitalRead(BT1)==0)&&(button1==false)) )
    {button1=true;
     button2=false;//Importante para o Bluetooth <<<<<<<<<<<<< 
     button3=false;//Importante para o Bluetooth <<<<<<<<<<<<<
     mouseRefresh=true;
    Serial.print("\nBotao1 acionado");
    }//manda a informação de botão acionado uma vez
  
  //botão1 aberto
  if( ((digitalRead(SEL)==1)&&(button1==true)) || ((digitalRead(BT1)==1)&&(button1==true)) )
    {button1=false;
     button2=false;//Importante para o Bluetooth <<<<<<<<<<<<< 
     button3=false;//Importante para o Bluetooth <<<<<<<<<<<<<
     mouseRefresh=true;
    Serial.print("\nBotaoa1 aberto");
  }//manda a informação de botão levantado uma vez
  
  //botão2 acionado
  if((digitalRead(BT2)==0)&&(button1==false))
    {button1=false;//Importante para o Bluetooth <<<<<<<<<<<<< 
     button2=true;
     button3=false;//Importante para o Bluetooth <<<<<<<<<<<<<
     mouseRefresh=true;
    Serial.print("\nBotao2 acionado");
    }//manda a informação de botão acionado uma vez
  
  //botão2 aberto
  if((digitalRead(BT2)==1)&&(button1==true))
    {button1=false;//Importante para o Bluetooth <<<<<<<<<<<<<
     button2=false; 
     button3=false;//Importante para o Bluetooth <<<<<<<<<<<<<
     mouseRefresh=true;
    Serial.print("\nBotao2 aberto");
  }//manda a informação de botão levantado uma vez
  
  */
  if(vertical2>800) 
    {wheel=1;
    mouseRefresh=true;
    delay(100);}//up //digitalWrite(greenLed, HIGH );digitalWrite(redLed, LOW);digitalWrite(blueLed, LOW);
  
  // Wheel Down
  if(vertical2<224) 
    {wheel=-1;
    mouseRefresh=true;
    delay(100);}//down //digitalWrite(redLed, HIGH);digitalWrite(blueLed, HIGH );digitalWrite(greenLed, LOW);
  
  if((vertical2<=800)&&(vertical2>=224)) wheel=0;
  
  
  if(mouseRefresh) {mouseStep();}//digitalWrite(redLed, HIGH );digitalWrite(greenLed, HIGH);}//digitalWrite(blueLed, HIGH);}
  }
  

int joystickSteperMouse0(int value)
  {
  int retorno;
  value=-(value-511);
  if(value<=-480) retorno=-13;
  if((value>-480)&&(value<=-416)) retorno=-9;
  if((value>-416)&&(value<=-352)) retorno=-6;
  if((value>-352)&&(value<=-288)) retorno=-4;
  if((value>-288)&&(value<=-224)) retorno=-3;
  if((value>-224)&&(value<=-160)) retorno=-2;
  if((value>-160)&&(value<=-96)) retorno=-1;
  if((value>-96)&&(value<=-32)) if(retorno==-1) retorno=0; else retorno=-1;//equivale a -0,5
  if((value>-32)&&(value<=32)) retorno=0;//dead zone 
  if((value>32)&&(value<=96)) if(retorno==1) retorno=0; else retorno=1;//equivale a 0,5
  if((value>96)&&(value<=160)) retorno=1;
  if((value>160)&&(value<=224)) retorno=2;
  if((value>224)&&(value<=288)) retorno=3;
  if((value>288)&&(value<=352)) retorno=4;
  if((value>352)&&(value<=416)) retorno=6;
  if((value>416)&&(value<=480)) retorno=9;
  if(value>480) retorno=13;
  return retorno;
  }
  
int joystickSteperMouse1(int value)//curva 4 mais suave e lenta que a 3
  {
  int retorno;
  int deadzone=90;//Mudou de 45 para 70 (havia um deslocamento para a esquerda
  value=-(value-511);
  if(value<=-480) retorno=-5;
  if((value>-480)&&(value<=-416)) retorno=-4;
  if((value>-416)&&(value<=-352)) retorno=-3;
  if((value>-352)&&(value<=-288)) retorno=-2;
  if((value>-288)&&(value<=-224)) retorno=-2;
  if((value>-224)&&(value<=-160)) retorno=-1;
  if((value>-160)&&(value<=-96)) retorno=-1;
  if((value>-96)&&(value<=-deadzone)) if(retorno==-1) retorno=0; else retorno=-1;//equivale a -0,5
  if((value>-deadzone)&&(value<=deadzone)) retorno=0;//dead zone maior que nas outras curvas aqui é 38 e nas outras é 32
  if((value>deadzone)&&(value<=96)) if(retorno==1) retorno=0; else retorno=1;//equivale a 0,5
  if((value>96)&&(value<=160)) retorno=1;
  if((value>160)&&(value<=224)) retorno=1;
  if((value>224)&&(value<=288)) retorno=2;
  if((value>288)&&(value<=352)) retorno=2;
  if((value>352)&&(value<=416)) retorno=3;
  if((value>416)&&(value<=480)) retorno=4;
  if(value>480) retorno=5;
  return retorno;
  }
  
int joystickSteperMouse2(int value)//curva 3 mais suave e lenta que a 0
  {
  int retorno;
  value=-(value-511);
  if(value<=-480) retorno=-9;
  if((value>-480)&&(value<=-416)) retorno=-7;
  if((value>-416)&&(value<=-352)) retorno=-5;
  if((value>-352)&&(value<=-288)) retorno=-4;
  if((value>-288)&&(value<=-224)) retorno=-3;
  if((value>-224)&&(value<=-160)) retorno=-2;
  if((value>-160)&&(value<=-96)) retorno=-1;
  if((value>-96)&&(value<=-32)) if(retorno==-1) retorno=0; else retorno=-1;//equivale a -0,5
  if((value>-32)&&(value<=32)) retorno=0;//dead zone 
  if((value>32)&&(value<=96)) if(retorno==1) retorno=0; else retorno=1;//equivale a 0,5
  if((value>96)&&(value<=160)) retorno=1;
  if((value>160)&&(value<=224)) retorno=2;
  if((value>224)&&(value<=288)) retorno=3;
  if((value>288)&&(value<=352)) retorno=4;
  if((value>352)&&(value<=416)) retorno=5;
  if((value>416)&&(value<=480)) retorno=7;
  if(value>480) retorno=9;
  return retorno;
  }


void mouseClear(void)
  {
  Mouse.release(MOUSE_ALL);// to avoid continuous click
  /*int value=0x00;
 
  Serial.write(0xFD);
  Serial.write(0x05);
  Serial.write(0x02);
  Serial.write(value);//botão
  Serial.write(value);//(x);
  Serial.write(value);//(y);
  Serial.write(value);//(wheel);*/
  
  delay(20);
  //delay(delayTime);
  }
  

void mouseStep(void)
  {
  Mouse.move(x, y, wheel);
  
 /* int value=0x00;
   
  Serial.write(0xFD);
  Serial.write(0x05);
  Serial.write(0x02);
  
  */
  if((!button1)&&(!button2)&&(!button3)) Mouse.release(MOUSE_ALL);// to avoid continuous click
  if(button3) Mouse.press(MOUSE_MIDDLE);
  if(button2) Mouse.press(MOUSE_RIGHT); 
  if(button1) Mouse.press(MOUSE_LEFT);//botão1 é prioritário
 /*
    
  Serial.write(x);
  Serial.write(y);
  Serial.write(wheel);*/
 
  delay(20);
  //delay(delayTime);
  }   


