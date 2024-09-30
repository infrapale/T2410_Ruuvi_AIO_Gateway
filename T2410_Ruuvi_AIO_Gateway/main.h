#ifndef __MAIN_H__
#define __MAIN_H__

#define BLE_VERBOSE               false

typedef struct 
{
    uint16_t  state;
    uint32_t  next_ble_scan_ms;
    uint32_t  next_print_ms;
    uint8_t   sensor_indx;
} main_ctrl_st;

#endif