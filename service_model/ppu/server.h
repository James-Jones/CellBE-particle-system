#ifndef SERVER_H_
#define SERVER_H_

#include <unistd.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include "common.h"
#include <misc/libmisc.h>
#include <vec_literal.h>
#include <vec_types.h>

const int PORT = 60000;
const int CLIENT_ID = 112358;


typedef union
{
	vector signed short v;
	short s[8];
} net_vec8;

typedef union
{
	vector float vec;
	float scalar[4];
} net_vecf;

typedef union
{
	float f;
	char c[4];
} net_float_t;

class Server
{
  private:
    
protected:
  int m_socket;
  int m_client_socket;
  int m_port;
  int m_client_id;
  
  bool print_traffic;

public:
	Server(){ print_traffic = false; }
  void set_port(int port){m_port = port;}
  void set_client_id(int id){m_client_id = id;}
  void set_print_traffic(bool b){print_traffic = b;}
  
  char read_char(){char c; recv(m_socket, &c, 1, 0); return c;}
  
  bool create();
  void shutdown(){close(m_socket);}
  
  bool bind_to_port();
  bool listen_for_client();

  void write_int(int s){ send( m_client_socket, &s, sizeof( int ), 0 ); }
  void write_float(float s){ send( m_client_socket, &s, sizeof( float ), 0 ); }

  void write_packet_batch(Particle* v);
};

#endif /*SERVER_H_*/

