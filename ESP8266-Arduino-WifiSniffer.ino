#include "ESP8266WiFi.h"
extern "C" {
#include "c_types.h"
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "user_interface.h"
#include "smartconfig.h"
#include "lwip/opt.h"
#include "lwip/err.h"
#include "lwip/dns.h"
//#include "user_config.h"
}

#define user_procTaskPrio        0
#define user_procTaskQueueLen    1
#define CHANNEL_HOP_INTERVAL   5000

//os_event_t    user_procTaskQueue[user_procTaskQueueLen];
static os_timer_t channelHop_timer;

//static void loop(os_event_t *events);
static void promisc_cb(uint8 *buf, uint16 len);

#define printmac(buf, i) Serial.print(buf[i+0],HEX); Serial.print(":"); Serial.print(buf[i+1],HEX); Serial.print(":"); Serial.print(buf[i+2],HEX); Serial.print(":"); Serial.print(buf[i+3],HEX); Serial.print(":"); Serial.print(":"); Serial.print(buf[i+4],HEX); Serial.print(":"); Serial.print(buf[i+5],HEX);

void PrintHex83(uint8_t *data, uint8_t start, uint8_t length) // prints 8-bit data in hex
{
   char tmp[length*2+1];
   byte first;
   int j=0;
   for (uint8_t i=0; i<length; i++) 
   {
     first = (data[i+start] >> 4) | 48;
     if (first > 57) tmp[j] = first + (byte)39;
     else tmp[j] = first ;
     j++;
    
     first = (data[i+start] & 0x0F) | 48;
     if (first > 57) tmp[j] = first + (byte)39; 
     else tmp[j] = first;
     j++;
   }
   tmp[length*2] = 0;
   Serial.print(tmp);
}

void channelHop(void *arg)
{
  // 1 - 13 channel hopping
  uint8 new_channel = wifi_get_channel() % 12 + 1;
  Serial.print("** hop to ");
  Serial.println(new_channel);
  wifi_set_channel(new_channel);
}

static void ICACHE_FLASH_ATTR
promisc_cb(uint8 *buf, uint16 len)
{
    Serial.print("-> ");
    Serial.print(wifi_get_channel(), DEC);
    Serial.print(",");
    PrintHex83(buf, 10, 6);
    //printmac(buf, 10); //source mac
    Serial.print(" -> ");
    PrintHex83(buf, 4, 6);
    //printmac(buf,  4); //destination mac
    Serial.print(" (");
    PrintHex83(buf, 16, 6);
    //printmac(buf, 16);
    Serial.println(")");
}

//Init function 
void setup ()
{
    Serial.begin(115200);
    delayMicroseconds(100);

    Serial.println("*** Monitor mode test ***");
    
    Serial.print(" -> Promisc mode setup ... ");
    wifi_set_promiscuous_rx_cb(promisc_cb);
    wifi_promiscuous_enable(1);
    Serial.println("done.");

    Serial.print(" -> Timer setup ... ");
    os_timer_disarm(&channelHop_timer);
    os_timer_setfn(&channelHop_timer, (os_timer_func_t *) channelHop, NULL);
    os_timer_arm(&channelHop_timer, CHANNEL_HOP_INTERVAL, 1);
    Serial.println("done.\n");
    
    Serial.print(" -> Set opmode ... ");
    wifi_set_opmode( 0x1 );
    Serial.println("done.");

    //Start os task
    //system_os_task(loop, user_procTaskPrio,user_procTaskQueue, user_procTaskQueueLen);

    Serial.println(" -> Init finished!");
}

void loop() {
  delay(10);
}
