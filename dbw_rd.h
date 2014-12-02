/** @file dbw_rd.h
 *
*/


#ifndef DBW_RD_H
#define DBW_RD_H

//#include "uart_arch.h"


#ifdef DEBUG_NMEA
#define NMEA_PRINT(...) {  UsbSPrintString( __VA_ARGS__);};
#else
#define NMEA_PRINT() {};
#endif

#define DBW_MAXLEN 511

struct DbwInfo {
  bool msg_available;
//  bool_t pos_available;
//  uint8_t gps_nb_ovrn;        // number if incomplete nmea-messages
  char msg_buf[DBW_MAXLEN];  ///< buffer for storing one nmea-line
  int msg_len;
	int set_len;
	int set_temp_len;
};


extern struct DbwInfo dbw_info;
//extern uint8_t dbw_info_flag;

#define DBW_LINK UART2




	
/** The function to be called when a characted friom the device is available */
extern void dbw_info_parse_char(char c);

extern void dbw_info_parse_msg(void);
extern void dbw_info_impl_init( void );



#endif /* GPS_NMEA_H */
