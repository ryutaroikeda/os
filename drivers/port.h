#ifndef _port_h_
#define _port_h_

typedef unsigned short Port;

void port_byte_out(Port port, uint8 data);

uint8 port_byte_in(Port port);

#endif

