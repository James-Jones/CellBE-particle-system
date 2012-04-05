#ifndef CLIENT_H_
#define CLIENT_H_

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include <fcntl.h>

#include <cassert>

const int PORT = 60000;
const int CLIENT_ID = 112358;

typedef union
{
	int i;
	char c[4];
} net_int_t;

typedef union
{
	short i;
	char c[2];
} net_short_t;

typedef union
{
  float f;
  char c[4];
} net_float_t;

class Packet
{
	public:
	float pos[4];
	float rgba[4];
};

class Client
{
protected:
	int m_socket;

	bool swap_bytes;
	
	bool print_traffic;
public:
	Client(){print_traffic = false;}

	void display_traffic(bool param){ print_traffic = param;}

  int create();
  
  int connect_to_server();
  
	size_t block_read(void* buffer, size_t nBytes);

  int read_int();
  
	short read_short();
	
	float read_float();
  
  Packet read_packet();
  
	void read_packet_batch(Packet*);

  void write_char(char c)
  {
    send(m_socket, &c, sizeof(char), 0);
  }

  void shutdown(){close(m_socket);}
};

#endif /*CLIENT_H_*/
