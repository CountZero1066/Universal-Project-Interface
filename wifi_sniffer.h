//Wi-Fi functions

#ifndef wifi_sniffer.h
#define wifi_sniffer.h

const wifi_promiscuous_filter_t filt = {
  .filter_mask = WIFI_PROMIS_FILTER_MASK_MGMT | WIFI_PROMIS_FILTER_MASK_DATA
};
String rssi_val;
int curChannel = 1, num_mac_add;
//-------------------------------------------------------------------------------------------
void sniffer(void* buf, wifi_promiscuous_pkt_type_t type) 
{ 
  wifi_promiscuous_pkt_t *p = (wifi_promiscuous_pkt_t*)buf;
  String packet;
  String mac;
  // ignore the first few characters and build up an easy to read packet
  for (int i = 8; i <= p->rx_ctrl.sig_len; i++) { 
    packet += String(p->payload[i], HEX);
  }
  for (int i = 4; i <= 15; i++) { // should leave us the 4 octets of the sender MAC address
    mac += packet[i];
  }
  rssi_val = p->rx_ctrl.rssi; //Get the frames RSSI
  mac.toUpperCase();
  char mac_array[64];
  strncpy(mac_array, mac.c_str(), sizeof(mac_array));
  String mac_address = mac_array;
  mac_address = mac_address + ":" + rssi_val;
  Add_text_to_scroll(mac_address, false);
  num_mac_add ++;
}
//-------------------------------------------------------------------------------------------
void enter_promiscuous_mode() 
{
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT(); 
  esp_wifi_init(&cfg); 
  esp_wifi_set_storage(WIFI_STORAGE_FLASH); 
  esp_wifi_set_mode(WIFI_MODE_NULL);
  esp_wifi_start();
  esp_wifi_set_promiscuous(true); 
  esp_wifi_set_promiscuous_filter(&filt); 
  esp_wifi_set_promiscuous_rx_cb(&sniffer);
  esp_wifi_set_channel(curChannel, WIFI_SECOND_CHAN_NONE); 
}
//-------------------------------------------------------------------------------------------
void List_nearby_APs(){
  display1.setTextSize(1);
      display1.setCursor(8,8);
      display2.setCursor(5, 16);
      display2.clearDisplay();
            delay(10);
            display1.clearDisplay();
            display1.display();
            delay(10);
            
            display2.println("searching...");
            display2.display();
            delay(10);
      WiFi.mode(WIFI_STA);
          int n = WiFi.scanNetworks();
          if (n == 0) {
            display2.println("No networks found.");
            display2.display();
          }
          else{
            display2.clearDisplay();
            int y[] = { 4, 12, 20, 28, 36, 44, 52};
            display2.println(String(n) + " networks found");
            display2.display();
            delay(10);
            Add_text_to_scroll("", true);
            for (int i = 0; i < n; ++i) {
              display1.setCursor(4, y[i]);
              Add_text_to_scroll("____________", false);
              Add_text_to_scroll(WiFi.SSID(i), false);
             Add_text_to_scroll(WiFi.BSSIDstr(i), false);
            }
            write_text_buffer_to_display();
            WiFi.disconnect();
            WiFi.mode(WIFI_OFF);
          }
}
//-------------------------------------------------------------------------------------------
void start_sniffing(){
  display2.clearDisplay();
      display2.println("Searching...");
      
      Add_text_to_scroll("Sniffer", true);
      enter_promiscuous_mode();
      int cycle = 0;
      display2.display();
       while (cycle < 1) 
       {

        if (curChannel > maxCh) 
        {
          curChannel = 1;
          cycle++;
        }
    esp_wifi_set_channel(curChannel, WIFI_SECOND_CHAN_NONE);
          delay(400);
          curChannel++;
          Serial.println("cycle =" + String(cycle));
      }
      display2.clearDisplay();
      esp_wifi_set_promiscuous(false);
      esp_wifi_stop();
      WiFi.mode(WIFI_OFF);
      write_text_buffer_to_display();
      display2.setCursor(18, 12);
      display2.println("Devices found:" + String(num_mac_add));
      display2.display();
}
void loop() {
 vTaskDelete(NULL);
}
#endif
