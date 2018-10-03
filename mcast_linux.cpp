//
// Created by kuusri on 30/09/2018.
//

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT 12345
#define GROUP "225.0.0.37"                      // Multicast -osoite, jota käytetään

#define USER_IDENT "<AME>: "                    // Käyttäjän tunniste

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void* multicast_listener ( void* data)          // Kuuntelee multicastia
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

{
     struct sockaddr_in addr;                   // Soketti-muuttujat
     int                soketti ;

     struct ip_mreq mreq;

     printf("Multicast-listener aloittelee...\n") ;

     // Muodostetaan tavan UDP soketti
     if ((soketti = socket(AF_INET,SOCK_DGRAM,0)) < 0)
     {
        perror("##Virhe: soketin muodostaminen");
        exit(1);
     }

     u_int yes = 1;                             // Optioiden asettaminen

     // Sallitaan useamman soketin käyttää samaa porttinumeroa
     if (setsockopt(soketti, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
     {
        perror("##Virhe: soketin yhteiskäyttö ei toimi");
        exit(1);
     }

     // Asetetaan kuuntelun IP ja portti kaikille interfaceille
     memset(&addr, 0, sizeof(addr));
     addr.sin_family = AF_INET;
     addr.sin_addr.s_addr = htonl(INADDR_ANY);
     addr.sin_port = htons(PORT);

     // Sitten sitoudutaan kuuntelulle
     if (bind(soketti,(struct sockaddr *) &addr,sizeof(addr)) < 0)
     {
        perror("##Virhe: bind()");
        exit(1);
     }

     // Pyydetään käyttikseltä liittymistä multicast groupiin, tehdään setsockopt() :lla
     mreq.imr_multiaddr.s_addr = inet_addr(GROUP);
     mreq.imr_interface.s_addr = htonl(INADDR_ANY);

     if (setsockopt(soketti, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
     {
        perror("##Virhe: setsockopt()");
        exit(1);
     }

     // Siirrytään kuuntelulle
     printf("Multicast-kuuntelu alkaa...\n") ;

     while (1)
     {
          char msg[1024];                               // Lukupuskuri
	  int addrlen = sizeof(addr);                   // Osoitekentän pituus kutsua varten

	  if ( recvfrom(soketti, msg, sizeof(msg), 0, (struct sockaddr *) &addr, &addrlen) < 0)
	  {
	       perror("##Virhe: recvfrom()");
	       exit(1);
	  }

	  printf("\n%s", msg);                          // Tulostetaan saatu sanoma
     }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void* multicast_sender( void* data)   // Lähettää multicastia
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

{
     struct sockaddr_in addr;
     int                soketti ;

     struct ip_mreq mreq;
     char msg[512] = USER_IDENT ;

     printf("Multicast lähettäminen aluillaan...\n") ;

     // Tehdään soketti normaaliin tapaan, DGRAM -> UDP
     if ( (soketti = socket(AF_INET,SOCK_DGRAM, 0)) < 0 )
     {
        perror("##Virhe: socket()");
        exit(1);
     }

     // Aseta kohdeosoite, osoitekentät on yleensä tapana nollata ensin :-)
     memset(&addr,0, sizeof(addr));
     addr.sin_family = AF_INET;
     addr.sin_addr.s_addr = inet_addr(GROUP);
     addr.sin_port = htons(PORT);

     // Sitten alamme puskea dataa sokettiin
     printf("Nyt voidaan aloittaa kirjoittaminen multicastiin...\n") ;
     strcat(msg, "Hello there, I'm here!\n") ;
     usleep(10000) ;                                        // pieni 10ms viive tähän

     int not_done = 1 ;

     while ( 1 > 0 )
     {
        if ( sendto(soketti, msg, strlen(msg) + 1, 0,(struct sockaddr *) &addr, sizeof(addr)) < 0 )
        {
                perror("##Virhe: sendto()");
                exit(1);
        }

        if ( not_done == 0 )         // On annettu EXIT viesti
            return NULL ;

        printf("msg>") ;
        fgets(&msg[strlen(USER_IDENT)], sizeof(msg) - strlen(USER_IDENT), stdin);
        not_done = strcmp(&msg[strlen(USER_IDENT)], "EXIT\n") ;
    }
}

// - - - - - - - - - - - - - - -
int main(int argc, char *argv[])
// - - - - - - - - - - - - - - -

{
    pthread_t mcl ;                                        // Kuuntelijan säie

    pthread_create(&mcl, NULL, multicast_listener, NULL);  // Aletaan kuunnella multicastia
    multicast_sender(NULL) ;                               // Aletaan lähettää multicastia (ei säie!)

    return 0;
}
