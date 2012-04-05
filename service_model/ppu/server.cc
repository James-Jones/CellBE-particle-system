
#include "server.h"
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>

void Server::write_packet_batch(Particle* v)
{
	net_vecf vect;
	
// FIRST PARTICLE	
	Packet p = v[0].packet;
  vect.vec = p.pos;
	

  send(m_client_socket, &vect.scalar[0], sizeof(float), 0);
  send(m_client_socket, &vect.scalar[1], sizeof(float), 0);
  send(m_client_socket, &vect.scalar[2], sizeof(float), 0);
  send(m_client_socket, &vect.scalar[3], sizeof(float), 0);

  vect.vec = p.rgba;
    
  send(m_client_socket, &vect.scalar[0], sizeof(float), 0);
  send(m_client_socket, &vect.scalar[1], sizeof(float), 0);
  send(m_client_socket, &vect.scalar[2], sizeof(float), 0);
  send(m_client_socket, &vect.scalar[3], sizeof(float), 0);

//SECOND PARTICLE
  p = v[1].packet;
  vect.vec = p.pos;

  send(m_client_socket, &vect.scalar[0], sizeof(float), 0);
  send(m_client_socket, &vect.scalar[1], sizeof(float), 0);
  send(m_client_socket, &vect.scalar[2], sizeof(float), 0);
  send(m_client_socket, &vect.scalar[3], sizeof(float), 0);
    
  vect.vec = p.rgba;

  send(m_client_socket, &vect.scalar[0], sizeof(float), 0);
  send(m_client_socket, &vect.scalar[1], sizeof(float), 0);
  send(m_client_socket, &vect.scalar[2], sizeof(float), 0);
  send(m_client_socket, &vect.scalar[3], sizeof(float), 0);
 
 //THIRD
  p = v[2].packet;
  vect.vec = p.pos;

  send(m_client_socket, &vect.scalar[0], sizeof(float), 0);
  send(m_client_socket, &vect.scalar[1], sizeof(float), 0);
  send(m_client_socket, &vect.scalar[2], sizeof(float), 0);
  send(m_client_socket, &vect.scalar[3], sizeof(float), 0);
    
  vect.vec = p.rgba;
    
  send(m_client_socket, &vect.scalar[0], sizeof(float), 0);
  send(m_client_socket, &vect.scalar[1], sizeof(float), 0);
  send(m_client_socket, &vect.scalar[2], sizeof(float), 0);
  send(m_client_socket, &vect.scalar[3], sizeof(float), 0);

//FOURTH
  p = v[3].packet;
  vect.vec = p.pos;
	  
  send(m_client_socket, &vect.scalar[0], sizeof(float), 0);
  send(m_client_socket, &vect.scalar[1], sizeof(float), 0);
  send(m_client_socket, &vect.scalar[2], sizeof(float), 0);
  send(m_client_socket, &vect.scalar[3], sizeof(float), 0);
    
  vect.vec = p.rgba;
    
  send(m_client_socket, &vect.scalar[0], sizeof(float), 0);
  send(m_client_socket, &vect.scalar[1], sizeof(float), 0);
  send(m_client_socket, &vect.scalar[2], sizeof(float), 0);
  send(m_client_socket, &vect.scalar[3], sizeof(float), 0);
 
 //FITH
  p = v[4].packet;
  vect.vec = p.pos;

  send(m_client_socket, &vect.scalar[0], sizeof(float), 0);
  send(m_client_socket, &vect.scalar[1], sizeof(float), 0);
  send(m_client_socket, &vect.scalar[2], sizeof(float), 0);
  send(m_client_socket, &vect.scalar[3], sizeof(float), 0);
    
  vect.vec = p.rgba;
    
  send(m_client_socket, &vect.scalar[0], sizeof(float), 0);
  send(m_client_socket, &vect.scalar[1], sizeof(float), 0);
  send(m_client_socket, &vect.scalar[2], sizeof(float), 0);
  send(m_client_socket, &vect.scalar[3], sizeof(float), 0);

//SIXTH
    p = v[5].packet;
    vect.vec = p.pos;
	  
    send(m_client_socket, &vect.scalar[0], sizeof(float), 0);
    send(m_client_socket, &vect.scalar[1], sizeof(float), 0);
    send(m_client_socket, &vect.scalar[2], sizeof(float), 0);
    send(m_client_socket, &vect.scalar[3], sizeof(float), 0);
    
    vect.vec = p.rgba;
    
    send(m_client_socket, &vect.scalar[0], sizeof(float), 0);
    send(m_client_socket, &vect.scalar[1], sizeof(float), 0);
    send(m_client_socket, &vect.scalar[2], sizeof(float), 0);
    send(m_client_socket, &vect.scalar[3], sizeof(float), 0);

 //SEVENTH
    p = v[6].packet;
    vect.vec = p.pos;
	  
    send(m_client_socket, &vect.scalar[0], sizeof(float), 0);
    send(m_client_socket, &vect.scalar[1], sizeof(float), 0);
    send(m_client_socket, &vect.scalar[2], sizeof(float), 0);
    send(m_client_socket, &vect.scalar[3], sizeof(float), 0);
    
    vect.vec = p.rgba;
    
    send(m_client_socket, &vect.scalar[0], sizeof(float), 0);
    send(m_client_socket, &vect.scalar[1], sizeof(float), 0);
    send(m_client_socket, &vect.scalar[2], sizeof(float), 0);
    send(m_client_socket, &vect.scalar[3], sizeof(float), 0);

//EIGTH
    p = v[7].packet;
    vect.vec = p.pos;
	  
    send(m_client_socket, &vect.scalar[0], sizeof(float), 0);
    send(m_client_socket, &vect.scalar[1], sizeof(float), 0);
    send(m_client_socket, &vect.scalar[2], sizeof(float), 0);
    send(m_client_socket, &vect.scalar[3], sizeof(float), 0);
    
    vect.vec = p.rgba;
    
    send(m_client_socket, &vect.scalar[0], sizeof(float), 0);
    send(m_client_socket, &vect.scalar[1], sizeof(float), 0);
    send(m_client_socket, &vect.scalar[2], sizeof(float), 0);
    send(m_client_socket, &vect.scalar[3], sizeof(float), 0);

//NINTH
    p = v[8].packet;
    vect.vec = p.pos;
	  
    send(m_client_socket, &vect.scalar[0], sizeof(float), 0);
    send(m_client_socket, &vect.scalar[1], sizeof(float), 0);
    send(m_client_socket, &vect.scalar[2], sizeof(float), 0);
    send(m_client_socket, &vect.scalar[3], sizeof(float), 0);
    
    vect.vec = p.rgba;
    
    send(m_client_socket, &vect.scalar[0], sizeof(float), 0);
    send(m_client_socket, &vect.scalar[1], sizeof(float), 0);
    send(m_client_socket, &vect.scalar[2], sizeof(float), 0);
    send(m_client_socket, &vect.scalar[3], sizeof(float), 0);

//TENTH
    p = v[9].packet;
    vect.vec = p.pos;
	  
    send(m_client_socket, &vect.scalar[0], sizeof(float), 0);
    send(m_client_socket, &vect.scalar[1], sizeof(float), 0);
    send(m_client_socket, &vect.scalar[2], sizeof(float), 0);
    send(m_client_socket, &vect.scalar[3], sizeof(float), 0);
    
    vect.vec = p.rgba;
    
    send(m_client_socket, &vect.scalar[0], sizeof(float), 0);
    send(m_client_socket, &vect.scalar[1], sizeof(float), 0);
    send(m_client_socket, &vect.scalar[2], sizeof(float), 0);
    send(m_client_socket, &vect.scalar[3], sizeof(float), 0);
}

bool Server::create()
{
  m_socket = socket( AF_INET, SOCK_STREAM, 0 );

  //confirm valid socket
  if( m_socket < 0 )
  {
    perror( "server could not open socket");
    return false;
  }

  //set socket options
  int sockVal = 1;
  setsockopt( m_socket, SOL_SOCKET, SO_REUSEADDR, (const void *) &sockVal, (socklen_t)sizeof( sockVal ) );
  return true;
}

bool Server::bind_to_port()
{
  //bind server port
  struct sockaddr_in serverAddr;
  serverAddr.sin_family = AF_INET; //address family
  serverAddr.sin_addr.s_addr = htonl( INADDR_ANY );
  serverAddr.sin_port = htons ( PORT );

  if( bind( m_socket, (struct sockaddr*) &serverAddr, sizeof( serverAddr ) ) < 0 )
  {
    perror("could not bind port");
    return false;
  }
  return true;
}
bool Server::listen_for_client()
{

  listen( m_socket, 1); //create a socket queue to accept an incomming connection

  printf("waiting for data from client\n");

  struct sockaddr_in clientAddr;
  unsigned int clientLength = sizeof ( clientAddr );

  m_client_socket = accept( m_socket, (struct sockaddr*) &clientAddr, &clientLength );

    //confirm valid socket
  if( m_client_socket < 0 )
  {
    perror( "server could not connect to client");
    return false;
  }

  printf("client <-> server connection confirmed\n");


	printf("waiting for client ID\n");
  //get client id sent by client
  int id;
  int data = recv( m_client_socket, &id, sizeof( int ), MSG_WAITALL );

  printf("client ID recieved\n Verifying client ID ...\n");

  id = ntohl( id );
  if( id != m_client_id )
  {
    perror("recieved bad client ID");
  }
  
  printf("client ID is OK\n");

	printf("sending ACK ...\n");

  //send ACK to client
  short acknowledgement = 1;

  data = send( m_client_socket, &acknowledgement, sizeof( short ), 0 );

  if( data != sizeof( short ) )
    perror("error in sending ACk");

	printf("ACK sent\n");
}
