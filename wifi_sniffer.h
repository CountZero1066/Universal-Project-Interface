
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

#endif
