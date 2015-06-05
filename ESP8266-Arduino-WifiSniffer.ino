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
#define CHANNEL_HOP_INTERVAL 5000

os_event_t    user_procTaskQueue[user_procTaskQueueLen];
static os_timer_t channelHop_timer;

//static void loop(os_event_t *events);
static void promisc_cb(uint8 *buf, uint16 len);

#define printmac(buf, i) os_printf("\t%02X:%02X:%02X:%02X:%02X:%02X", buf[i+0], buf[i+1], buf[i+2], \
                                                   buf[i+3], buf[i+4], buf[i+5])

void channelHop(void *arg)
{
    // 1 - 13 channel hopping
    uint8 new_channel = wifi_get_channel() % 12 + 1;
    os_printf("** hop to %d **\n", new_channel);
    wifi_set_channel(new_channel);
}

static void ICACHE_FLASH_ATTR
promisc_cb(uint8 *buf, uint16 len)
{
    os_printf("-> %3d: %d", wifi_get_channel(), len);
    printmac(buf,  4);
    printmac(buf, 10);
    printmac(buf, 16);
    os_printf("\n");
}

//Init function 
void setup ()
{
    Serial.begin(115200);
    delayMicroseconds(100);

    Serial.println("*** Monitor mode test ***\r\n");
    
    Serial.println(" -> Promisc mode setup ... ");
    wifi_set_promiscuous_rx_cb(promisc_cb);
    wifi_promiscuous_enable(1);
    Serial.println("done.\n");

    Serial.println(" -> Timer setup ... ");
    os_timer_disarm(&channelHop_timer);
    os_timer_setfn(&channelHop_timer, (os_timer_func_t *) channelHop, NULL);
    os_timer_arm(&channelHop_timer, CHANNEL_HOP_INTERVAL, 1);
    Serial.println("done.\n");
    
    Serial.println(" -> Set opmode ... ");
    wifi_set_opmode( 0x1 );
    Serial.println("done.\n");

    //Start os task
    //system_os_task(loop, user_procTaskPrio,user_procTaskQueue, user_procTaskQueueLen);

    Serial.println(" -> Init finished!\n\n");
}

void loop() {
  delay(10);
}
