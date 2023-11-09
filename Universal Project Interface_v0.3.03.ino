/*
Universal Project Interface v0.3.03
Robert James Hastings
Github CountZero1066

   ###############################################################
   #              Universal Project Interface v0.3.03            #
   #             -----------------------------------             #
   #      A handheld, ESP32 based platform intended to serve     #
   #      as an interface for all personal projects via          #
   #      a tile based GUI and facilitated by ESPnow/Bluetooth   #
   ###############################################################
   
   Version Detail:
            -Added functional keyboard demo,Smart 12V power system UI and ESP-now functionality   

   Hardware:
            ESP32(NodeMCU)
            PCF8574
            2x SSD1306 OLED (32x128, 64x128)      
            3.7V 500mAh Lipo
            TP4056 battery charging module
            3.7V - 5V boost converter module
            5x 4 pin momentary push button switches
            2x 4.7k ohm resistors
            Dupont & JST connectors
            perf board/prototype board

   Logical Code Structure:
                    [Primary Keypad Read Logic]
                                 ↓ 
                          [Index Manager]
                                 ↓
                         [Interface Logic]
*/
//-------------------------------------------------------------------------------------------
//Preprocessor directives,Libaries,Headers,Global Variables and Definitions
//-------------------------------------------------------------------------------------------
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <esp_now.h>
#include <WiFi.h>
#include <Preferences.h>
#include "bitmap_images.h"

#define CHANNEL 1
#define OLED_RESET    -1
#define splash_height   128
#define splash_width    64
#define LOGO_HEIGHT   51
#define LOGO_WIDTH    52
#define I2C_SDA 23
#define I2C_SCL 22
//use non-default I2C SDA and SCL pins via Twowire instance
TwoWire I2Cbus1 = TwoWire(0);
Adafruit_SSD1306 display1(128, 64, &I2Cbus1, OLED_RESET);
Adafruit_SSD1306 display2(128, 32, &I2Cbus1, OLED_RESET);

Preferences preferences;

TaskHandle_t Task_0;
TaskHandle_t Task_1;

esp_now_peer_info_t slave;
esp_now_peer_info_t peerInfo;

uint8_t NewAddress[6];
short int i;
uint8_t Smart_12v_pwr_Address[] = {0x08,0x3A,0xF2,0x6D,0x25,0x78};
String message_in, text_input;
int menu_index = 0, prior_menu_index = 0, check_index_change = 0, current_key_selection = 0, prior_key_selection = 0, smart_12v_index = 0, prior_smart_12v_index = 0;
bool main_menu_active, item_select, keyboard_active, smart_12V_UI_active, capslock_active;
//-------------------------------------------------------------------------------------------
//Setup
//-------------------------------------------------------------------------------------------
void setup() 
{  
  Serial.begin(115200);
  I2Cbus1.begin(I2C_SDA, I2C_SCL,400000); //initialize the I2C communication with non-default pins
  display1.begin(SSD1306_SWITCHCAPVCC, 0x3D);
  delay(50);
  display2.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    disableCore0WDT();
    disableCore1WDT();            
  display1.clearDisplay();
  delay(50);
  keyboard_active = false;
  main_menu_active = false;
  item_select = false;
  smart_12V_UI_active = false;
  capslock_active = false;
  
  WiFi.mode(WIFI_STA);// Set device as a Wi-Fi Station
  WiFi.disconnect();

  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  esp_now_register_recv_cb(OnDataRecv);
  esp_now_register_send_cb(OnDataSent); 

  memcpy(peerInfo.peer_addr, Smart_12v_pwr_Address, 6);
  peerInfo.channel = 1;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  else if (esp_now_add_peer(&peerInfo) == ESP_OK){
    Serial.println("Peer added");
    return;
  }

  display2.clearDisplay();
  display2.display();
  display2.setTextColor(WHITE);
  display2.setTextSize(1);
  display2.setCursor(20, 16);
  display2.println("CountZero1066");
  display2.display();
  show_splash_bitmap();
  draw_github_details();
  progress_bar();
  Draw_Main_Menu_GUI();
  main_menu_active = true;
//set up keypad
  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.beginTransmission(0x20);
  Wire.write(0xFF);
  Wire.endTransmission();
          xTaskCreatePinnedToCore(
                            Task_Core_0,   /* Task function. */
                            "Task_0",     /* name of task. */
                            16000,       /* Stack size of task */
                            NULL,        /* parameter of the task */
                            6,           /* priority of the task */
                            &Task_0,      /* Task handle to keep track of created task */
                            0);          /* pin task to core 0 */                  
        
          //create a task that will be executed in the Task_Draw_Dispaly_code() function, with priority 1 and executed on core 1
          xTaskCreatePinnedToCore(
                            Task_Core_1,   /* Task function. */
                            "Task_1",     /* name of task. */
                            10000,       /* Stack size of task */
                            NULL,        /* parameter of the task */
                            7,           /* priority of the task */
                            &Task_1,      /* Task handle to keep track of created task */
                            1);          /* pin task to core 1 */
}

//-------------------------------------------------------------------------------------------
//Main Loop
//-------------------------------------------------------------------------------------------
//Core0
//-------------------------------------------------------------------------------------------
void Task_Core_0( void * pvParameters )
{
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());
  for(;;){ 

      delay(100);             
  }
  vTaskDelay(10);
}
//-------------------------------------------------------------------------------------------
//Core1 Loop
//-------------------------------------------------------------------------------------------
void Task_Core_1( void * Parameters )
{
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());
  
  for(;;){ 
    Primary_keypad_read_logic();
     }
     vTaskDelay( pdMS_TO_TICKS( 150 ) );
  }
/*_________________________________________________________________________________________________________
 *   _____      _                              _  __                          _   _                 _      
    |  __ \    (_)                            | |/ /                         | | | |               (_)     
    | |__) | __ _ _ __ ___   __ _ _ __ _   _  | ' / ___ _   _ _ __   __ _  __| | | |     ___   __ _ _  ___ 
    |  ___/ '__| | '_ ` _ \ / _` | '__| | | | |  < / _ \ | | | '_ \ / _` |/ _` | | |    / _ \ / _` | |/ __|
    | |   | |  | | | | | | | (_| | |  | |_| | | . \  __/ |_| | |_) | (_| | (_| | | |___| (_) | (_| | | (__ 
    |_|   |_|  |_|_| |_| |_|\__,_|_|   \__, | |_|\_\___|\__, | .__/ \__,_|\__,_| |______\___/ \__, |_|\___|
                                        __/ |            __/ | |                               __/ |       
                                       |___/            |___/|_|                              |___/        
 */
//-------------------------------------------------------------------------------------------
//Primary Keypad Logic
//responsible for communication with the PCF8574P on the I2C bus and basic low level decision logic
//delelegates futher decision logic to appropriate index managers based on the current state of boolean flags
//-------------------------------------------------------------------------------------------
void Primary_keypad_read_logic()
{
  Wire.requestFrom(0x20,1);
    while (Wire.available()) {
        uint8_t bits = Wire.read();
        char c = ((char)Wire.read()); 
             if(bits != 255){
                      switch(bits){
                              case 247:
                              //up key
                                     if(main_menu_active ==true){
                                          Main_menu_index_manager(1);
                                          context_display();      
                                     }
                                     else if (main_menu_active == false && smart_12V_UI_active == true){
                                        Smart_12V_index_manager(1);
                                      }
                                     else if (main_menu_active == false && keyboard_active == true){
                                        Keyboard_index_manager(1);
                                     }
                              break;

                              case 254:
                              //down key
                                     if(main_menu_active ==true){
                                          Main_menu_index_manager(3);
                                          context_display();      
                                     }
                                      else if (main_menu_active == false && smart_12V_UI_active == true){
                                          Smart_12V_index_manager(3);
                                      }
                                      else if (main_menu_active == false && keyboard_active == true){
                                          Keyboard_index_manager(3);
                                     }
                              break;

                              case 239:
                              //left key
                                     if(main_menu_active ==true){
                                          Main_menu_index_manager(4);
                                          context_display();
                                     }
                                      else if (main_menu_active == false && keyboard_active == true){
                                          Keyboard_index_manager(4);
                                     }
                              break;

                              case 251:
                              //right key
                                     //Main menu right button pressed
                                     if(main_menu_active ==true){
                                          Main_menu_index_manager(2);
                                          context_display();
                                     }
                                     //Smart 12V right button pressed
                                     else if (main_menu_active == false && smart_12V_UI_active == true){
                                          Smart_12V_index_manager(2);
                                      }
                                      //Keyboard right button pressed
                                      else if (main_menu_active == false && keyboard_active == true){
                                          Keyboard_index_manager(2);
                                     }
                             break;

                             case 253:
                             //center key
                                    if(main_menu_active ==true){
                                      Main_menu_index_manager(5);
                                    }
                                    else if (main_menu_active ==false && keyboard_active == true){
                                      Keyboard_index_manager(5);
                                    }
                                      else if (main_menu_active ==false && smart_12V_UI_active == true){
                                      Smart_12V_index_manager(5);
                                    } 
                             break;
                    } //end of switch  
                    } //end of If
                    delay(149);
            }  //end of while
}
/*  ___________________________________________________________________________________________

          _____           _             __  __                                       
         |_   _|         | |           |  \/  |                                      
           | |  _ __   __| | _____  __ | \  / | __ _ _ __   __ _  __ _  ___ _ __ ___ 
           | | | '_ \ / _` |/ _ \ \/ / | |\/| |/ _` | '_ \ / _` |/ _` |/ _ \ '__/ __|
          _| |_| | | | (_| |  __/>  <  | |  | | (_| | | | | (_| | (_| |  __/ |  \__ \
         |_____|_| |_|\__,_|\___/_/\_\ |_|  |_|\__,_|_| |_|\__,_|\__, |\___|_|  |___/
                                                            __/ |              
                                                           |___/               
 */
//-------------------------------------------------------------------------------------------
//Main Menu Index Manager
//decision logic to determine new index location
//-------------------------------------------------------------------------------------------
void Main_menu_index_manager(int button)
{
  switch(button){
     case 1: //up
        if(menu_index <= 3){
                    prior_menu_index = menu_index;
                    menu_index = menu_index + 4;
        }
        else if(menu_index >=4){
                    prior_menu_index = menu_index;
                    menu_index = menu_index -4;
        }
        break;
     case 2: //right
        if(menu_index >= 7){
                    prior_menu_index = menu_index;
                    menu_index = 0;
        }
        else {
                    prior_menu_index = menu_index;
                    menu_index++;
        }
        break;
     case 3: //down
        if(menu_index >= 4){
                   prior_menu_index = menu_index;
                   menu_index = menu_index  - 4;
        }
                   else if(menu_index <=3){
                   prior_menu_index = menu_index;
                   menu_index = menu_index  +4;
        }
        break;
     case 4: //left
        if(menu_index <= 0){
                  prior_menu_index = menu_index;
                  menu_index = 7;
        }
                  else {
                  prior_menu_index = menu_index;
                  menu_index--;
        }
        break;
     case 5:
            switch(menu_index){
                 case 0:
                     if(menu_index == 0){
                        main_menu_active = false;
                        keyboard_active = true;
                        item_select = true;
                        display1.clearDisplay();
                        display1.display();
                        display2.clearDisplay();
                        display2.display();
                        Draw_keyboard_UI();
                     }
                 case 1:
                     if(menu_index == 1){
                        main_menu_active = false;
                        smart_12V_UI_active = true;
                        display1.clearDisplay();
                        display1.display();
                        display2.clearDisplay();
                        display2.display();
                        Draw_smart_12V_power_system_UI();
                        context_display();
                      }
             }
        break;
  }
  main_menu_interface_logic();
}
//-------------------------------------------------------------------------------------------
//Keyboard Index Manager
//decision logic to determine new index location
//-------------------------------------------------------------------------------------------
void Keyboard_index_manager(int button)
{
  switch(button){
    case 1: //up
        if (current_key_selection <=9){
                 prior_key_selection = current_key_selection;
                 current_key_selection = current_key_selection + 30;
        }
        else if (current_key_selection >9){
                 prior_key_selection = current_key_selection;
                 current_key_selection = current_key_selection - 10;
        }
        else if (current_key_selection == 40){
                 prior_key_selection = current_key_selection;
                 current_key_selection = 31;
        }
        else if (current_key_selection == 41){
                 prior_key_selection = current_key_selection;
                 current_key_selection = 34;
        }
        else if (current_key_selection == 42 || current_key_selection == 43){
                 prior_key_selection = current_key_selection;
                 current_key_selection = 38;
        } 
        Keyboard_interface_logic();
        break;
    case 2: //right
        if (current_key_selection >=43){
                prior_key_selection = current_key_selection;
                current_key_selection = 0;
        }
        else{
                prior_key_selection = current_key_selection;
                current_key_selection++;
        }
        Keyboard_interface_logic();
        break;
    case 3: //down
        if (current_key_selection >= 0 && current_key_selection <29){
                prior_key_selection = current_key_selection;
                current_key_selection = current_key_selection + 10;
        }
        else if (current_key_selection == 30 || current_key_selection == 31 || current_key_selection == 32)  {
                prior_key_selection = current_key_selection;
                current_key_selection = 40;
        }
        else if (current_key_selection == 33 || current_key_selection == 34 || current_key_selection == 35 || current_key_selection == 36)  {
                prior_key_selection = current_key_selection;
                current_key_selection = 41;
        }
        else if (current_key_selection == 37 || current_key_selection == 38)  {
                prior_key_selection = current_key_selection;
                current_key_selection = 42;
        }
        else if (current_key_selection == 39){
                prior_key_selection = current_key_selection;
                current_key_selection = 43;
        } 
        Keyboard_interface_logic();
        break;
    case 4: //right
        if (current_key_selection <=0){
                prior_key_selection = current_key_selection;
                current_key_selection = 43;
        }
        else {
                prior_key_selection = current_key_selection;
                current_key_selection--;
        } 
        Keyboard_interface_logic();
        break;
    case 5: //middle
        if (current_key_selection == 43){
               Draw_Main_Menu_GUI();
               keyboard_active = false;
               main_menu_active = true;
               text_input="";
               context_display(); 
        }
        else if (current_key_selection == 41){
               text_input.concat(" ");
               context_display();
        }
        else if (current_key_selection == 0 || current_key_selection <=39){
           if(capslock_active == true){
            text_input.concat(keyboard_keys[current_key_selection + 40]);
           }
           else{
            text_input.concat(keyboard_keys[current_key_selection]);
           }   
               context_display();
        }
        else if(current_key_selection == 40){
          if(capslock_active == true){
             capslock_active = false;
          }
          else if (capslock_active == false){
             capslock_active = true;
          }
          Draw_keyboard_UI();
        }
        break;
  }  
}
//-------------------------------------------------------------------------------------------
//Smart 12V Idex Manager
//decision logic to determine new index location
//-------------------------------------------------------------------------------------------
void Smart_12V_index_manager(int button)
{
  switch(button){
    case 1: //up
          if (smart_12v_index == 0){
           prior_smart_12v_index = smart_12v_index;
           smart_12v_index = 4;
           Smart_12V_power_system_interface_logic(smart_12v_index);
          }
          else if (smart_12v_index >=1){
           prior_smart_12v_index = smart_12v_index;
           smart_12v_index = smart_12v_index - 1;
           Smart_12V_power_system_interface_logic(smart_12v_index);
         }
          break;
    case 2: //right
        if (smart_12v_index == 5){
          prior_smart_12v_index = smart_12v_index;
          smart_12v_index = 4;
          Smart_12V_power_system_interface_logic(smart_12v_index);
        }
         else if (smart_12v_index == 4) {
          prior_smart_12v_index = smart_12v_index;
          smart_12v_index = 5;
          Smart_12V_power_system_interface_logic(smart_12v_index);
         }
         else{
         }
         break;
    case 3://down
      if (smart_12v_index == 0 || smart_12v_index <4){
        prior_smart_12v_index = smart_12v_index;
        smart_12v_index = smart_12v_index + 1;
        Smart_12V_power_system_interface_logic(smart_12v_index);
      }
      else if (smart_12v_index >=4){
        prior_smart_12v_index = smart_12v_index;
        smart_12v_index = 0;
        Smart_12V_power_system_interface_logic(smart_12v_index);
      }
       break;
    case 5:
        if (smart_12v_index == 5){
         Draw_Main_Menu_GUI();
         smart_12V_UI_active = false;
         main_menu_active = true;  
        }
        else if (smart_12v_index == 4){
           
        }
       break;
  }
}
/*___________________________________________________________________________________________
 *   _____                        _____                 _     _          
    |  __ \                      / ____|               | |   (_)         
    | |  | |_ __ __ ___      __ | |  __ _ __ __ _ _ __ | |__  _  ___ ___ 
    | |  | | '__/ _` \ \ /\ / / | | |_ | '__/ _` | '_ \| '_ \| |/ __/ __|
    | |__| | | | (_| |\ V  V /  | |__| | | | (_| | |_) | | | | | (__\__ \
    |_____/|_|  \__,_| \_/\_/    \_____|_|  \__,_| .__/|_| |_|_|\___|___/
                                                 | |                     
                                                 |_|                     
 */
//-------------------------------------------------------------------------------------------
//Draw Main Menu Graphics
//-------------------------------------------------------------------------------------------
void Draw_Main_Menu_GUI()
{
  menu_index = 0;
  display1.clearDisplay();
  display1.display();
  display1.setTextSize(1); 
  //first row
   for(int i = 0; i <=3; i++){
     display1.drawRect(gui_x_cords[i], 4, 27, 27, WHITE);
   }
   display1.drawBitmap(5, 5, keyboard_bmp_inverted,25,25,WHITE);
   display1.drawBitmap(36, 5, Power_bmp,25,25,WHITE);
   display1.drawBitmap(67, 5, Ozone_bmp,25,25,WHITE);
   display1.drawBitmap(98, 5, JSON_bmp,25,25,WHITE);
//second row
for(int i = 0; i <=3; i++){
     display1.drawRect(gui_x_cords[i], 34, 27, 27, WHITE);
   }
   display1.drawBitmap(5, 34, wifi_bmp,25,25,WHITE);
   display1.drawBitmap(36, 34, esp_now_bmp,25,25,WHITE);
   display1.drawBitmap(67, 34, RFID_bmp,25,25,WHITE);
   display1.drawBitmap(98, 34, settings_bmp,25,25,WHITE);  
//perimeter   
   display1.drawRect(0, 0, 128, 64, WHITE);
   display1.display();
}
//-------------------------------------------------------------------------------------------
//Draw Keyboard Graphics
//-------------------------------------------------------------------------------------------
void Draw_keyboard_UI()
{
if (keyboard_active == true){
    display1.setTextSize(1);
    display1.setTextColor(WHITE);
    display1.clearDisplay();
    display1.display();
  for(int n = 0; n <=3; n++){
    for(int i = 0; i <=9; i++){
      display1.drawRect(keyboard_x_cords[i], keyboard_y_cords[n], 10, 10, WHITE);
      display1.setCursor(keyboard_x_cords[i] + 2, keyboard_y_cords[n] + 1);
      switch(n){
        case 0:
          display1.println(keyboard_keys[i]);
          break;
        case 1:
          if(capslock_active == true){
            display1.println(keyboard_keys[i + 50]);
          }
          else if(capslock_active == false){
            display1.println(keyboard_keys[i + 10]);
          }
          break;
        case 2:
          if(capslock_active == true){
            display1.println(keyboard_keys[i + 60]);
          }
          else if(capslock_active == false){
            display1.println(keyboard_keys[i + 20]);
          }
          break;
        case 3:
        if(capslock_active == true){
            display1.println(keyboard_keys[i + 70]);
          }
          else if(capslock_active == false){
            display1.println(keyboard_keys[i + 30]);
          }
          break;
      }//end switch  
   }//end for
  }//end for
  display1.display();
  Redraw_keyboard_special_keys();
  Keyboard_interface_logic();
}//end if
}
//-------------------------------------------------------------------------------------------
//Redraw Keyboard Special character Keys
//-------------------------------------------------------------------------------------------
void Redraw_keyboard_special_keys()
{
  unsigned short int special_key_array_1[4][3] = {{4 ,50,34},{40 ,50,46},{88 ,50,22},{112 ,50,10}};
  unsigned short int special_key_setcursor_array[4][2] = {{6,51},{42,51},{90,51},{114,51}};
  char *special_key_text[] = {"CAPS", "space", "OK","X"};
  display1.setTextColor(WHITE);
  for(int i = 0; i<= 3; i++){
    display1.fillRect(special_key_array_1[i][0] ,special_key_array_1[i][1],special_key_array_1[i][2],10, BLACK);
    display1.drawRect(special_key_array_1[i][0] ,special_key_array_1[i][1],special_key_array_1[i][2],10, WHITE);
    display1.setCursor(special_key_setcursor_array[i][0],special_key_setcursor_array[i][1]);
    display1.print(F(special_key_text[i]));
  }//end for  
}
//-------------------------------------------------------------------------------------------
//Redraw Keyboard Keys
//-------------------------------------------------------------------------------------------
void Redraw_deselected_keyboard_key()
{ 
    if (prior_key_selection >= 0 && prior_key_selection <=39) {
    display1.fillRect(keyboard_key_coordinate_map[prior_key_selection][1], keyboard_key_coordinate_map[prior_key_selection][2],10, 10, BLACK);
    display1.drawRect(keyboard_key_coordinate_map[prior_key_selection][1], keyboard_key_coordinate_map[prior_key_selection][2], 10, 10, WHITE);
    display1.setTextSize(1);
    display1.setTextColor(WHITE);
    display1.setCursor(keyboard_key_coordinate_map[prior_key_selection][1] + 2, keyboard_key_coordinate_map[prior_key_selection][2] + 1);
    display1.println(keyboard_keys[prior_key_selection]);
    display1.display();
    }//end if
}
//-------------------------------------------------------------------------------------------
//Draw Smart 12V power system Graphics
//-------------------------------------------------------------------------------------------
void Draw_smart_12V_power_system_UI()
{
  unsigned short int y[4] = {0,12,24,36};
  display1.setTextSize(1);     
       for(int i = 0; i <=3; i++){
          display1.drawRect(redraw_12V_array[0], y[i], redraw_12V_array[1], 12, WHITE);
          display1.drawRect(redraw_12V_array[0]+4, y[i]+4, redraw_12V_array[1]-8, 12-8, WHITE);
          display1.drawRect(redraw_12V_array[2], y[i], redraw_12V_array[3], 12, WHITE);
          display1.fillRect(redraw_12V_array[4], y[i], redraw_12V_array[5], 12, WHITE);           
          display1.setTextColor(WHITE);
          
          display1.setCursor(redraw_12V_array[2]+ 2,y[i] + 1);
          display1.println("Circuit " +  String(i + 1 ));
          display1.setCursor(redraw_12V_array[4]+ 8,y[i] + 2);
          display1.setTextColor(BLACK);
          display1.print(F("--"));
          display1.setTextColor(WHITE);        
       }
          display1.drawRect(0, 48, 112, 15, WHITE);
          display1.drawRect(112, 48, 15, 15, WHITE);
          display1.setCursor(16,51);
          display1.print(F("connect/update"));
          display1.setCursor(117,52);
          display1.print(F("X"));

          display1.fillRect(smart_12V_UI_coordinate_map[10][0],smart_12V_UI_coordinate_map[10][1],smart_12V_UI_coordinate_map[10][2],smart_12V_UI_coordinate_map[10][3],WHITE);
          smart_12v_index = 0;
          prior_smart_12v_index = 4;
          display1.display();
}
/*__________________________________________________________________________________________
 *    _____       _             __                 _                 _      
     |_   _|     | |           / _|               | |               (_)     
       | |  _ __ | |_ ___ _ __| |_ __ _  ___ ___  | |     ___   __ _ _  ___ 
       | | | '_ \| __/ _ \ '__|  _/ _` |/ __/ _ \ | |    / _ \ / _` | |/ __|
      _| |_| | | | ||  __/ |  | || (_| | (_|  __/ | |___| (_) | (_| | | (__ 
     |_____|_| |_|\__\___|_|  |_| \__,_|\___\___| |______\___/ \__, |_|\___|
                                                                __/ |       
                                                               |___/        
 */
//-------------------------------------------------------------------------------------------
//Main menu tile selection
//-------------------------------------------------------------------------------------------
void main_menu_interface_logic()
{
  if(main_menu_active == true){
    display1.fillRect(redraw_menu_tile_array[prior_menu_index][0],redraw_menu_tile_array[prior_menu_index][1],25,25,BLACK);
    display1.drawBitmap(redraw_menu_tile_array[prior_menu_index][0], redraw_menu_tile_array[prior_menu_index][1], main_menu_bmp_pointer[prior_menu_index],25,25,WHITE);
    display1.fillRect(redraw_menu_tile_array[menu_index][0],redraw_menu_tile_array[menu_index][1],25,25,BLACK);
    display1.drawBitmap(redraw_menu_tile_array[menu_index][0], redraw_menu_tile_array[menu_index][1], main_menu_bmp_pointer[menu_index + 8],25,25,WHITE);
    display1.display();
  }
}
//-------------------------------------------------------------------------------------------
//Keyboard interface Logic
//describes UI behaviour upon changes to respective index value
//-------------------------------------------------------------------------------------------
void Keyboard_interface_logic()
{
  keyboard_active = true;
  main_menu_active = false;
  if (current_key_selection == prior_key_selection){
    display1.fillRect(2, 2, 10, 10, WHITE);
    display1.setCursor(4, 3);
    display1.setTextColor(BLACK); 
    display1.print(F("1"));
    display1.display();  
  }
  else if (current_key_selection >= 0 && current_key_selection <=39) {
    Redraw_keyboard_special_keys();
    display1.fillRect(keyboard_key_coordinate_map[prior_key_selection][1], keyboard_key_coordinate_map[prior_key_selection][2],10, 10, BLACK);
    display1.drawRect(keyboard_key_coordinate_map[prior_key_selection][1], keyboard_key_coordinate_map[prior_key_selection][2], 10, 10, WHITE);
    display1.setTextSize(1);
    display1.setTextColor(WHITE);
    display1.setCursor(keyboard_key_coordinate_map[prior_key_selection][1] + 2, keyboard_key_coordinate_map[prior_key_selection][2] + 1);
    if(capslock_active == false){
      display1.println(keyboard_keys[prior_key_selection]);
    }
    else if(capslock_active == true){
      display1.println(keyboard_keys[prior_key_selection + 40]);
    }
    display1.display();
    display1.fillRect(keyboard_key_coordinate_map[current_key_selection][1], keyboard_key_coordinate_map[current_key_selection][2],10, 10, WHITE);
    display1.setTextSize(1);
    display1.setTextColor(BLACK);
    display1.setCursor(keyboard_key_coordinate_map[current_key_selection][1] + 2, keyboard_key_coordinate_map[current_key_selection][2] + 1);
    if(capslock_active == false){
        display1.println(keyboard_keys[current_key_selection]);
    }
    else if(capslock_active == true){
        display1.println(keyboard_keys[current_key_selection + 40]);
    }
        display1.display();
  }
  else if (current_key_selection == 40)
    {
    Redraw_deselected_keyboard_key();
    Redraw_keyboard_special_keys();
    display1.fillRect(4 ,50,34,10, WHITE);
    display1.setCursor(6,51);
    display1.setTextColor(BLACK);
    display1.setTextSize(1);
    display1.print(F("CAPS"));
    display1.display();
    }
    else if (current_key_selection == 41){
    Redraw_deselected_keyboard_key();
    Redraw_keyboard_special_keys();
    display1.fillRect(40 ,50,46,10, WHITE);
    display1.setTextColor(BLACK);
    display1.setTextSize(1);
    display1.setCursor(42,51);
    display1.print(F("space"));
    display1.display();
    }
    else if (current_key_selection == 42){
    Redraw_deselected_keyboard_key();
    Redraw_keyboard_special_keys();
    display1.fillRect(88 ,50,22,10, WHITE);
    display1.setTextColor(BLACK);
    display1.setTextSize(1);
    display1.setCursor(90,51);
    display1.print(F("OK"));
    display1.display();
  }  
  else if (current_key_selection == 43){
    Redraw_deselected_keyboard_key();
    Redraw_keyboard_special_keys();
    display1.fillRect(112 ,50,10,10, WHITE);
    display1.setTextColor(BLACK);
    display1.setTextSize(1);
    display1.setCursor(114,51);
    display1.print(F("X"));
    display1.display();
  }
}
//-------------------------------------------------------------------------------------------
//Smart 12V power system interface Logic
//describes UI behaviour upon changes to respective index value
//-------------------------------------------------------------------------------------------
void Smart_12V_power_system_interface_logic(int index)
{
  if (index <=3){
    display1.fillRect(smart_12V_UI_coordinate_map[index + 10][0],smart_12V_UI_coordinate_map[index + 10][1],smart_12V_UI_coordinate_map[index + 10][2],smart_12V_UI_coordinate_map[index + 10][3],WHITE);
  }
  else if (index == 4){
    display1.fillRect(smart_12V_UI_coordinate_map[8][0],smart_12V_UI_coordinate_map[8][1],smart_12V_UI_coordinate_map[8][2],smart_12V_UI_coordinate_map[8][3],WHITE);
    display1.drawRect(4,48,104,13, BLACK),
          display1.setTextColor(BLACK);
          display1.setCursor(16,51);
          display1.print(F("connect/update"));
          display1.setTextColor(WHITE);
  }
  else if (index == 5){
    display1.fillRect(smart_12V_UI_coordinate_map[9][0],smart_12V_UI_coordinate_map[9][1],smart_12V_UI_coordinate_map[9][2],smart_12V_UI_coordinate_map[9][3],WHITE);
    display1.setCursor(117,52);
    display1.setTextColor(BLACK);
    display1.print(F("X"));
  }
  if (prior_smart_12v_index <=3){
    display1.fillRect(smart_12V_UI_coordinate_map[prior_smart_12v_index + 10][0],smart_12V_UI_coordinate_map[prior_smart_12v_index + 10][1],smart_12V_UI_coordinate_map[prior_smart_12v_index + 10][2],smart_12V_UI_coordinate_map[prior_smart_12v_index + 10][3],BLACK);
    display1.drawRect(smart_12V_UI_coordinate_map[prior_smart_12v_index + 10][0],smart_12V_UI_coordinate_map[prior_smart_12v_index + 10][1],smart_12V_UI_coordinate_map[prior_smart_12v_index + 10][2],smart_12V_UI_coordinate_map[prior_smart_12v_index + 10][3],WHITE);  
  }
  else if (prior_smart_12v_index == 4){
    display1.fillRect(smart_12V_UI_coordinate_map[8][0],smart_12V_UI_coordinate_map[8][1],smart_12V_UI_coordinate_map[8][2],smart_12V_UI_coordinate_map[8][3],BLACK);
    display1.drawRect(smart_12V_UI_coordinate_map[8][0],smart_12V_UI_coordinate_map[8][1],smart_12V_UI_coordinate_map[8][2],smart_12V_UI_coordinate_map[8][3],WHITE);
          display1.setCursor(16,51);
          display1.setTextColor(WHITE);
          display1.print(F("connect/update"));
  }
  else if (prior_smart_12v_index == 5){
    display1.fillRect(smart_12V_UI_coordinate_map[9][0],smart_12V_UI_coordinate_map[9][1],smart_12V_UI_coordinate_map[9][2],smart_12V_UI_coordinate_map[9][3],BLACK);
    display1.drawRect(smart_12V_UI_coordinate_map[9][0],smart_12V_UI_coordinate_map[9][1],smart_12V_UI_coordinate_map[9][2],smart_12V_UI_coordinate_map[9][3],WHITE);
          display1.setCursor(117,52);
          display1.print(F("X"));      
  }
  display1.display();
  display1.setTextColor(WHITE);
}
/*_________________________________________________________________________________________
 *    _____            _            _     _____  _           _             
     / ____|          | |          | |   |  __ \(_)         | |            
    | |     ___  _ __ | |_ _____  _| |_  | |  | |_ ___ _ __ | | __ _ _   _ 
    | |    / _ \| '_ \| __/ _ \ \/ / __| | |  | | / __| '_ \| |/ _` | | | |
    | |___| (_) | | | | ||  __/>  <| |_  | |__| | \__ \ |_) | | (_| | |_| |
     \_____\___/|_| |_|\__\___/_/\_\\__| |_____/|_|___/ .__/|_|\__,_|\__, |
                                                      | |             __/ |
                                                      |_|            |___/ 
 */
//-------------------------------------------------------------------------------------------
//Secondary Display interface
//-------------------------------------------------------------------------------------------
void context_display()
{
  if(main_menu_active == true){
      display2.clearDisplay();
      display2.setTextColor(WHITE);
      display2.setTextSize(2);
      display2.setCursor(5, 16);
        switch(menu_index){
          case 0:
              display2.print(F("Keyboard"));
              display2.fillRect(98,5,25,25,BLACK);
              display2.drawBitmap(98,5, main_menu_bmp_pointer[menu_index],25,25,WHITE);
          break;
          case 1:
              display2.print(F("12V Pwr"));
              display2.fillRect(98,5,25,25,BLACK);
              display2.drawBitmap(98,5, main_menu_bmp_pointer[menu_index],25,25,WHITE);
          break;
          case 2:
              display2.print(F("Ozone"));
              display2.fillRect(98,5,25,25,BLACK);
              display2.drawBitmap(98,5, main_menu_bmp_pointer[menu_index],25,25,WHITE);
          break;
          case 3:
              display2.print(F("JSON"));
              display2.fillRect(98,5,25,25,BLACK);
              display2.drawBitmap(98,5, main_menu_bmp_pointer[menu_index],25,25,WHITE);
          break;
          case 4:
              display2.print(F("WiFi"));
              display2.fillRect(98,5,25,25,BLACK);
              display2.drawBitmap(98,5, main_menu_bmp_pointer[menu_index],25,25,WHITE);
          break;
          case 5:
              display2.print(F("Remote"));
              display2.fillRect(98,5,25,25,BLACK);
              display2.drawBitmap(98,5, main_menu_bmp_pointer[menu_index],25,25,WHITE);
          break;
          case 6:
              display2.print(F("RFID"));
              display2.fillRect(98,5,25,25,BLACK);
              display2.drawBitmap(98,5, main_menu_bmp_pointer[menu_index],25,25,WHITE);
          break;
          case 7:
              display2.print(F("Settings"));
              display2.fillRect(98,5,25,25,BLACK);
              display2.drawBitmap(98,5, main_menu_bmp_pointer[menu_index],25,25,WHITE);
          break;
        }    
  }
  else if (main_menu_active == false && smart_12V_UI_active == true){
      display2.clearDisplay();
      display2.setTextSize(1);
      display2.setCursor(5, 16);
      display2.print(F("Smart 12V Power Mgmt"));
  }
  else if (main_menu_active == false && keyboard_active == true){
      display2.clearDisplay();
      display2.setTextSize(1);
      display2.setCursor(5, 16);
      display2.println(text_input);
  }
  display2.display();
  display2.setTextSize(1);
}
void context_display(String text)
{
      display2.clearDisplay();
      display2.setTextColor(WHITE);
      display2.setCursor(5, 16);
      display2.println(text);
      display2.display();
}
/*___________________________________________________________________________________________
 *   ______  _____ _____    _   _               
    |  ____|/ ____|  __ \  | \ | |              
    | |__  | (___ | |__) | |  \| | _____      __
    |  __|  \___ \|  ___/  | . ` |/ _ \ \ /\ / /
    | |____ ____) | |      | |\  | (_) \ V  V / 
    |______|_____/|_|      |_| \_|\___/ \_/\_/  
 */
//-------------------------------------------------------------------------------------------
//ESP Now 'data-sent' Callback
//-------------------------------------------------------------------------------------------
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) 
{ 
  Serial.print("Last Packet Send Status: ");
  if ((status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail") == "Delivery Success" || (status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail") == "Success"){
    context_display("ESP Now message sent");
  }
  else{
    context_display("ESP Now send Error");
  }
}
//-------------------------------------------------------------------------------------------
//ESP Now 'data-received' Callback
//-------------------------------------------------------------------------------------------
void OnDataRecv(const uint8_t * mac,const uint8_t *incomingData, int len) 
{ 
     char* buff = (char*) incomingData;
     String buffStr = String(buff);
     buffStr.trim();
     Serial.print(buffStr);
     context_display("ESP Now data received");
}
//-------------------------------------------------------------------------------------------
//Add ESP Now broadcast slave
//-------------------------------------------------------------------------------------------
void initBroadcastSlave(String temp_mac) 
{    
char mac_received[32]; 
temp_mac.toCharArray(mac_received,32);
byte byteMac[6];
byte index = 0;
char *token;
char *ptr;
const char colon[2] = ":";
//Split string into tokens
token = strtok(mac_received, colon);

            while ( token != NULL ){
              //Convert string to unsigned long integer
              byte tokenbyte = strtoul(token, &ptr, 16);
              byteMac[index] = tokenbyte;
              index++;
              token = strtok(NULL, colon);
            }
  //zero slave memory for rewrite          
  memset(&slave, 0, sizeof(slave));
  //add new address to slave
  for (int n = 0; n < 6; ++n) {
       slave.peer_addr[n] = (uint8_t)byteMac[n];
  }
  
  slave.channel = 1;
  slave.encrypt = 0;
  //check if slave added successfully
  manageSlave();
}
//-------------------------------------------------------------------------------------------
//Check if slave device paired
//-------------------------------------------------------------------------------------------
bool manageSlave() 
{
    const esp_now_peer_info_t *peer = &slave;
    const uint8_t *peer_addr = slave.peer_addr;
    // check if the peer exists
    bool exists = esp_now_is_peer_exist(peer_addr);
    if (exists) {
      // Slave already paired.
      return true;
    }
    else {
      // Slave not paired, attempt pair
      esp_err_t addStatus = esp_now_add_peer(peer);
      if (addStatus == ESP_OK) {
        context_display("pair success");
        // Pair success
        return true;
      }
      else if (addStatus == ESP_ERR_ESPNOW_NOT_INIT) {
        context_display("ESPNOW Not Init");
        return false;
      }
      else if (addStatus == ESP_ERR_ESPNOW_ARG) {
        context_display("Invalid Argument");
        return false;
      }
      else if (addStatus == ESP_ERR_ESPNOW_FULL) {
        context_display("Peer list full");
        return false;
      }
      else if (addStatus == ESP_ERR_ESPNOW_NO_MEM) {
        context_display("Out of memory");
        return false;
      }
      else if (addStatus == ESP_ERR_ESPNOW_EXIST) {
       context_display("Peer Exists");
        return true;
      }
      else {
        context_display("Not sure what happened");
        return false;
      }
    }
  }
//-------------------------------------------------------------------------------------------
//Send ESP Now message
//-------------------------------------------------------------------------------------------
void Send_message(String instruction)
{
  Serial.println("data to be sent = " + instruction);
   const uint8_t *peer_addr = slave.peer_addr;
   uint8_t *buff = (uint8_t*) instruction.c_str();
   size_t sizeBuff = sizeof(buff) * instruction.length();
   esp_err_t result = esp_now_send(peer_addr, buff, sizeBuff);  
        if (result == ESP_OK) {
              context_display("Success");
        }        
        else if (result == ESP_ERR_ESPNOW_ARG) {
              context_display("err,Invalid Argument");
        }
        else if (result == ESP_ERR_ESPNOW_INTERNAL) {
              context_display("err,Internal Error");
        }
        else if (result == ESP_ERR_ESPNOW_NO_MEM) {
              context_display("err,No Memory");
        }
        else if (result == ESP_ERR_ESPNOW_NOT_FOUND) {
              context_display("err,Peer not found.");
        }
        else {
              context_display("err,Unknown Error");
        }
}
/*__________________________________________________________________________________________
 *    ____  _   _                 ______                _   _                 
     / __ \| | | |               |  ____|              | | (_)                
    | |  | | |_| |__   ___ _ __  | |__ _   _ _ __   ___| |_ _  ___  _ __  ___ 
    | |  | | __| '_ \ / _ \ '__| |  __| | | | '_ \ / __| __| |/ _ \| '_ \/ __|
    | |__| | |_| | | |  __/ |    | |  | |_| | | | | (__| |_| | (_) | | | \__ \
     \____/ \__|_| |_|\___|_|    |_|   \__,_|_| |_|\___|\__|_|\___/|_| |_|___/
 */
//-------------------------------------------------------------------------------------------
//Draw Bitmap image of Github Logo
//-------------------------------------------------------------------------------------------
void draw_github_details(void) 
{
  display1.clearDisplay();
  display1.display();
  display1.drawBitmap(38, 0, github_bmp_logo,52,52,WHITE);
  display1.setTextColor(WHITE);
  display1.setTextSize(1);
  display1.setCursor(26, 53);
  display1.print(F("CountZero1066"));
  display1.display();
  delay(1500); //main program tasks have yet to be invoked, thus a blocking delay has no negative impact 
}
//-------------------------------------------------------------------------------------------
//Animated Loading Screen
//-------------------------------------------------------------------------------------------
void progress_bar()
{  
  display2.clearDisplay();
  display2.drawRect(0, 4, 128, 20, WHITE);
  display2.display();
  delay(200);     
      int x = 2;
      for(int i = 0; i <= 25; i++){
        display2.fillRect(x, 6, 4, 16, WHITE); 
        x = x + 6;
        display2.display();
        delay(25);
      }
  delay(200);
  display2.clearDisplay();
  display2.setTextSize(2);
  display2.setCursor(18, 2);
  display2.println("Main Menu");
  display2.display();
  display2.setTextSize(1);
}
//-------------------------------------------------------------------------------------------
//Draw Bitmap Splashscreen
//-------------------------------------------------------------------------------------------
void show_splash_bitmap(void) 
{
  display1.clearDisplay();
  display1.display();
  display1.drawBitmap(0, 0, bitmap, splash_height, splash_width, WHITE);
  display1.display();
  delay(3000);
}
//-------------------------------------------------------------------------------------------
void loop() {
 vTaskDelete(NULL);
}
