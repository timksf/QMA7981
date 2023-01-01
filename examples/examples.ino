#include <Wire.h>

#include "QMA7981.hpp"

QMA7981 acc(0x12);

void setup(){

    acc.begin();
    delay(10); //wait for acc to complete startup
    acc.set_range(QMA7981::acc_range::RANGE_16G);
    acc.set_mclk_freq(QMA7981::mclk_freq::MCLK_100KHz);
    acc.set_bandwidth(0b011);
    acc.set_z_cal_offs_raw(0b11111100);
    acc.activate();
    delay(10); //wait for acc to wake up

    Serial.begin(115200);
    Serial.println("Setup complete");

    test_acc();
    delay(1000);
}

uint8_t i = 0;

void loop(){

    acc.read();
    Serial.print("x:"); Serial.print(acc.x_g); Serial.print(" ");
    Serial.print("y:"); Serial.print(acc.y_g); Serial.print(" ");
    Serial.print("z:"); Serial.print(acc.z_g); Serial.print(" ");
    Serial.println();

    delay(100);
}

void printBits(uint8_t val){ 
    for(uint8_t mask = 0x80; mask; mask >>= 1){
        if(mask & val) Serial.print('1');
        else Serial.print('0'); 
    }
}

void printBits(uint16_t val){ 
    for(uint16_t mask = 0x8000; mask; mask >>= 1){
        if(mask & val) Serial.print('1');
        else Serial.print('0'); 
    }
}

void printBits(uint32_t val){
    for(uint32_t mask = 0x80000000; mask; mask >>= 1){
        if(mask & val) Serial.print('1');
        else Serial.print('0'); 
    }
}

void test_acc(){
    acc.read();

    Serial.print("Accelerometer present: "); Serial.println(acc.is_present() ? "yes" : "no");
    Serial.print("Acc ID: "); Serial.println(acc.get_chip_id(), HEX);
    Serial.print("raw X: "); printBits(acc.read_x_raw()); Serial.println();
    Serial.print("raw Y: "); printBits(acc.read_y_raw()); Serial.println();
    Serial.print("raw Z: "); printBits(acc.read_z_raw()); Serial.println();
    Serial.print("raw Z offset: "); printBits(acc.read_z_cal_offs_raw()); Serial.println();
    Serial.print("PM: "); printBits(acc.read_mode_reg()); Serial.println();
    Serial.print("BW: "); printBits(acc.get_bandwidth()); Serial.println();
    Serial.print("X raw: "); Serial.println(acc.x_raw);
    Serial.print("Y raw: "); Serial.println(acc.y_raw);
    Serial.print("Z raw: "); Serial.println(acc.z_raw);
    Serial.print("X: "); Serial.print(acc.x_g);   Serial.println("g"); 
    Serial.print("Y: "); Serial.print(acc.y_g);   Serial.println("g");
    Serial.print("Z: "); Serial.print(acc.z_g);   Serial.println("g");

}
