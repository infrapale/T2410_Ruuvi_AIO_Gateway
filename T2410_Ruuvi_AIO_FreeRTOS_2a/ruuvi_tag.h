#ifndef __RUUVI_TAG_H__
#define __RUUVI_TAG_H__

#define  MAX_NBR_RUUVI_TAG  8
#define  RUUVI_LOCATION_LEN 20
#define  NOT_A_RUUVI        255

typedef struct
{
    String    mac_addr;
    char      location[RUUVI_LOCATION_LEN];
    float     temp_fp;
    uint16_t  temperature;
    float     humidity;
    uint16_t  pressure;
    int16_t   acc[3];
    uint16_t  voltage_power;
    float     voltage;
    uint16_t  power;
    int16_t   rssi;
    uint16_t  movement;
    uint16_t  measurement;
    bool      updated;
} ruuvi_tag_st;


class RuuviTag
{
    public:    
        RuuviTag(void);
        int8_t  add(const char *addr, const char *loc);
        String  *get_addr(uint8_t indx);
        uint8_t get_index(String mac_addr);
        int8_t  add_if_new(const char *mac_address, const char *loc);
        ruuvi_tag_st *get_data_ptr(String mac_addr);
        bool    is_a_defined_ruuvi(String mac_addr);
        float   get_temperature(uint8_t indx);
        float   get_humidity(uint8_t indx);
        float   get_rssi(uint8_t indx);
        char    *get_location(uint8_t indx);
        bool    get_updated(uint8_t indx);
        void    clr_updated(uint8_t indx);
        void    decode_raw_data(String mac_addr, String raw_data, int rssi);
        //bool is_updated(uint8_t indx);
        //void clear_update(uint8_t indx);
        ruuvi_tag_st  ruuvi[MAX_NBR_RUUVI_TAG];
        uint8_t   nbr_of = 0;
   private:
        int8_t    _active_indx = 0;
        String    undef_addr = "xxx";
};


//void ruuvi_tag_initialize(void);

#endif
