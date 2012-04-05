#include "client.h"

#include <stdio.h>
#include <strings.h>
#include <cstring>
#include <netinet/in.h>

int Client::create()
{

  m_socket = socket(AF_INET, SOCK_STREAM, 0);

  //confirm valid socket
  if( m_socket < 0)
  {
    perror("client could not open socket");
    return -1;
  }

  //set socket options
  int sockVal = 1;
  setsockopt( m_socket, SOL_SOCKET, SO_REUSEADDR, (const void *) &sockVal, (socklen_t)sizeof( sockVal ) );

  return 1;
}

int Client::connect_to_server()
{
  swap_bytes = false;

  struct sockaddr_in sin;
  struct hostent* hp;
  
  //fill in sin with server data
  bzero(&sin, sizeof( sin ));
  sin.sin_family = AF_INET; //address family
  hp = gethostbyname ( "172.20.0.2" );
  bcopy(hp->h_addr, &sin.sin_addr, hp->h_length);
  sin.sin_port = htons( PORT );

  //connect to the server
  if( connect( m_socket, (struct sockaddr*) &sin, sizeof( sin ) ) < 0)
  {
    perror("client cannot connect");
    close( m_socket);
    return -1;
  }

  //tell the server what the client ID is
  int clientid=htonl( CLIENT_ID );
  if( send( m_socket, &clientid, sizeof( int ), 0 ) < 0)
  {
    perror("could not send client id to server");
    close( m_socket );
    return -1;
  }

  //wait for ACK from server to confirm server is online
  short acknowledgement;
	acknowledgement = read_short();
		
  if( acknowledgement )
    printf("successfully connected to server.\n");
  else
  {
    perror("");
    close( m_socket );
    return -1;
  }

  if(acknowledgement >> 8)
    swap_bytes = true;

  return 1;
}

size_t Client::block_read(void* buffer, size_t nBytes)
{
	size_t bytes_pending = nBytes;
	char* ptr = (char*) buffer;
		
	while(bytes_pending > 0)
	{
		size_t bytes_read;
		bytes_read = recv( m_socket, ptr, bytes_pending, 0);
		if( bytes_read < 0)
		{
			if(( errno == EINTR) || (errno == EWOULDBLOCK))
				bytes_read = 0;
			else
				return -errno;
		}
		else
		if( bytes_read == 0)
			break;
			
		bytes_pending -= bytes_read;
		ptr += bytes_read;
	}
	return nBytes-bytes_pending; 
}

short Client::read_short()
{
	net_short_t buffer;
	int n = block_read( buffer.c, 2 );
	if(print_traffic)
		printf("Client recieved: %d \n", n );
	if(n < 0)
	{
		fprintf(stderr, "block_read returned -ve (errno =%d)\n", errno);
		exit(1);
	}
//	print_data(buffer.i);
	if(swap_bytes)
	{
		char temp;
		temp = buffer.c[0];
		buffer.c[0] = buffer.c[1];
		buffer.c[1] = temp;
	}
	return buffer.i;
}
int Client::read_int()
{
	net_int_t buffer;
	int n = block_read( buffer.c, 4 );
	if(print_traffic)
		printf("Client recieved: %d \n", n );
	if(n < 0)
	{
		fprintf(stderr, "block_read returned -ve (errno =%d)\n", errno);
		exit(1);
	}
	if(swap_bytes)
	{
		char temp;
		temp = buffer.c[0];
		buffer.c[0] = buffer.c[1];
		buffer.c[1] = temp;
		
		temp = buffer.c[2];
		buffer.c[2] = buffer.c[3];
		buffer.c[3] = temp;
	}
	return buffer.i;
}
float Client::read_float()
{
	net_float_t buffer;
	int n = block_read( buffer.c, 4 );
	if(print_traffic)
		printf("Client recieved: %.2f\n", buffer.f);
	if(n < 0)
	{
		fprintf(stderr, "block_read returned -ve (errno =%d)\n", errno);
		exit(1);
	}
	if(swap_bytes)
	{
		char temp;
		temp = buffer.c[0];
		buffer.c[0] = buffer.c[1];
		buffer.c[1] = temp;
		
		temp = buffer.c[2];
		buffer.c[2] = buffer.c[3];
		buffer.c[3] = temp;
	}
	return buffer.f;
}

Packet Client::read_packet()
{
	Packet p;

	p.pos[0] = read_float();
	p.pos[1] = read_float();
	p.pos[2] = read_float();
	p.pos[3] = read_float();
	
	p.rgba[0] = read_float();
	p.rgba[1] = read_float();
	p.rgba[2] = read_float();
	p.rgba[3] = read_float();
    
    return p;
}

void Client::read_packet_batch(Packet* p)
{
	p[0] = read_packet();
	p[1] = read_packet();
	p[2] = read_packet();
	p[3] = read_packet();
	p[4] = read_packet();
	p[5] = read_packet();
	p[6] = read_packet();
	p[7] = read_packet();
	p[8] = read_packet();
	p[9] = read_packet();

}
  
