#include <Arduino.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <time.h>
#include <lvgl.h>
#include "ui/ui/ui.h"

// --- USER CONFIGURATION ---
const char* ssid     = " YOUR WIFI";      // ENTER YOUR WIFI NAME HERE
const char* password = "PASSWORD";  // ENTER YOUR WIFI PASSWORD HERE
String serverUrl     = "http://192.000.0.0:5000/api/status"; // CHANGE TO YOUR PC IP ADDRESS

#define PIN_BACKLIGHT 27 
XPT2046_Touchscreen touch(33, 36);
TFT_eSPI tft = TFT_eSPI();

int totalNotifications = 0;
int lastML = -1;  // IN BRAZIL THE SHOP'S WHICH I SELL IS MERCADO LIVRE AND SHOPEE.
int lastShopee = -1;
unsigned long lastCheck = 0;
unsigned long lastClock = 0;

lv_obj_t* notifPanels[8];
lv_obj_t* notifTexts[8];

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[320 * 10];

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors((uint16_t *)&color_p->full, w * h, true);
    tft.endWrite();
    lv_disp_flush_ready(disp);
}

void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
    if (touch.touched()) {
        TS_Point p = touch.getPoint();
        data->point.x = map(p.y, 3854, 320, 0, 320);
        data->point.y = map(p.x, 3937, 326, 0, 240);
        data->state = LV_INDEV_STATE_PR;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}

void addNotification(String message) {
    totalNotifications++;
    if (totalNotifications > 99) totalNotifications = 99;
    lv_label_set_text(ui_NotificationNumber, String(totalNotifications).c_str());

    for (int i = 7; i > 0; i--) {
        const char* oldText = lv_label_get_text(notifTexts[i-1]);
        lv_label_set_text(notifTexts[i], oldText);
        if (!lv_obj_has_flag(notifPanels[i-1], LV_OBJ_FLAG_HIDDEN)) {
            lv_obj_clear_flag(notifPanels[i], LV_OBJ_FLAG_HIDDEN);
        }
    }

    lv_label_set_text(notifTexts[0], message.c_str());
    lv_obj_clear_flag(notifPanels[0], LV_OBJ_FLAG_HIDDEN);
}

void clearNotifications(lv_event_t * e) {
    totalNotifications = 0;
    lv_label_set_text(ui_NotificationNumber, "0");
    for (int i = 0; i < 8; i++) {
        lv_obj_add_flag(notifPanels[i], LV_OBJ_FLAG_HIDDEN);
    }
}

void fetchData() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(serverUrl);
        http.setConnectTimeout(1500);
        int code = http.GET();

        if (code > 0) {
            String payload = http.getString();
            StaticJsonDocument<1024> doc;
            deserializeJson(doc, payload);

            // Inventory
            lv_label_set_text(ui_LabelStockCP1, doc["stock_cp1"].as<const char*>());
            lv_label_set_text(ui_LabelStockCP2, doc["stock_cp2"].as<const char*>());
            
            // Orders
            lv_label_set_text(ui_LabelToShip, doc["to_ship"].as<const char*>());
            lv_label_set_text(ui_LabelToPrint, doc["to_print"].as<const char*>());
            lv_label_set_text(ui_LabelToRetrieve, doc["to_retrieve"].as<const char*>());

            // Financials
            lv_label_set_text(ui_LabelProfitToday, doc["profit_today"].as<const char*>());
            lv_label_set_text(ui_LabelProfit7d, doc["profit_7d"].as<const char*>());
            lv_label_set_text(ui_LabelProfit30d, doc["profit_30d"].as<const char*>());
            lv_label_set_text(ui_LabelProfitMonth, doc["profit_month"].as<const char*>());

            // Sales Logic
            int currentML = doc["total_ml"].as<int>();
            int currentShopee = doc["total_shopee"].as<int>();

            lv_label_set_text(ui_LabelSalesML, String(currentML).c_str());
            lv_label_set_text(ui_LabelSalesShopee, String(currentShopee).c_str());

            if (lastML != -1) {
                int diffML = currentML - lastML;
                int diffShopee = currentShopee - lastShopee;

                if (diffML > 0) {
                    addNotification("New Sale: ML (+ " + String(diffML) + ")");
                }
                if (diffShopee > 0) {
                    addNotification("New Sale: Shopee (+ " + String(diffShopee) + ")");
                }
            }

            lastML = currentML;
            lastShopee = currentShopee;
        }
        http.end();
    }
}

void updateClock() {
    struct tm timeinfo;
    if(getLocalTime(&timeinfo)){
        char tBuffer[10];
        strftime(tBuffer, 10, "%H:%M", &timeinfo);
        lv_label_set_text(ui_LabelClock, tBuffer);
    }
}

void updateWifiSlider() {
    if (WiFi.status() == WL_CONNECTED) {
        int q = map(WiFi.RSSI(), -100, -50, 0, 100);
        lv_slider_set_value(ui_SliderWifi, constrain(q, 0, 100), LV_ANIM_ON);
    } else {
        int v = (millis()/15) % 200; 
        if(v>100) v=200-v;
        lv_slider_set_value(ui_SliderWifi, v, LV_ANIM_OFF);
    }
}

void setup() {
    Serial.begin(115200);
    SPI.begin(25, 39, 32, 33);
    touch.begin(); touch.setRotation(1);
    pinMode(PIN_BACKLIGHT, OUTPUT); digitalWrite(PIN_BACKLIGHT, HIGH);
    tft.begin(); tft.setRotation(3);

    lv_init();
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, 320 * 10);
    static lv_disp_drv_t disp_drv; lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = 320; disp_drv.ver_res = 240;
    disp_drv.flush_cb = my_disp_flush; disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);
    static lv_indev_drv_t indev_drv; lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER; indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);
    
    ui_init();

    // Mapping UI Objects
    notifPanels[0] = ui_PanelNotif0;  notifTexts[0] = ui_LabelNotif0;
    notifPanels[1] = ui_PanelNotif1;  notifTexts[1] = ui_LabelNotif1;
    notifPanels[2] = ui_PanelNotif2;  notifTexts[2] = ui_LabelNotif2;
    notifPanels[3] = ui_PanelNotif3;  notifTexts[3] = ui_LabelNotif3;
    notifPanels[4] = ui_PanelNotif4;  notifTexts[4] = ui_LabelNotif4;
    notifPanels[5] = ui_PanelNotif5;  notifTexts[5] = ui_LabelNotif5;
    notifPanels[6] = ui_PanelNotif6;  notifTexts[6] = ui_LabelNotif6;
    notifPanels[7] = ui_PanelNotif7;  notifTexts[7] = ui_LabelNotif7;

    lv_obj_add_event_cb(ui_BtnClearNotifications, clearNotifications, LV_EVENT_CLICKED, NULL);

    WiFi.begin(ssid, password);
    configTime(-3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

    // Boot Test
    for(int i=1; i<=8; i++) {
        addNotification("Boot Test " + String(i));
        delay(50);
    }
}

void loop() {
    lv_timer_handler();
    delay(5);

    if (lv_scr_act() == ui_ScreenSleepMode) digitalWrite(PIN_BACKLIGHT, LOW);
    else digitalWrite(PIN_BACKLIGHT, HIGH);

    updateWifiSlider();

    if (millis() - lastClock > 1000) {
        updateClock();
        lastClock = millis();
    }

    if (millis() - lastCheck > 5000) {
        fetchData();
        lastCheck = millis();
    }
}