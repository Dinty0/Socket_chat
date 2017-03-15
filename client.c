/*-----------------------------------------------------------
Client a lancer apres le serveur avec la commande :
client <adresse-serveur> <message-a-transmettre>
------------------------------------------------------------*/
#include <stdlib.h>
//#include <stdio.h>
#include <linux/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

#include <pthread.h>
#include <ncurses.h>

typedef struct sockaddr
sockaddr;

typedef struct sockaddr_in
sockaddr_in;

typedef struct hostent
hostent;

typedef struct servent
servent;

int connected = 1;

void recieve_m(int socket_descriptor)
{
    //system("xterm");
    int longueur;
    char buffer[256];

    while(connected)
    {
        longueur = read(socket_descriptor, buffer, sizeof(buffer));

        if (longueur > 0)
        {
            write(1,buffer,longueur);
            printf("\n");
        }
    }
    printf("Déconnecté de la reception.\n");

}

void reset_cache()
{
    int c;
    do {
        c = getchar();
    } while (c != '\n' && c != EOF);
}

void send_m(int socket_descriptor)
{
    int longueur;
    char msg[256];

    while(connected)
    {   
        fgets(msg,256,stdin);

        if (strlen(msg) >= 255)
        {
            reset_cache();           
        }

        if ((write(socket_descriptor, msg, strlen(msg))) < 0)
        {
            perror("erreur : impossible d'ecrire le message destine au serveur.");
            exit(1);
        }

        if (strcmp(msg,"/quit" ) == 10)
        {
            connected = 0;
            printf("Déconnecté de l'envoi.\n");
            return;
        }


    }



}


int main(int argc, char **argv) {

    pthread_t reception;
    pthread_t envoi;

    // WINDOW *haut, *bas;
    // initscr();
    // haut= subwin(stdscr, LINES / 2, COLS, 0, 0);        // Créé une fenêtre de 'LINES / 2' lignes et de COLS colonnes en 0, 0
    // bas= subwin(stdscr, LINES / 2, COLS, LINES / 2, 0); // Créé la même fenêtre que ci-dessus sauf que les coordonnées changent
    
    // box(haut, ACS_VLINE, ACS_HLINE);
    // box(bas, ACS_VLINE, ACS_HLINE);
    
    // mvwprintw(haut, 1, 1, "Ceci est la fenetre du haut");
    // mvwprintw(bas, 1, 1, "Ceci est la fenetre du bas");
    
    // wrefresh(haut);
    // wrefresh(bas);
    
    // getch();
    // endwin();

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
        perror("usage : client <adresse-serveur> <pseudo>");
        exit(1);
    }

    prog = argv[0];
    host = argv[1];
    pseudo = argv[2];
 
    if (strlen(pseudo) > 15)
    {
        perror("pseudo trop long, 15 char max");
        exit(0);
    }

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


    /* 2 facons de definir le service que l'on va utiliser a distance (le même que sur le serveur) */
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

    /* tentative de connexion au serveur dont les infos sont dans
adresse_locale */
    if ((connect(socket_descriptor, (sockaddr*)(&adresse_locale), sizeof(adresse_locale))) < 0)
    {
        perror("erreur : impossible de se connecter au serveur.");
        exit(1);
    }

    printf("connexion etablie avec le serveur. \n");
    printf("envoi d'un message au serveur. \n");

    if ((write(socket_descriptor, pseudo, strlen(pseudo))) < 0)
    {
        perror("erreur : impossible d'ecrire le message destine au serveur.");
        exit(1);
    }

    pthread_create(&reception,NULL,recieve_m,(int *)socket_descriptor);
    pthread_create(&envoi,NULL,send_m,(int *)socket_descriptor);

    /* mise en attente du prgramme pour simuler un delai de transmission */
    sleep(3);

    (void) pthread_join(reception,NULL);
    (void) pthread_join(envoi,NULL);

    // printf("\nfin de la reception.\n");

    // close(socket_descriptor);

    // printf("connexion avec le serveur fermee, fin du programme.\n");

    exit(0);
}