#include <Arduino.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>

//========================================BUZZER================================================
int pinBuzzer = 11;

#define SONIDO_CURSOR tone(pinBuzzer,2500,100);
#define SONIDO_BARCO tone(pinBuzzer,4000,150);
#define SONIDO_ELIM tone(pinBuzzer,250,100);
#define SONIDO_ADD1 tone(pinBuzzer,3000,80);
#define SONIDO_ADD2 tone(pinBuzzer,2000,80);
#define SONIDO_NOVAL_BARCO1 tone(pinBuzzer,800,80);
#define SONIDO_NOVAL_BARCO2 tone(pinBuzzer,900,80);
#define SONIDO_AYUDA tone(pinBuzzer,80,100);
#define SONIDO_ASK_FIJAR1 tone(pinBuzzer,1535,80);
#define SONIDO_ASK_FIJAR2 tone(pinBuzzer,1853,80);

void soundNoValBarco(){
  SONIDO_NOVAL_BARCO1
  SONIDO_NOVAL_BARCO2
}

void soundAdd(){
  SONIDO_ADD1
  SONIDO_ADD2
}

void soundAskFijar(){
  SONIDO_ASK_FIJAR1
  SONIDO_ASK_FIJAR2
}

//========================================KEYPAD================================================
const byte ROWS = 4;
const byte COLS = 4;
//Assign key values in 4x4 matrix
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
//connect to the row pinouts of the keypad
byte rowPins[ROWS] = {12, 10, 9, 8}; //R1,R2,R3,R4
//connect to the column pinouts of the keypad
byte colPins[COLS] = {7, 6, 5, 4};//C1,C2,C3,C4

char mapeoNumeros(int num){
  return num+48;
}

char mapeoFilas(int num){
  return num + 65;
}

int mapeoLetras(char letra){
  if(letra<65){
    return letra - 48;
  }
  //NUMERO
  else{
    return letra - 65;
  }
}

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

//========================================PANTALLA LCD================================================

LiquidCrystal_I2C lcd(0x27,16,2);

volatile int iHappened =0;
volatile int indice=1;
int pinSubir = 2;
int pinBajar = 3;

/*byte check[8] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
};
*/

byte popaVertical[8] = {
  B11111,
  B10001,
  B10001,
  B10001,
  B10001,
  B10001,
  B10001,
  B10001,
};

byte cuerpoHorizontal[8] = {
  B11111,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
};

byte cuerpoVertical[8] = {
  B10001,
  B10001,
  B10001,
  B10001,
  B10001,
  B10001,
  B10001,
  B10001,
};

byte barco[8] = {
  B11000,
  B10100,
  B10010,
  B10001,
  B10010,
  B10100,
  B11000,
};

byte fallo[8] = {
  B00000,
  B00100,
  B00000,
  B10101,
  B00000,
  B00100,
  B00000,
};

byte misil[8] = {
  B10101,
  B01010,
  B00100,
  B01110,
  B01110,
  B11111,
  B01110,
  B00100,
};

byte objetivo[8] = {
  B01110,
  B10101,
  B10101,
  B11111,
  B10101,
  B10101,
  B01110,
};

byte mar[8] = {
  B00000,
  B01010,
  B10101,
  B00000,
  B00000,
  B01010,
  B10101,
  B00000,
};

//========================================JUEGO================================================

byte numBatalla;
byte batallasA;
byte batallasB;
int puntajeA=0;
int puntajeB=0;
byte cantidadA;
int misilesA[10][2] = {{4,0},{4,0},{4,0},{4,0},{4,0},{4,0},{4,0},{4,0},{4,0},{4,0}};
int misilesB[10][2] = {{1,3},{3,2},{0,1},{2,1},{3,8},{0,9},{1,5},{2,4},{4,0},{4,0}};
int misilesTirados = 0;
int barcos[10][2] = {{3,0},
                     {3,0},
                     {3,0},
                     {2,0},
                     {2,0},
                     {1,0},
                     {1,0},
                     {1,0},
                     {2,0},
                     {3,0}
  };
int letrasBarcos[10][2] = {{'A',0},
                           {'B',0},
                           {'C',0},
                           {'D',0},
                           {'E',0},
                           {'F',0},
                           {'G',0},
                           {'H',0},
                           {'I',0},
                           {'J',0}
  };
int numerosBarcos[10][2] = {{'0',0},
                           {'1',0},
                           {'2',0},
                           {'3',0},
                           {'4',0},
                           {'5',0},
                           {'6',0},
                           {'7',0},
                           {'8',0},
                           {'9',0}
  };
int barcosColocados = 0;
int estado = 0;

char tableroBarcosA[4][10] = {{'T','T','T','T','T','T','T','T','T','T'},
                              {'T','T','T','T','T','T','T','T','T','T'},
                              {'T','T','T','T','T','T','T','T','T','T'},
                              {'T','T','T','T','T','T','T','T','T','T'}
  };

char tableroBarcosB[4][10] = {{'T','0','0','0','T','T','5','B','T','A'},
                              {'2','2','T','T','3','T','T','B','T','A'},
                              {'T','T','T','T','T','T','T','T','T','A'},
                              {'6','T','T','1','1','1','T','T','4','T'}
  };


char tableroMisilesA[4][10] = {{'T','T','T','T','T','T','T','T','T','T'},
                               {'T','T','T','T','T','T','T','T','T','T'},
                               {'T','T','T','T','T','T','T','T','T','T'},
                               {'T','T','T','T','T','T','T','T','T','T'}
  };

void limpiarTableroBarcosA(){
  for (int i = 0; i<=3;i++)
    for(int j = 0; j<=9;j++)
      tableroBarcosA[i][j] = 'T';
}

void limpiarTableroMisilesA(){
  for (int i = 0; i<=3;i++)
    for(int j = 0; j<=9;j++)
      tableroMisilesA[i][j] = 'T';
}

int detSizeBarcoHorizontal(int popa,char fila[10]){
    
  int tam = 1;
  for(int i=popa+1;(i<=9)&&(fila[i]==fila[popa]);i++){
    tam ++;
  }

  return tam;
  
}

char detPosBarcoVertical(int fil,int col,char tablero[4][10]){

  int popa = 5;
  char posAc = tablero[fil][col];
  int tam = 1;

  //Encontrar Popa y tamaño
  for(int i = 0;i<=3;i++){
    if((posAc == tablero[i][col])&&(popa==5))popa = i;
    else if((posAc == tablero[i][col])&&(popa!=5))tam++;
  }

  if((tam==1)||((tam==2)&&(fil!=popa))||((tam==3)&&(popa + 2==fil))) return 'V'; //PROA
  if(((tam==2)||(tam==3))&&(fil==popa)) return 'M'; //POPA
  if(((tam==3)&&(popa + 1==fil))) return 'L';//CUERPO

  return 'E';
  
}

void impTablero(char tablero[4][10],int indice,int adds){

  int sizeBarco;
  int posAc;
  int puntaje;
  int puntajeLleno = 0;
  int auxPrint[3];
  int posPunt=-1;
  char imp[2][16];
  char pos;
  char flecha = 126;
  char aux;

  //Llenado de espacios inaccesibles
  imp[0][0] = 65+indice;imp[0][1] = 30;imp[0][12] = 30;
  imp[0][13] = 30;imp[0][14] = 30;imp[0][15] = 30;
  imp[1][0] = 66+indice;imp[1][1] = 30 ;imp[1][12] = 30;
  imp[1][13] = 30;imp[1][14] = 30;imp[1][15] = 30;
  
  
  for(int i = 0;i < 2;i++)
    for (int j = 0;j<10;j++){
      posAc = tablero[i+indice][j];
      if((posAc>=48)&&(posAc<=57)){
        //Barco horizontal encontrado
        sizeBarco = detSizeBarcoHorizontal(j,tablero[i+indice]);
        switch(sizeBarco){
          case 1:
            imp[i][j+2] = 'N';
            break;
          case 2:
            imp[i][j+2] = '[';
            imp[i][j+3] = '>';
            j++;
            break;
          case 3:
            imp[i][j+2] = '[';
            imp[i][j+3] = 'K';
            imp[i][j+4] = '>';
            j+=2;
            break;
        }
        //imp[i][j+2] = '[';
      } else if((posAc>=65)&&(posAc<=74)){
        //Barco vertical encontrado
        imp[i][j+2] = detPosBarcoVertical(indice+i,j,tablero);
      } else imp[i][j+2] = tablero[indice+i][j];
    }
  

  lcd.clear();

  for(int i = 0; i<2;i++){
   lcd.setCursor(0,i);
    for (int j = 0; j<16;j++){
      pos = imp[i][j];
      //mar
      if(pos=='T') {
        lcd.write(byte(3));
      }
      //objetivo
      else if(pos == 'X') {
        lcd.write(byte(2));
      }
      //Barco derribado
      else if(pos == 'S'){
        lcd.print('/');
      }
      //Misil fallado
      else if(pos == 'O')
        lcd.write(byte(1));
      //Popa vertical
      else if(pos == 'M')
        lcd.write(byte(4));
      //Cuerpo horizontal
      else if(pos == 'K')
        lcd.write(byte(5));
      //Cuerpo vertical
      else if(pos == 'L')
        lcd.write(byte(6));
      else if(pos == 'N')
      //Barco
        lcd.write(byte(7));
      else lcd.print(pos);
    }
  }  
  
  lcd.setCursor(14,1);
  lcd.noBlink();
  switch(adds){

  //TABLERO COLOCACION (0->agregando barcos 1-> barco eliminable 2->barcos fijables 3->10 barcos)
  //ENTRE 9 Y 7 BARCOS
  case 2: lcd.setCursor(13,0);
  lcd.print('#');
  lcd.print(flecha);
  lcd.setCursor(13,1);
  lcd.print('B');
  lcd.write(byte(7));
  lcd.print(mapeoNumeros(barcosColocados));
  break;
  case 3:

  case 9: 
  lcd.setCursor(13,1);
  lcd.print('#');
  lcd.print(flecha);
  lcd.print('M');
  if(adds == 3)break;
  //QUITAR BARCO
  case 1: lcd.setCursor(13,0);
  lcd.print("*X");
  if(adds == 9) break;
  //AGREGAR BARCO
  case 0: lcd.setCursor(13,1);
  if(estado == 1)lcd.print('#'); else lcd.print('B');
  lcd.write(byte(7));
  lcd.print(mapeoNumeros(barcosColocados));
  lcd.setCursor(14,1);
  break;
  //TABLERO ATAQUE (4->nada colocado 5-> colocando 6->colocados)
  case 6: lcd.blink();
  case 5: lcd.setCursor(13,0);
  lcd.print("*X");
  case 4: lcd.setCursor(13,1);
  lcd.print('#');
  lcd.write(byte(0));
  if (misilesTirados == 10) lcd.print('M');
  else lcd.print(mapeoNumeros(misilesTirados));
  lcd.setCursor(14,1);
  break;
  //TABLERO ATACADO (7-> tableroB 8-> tableroA)
  case 7:
  puntaje = puntajeA;
  puntajeLleno = 1;
  case 8:
  if(!puntajeLleno) puntaje = puntajeB;
  do{
    posPunt++;
    auxPrint[posPunt] = puntaje % 10;
    puntaje /= 10;
   }
   while(puntaje > 0);

   lcd.setCursor(13,0);
   lcd.print("Pts");

   lcd.setCursor(13 + 2-posPunt,1);
   
   for (int i=posPunt;i>=0;i--)
    lcd.print(mapeoNumeros(auxPrint[i]));
    break;
  }

}

void refrescarLCD(int indice, int ret,int adds,char tablero[4][10]){
  if(iHappened){
    impTablero(tablero,indice,adds);
    iHappened = 0;
    delay(ret);
   }
  }

void menu(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.write(byte(7));
  lcd.setCursor(3,0);
  lcd.print("Battleship  ");
  lcd.write(byte(0));
  lcd.setCursor(1,1);
  lcd.print("0 para empezar");
  while(keypad.getKey()!='0'); 
  soundAdd();
}

void mensajeFaseColocacion(){

  char flecha = 126;
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Fase de ");
  lcd.setCursor(0,1);
  lcd.print("Colocacion ");
  lcd.setCursor(14,1);
  lcd.print("#");
  lcd.print(flecha);
  while(keypad.getKey()!='#');
  soundAdd();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Posiciona tus");
  lcd.setCursor(0,1);
  lcd.print("barcos");
  lcd.setCursor(14,1);
  lcd.print('#');
  lcd.print(flecha);
  while(keypad.getKey()!='#');
  soundAdd();
}


void mensajeAyudaMisil(){

  char flecha = 126;
 
  SONIDO_AYUDA
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Poner misil #");
  lcd.setCursor(14,0);
  lcd.print('A');
  lcd.print(flecha);
  lcd.setCursor(0,1);
  lcd.print("Quitar misil *");
  while(keypad.getKey()!='A');
  SONIDO_AYUDA
  
}

void limpiarEstructuras(){

  for(int i = 0; i<=9; i++){
      letrasBarcos[i][1] = 0;
      barcos[i][1] = 0;
      numerosBarcos[i][1]=0;
      misilesA[i][0] = 4;
      misilesA[i][1] = 0;
      /*misilesB[i][0] = 4;
      misilesB[i][1] = 0;*/
    }

}

int siguienteBarco(){

  for(int i = 0;i<=9;i++)
    if (barcos[i][1] == 0) return barcos[i][0];

  return 0;
  
}

void marcarSiguienteBarco(){

  for(int i = 0;i<=9;i++)
    if (barcos[i][1] == 0) {
      barcos[i][1] = 1;
      break;
    }
  
}

void desmarcarSiguienteBarco(int tam){

  for(int i = 9;i>=0;i--)
    if ((barcos[i][1] == 1) && barcos[i][0] == tam) {
      barcos[i][1] = 0;
      break;
    }
  
}

char siguienteNumeroBarco(){

  for(int i = 0;i<=9;i++)
    if (numerosBarcos[i][1] == 0) return numerosBarcos[i][0];

  return 0;
  
}

void marcarNumeroBarco(char num){

  for(int i = 0;i<=9;i++)
    if (numerosBarcos[i][0] == num) {
      numerosBarcos[i][1] = 1;
      break;
    }
  
}

void desmarcarNumeroBarco(char num){

  for(int i = 0;i<=9;i++)
    if (numerosBarcos[i][0] == num) {
      numerosBarcos[i][1] = 0;
      break;
    }
  
}

char siguienteLetraBarco(){

  for(int i = 0;i<=9;i++)
    if (letrasBarcos[i][1] == 0) return letrasBarcos[i][0];

  return 0;
  
}

void marcarLetraBarco(char letra){

  for(int i = 0;i<=9;i++)
    if (letrasBarcos[i][0] == letra) {
      letrasBarcos[i][1] = 1;
      break;
    }
  
}

void desmarcarLetraBarco(char letra){

  for(int i = 0;i<=9;i++)
    if (letrasBarcos[i][0] == letra) {
      letrasBarcos[i][1] = 0;
      break;
    }
  
}

int setBarcosValidos(){

  int numBarco;

  if (barcosColocados<7) return 0;

  for(int i = 9;i>=0;i--){
    if(barcos[i][1] == 1){
      numBarco = i;
      break;
    }
  }

  for (int i = numBarco; i>=0;i--){
    if(barcos[i][1] == 0) return 0;
  }

  return 1;

}

void impresionBarcoHorizontal(int tam){

  switch(tam){
  case 1:
    lcd.write(byte(7));
    break;
  case 2:
    lcd.print("[>");
    break;
  case 3:
    lcd.print('[');
    lcd.write(byte(5));
    lcd.print('>');
  }

}

void impresionBarcoVertical(int tam,int col){
  
  lcd.write(byte(4));
  lcd.setCursor(col,1);

  switch(tam){
  case 2:
    lcd.print('V');
    break;
  case 3:
    lcd.write(byte(6));
  }

  lcd.setCursor(col,0);
}

void mensajeFaseAtaque(){
  
  char flecha = 126;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Fase de");
  lcd.setCursor(0,1);
  lcd.print("Ataque");
  lcd.setCursor(14,1);
  lcd.print("#");
  lcd.print(flecha);
  while(!keypad.getKey());
  soundAdd();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Lanza tus");
  lcd.setCursor(0,1);
  lcd.print("misiles");
  lcd.setCursor(14,1);
  lcd.print("#");
  lcd.print(flecha);
  while(keypad.getKey()!='#'); 
  soundAdd();
}

int mensajeMisilesTirados(){
  char flecha = 126;
  char key;
  
  soundAskFijar();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Tirar misiles #");
  lcd.write(byte(0));
  lcd.setCursor(0,1);
  lcd.print("Volver        *");
  lcd.print(flecha);
  do{
    key = keypad.getKey();
  }
  while(key!='#' && key!='*');
  if(key == '#') soundAdd();
  return  key=='#';
}

int addMisilA(int fil,int col){

  for(int i=0;i<=10;i++)
    if(misilesA[i][0]==4){
      //Misil agregado
      misilesA[i][0] = fil;
      misilesA[i][1] = col;
      tableroMisilesA[fil][col] = 'X'; 
      return 1;
    } else if((misilesA[i][0]==fil)&&(misilesA[i][1]==col))
      //Misil ya existe
      return 0;

  //Lleno
  return 0;
  
}

int deleteMisilA(int fil,int col){
  int found = 0;
  
  for(int i = 0;i<10;i++){
    if((misilesA[i][0] == fil)&&(misilesA[i][1]==col)){
      //Misil encontrado
      tableroMisilesA[fil][col]='T';
      found = i+1;
    }
  }

  //ordenar misiles
  if(found){
    for(int i=found-1  ;i<=cantidadA-2;i++){
      misilesA[i][0] = misilesA[i+1][0];
      misilesA[i][1] = misilesA[i+1][1];
    }
    misilesA[cantidadA][0] = 4;
    misilesA[cantidadA][1] = 0;
    return 1;
  }
  return 0;
}

int addMisilB(int fil,int col){

  for(int i=0;i<=10;i++)
    if(misilesB[i][0]==4){
      //Misil agregado
      misilesB[i][0] = fil;
      misilesB[i][1] = col;
      return 1;
    } else if((misilesB[i][0]==fil)&&(misilesB[i][1]==col))
      //Misil ya existe
      return 0;

  //Lleno
  return 0;
}

void mensajeAyudaBarco(){

  char flecha = 126;

  SONIDO_AYUDA
  lcd.noBlink();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Nuevo barco B");
  lcd.setCursor(14,0);
  lcd.print('A');
  lcd.print(flecha);
  lcd.setCursor(0,1);
  lcd.print("Quitar barco *");
  while(keypad.getKey()!='A');
  SONIDO_AYUDA
  delay(200);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Fijar barco #");
  lcd.setCursor(14,0);
  lcd.print('A');
  lcd.print(flecha);
  lcd.setCursor(0,1);
  lcd.print("Elim. barco *");
  while(keypad.getKey()!='A');
  SONIDO_AYUDA
}

int mensajeFijarBarcos(){
  char flecha = 126;
  char key;
  
  delay(200);
  soundAskFijar();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Fijar barcos  #");
  lcd.write(byte(7));
  lcd.setCursor(0,1);
  lcd.print("Volver        *");
  lcd.print(flecha);
  do{
    key = keypad.getKey();
  }
  while(key!='#' && key!='*');
  if(key == '#') soundAdd();
  return  key=='#';
}

void mensajeBatalla(){
  char flecha = 126;

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("   Batalla: ");
  lcd.print(mapeoNumeros(numBatalla));
  if(numBatalla != 3){
    lcd.setCursor(4,1);
    lcd.write(byte(0));
    lcd.print(" [");
    lcd.write(byte(5));
    lcd.print('>');
  } else {
    lcd.setCursor(0,1);
    lcd.print(" Ronda final ");
  }
  lcd.setCursor(13,1);
  lcd.print('#');
  lcd.print(flecha);

  while(keypad.getKey() != '#');
  soundAdd();
}

void mensajeBatallaGanada(){
  char flecha = 126;

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Ganaste esta");
  lcd.setCursor(0,1);
  lcd.print("ronda!");
  lcd.setCursor(13,1);
  lcd.print('#');
  lcd.print(flecha);

  while(keypad.getKey() != '#');

}

void mensajeBatallaPerdida(){
  char flecha = 126;

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Perdiste la");
  lcd.setCursor(0,1);
  lcd.print("ronda...");
  lcd.setCursor(13,1);
  lcd.print('#');
  lcd.print(flecha);

  while(keypad.getKey() != '#');

}

void mensajeBatallaEmpatada(){
  char flecha = 126;

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Empate! Bien");
  lcd.setCursor(0,1);
  lcd.print("jugado");
  lcd.setCursor(13,1);
  lcd.print('#');
  lcd.print(flecha);

  while(keypad.getKey() != '#');

}

void mensajeTableroB(){
  char flecha = 126;

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Resultado de");
  lcd.setCursor(0,1);
  lcd.print("tu ataque");
  lcd.setCursor(13,1);
  lcd.print('#');
  lcd.print(flecha);

  while(keypad.getKey() != '#');
  soundAdd();
}

void mensajeTableroA(){
  char flecha = 126;

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Resultado del");
  lcd.setCursor(0,1);
  lcd.print("ataque rival");
  lcd.setCursor(13,1);
  lcd.print('#');
  lcd.print(flecha);

  while(keypad.getKey() != '#');
  soundAdd();
}

/*int mensajeLanzarMisil(){

  char flecha = 126;
  char key;
  char fila;
  int columna;
  int filaLlena = 0;
  int columnaLlena = 0;
 
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Fila:");
  lcd.setCursor(11,0);
  lcd.print("#");
  lcd.write(byte(0));//caracter misil
  lcd.print(" *");
  lcd.print(flecha);//flecha derecha
  lcd.setCursor(0,1);
  lcd.print("Columna:");
  do{
  key = keypad.getKey();
  if((key>=65)&&(key<=68)){
    lcd.setCursor(5,0);
    fila = key;
    lcd.print(key);
    filaLlena = 1;
  }
  if((key>=48)&&(key<=57)){
    lcd.setCursor(8,1);
    columna = key;
    lcd.print(key);
    columnaLlena = 1;
  }
  if(key == '#'){
    if(!columnaLlena){
       lcd.setCursor(8,1);
       lcd.print('!');
       key = 'a';
    }
    if(!filaLlena){
       lcd.setCursor(5,0);
       lcd.print('!');
       key = 'a';
    }
  } 
  }
  while((key!='*')&&(key!='#'));
  int intFila;
  if(key == '#') {
    intFila = mapeoLetras(fila);
    int intColumna = mapeoLetras(columna);
    addMisilA(intFila,intColumna);
    misilesTirados++;
    intFila = constrain(intFila,0,2);
  }
  if(key == '*') intFila = indice;
  delay(200);
  return intFila;
}

int mensajeQuitarMisil(){

  int seleccionado = 0;
  char key;
  char flecha = 126;
  int fil;
  int col;
  lcd.noBlink();
  lcd.clear();
  lcd.setCursor(0,0);          
  lcd.print("Quitar misil: ");
  lcd.setCursor(0,1);
  lcd.print("Pos.:[ , ] *X #");
  lcd.print(flecha);
  do{
    key = keypad.getKey();
    if((key>=48)&&(key<=(47+misilesTirados))){
       lcd.setCursor(13,0);
       lcd.print(key);
       fil = misilesA[mapeoLetras(key)][0];
       col = misilesA[mapeoLetras(key)][1];
       lcd.setCursor(6,1);
       lcd.print(mapeoFilas(fil));
       lcd.setCursor(8,1);
       lcd.print(mapeoNumeros(col));
       seleccionado = 1;
    }
    if((key == '*')&&(seleccionado == 0)){
    lcd.setCursor(13,0);
    lcd.print('!');
    key = 'a';
  }
  }while((key!='*')&&(key!='#'));

  if(key == '*') {
    deleteMisilA(fil,col);
    misilesTirados--;
    fil = constrain(fil,0,2);
  }
  if(key == '#') fil = indice;
  delay(200);
  lcd.blink();
  return fil;
  
}*/


int refrescarModoAtaque(int misilesTirados,int fil,int col,int modoAc){
      
      int ret;

      if(tableroMisilesA[fil][col]!='T') {
        ret = 5;
       }
       else ret = 4;

      if(ret != modoAc) iHappened=1;

      return ret;
}

int refrescarModoColocacion(int barcosColocados){
  if(barcosColocados==0){
        return 4;
       }
       else if(misilesTirados < cantidadA) {
        return 5;
       }
       else if(misilesTirados == cantidadA) {
        return 6;
       }
}

char addBarcoA(int fil,int col,int tam,char orientacion){

  char carac;

  if(orientacion == 'H'){
      carac = siguienteNumeroBarco();
      for (int i=0;i<tam;i++)
        tableroBarcosA[fil][col + i] = carac;
      
      marcarNumeroBarco(carac);
  } else if(orientacion == 'V'){
      carac = siguienteLetraBarco();
      for (int i=0;i<tam;i++)
        tableroBarcosA[fil+i][col] = carac;
      marcarLetraBarco(carac);
  }

  
   return carac;
  
}

int misilA(int fil,int col,char barco,int cadena, char fase){ 

  //fase 'C' -> Colocacion
  //fase 'A' -> Ataque

  char caracElim;
  if(fase == 'C') caracElim = 'T';
  else caracElim = '/';

  int posiciones = 0;

  char pos = tableroBarcosA[fil][col];
  
  if(pos == 'T' && !cadena){
    //Misil falla
    tableroBarcosA[fil][col] = 'O';
    }
  else if((pos == 'O')||(pos == '/')){
    
  }
  else if((pos == barco)&&((pos>=48)&&(pos<=57))){
    //Misil hunde barco horizontal
    tableroBarcosA[fil][col] = caracElim;
    posiciones += misilA(fil,constrain(col+1,0,9),barco,cadena+1,fase);
    posiciones += misilA(fil,constrain(col-1,0,9),barco,cadena+1,fase);
    posiciones ++;
  }
  else if((pos == barco)&&((pos>=65)&&(pos<=74))){
    //Misil hunde barco vertical
    tableroBarcosA[fil][col] = caracElim;
    posiciones += misilA(constrain(fil+1,0,3),col,barco,cadena+1,fase);
    posiciones += misilA(constrain(fil-1,0,3),col,barco,cadena+1,fase);
    posiciones ++;
  }

  return posiciones;

}

int misilB(int fil,int col,char barco,int cadena){ 

  int posiciones = 0;

  char pos = tableroBarcosB[fil][col];

  //lcd.print(barco);
  //while(!keypad.getKey());
  
  if(pos == 'T' && !cadena){
    //Misil falla
    tableroBarcosB[fil][col] = 'O';
    }
  else if((pos == 'O')||(pos == '/')){
    
  }
  else if((pos == barco)&&((pos>=48)&&(pos<=57))){
    //Misil hunde barco horizontal
    tableroBarcosB[fil][col] = '/';
    posiciones += misilB(fil,constrain(col+1,0,9),barco,cadena+1);
    posiciones += misilB(fil,constrain(col-1,0,9),barco,cadena+1);
    posiciones ++;
  }
  else if((pos == barco)&&((pos>=65)&&(pos<=74))){
    //Misil hunde barco vertical
    tableroBarcosB[fil][col] = '/';
    posiciones += misilB(constrain(fil+1,0,3),col,barco,cadena+1);
    posiciones += misilB(constrain(fil-1,0,3),col,barco,cadena+1);
    posiciones ++;
  }

  return posiciones;

}

int inBoundsHorizontal(int indice,int fil){

  return fil==indice || fil==indice+1;
  
}

void scrollUp(){
  indice = constrain(indice-1,0,2);
  iHappened = 1;
}

void scrollDown(){
  indice = constrain(indice+1,0,2);
  iHappened = 1;
}

void setup() {
  // put your setup code here, to run once:
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, misil);
  lcd.createChar(1, fallo);
  lcd.createChar(2, objetivo);
  lcd.createChar(3, mar);
  lcd.createChar(4, popaVertical);
  lcd.createChar(5, cuerpoHorizontal);
  lcd.createChar(6, cuerpoVertical);
  lcd.createChar(7, barco);
  //impTablero(tableroMisilesA,0);
  //misilA(0,0,'1',0);
  //misilA(1,1,'1',0);
  //misilA(0,9,'1',0);
  pinMode(pinSubir, INPUT_PULLUP);
  pinMode(pinBajar, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pinSubir),scrollUp, FALLING);
  attachInterrupt(digitalPinToInterrupt(pinBajar),scrollDown, FALLING);
}

void loop() {
   //=========================SINCRONIZAR CON PC===================
   //==============================================================
   //=========================FASE DE COLOCACION===================
   int modo;
   
   menu();
   soundAdd();
   batallasA = 0;
   batallasB = 0;
   numBatalla = 1;
   while(batallasA < 2 && batallasB <2 && numBatalla <= 3){
   //Estado 0-> no colocando barcos
   //Estado 1-> colocando barcos
   limpiarTableroBarcosA();
   limpiarTableroMisilesA();
   limpiarEstructuras();
   mensajeBatalla();
   mensajeFaseColocacion();
   indice = 0;
   int auxIndice = 0;
   int filCursor=0;
   int colCursor=2;
   int filBarco;
   int colBarco;
   char orientacion;
   char key;
   int tamBarco;
   int valido;
   modo = 0;
   estado = 0;
   int barcosFijados = 0;
   barcosColocados = 0;
   impTablero(tableroBarcosA,indice,modo);
   while(!barcosFijados ){  

      //ESTABLECER MODO
      if((((tableroBarcosA[filCursor+indice][colCursor - 2]!='T')&&(estado == 0))||estado == 1)&&modo!=1&&modo!=9){
        if(barcosColocados<10)modo = 1;
        else modo = 9;
        iHappened = 1;
      } else if(modo != 0 && modo != 2 && modo!=3 && (tableroBarcosA[filCursor+indice][colCursor - 2]=='T') && estado == 0){
        if(barcosColocados>=7&&barcosColocados<=9 && setBarcosValidos()) modo = 2;
        else if(barcosColocados == 10) modo = 3;
        else modo = 0;
        iHappened = 1;
      }
      lcd.setCursor(colCursor,filCursor);
      if (!estado) refrescarLCD(indice,200,modo,tableroBarcosA);
      lcd.blink();
      
      key = keypad.getKey();
      if(key == '2' || key == '4' || key == '6' || key == '8') SONIDO_CURSOR;

      if(key == 'B' && estado == 0 && barcosColocados <10){
        //=====================================Nuevo Barco=========================================
        SONIDO_BARCO
        lcd.setCursor(13,1);
        lcd.print("#");
        lcd.setCursor(13,0);
        lcd.print("*X");
        auxIndice = indice;
        estado=1;
        orientacion = 'H';
        tamBarco = siguienteBarco();
        if((colCursor + tamBarco)>12) {
          colCursor = 12-tamBarco;
        }
        lcd.setCursor(colCursor,filCursor);
        impresionBarcoHorizontal(tamBarco);
      } else if(key == '*' && estado == 1){
        //=====================================No agregar Barco=========================================
        SONIDO_ELIM
        estado = 0;
        iHappened = 1;
        indice = auxIndice;
        modo = refrescarModoColocacion(barcosColocados);
      }
      else if(key == '#' && estado == 1 ){
        //=====================================Agregar Barco============================================
        valido = 1;
        filBarco = filCursor + auxIndice;
        colBarco = colCursor - 2;

        if(orientacion == 'H'){
          
          //Validar la insercion
          for (int i = 0;i<tamBarco;i++){
            if(tableroBarcosA[filBarco][colBarco + i]!='T') valido = 0;
          }

          //Insertar si es valido
          if (valido) {
            estado = 0;
            soundAdd();
            addBarcoA(filBarco,colBarco,tamBarco,'H');
            marcarSiguienteBarco();
            indice = auxIndice;
            lcd.setCursor(13,1);
            lcd.print("B");
            barcosColocados++;
            if(barcosColocados == 10) modo = 9;
            impTablero(tableroBarcosA,indice,modo);
            
          } else soundNoValBarco();
        } else{
            for (int i = 0;i<tamBarco;i++){
            if(tableroBarcosA[filBarco + i][colBarco]!='T') valido = 0;
          }

          if(valido){
            estado = 0;
            soundAdd();
            addBarcoA(filBarco,colBarco,tamBarco,'V');
            marcarSiguienteBarco();
            indice = auxIndice;
            lcd.setCursor(13,1);
            lcd.print("B");
            barcosColocados++;
             if(barcosColocados == 10) modo = 9;
            impTablero(tableroBarcosA,indice,modo);
            
          } else if(tamBarco == 3 && tableroBarcosA[filBarco+2][colBarco]!='T'){
            impTablero(tableroBarcosA,auxIndice+1,modo);
            lcd.setCursor(colCursor,filCursor);
            lcd.write(byte(5));
            lcd.setCursor(colCursor,1);
          }

          if(!valido) soundNoValBarco();

        }
      }
      else if(key == '*' && estado == 0 && (modo == 1||modo==9)){
        //=====================================Eliminar barco===========================================
        SONIDO_ELIM
        key = tableroBarcosA[filCursor+indice][colCursor-2];
        if(key>=48&&key<=57) desmarcarNumeroBarco(key);
        else desmarcarLetraBarco(key);
        tamBarco = misilA(filCursor+indice,colCursor-2,tableroBarcosA[filCursor+indice][colCursor-2],0,'C');
        desmarcarSiguienteBarco(tamBarco);
        barcosColocados--;
        if(barcosColocados>=7&&barcosColocados<=9&&setBarcosValidos()) modo = 2;
        else modo = 0;
        impTablero(tableroBarcosA,indice,modo);
      }
      else if(key == '#' && (modo == 9 || modo == 3 || modo == 2)){
        //=====================================Fijar Barcos=====================================
        barcosFijados = mensajeFijarBarcos();
        if(!barcosFijados) impTablero(tableroBarcosA,indice,modo);
      }
      else if(key == '5' && estado == 1 && tamBarco>1){
        //=====================================Girar barco=====================================
        //Revisa si al girar se sale del tablero
        if(orientacion == 'H'){
        orientacion = 'V';
        if(tamBarco == 3 && (auxIndice + filCursor > 1)) auxIndice = 1;
        else if(filCursor == 1) auxIndice=constrain(auxIndice+1,0,2);
        
        impTablero(tableroBarcosA,auxIndice,modo);
        filCursor = 0;
        lcd.setCursor(colCursor,filCursor);
        impresionBarcoVertical(tamBarco,colCursor);
        } else {
        orientacion = 'H';
        impTablero(tableroBarcosA,auxIndice,modo);
        if((colCursor + tamBarco)>12) {
          colCursor = 12-tamBarco;
        }
        lcd.setCursor(colCursor,filCursor);
        impresionBarcoHorizontal(tamBarco);
        }
      }
      else if(key == '2' && estado == 0){
        //=====================================Mover cursor arriba=====================================
        if(filCursor == 1) filCursor = 0;
        else scrollUp();
      }
      else if(key == '2' && estado == 1){
        //=====================================Mover barco arriba=====================================
        if(orientacion == 'H'){
          if(filCursor == 1){
            impTablero(tableroBarcosA,auxIndice,modo);
            filCursor = 0;
            
          } else {
            if(auxIndice) {
              auxIndice -= 1;
              impTablero(tableroBarcosA,auxIndice,modo);
            }
          }
          lcd.setCursor(colCursor,filCursor);
          impresionBarcoHorizontal(tamBarco);
          filBarco = auxIndice;
        } else {
          auxIndice = constrain(auxIndice-1,0,2);
          impTablero(tableroBarcosA,auxIndice,modo);
          lcd.setCursor(colCursor,filCursor);
          impresionBarcoVertical(tamBarco,colCursor);
        }
      }
      else if(key == '8' && estado == 0){
        //=====================================Mover cursor abajo=====================================
        if(filCursor == 0) filCursor = 1;
        else scrollDown();
      }
      else if(key == '8' && estado == 1){
        //=====================================Mover barco abajo=====================================
        if(orientacion == 'H'){
          if(filCursor == 0){
            impTablero(tableroBarcosA,auxIndice,modo);
            filCursor = 1;
          } else {
            if(auxIndice < 2) {
              auxIndice += 1;
              impTablero(tableroBarcosA,auxIndice,modo);
            }
          }
          lcd.setCursor(colCursor,filCursor);
          impresionBarcoHorizontal(tamBarco);
          filBarco = auxIndice+1;
        } else {
          if((auxIndice+tamBarco) < 4) {
            auxIndice ++;
            impTablero(tableroBarcosA,auxIndice,modo);
            lcd.setCursor(colCursor,filCursor);
            impresionBarcoVertical(tamBarco,colCursor);
          }
        }
      }
      else if(key == '4' && estado == 0){
        //=====================================Mover cursor izquierda=====================================
        colCursor = constrain(colCursor-1,2,11);
      }
      else if(key == '4' && estado == 1){
        //=====================================Mover barco izquierda=====================================
        impTablero(tableroBarcosA,auxIndice,modo);
        colCursor = constrain(colCursor-1,2,11);
        lcd.setCursor(colCursor,filCursor);
        if (orientacion == 'H') impresionBarcoHorizontal(tamBarco);
        else impresionBarcoVertical(tamBarco,colCursor);
        
      }
      else if(key == '6' && estado == 0){
        //=====================================Mover cursor derecha=====================================
        colCursor = constrain(colCursor+1,2,11);
      }
      else if(key == '6' && estado == 1){
        //=====================================Mover barco derecha=====================================
        impTablero(tableroBarcosA,auxIndice,modo);
        
        if (orientacion == 'H') {
          colCursor = constrain(colCursor+1,2,12-tamBarco);
          lcd.setCursor(colCursor,filCursor);
          impresionBarcoHorizontal(tamBarco);
        }
        else {
          colCursor = constrain(colCursor+1,2,11);
          lcd.setCursor(colCursor,filCursor);
          impresionBarcoVertical(tamBarco,colCursor);
        }
      }
      else if(key == 'A'){
        //=====================================Ayuda=====================================
        mensajeAyudaBarco();
        impTablero(tableroBarcosA,indice,modo);
      }
   }
   //==============================================================
   //=========================FASE DE ATAQUE=======================
   mensajeFaseAtaque();
   indice = 0;
   int fila;
   int columna;
   modo = 4;
   misilesTirados = 0;
   cantidadA = barcosColocados;
   impTablero(tableroMisilesA,indice,modo);
   filCursor = 0;
   colCursor = 2;
   while((misilesTirados <= cantidadA) ){
    modo = refrescarModoAtaque(misilesTirados,filCursor+indice,colCursor-2,modo);
    refrescarLCD(indice,200,modo,tableroMisilesA);
    lcd.setCursor(colCursor,filCursor);
    lcd.blink();
    key = keypad.getKey();
    if(key == '2' || key == '4' || key == '6' || key == '8') SONIDO_CURSOR

    if (key == '2'){
      //===================================MOVER CURSOR ARRIBA==============================================
      if(filCursor == 1) filCursor = 0;
      else scrollUp();
    }
    else if (key == '8'){
      //===================================MOVER CURSOR ABAJO==============================================
      if(filCursor == 0) filCursor = 1;
      else scrollDown();
    }
    else if (key == '4'){
      //===================================MOVER CURSOR IZQUIERDA==============================================
      colCursor = constrain(colCursor - 1,2,11);
    }
    else if (key == '6'){
      //===================================MOVER CURSOR DERECHA==============================================
      colCursor = constrain(colCursor + 1,2,11);
    } else if (key == '#' && modo == 4 && misilesTirados<cantidadA){
      //===================================AÑADIR MISIL==============================================
       soundAdd();
       addMisilA(filCursor + indice,colCursor-2);
       misilesTirados ++;
       modo = refrescarModoAtaque(misilesTirados,filCursor+indice,colCursor-2,modo);
       impTablero(tableroMisilesA,indice,modo);
    } else if (key == '#' && misilesTirados == cantidadA) {
      //===================================FIJAR MISILES==============================================
      if(mensajeMisilesTirados()){
        misilesTirados ++;
        lcd.noBlink();
      } else impTablero(tableroMisilesA,indice,modo);
    } else if(key == '*'&&modo == 5){
      //===================================ELIMINAR MISIL==============================================
      deleteMisilA(filCursor + indice,colCursor - 2);
      SONIDO_ELIM
      misilesTirados--;
      modo = 4;
      impTablero(tableroMisilesA,indice,modo);
    } else if(key == 'A'){
      //===================================AYUDA==============================================
      mensajeAyudaMisil();
      impTablero(tableroMisilesA,indice,modo);
    }
   }
   //==============================================================
   //=========================FASE DE LANZAMIENTO==================
   int aux;

    mensajeTableroB();
   //MISILES DE JUG A AL TABLERO B
   for(int i = 0;i<=cantidadA-1;i++){
    fila = misilesA[i][0];
    columna = misilesA[i][1];
    aux = misilB(fila,columna,tableroBarcosB[fila][columna],0);
    if(aux) puntajeA += 25 - (aux-1)*5;
   }

   modo = 7;
   indice = 0;
   
   impTablero(tableroBarcosB,indice,modo);
   while(keypad.getKey()!='#'){
   refrescarLCD(indice,200,modo,tableroBarcosB);
   }
   soundAdd();


  mensajeTableroA();
   //MISILES DE JUG B AL TABLERO A
   for(int i = 0;i<=cantidadA-1;i++){
    fila = misilesB[i][0];
    columna = misilesB[i][1];
    aux = misilA(fila,columna,tableroBarcosA[fila][columna],0,'A');
    if(aux) puntajeB += 25 - (aux-1)*5;
   }

   modo = 8;
   indice = 0;
   
   impTablero(tableroBarcosA,indice,modo);
   while(keypad.getKey()!='#'){
   refrescarLCD(indice,200,modo,tableroBarcosA);
   }

   soundAdd();

   if (puntajeA>puntajeB) {
    batallasA ++;
    mensajeBatallaGanada();
   }
   else if (puntajeA<puntajeB) {
    batallasB ++;
    mensajeBatallaPerdida();
   } else {
    mensajeBatallaEmpatada();
   }

   soundAdd();

   puntajeA = 0;
   puntajeB = 0;
   numBatalla ++;
   //==============================================================
  } 
 char flecha = 126;
  // RESOLUCION DE PARTIDA
  lcd.clear();
  if(batallasA>batallasB){
    //GANA JUGADOR A (ARDUINO)
    lcd.setCursor(0,0);
    lcd.print("Felicidades! Has");
    lcd.setCursor(0,1);
    lcd.print("ganado :D    ");
    //musiquita
    lcd.print('#');
    lcd.print(flecha);
    while(keypad.getKey()!='#');
  } else if(batallasA<batallasB){
    //GANA JUGADOR B (PC)
    lcd.setCursor(0,0);
    lcd.print("Perdiste! Suerte");
    lcd.setCursor(0,1);
    lcd.print("la proxima   ");
    //musiquita
    lcd.print('#');
    lcd.print(flecha);
    while(keypad.getKey()!='#');
  } else {
    //EMPATE
    lcd.setCursor(0,0);
    lcd.print("Empate! Gran ");
    lcd.setCursor(0,1);
    lcd.print("partida      ");
    //musiquita
    lcd.print('#');
    lcd.print(flecha);
    while(keypad.getKey()!='#');
  }
  soundAdd();
}