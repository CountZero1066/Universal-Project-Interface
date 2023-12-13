//GUI definitions used in conjunction with the Adafruit_GFX.h library

const unsigned short int keyboard_key_coordinate_map[40][3] = {
  {0,4,2},{1,16,2},{2,28,2},{3,40,2},{4,52,2},{5,64,2},{6,76,2},{7,88,2},{8,100,2},{9,112,2},
  {10,4,14},{11,16,14},{12,28,14},{13,40,14},{14,52,14},{15,64,14},{16,76,14},{17,88,14},{18,100,14},{19,112,14},
  {20,4,26},{21,16,26},{22,28,26},{23,40,26},{24,52,26},{25,64,26},{26,76,26},{27,88,26},{28,100,26},{29,112,26},
  {30,4,38},{31,16,38},{32,28,38},{33,40,38},{34,52,38},{35,64,38},{36,76,38},{37,88,38},{38,100,38},{39,112,38}
};

//OLED coordinates for redrawing main menu tiles
const unsigned short int redraw_menu_tile_array[8][4] = {
  {5,5,25,25}, 
  {36,5,25,25}, 
  {67,5,25,25}, 
  {98,5,25,25}, 
  {5,35,25,25}, 
  {36,35,25,25}, 
  {67,35,25,25}, 
  {98,35,25,25}
};

const unsigned short int smart_12V_UI_coordinate_map[14][4] ={
  {0,0,32,12},       //L1.S1
  {96,0,32,12},      //L1.S3
  {0,12,32,12},      //L2.S1
  {96,12,32,12},     //L2.S3
  {0,24,32,12},      //L3.S1
  {96,24,32,12},     //L3.S3
  {0,36,32,12},      //L4.S1
  {96,36,32,12},     //L4.S3
  {0,48,112,15},     //L5.S1
  {112,48,15,15},    //L5.S2
  {4,4,24,4},        //L1.S1.I
  {4,16,24,4},       //L2.S1.I
  {4,28,24,4},       //L3.S1.I
  {4,40,24,4}        //L4.S1.I
};  

const unsigned short int gui_x_cords[] = {
  4, 35, 66, 97
};

const unsigned short int scan_Wifi_cords[] = {
  2,20,38, 8, 26, 44
};
const String Wifi_menu_txt[]{
  "SCNR","SNFR","back"
};
const unsigned short int keyboard_x_cords[] = {
  4, 16, 28, 40, 52, 64, 76, 88, 100, 112
};

const unsigned short int keyboard_y_cords[] = {
  2, 14, 26, 38
};

const char keyboard_keys[] = "0123456789abcdefghijklmnopqrstuvwxyz?!_<0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ.,$<";

unsigned short int redraw_12V_array[6] = {
  0,32,32,64,96,32
}; 
