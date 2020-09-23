/*********************************************************************
 *                
 * Filename:      crc.h
 * Version:       
 * Description:   CRC routines
 * Status:        Experimental.
 * Author:        Dag Brattli <dagb@cs.uit.no>
 * Created at:    Mon Aug  4 20:40:53 1997
 * Modified at:   Sun May  2 20:25:23 1999
 * Modified by:   Dag Brattli <dagb@cs.uit.no>
 * 
 ********************************************************************/

#ifndef _CRC_H_
#define _CRC_H_

class CCrc16{
public:
	static const unsigned short crc16_table[];

	/* Recompute the FCS with one more character appended. */
	static unsigned short clac_fcs(unsigned short fcs, char c){
		return (((fcs) >> 8) ^ crc16_table[((fcs) ^ (c)) & 0xff]);
	}

	/* Recompute the FCS with len bytes appended. */
	static unsigned short calc(unsigned char  *buf, int len);
};
class CCrc8{
public:
	static const unsigned char crc8_table[];
	static unsigned char clac_fcs(unsigned char fcs, unsigned char c){
		return crc8_table[(fcs) ^ (c)];
	}
	static unsigned char calc(unsigned char *buf, int len);
};
#endif
