//#include "gps.h"
#include "dbw_rd.h"

//#include "led.h"



//#include <inttypes.h>
#include <string.h>
//#include <math.h>
//#include <stdlib.h>
#ifdef DEBUG_NMEA
// do debug-output if run on the DEBUG_NMEA-target

#endif

#define FLAG_FDJ_BIT 0
#define FLAG_WQ_BIT 1
#define FLAG_QLJ_BIT 2
#define FLAG_JY_BIT 5


struct DbwInfo dbw_info;
uint8_t dbw_info_flag = 0x39;
extern uint8_t INFO_ADDR[4];
extern char RCV_CMD[255];
extern uint8_t cmd_rcv_flag;
extern uint16_t cmd_ch_num;




void parse_dbw_info_GM002(void);


void dbw_info_impl_init( void ) {
  dbw_info.msg_available = FALSE;
//  dbw_info.pos_available = FALSE;
//  dbw_info.gps_nb_ovrn = 0;
  dbw_info.msg_len = 0;
	dbw_info.set_len = DBW_MAXLEN-1;
	dbw_info.set_temp_len = DBW_MAXLEN-1;
}



/**
 * parse GPGGA-nmea-messages stored in
 * dbw_info.msg_buf .
 */
void parse_dbw_info_GM002(void) {
  int i = 7;     // current position in the message, start after: GPGGA,
	int j = 0;
	uint16_t temp_len = 255;

	//addr_   uint8_t INFO_ADDR[4]={0x04,0xE6,0x10};//321040  addr 3B
	if(dbw_info.msg_buf[i] != INFO_ADDR[0])
	{
	return;
	}
	i++;
		if(dbw_info.msg_buf[i] != INFO_ADDR[1])
	{
	return;
	}
		i++;
		if(dbw_info.msg_buf[i] != INFO_ADDR[2])
	{
	return;
	}
	
	
	
	if(dbw_info.msg_buf[dbw_info.msg_len-2] != 0)
	{
		NMEA_PRINT("CRC WRONG \n\r");
    return;
	}
//	i+=6;
	i=16;
	temp_len = ((dbw_info.msg_buf[i] << 8) + dbw_info.msg_buf[i+1])/8;
	if(temp_len > dbw_info.msg_len-1-20 +1)
	{
		return;
	}
	//form 18 0xA4;
	for(j=0;j<(dbw_info.msg_len-2-19);j++)
	{
		RCV_CMD[j] = dbw_info.msg_buf[19+j];
	}
	RCV_CMD[j]='\0';
	cmd_ch_num=j;
	cmd_rcv_flag = 1;


	

//END parse GGA
}


/**
 * parse_nmea_char() has a complete line.
 * Find out what type of message it is and
 * hand it to the parser for that type.
 */
void dbw_info_parse_msg( void ) {

//  if(dbw_info.msg_len > 5 && !strncmp(dbw_info.msg_buf , "GMING", 5)) {
	if(dbw_info.msg_len > 5 && !strncmp(dbw_info.msg_buf , "$TXXX", 5)) {
    dbw_info.msg_buf[dbw_info.msg_len] = 0;

		parse_dbw_info_GM002();
  //  parse_nmea_GPRMC();
  }
  else {
    
        dbw_info.msg_buf[dbw_info.msg_len] = 0;
        NMEA_PRINT("ignoring: len=%i \n\r \"%s\" \n\r", dbw_info.msg_len, dbw_info.msg_buf);
      }

  // reset message-buffer
  dbw_info.msg_len = 0;
	dbw_info.set_len = DBW_MAXLEN-1;
	dbw_info.set_temp_len = DBW_MAXLEN-1;
}


/**
 * This is the actual parser.
 * It reads one character at a time
 * setting dbw_info.msg_available to TRUE
 * after a full line.
 */
void dbw_info_parse_char( uint8_t c ) {
	  //reject empty lines


	  // fill the buffer, unless it's full
	  if (dbw_info.msg_len < DBW_MAXLEN - 1) {

		// messages end with a linefeed
		//AD: TRUNK:       if (c == '\r' || c == '\n')
		if (dbw_info.msg_len < dbw_info.set_len-1) {
				dbw_info.msg_buf[dbw_info.msg_len] = c;
		  dbw_info.msg_len ++;
		} else {
				dbw_info.msg_buf[dbw_info.msg_len] = c;
		  dbw_info.msg_len ++;
				dbw_info.msg_available = TRUE;
		}
			if(dbw_info.msg_len==6)
			{
				dbw_info.set_temp_len = c;
			}
			if(dbw_info.msg_len==7)
			{
				dbw_info.set_temp_len = (dbw_info.set_temp_len << 8) + c;
				if((dbw_info.set_temp_len>=11)&&(dbw_info.set_temp_len<=230))
				{
					dbw_info.set_len = dbw_info.set_temp_len;
				}
			}
			if ( c == '$')
			{
		  dbw_info.msg_buf[0] = c;
		  dbw_info.msg_len =1;   
				dbw_info.set_len = DBW_MAXLEN-1;
				dbw_info.set_temp_len = DBW_MAXLEN-1;
			}

	  }

	  if (dbw_info.msg_len >= DBW_MAXLEN - 1)
		dbw_info.msg_available = TRUE;

  }
