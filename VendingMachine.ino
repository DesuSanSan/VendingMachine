#include <LiquidCrystal.h>
#include <Keypad.h>
#include <string.h>

#define LED1 A5
#define LED2 A4
#define LED3 A3
#define LED4 A2

struct ProductList
{
  String code;
  double price;
};

ProductList productArray[] =
{
  {"A40", 4.99},
  {"B32", 3.99},
  {"B25", 2.99},
  {"D31", 5.99}
};



enum class VMachineEventType
{
  resetAfterInactive, 
};

enum class VMachineState
{
  insertMoney, typedCode, giveProduct, giveChange, reset
};

struct ProductData
{
  ProductList* product;
};

struct VMachineEvent
{
  VMachineEventType eventType;
  union
  {
    ProductData productdata;
  } eventData{};
};

const byte ROWS = 4;
const byte COLS = 4;

byte rowPins[ROWS] = {11, 10, 9, 8};
byte colPins[COLS] = {12, 13, A0, A1};

char keys[ROWS][COLS] =
{
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

Keypad keyPad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

struct Counting
{
  int countCodes = 4;
  int i = 0;
};

Counting counting;

struct Code
{
  String typedCode = "";
};

Code code;

struct Money
{
  double storedMoney = 0;
};

Money money;

struct Time
{
  unsigned long savedTime = 0;
  unsigned long timer = 0;
  unsigned long timerReset = 0;
};

Time time;

struct BoolStates
{
  bool insertingMoney = true;
  bool found = false;
  bool changeMode = false;
  bool reset = false;
  bool start = true;
};

BoolStates boolStates;

char button = keyPad.getKey(); 

void turnOffLED()
{
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  digitalWrite(LED4, LOW); 
}

void turnOnLED1()
{
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  digitalWrite(LED4, LOW);
}

void turnOnLED2()
{
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, HIGH);
  digitalWrite(LED3, LOW);
  digitalWrite(LED4, LOW);
}

void turnOnLED3()
{
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, HIGH);
  digitalWrite(LED4, LOW);
}

void turnOnLED4()
{
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  digitalWrite(LED4, HIGH);
}

class VMachine
{
  public:
    void handleEvent(VMachineEvent& event)
    {
      switch (vm_state)
      {
        case VMachineState::insertMoney:
          insertMoney_handleEvent(event);
        break;

        case VMachineState::typedCode:
          typedCode_eventHandle(event);
        break;

        case VMachineState::giveProduct:
          giveProduct_handleEvent(event);
        break;

        case VMachineState::giveChange:
          giveChange_handleEvent(event);
        break;

        case VMachineState::reset:
          reset_handleEvent(event);
        break;
      }
    }

  private:

    void insertMoney_handleEvent(VMachineEvent& event)
    {
      boolStates.changeMode = false;

      switch (button)
      {
        case '1':
          money.storedMoney += 0.01;
        break;

        case '2':
          money.storedMoney += 0.02;
        break;

        case '3':
          money.storedMoney += 0.05;
        break;

        case '4':
          money.storedMoney += 0.10;
        break;

        case '5':
          money.storedMoney += 0.20;
        break;

        case '6':
          money.storedMoney += 0.50;
        break;

        case '7':
          money.storedMoney += 1.00;
        break;

        case '8':
          money.storedMoney += 2.00;
        break;

        case '9':
          money.storedMoney += 5.00;
        break;

        case '#':
          boolStates.changeMode = true;
        break;

        case '*':
          returnMoney();
        break;
      }
      if(boolStates.changeMode == true)
      {
        vm_state = VMachineState::typedCode;
      }
      else
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Ilosc pieniedzy");
        lcd.setCursor(0, 1);
        lcd.print(money.storedMoney);
        boolStates.found = false;
      }

      if(boolStates.reset == true)
      {
        vm_state = VMachineState::reset;
      }
    }

    void VMachine::returnMoney()
    {
      while (money.storedMoney > 0)
      {
        lcd.clear();
        time.savedTime = millis();
        time.timer = millis();
        while(time.timer - time.savedTime < 1500)
        {
          lcd.setCursor(0, 0);
          lcd.print("Zwracanie:");
          lcd.setCursor(0, 1);
          lcd.print(money.storedMoney);
          time.timer = millis();
        }
        if(money.storedMoney >= 5)
        {
          money.storedMoney -= 5;
        }
        else if(money.storedMoney >= 2)
        {
          money.storedMoney -= 2;
        }
        else if(money.storedMoney >= 1)
        {
          money.storedMoney -= 1;
        }
        else if(money.storedMoney >= 0.5)
        {
          money.storedMoney -= 0.5;
        }
        else if(money.storedMoney >= 0.2)
        {
          money.storedMoney -= 0.2;
        }
        else if(money.storedMoney >= 0.1)
        {
          money.storedMoney -= 0.1;
        }
        else if(money.storedMoney >= 0.05)
        {
          money.storedMoney -= 0.05;
        }
        else if(money.storedMoney >= 0.02)
        {
          money.storedMoney -= 0.02;
        }
        else if(money.storedMoney >= 0.01)
        {
          money.storedMoney -= 0.01;
        }
        else if(money.storedMoney >= 0)
        {
          break;
        }
      }
    }

    void typedCode_eventHandle(VMachineEvent& event)
    {
      if(button != '#' && boolStates.changeMode == false)
      {
        code.typedCode+=button;
      }

      boolStates.changeMode = false;

      if(button == '#')
      {
        boolStates.changeMode = true;
      }

      if(code.typedCode.length() == 3)
      {
        selectingProduct();
      }

      if(boolStates.changeMode == true)
      { 
        vm_state = VMachineState::insertMoney;
      }
      else
      {
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print("Wpisany kod:");
        lcd.setCursor(6, 1);
        lcd.print(code.typedCode);
      }

      if(boolStates.reset == true)
      {
        vm_state = VMachineState::reset;
      }
    }

    void VMachine::selectingProduct()
    {
      code.typedCode.trim();
      for(counting.i = 0; counting.i < counting.countCodes; counting.i++)
      {
        if(code.typedCode == productArray[counting.i].code)
        {
          boolStates.found = true;
          break;
        }

        else
        {
          boolStates.found = false;
        }
      }

      if(boolStates.found == true)
      {
        if(money.storedMoney >= productArray[counting.i].price)
        {
          vm_state = VMachineState::giveProduct;
        }
        else if(money.storedMoney < productArray[counting.i].price)
        {
          lcd.clear();
          time.savedTime = millis();
          time.timer = millis();
          while(time.timer - time.savedTime < 2000)
          {
            lcd.setCursor(0, 0);
            lcd.print("Wrzucono za malo");
            lcd.setCursor(3, 1);
            lcd.print("pieniedzy");
            time.timer = millis();
          }
          code.typedCode = "";
          vm_state = VMachineState::insertMoney;
        }
      }

      else
      {
        Serial.println("B");
        code.typedCode = "";
        time.savedTime = millis();
        time.timer = millis();
        lcd.clear();
        while(time.timer - time.savedTime < 2000)
        {
          lcd.setCursor(0, 0);
          lcd.print("Wpisano zly kod");
          lcd.setCursor(0, 1);
          lcd.print("XXXXXXXXXXXXXXXX");
          time.timer = millis();
        }
      }
    }

    void VMachine::productList()
    {
      switch(counting.i)
      {
        case 0:
          lcd.setCursor(0, 0);
          lcd.print("Wybrano produkt");
          lcd.setCursor(0, 1);
          lcd.print("Pepsi");
          turnOnLED1();
        break;

        case 1:
          lcd.setCursor(0, 0);
          lcd.print("Wybrano produkt");
          lcd.setCursor(0, 1);
          lcd.print("Twix");
          turnOnLED2();
        break;

        case 2:
          lcd.setCursor(0, 0);
          lcd.print("Wybrano produkt");
          lcd.setCursor(0, 1);
          lcd.print("Wode");
          turnOnLED3();
        break;

        case 3:
          lcd.setCursor(0, 0);
          lcd.print("Wybrano produkt");
          lcd.setCursor(0, 1);
          lcd.print("Goralki");
          turnOnLED4();
        break;
      }
    }

    void giveProduct_handleEvent(VMachineEvent &event)
    {
      boolStates.reset = true;
      lcd.clear();
      time.savedTime = millis();
      time.timer = millis();
      while(time.timer - time.savedTime < 3000)
      {
        productList();
        time.timer = millis();
      }
      if(money.storedMoney > productArray[counting.i].price)
      {
        vm_state = VMachineState::giveChange;
      }
      else if(money.storedMoney <= productArray[counting.i].price)
      {
        vm_state = VMachineState::reset;
      }
        
    }

    void giveChange_handleEvent(VMachineEvent &event)
    {
      lcd.clear();
      time.savedTime = millis();
      time.timer = millis();
      while(time.timer - time.savedTime < 3000)
      {
        double change = money.storedMoney - productArray[counting.i].price;
        lcd.setCursor(0, 0);
        lcd.print("Wydawanie reszty");
        lcd.setCursor(0, 1);
        lcd.print(change);
        time.timer = millis();
      }
      vm_state = VMachineState::reset;
    }

    void reset_handleEvent(VMachineEvent &event)
    {
      Serial.println("reset");
      boolStates.found = false;
      money.storedMoney = 0;
      code.typedCode = "";
      turnOffLED();
      lcd.begin(16, 2);
      lcd.setCursor(2,0);
      lcd.print("Wrzuc monety");
      lcd.setCursor(0, 1);
      lcd.print("----------------");
      if(boolStates.reset == false)
      {
        boolStates.start = false;
        vm_state = VMachineState::insertMoney;
      }
    }

    VMachineState vm_state = VMachineState::reset;
};

VMachine globalVMachine;

void eventHandler()
{
  VMachineEvent event{};
}

void setup() {
  Serial.begin(9600);

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);

  lcd.begin(16, 2);
  lcd.setCursor(2,0);
  lcd.print("Wrzuc monety");
  lcd.setCursor(0, 1);
  lcd.print("----------------");
}

void loop() 
{
  // put your main code here, to run repeatedly:
  button = keyPad.getKey();
  if(button || boolStates.changeMode == true || boolStates.found == true || boolStates.start == true)
  {
    boolStates.reset = false;
    time.savedTime = millis();
    VMachineEvent event{};
    globalVMachine.handleEvent(event);
  }
  time.timerReset = millis();

  if((time.timerReset - time.savedTime > 20000) && money.storedMoney == 0)
  {
    boolStates.reset = true;
    time.savedTime = millis();
    VMachineEvent event{};
    globalVMachine.handleEvent(event);
  }
}