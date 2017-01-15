#ifndef _port_h_
#define _port_h_

typedef unsigned short Port;

void port_byte_out(Port port, unsigned char data);

unsigned char port_byte_in(Port port);

#endif

