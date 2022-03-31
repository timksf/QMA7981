#include "I2CUtils/I2C_Device.hpp"

#pragma once

#ifndef __QMA7981_HPP__
#define __QMA7981_HPP__

class QMA7981 : public I2CDevice {
public: 

    enum class acc_range : uint8_t{
        RANGE_2G    =   0b0001,
        RANGE_4G    =   0b0010,
        RANGE_8G    =   0b0100,
        RANGE_16G   =   0b1000,
        RANGE_32G   =   0b1111    
    };

    enum class mclk_freq : uint8_t{
        MCLK_500KHz     =   0b0000,
        MCLK_333KHz     =   0b0001,
        MCLK_200KHz     =   0b0010,
        MCLK_100KHz     =   0b0011,
        MCLK_50KHz      =   0b0100,
        MCLK_25KHz      =   0b0101,    
        MCLK_12_5KHz    =   0b0110,
        MCLK_5KHz       =   0b0111
    };

private:

    //register addresses 
    constexpr static uint8_t _id_reg = 0x00;

    //data registers
    constexpr static uint8_t _data_reg_xL = 0x01;
    constexpr static uint8_t _data_reg_xH = 0x02;
    constexpr static uint8_t _data_reg_yL = 0x03;
    constexpr static uint8_t _data_reg_yH = 0x04;
    constexpr static uint8_t _data_reg_zL = 0x05;
    constexpr static uint8_t _data_reg_zH = 0x06;

    //data registers for step counting - not used in implementation
    constexpr static uint8_t _step_cnt_regXL = 0x07;
    constexpr static uint8_t _step_cnt_regL = 0x08;
    constexpr static uint8_t _step_cnt_regH = 0x0E;

    //interrupt status registers - not used in implementation
    constexpr static uint8_t _int_stat_reg = 0x09;  //...interrupt status registers stretch to 0x0D
    
    //default config registers
    constexpr static uint8_t _full_scale_range = 0x0F;
    constexpr static uint8_t _bandwidth = 0x10;
    constexpr static uint8_t _pm = 0x11;

    //config registers for step counter - not used in implementation
    constexpr static uint8_t _step_conf0 = 0x12;
    constexpr static uint8_t _step_conf1 = 0x13;
    constexpr static uint8_t _step_conf2 = 0x14;
    constexpr static uint8_t _step_conf3 = 0x15;

    //interrupt config registers - not used in implementation
    constexpr static uint8_t _int_enable0 = 0x16;
    constexpr static uint8_t _int_enable1 = 0x17;
    constexpr static uint8_t _int_enable2 = 0x18;
    constexpr static uint8_t _int_map0 = 0x19;
    constexpr static uint8_t _int_map1 = 0x1A;
    constexpr static uint8_t _int_map2 = 0x1B;
    constexpr static uint8_t _int_map3 = 0x1C;

    constexpr static uint8_t _int_pin_conf = 0x20;
    constexpr static uint8_t _int_conf = 0x21;

    //offset calibration registers
    constexpr static uint8_t _os_cust_x = 0x27;
    constexpr static uint8_t _os_cust_y = 0x28;
    constexpr static uint8_t _os_cust_z = 0x29;

    //registers from 0x29 to 0x2f are interrupt configuration registers - not implemented

    constexpr static uint8_t _detector_conf = 0x30;
    constexpr static uint8_t _selftest_conf = 0x32;

    constexpr static uint8_t _soft_reset = 0x36;

    constexpr static uint8_t _def_base_address = 0x12;

    acc_range _range;

    /*
    Resolution of LSB in mg/LSB, interpret as float
    */
    constexpr static inline uint32_t _resolution_lsb_val[] = {
        0x3e79db23, //0.244mg/LSB for +-2g res
        0x3ef9db23, //0.488mg/LSB for +-4g res
        0x3f7a1cac, //0.977mg/LSB for +-8g res
        0x3ffa0000, //1.953mg/LSB for +-16g res
        0x407a0000, //3.906mg/LSB for +-32g res
    };

    /*
    Resolution of LSB in mg/LSB, interpret as float
    */
    constexpr static inline uint32_t _cal_offs_lsb_val[] = {
        0x407a0000, //3.906mg/LSB for +-2g res
        0x40fa0000, //7.8125mg/LSB for +-4g res
        0x417a0000, //15.625mg/LSB for +-8g res
        0x41fa0000, //31.25mg/LSB for +-16g res
        0x427a0000, //62.5mg/LSB for +-32g res
    };

    //const uint8_t _i2c_address;

    void write_register(uint8_t addr, uint8_t val){
        Wire.beginTransmission(_i2c_address);
        Wire.write(addr);
        Wire.write(val);
        Wire.endTransmission();
    }

    /*
    Writes a single bit in a register while mainting the state of other bits in that register
    */ 
    void write_bit(uint8_t addr, uint8_t bit, bool val){
        //old register contents
        uint8_t value = read8(addr); 
        //first zero position, then either keep it that way if val == 0
        //otherwise the or-operation will set the bit to 1
        uint8_t bitmask = 1 << bit;
        value = (value & ~bitmask) | (val << bit);
        write_register(addr, value);
    }

public:

    int16_t x_raw;
    int16_t y_raw;
    int16_t z_raw;

    float x_g;
    float y_g;
    float z_g;

    QMA7981(uint8_t i2c_address) : I2CDevice(i2c_address){};

    QMA7981() : I2CDevice(_def_base_address){};

    void begin(){
        Wire.begin();
    }

    bool is_present(){
        Wire.beginTransmission(_i2c_address);
        delay(10);
        uint8_t res = Wire.endTransmission(_i2c_address);
        return res == 0u;
    }
    
    uint8_t get_chip_id(){
        return read8(_id_reg);
    }

    /*
    Read both x data registers and return it as a raw uint16_t
     */
    uint16_t read_x_raw(){
        uint8_t lower = read8(_data_reg_xL);
        uint8_t upper = read8(_data_reg_xH);
        uint16_t res = upper;
        res <<= 8;
        res |= lower;
        return res;
    }

    /*
    Read both y data registers and return it as a raw uint16_t
     */
    uint16_t read_y_raw(){
        uint8_t lower = read8(_data_reg_yL);
        uint8_t upper = read8(_data_reg_yH);
        uint16_t res = upper;
        res <<= 8;
        res |= lower;
        return res;
    }

    /*
    Read both z data registers and return it as a raw uint16_t
     */
    uint16_t read_z_raw(){
        uint8_t lower = read8(_data_reg_zL);
        uint8_t upper = read8(_data_reg_zH);
        uint16_t res = upper;
        res <<= 8;
        res |= lower;
        return res;
    }

    uint8_t read_x_cal_offs_raw(){
        return read8(_os_cust_x);
    }

    uint8_t read_y_cal_offs_raw(){
        return read8(_os_cust_y);
    }

    uint8_t read_z_cal_offs_raw(){
        return read8(_os_cust_z);
    }

    void set_x_cal_offs_raw(uint8_t offs){
        write_register(_os_cust_x, offs);
    }

    void set_y_cal_offs_raw(uint8_t offs){
        write_register(_os_cust_y, offs);
    }

    void set_z_cal_offs_raw(uint8_t offs){
        write_register(_os_cust_z, offs);
    }

    /*
    Returns if there is new data to be read in the x acc data register
    */
    bool new_data_x(){
        uint8_t res = read8(_data_reg_xL);
        return (res & 0x01);
    }

    /*
    Returns if there is new data to be read in the x acc data register
    */
    bool new_data_y(){
        uint8_t res = read8(_data_reg_yL);
        return (res & 0x01);
    }

    /*
    Returns if there is new data to be read in the x acc data register
    */
    bool new_data_z(){
        uint8_t res = read8(_data_reg_zL);
        return (res & 0x01);
    }

    /*
    Puts device into active mode by writing one to register PM[7]
    */
    void activate(){
        write_bit(_pm, 7, 1);
    }

    uint8_t read_mode_reg(){
        return read8(_pm);
    }

    /*
    Sets the full scale of the accelerometer 
    */
    void set_range(acc_range range){
        _range = range;
        uint8_t _fsr = read8(_full_scale_range);
        //bitmask for range bits
        uint8_t bitmask = 0b00001111;
        _fsr = (_fsr & ~bitmask) | ((uint8_t) range);
        write_register(_full_scale_range, _fsr);
    }

    acc_range get_range(){
        _range;
    }

    /*
    Sets master clock frequency according to datasheet
    */
    void set_mclk_freq(mclk_freq f){
        uint8_t pm = read8(_pm);
        //generate bitmask to only manipulate mclk bits
        uint8_t bitmask = 0x0F;
        pm = (pm & ~bitmask) | ((uint8_t) f);
        write_register(_pm, pm);
    }

    /*
    Sets bandwidth according to datasheet
    */
    void set_bandwidth(uint8_t bw){
        uint8_t bw_default = 0xE0;
        //only lowest three bits are a valid input
        bw &= 0x07;
        bw |= bw_default;
        write_register(_bandwidth, bw);
    }

    uint8_t get_bandwidth(){
        return read8(_bandwidth);
    }

    void read(){
        //read values as fast as possible after each other
        uint16_t buf[3];
        buf[0] = read_x_raw();
        buf[1] = read_y_raw();
        buf[2] = read_z_raw();

        //shift values bc only 14bits are actual data bits
        //convert to int16_t to force arithmetic shift 
        x_raw = ((int16_t)buf[0]) >> 2;
        y_raw = ((int16_t)buf[1]) >> 2;
        z_raw = ((int16_t)buf[2]) >> 2;

        //calc "g" values
        uint8_t res_idx = 0; //default index for default res of +-2G
        switch(_range){
            case acc_range::RANGE_2G: break;
            case acc_range::RANGE_4G: 
                res_idx = 1;
            break;
            case acc_range::RANGE_8G: 
                res_idx = 2;
            break;
            case acc_range::RANGE_16G: 
                res_idx = 3;
            break;
            case acc_range::RANGE_32G: 
                res_idx = 4;
            break;
        }
        float res = *(float *) &_resolution_lsb_val[res_idx];
        x_g = (float) x_raw * res / 1000.0f;
        y_g = (float) y_raw * res / 1000.0f;
        z_g = (float) z_raw * res / 1000.0f;
    }

};

#endif