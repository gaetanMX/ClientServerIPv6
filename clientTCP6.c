/*********************************************************************
 *                                                                   *
 * FICHIER: CLIENT_TCP                                               *
 *                                                                   *
 * DESCRIPTION: Utilisation de TCP socket par une application client *
 *                                                                   *
 * principaux appels (du point de vue client) pour un protocole      *
 * oriente connexion:                                                *
 *     socket()                                                      *
 *                                                                   * 
 *     connect()                                                     *
 *                                                                   *
 *     write()                                                       *
 *                                                                   *
 *     read()                                                        *
 *                                                                   *
 *********************************************************************/
#include     <stdio.h>
#include     <sys/types.h>
#include     <sys/socket.h>
#include     <netinet/in.h>
#include     <arpa/inet.h>
#include     <string.h>
#include     <netdb.h>
#include     <stdlib.h>
#include     "util.c"
#define MAXLINE 80

usage(){
  printf("usage : cliecho adresseIPv6_serveur(x.x.x.x)  numero_port_serveur\n");
}




main (argc, argv)
int argc;
char *argv[];

{
  int serverSocket, servlen, n, retread,ret;
  //sockaddr_in to sockaddr_in6
  struct sockaddr_in6  serv_addr;
  char fromServer[MAXLINE];
  char fromUser[MAXLINE];
  struct hostent *hp;  
  struct addrinfo req, *ans;
  req.ai_flags = 0;
  req.ai_family = AF_INET6;
  req.ai_socktype = SOCK_STREAM; 
  req.ai_protocol = 0;
  void *addr;
  char *ipver;
  char ipstr[INET6_ADDRSTRLEN];
  
  /* Verifier le nombre de paramètre en entrée */
  /* clientTCP <hostname> <numero_port>        */
  if (argc != 3){
    usage();
    exit(1);
    }


  
  /* 
   * Remplir la structure  serv_addr avec l'adresse du serveur 
   */
  
  bzero( (char *) &serv_addr, sizeof(serv_addr) );
  serv_addr.sin6_family = AF_INET6; //AF_INET to AF_INET6 et sin_family to sin6_family

  serv_addr.sin6_port = atoi(argv[2]); //sin_port to sin6_port
  ; //getaddrinfo à la place de gethostbyname
  if (getaddrinfo(argv[1], argv[2],&req,&ans) != 0) {
    fprintf(stderr, "%s: %s non trouve dans in /etc/hosts ou dans le DNS\n",
            argv[0], argv[1]);
    exit(1);
  }
  
  struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)ans->ai_addr;
  addr = &(ipv6->sin6_addr);  
  serv_addr.sin6_addr = * ((struct in6_addr *)(ans->ai_addr));
  ipver = "IPv6";
  inet_ntop(ans->ai_family, addr, ipstr, sizeof ipstr);
  printf("Version %s: adresse serveur %s , numero_port : %d\n", ipver, ipstr,serv_addr.sin6_port);
   
  /*
   * Ouvrir socket (socket STREAM)
   */
  if ((serverSocket = socket(ans->ai_family, ans->ai_socktype, ans->ai_protocol)) <0) {
    perror ("erreur socket");
    exit (1);
  }

  /*
   * Connect to the serveur 
   */
  if (connect (serverSocket, ans->ai_addr, ans->ai_addrlen )  < 0){
     perror ("erreur connect");
    exit (1);
  }

  while ( (retread =readline (serverSocket, fromServer, MAXLINE)) > 0)
    {
      printf ("corr: %s", fromServer);
      if (strcmp (fromServer,"Au revoir\n") == 0) 
	break ; /* fin de la lecture */
      
      /* saisir message utilisateur */
      printf("vous: ");
      if (fgets(fromUser, MAXLINE,stdin) == NULL) {
	perror ("erreur fgets \n");
	exit(1);
      }
      
      /* Envoyer le message au serveur */
      if ( (n= writen (serverSocket, fromUser, strlen(fromUser)) ) != strlen(fromUser))  {
	printf ("erreur writen");
	exit (1);
      }
    }
  if(retread < 0 ) {
    	perror ("erreur readline \n");
	//	exit(1);
  }
  freeaddrinfo(ans);
  close(serverSocket);
}
