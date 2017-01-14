#ifndef _io_h_
#define _io_h_

typedef unsigned short Port;

void port_byte_out(Port port, unsigned char data);

unsigned char port_byte_in(Port port);

#endif

