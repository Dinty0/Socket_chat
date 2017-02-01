/*-----------------------------------------------------------
Client a lancer apres le serveur avec la commande :
client <adresse-serveur> <message-a-transmettre>
------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <linux/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

#include <pthread.h>

typedef struct sockaddr
sockaddr;

typedef struct sockaddr_in
sockaddr_in;

typedef struct hostent
hostent;

typedef struct servent
servent;

void recieve_m(int socket_descriptor)
{
    //system("xterm");
    int longueur;
    char buffer[256];

    while(1)
    {
        // clear buffer

        longueur = read(socket_descriptor, buffer, sizeof(buffer));

        if (longueur > 0)
        {
            printf("\n ### \n");
            write(1,buffer,longueur);
            printf("\n ### \n");
        }
    }

}

void send_m(int socket_descriptor)
{
    int longueur;
    char msg[256];

    while(1)
    {
        //clear buffer

        printf("Saisissez un message : ");
        scanf("%s",msg);

        if ((write(socket_descriptor, msg, strlen(msg))) < 0)
        {
            perror("erreur : impossible d'ecrire le message destine au serveur.");
            exit(1);
        }

        if (msg == "/quit")
            return;
    }



}


int main(int argc, char **argv) {

    pthread_t reception;
    pthread_t envoi;

    int socket_descriptor,  /* descripteur de socket */
        longueur;   /* longueur d'un buffer utilisé */

    sockaddr_in adresse_locale; /* adresse de socket local */
    hostent * ptr_host; /* info sur une machine hote */
    servent * ptr_service; /* info sur service */
    char buffer[256];
    char *prog; /* nom du programme */
    char *host; /* nom de la machine distante */
    char *mesg; /* message envoyé */
    char *pseudo;
    char* msg;

    if (argc != 3)
    {
        perror("usage : client <adresse-serveur> <message-a-transmettre>");
        exit(1);
    }

    prog = argv[0];
    host = argv[1];
    pseudo = argv[2];

    //mesg = argv[2];

    printf("nom de l'executable : %s \n", prog);
    printf("adresse du serveur  : %s \n", host);
    printf("Pseudonyme      : %s \n", pseudo);



    if ((ptr_host = gethostbyname(host)) == NULL)
    {
        perror("erreur : impossible de trouver le serveur a partir de son adresse.");
        exit(1);
    }

    printf("hostent : %s \n", (char *)ptr_host);

    /* copie caractere par caractere des infos de ptr_host vers adresse_locale */
    bcopy((char*)ptr_host->h_addr, (char*)&adresse_locale.sin_addr, ptr_host->h_length);
    adresse_locale.sin_family = AF_INET; /* ou ptr_host->h_addrtype; */


    /* 2 facons de definir le service que l'on va utiliser a distance (lemême que sur le serveur) */
    /* (commenter l'une ou l'autre des solutions) */
    /*-----------------------------------------------------------*/
    /* SOLUTION 1 : utiliser un service existant, par ex. "irc" */
    /*
    if ((ptr_service = getservbyname("irc","tcp")) == NULL)
    {
        perror("erreur : impossible de recuperer le numero de port du service desire.");
        exit(1);
    }
    adresse_locale.sin_port = htons(ptr_service->s_port);
    /*-----------------------------------------------------------*/
    /* SOLUTION 2 : utiliser un nouveau numero de port */
    adresse_locale.sin_port = htons(5001);
    /*-----------------------------------------------------------*/

    printf("numero de port pour la connexion au serveur : %d \n", ntohs(adresse_locale.sin_port));

    /* creation de la socket */
    if ((socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("erreur : impossible de creer la socket de connexion avec le serveur.");
        exit(1);
    }

    /* tentative de connexion au serveur dont les infos sont dans adresse_locale */
    if ((connect(socket_descriptor, (sockaddr*)(&adresse_locale), sizeof(adresse_locale))) < 0)
    {
        perror("erreur : impossible de se connecter au serveur.");
        exit(1);
    }

    printf("connexion etablie avec le serveur. \n");
    printf("envoi d'un message au serveur. \n");

    if ((write(socket_descriptor, mesg, strlen(mesg))) < 0)
    {
        perror("erreur : impossible d'ecrire le message destine au serveur.");
        exit(1);
    }

    //clear buffer

    pthread_create(&reception,NULL,recieve_m,(void *)socket_descriptor);
    pthread_create(&envoi,NULL,send_m,(void *)socket_descriptor);

    /* mise en attente du prgramme pour simuler un delai de transmission */
    sleep(3);

    (void) pthread_join(reception,NULL);
    (void) pthread_join(envoi,NULL);

    // printf("\nfin de la reception.\n");

    // close(socket_descriptor);

    // printf("connexion avec le serveur fermee, fin du programme.\n");

    exit(0);
}