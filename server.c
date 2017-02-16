/*----------------------------------------------
Serveur à lancer avant le client
------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <linux/types.h> /* pour les sockets */
#include <sys/socket.h>
#include <netdb.h> /* pour hostent, servent */
#include <string.h> /* pour bcopy, ... */
#define TAILLE_MAX_NOM 256

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;
typedef struct hostent hostent;
typedef struct servent servent;

typedef struct inttab
{
    int* tab;
    char** pseudos;
    int taille;
} inttab;

void deconnexion(inttab* sockets,int ind)
{
    int i;
    for (i=ind;i<sockets->taille-1;++i)
    {
        sockets->tab[i]=sockets->tab[i+1];    
    }
    //free(sockets->tab[sockets->taille-1]);
    --sockets->taille;
}


/*------------------------------------------------------*/


void renvoi (inttab* sockets)
{
    printf("Nombre de clients : %d\n",sockets->taille);

    int client = sockets->tab[sockets->taille-1];
    int ind = sockets->taille-1;

    char buffer[256];
    char stockB[256];
    char stockP[256];
    char pseudo[256];
    char npseu[256];

    char* connexion = malloc(256);

    int longueur;

    if ((longueur = read(client, buffer, sizeof(buffer))) <= 0)
        return;

    printf("pseudo lu : %s \n", buffer);

    memmove(connexion,"Nouvelle connexion, pseudo : ",30);
    connexion = strcat(connexion,buffer);
    connexion = strcat(connexion,".\n");

    int i;
    for (i=0;i<sockets->taille;++i)
    {
        write(sockets->tab[i],connexion,strlen(connexion));
    }

    strcpy(stockB,buffer);

    printf("ind : %d\n", ind);
    sockets->pseudos[ind] = malloc(15 * sizeof(char));
    strcpy(sockets->pseudos[ind],buffer);

    int m;
    for (m=0;m<sockets->taille;++m)
    {
        printf("pseudo tab : %s\n",sockets->pseudos[m]);
    }

    strcat(buffer, " : ");
    strcpy(pseudo,buffer);
    strcpy(stockP,buffer);

    while(strcmp(buffer,"/quit" ) < 0 || strcmp(buffer,"/quit") > 0)
    {
        strcpy(pseudo,stockP);
        memset(buffer,'\0', sizeof(buffer));

        longueur = read(client, buffer, sizeof(buffer));

        if (longueur > 0)
        {
            char* msg = NULL;

            if (strcmp(buffer,"!help") == 0)
            {
                msg = malloc(55 * sizeof(char));
                memmove(msg,"Type </w> <pseudo dest> <private message> to whisper !",54);
                write(sockets->tab[ind],msg,strlen(msg));
            }
            else if (buffer[0]=='/' && buffer[1]=='w' && buffer[2]==' ')
            {
                char key[3];
                char dest[16];
                char whisper[256];

                sscanf(buffer,"%s %s %*s", key, dest);

                int inddest;
                int k;
                int destfound = 0;
                for (k=0;k<sockets->taille;++k)
                {
                    if (strcmp(sockets->pseudos[k],dest) == 0)
                    {   
                        inddest = k;
                        destfound = 1;
                    }
                }

                if (destfound)
                {
                    int discardlength = strlen(key)+strlen(dest)+2;
                    int l;
                    for (l=0;l<strlen(buffer)-discardlength;++l)
                    {
                        whisper[l] = buffer[l+discardlength];
                    }


                    msg = malloc((20+strlen(sockets->pseudos[inddest])+strlen(whisper)+1) * sizeof(char));

                    memmove(msg,"(whisper from ",14);

                    msg = strcat(msg,sockets->pseudos[ind]);
                    msg = strcat(msg,") : ");
                    msg = strcat(msg,whisper);

                    write(sockets->tab[inddest],msg,strlen(msg));

                }
                else
                {
                    write(sockets->tab[ind],"### Utilisateur non trouve ###",22);
                }

            }
            else
            {
                msg = malloc(256 * sizeof(char));
                if ((strcmp(buffer,"/quit") > 0 || strcmp(buffer,"/quit") < 0))
                {
                    printf("Reception d'un message.\n");
                    printf("message lu : %s \n", buffer);
                    printf("Renvoi du message pour les clients connectés.\n");

                    msg = strcat(pseudo,buffer);   
                }
                else //déconnexion
                {
                    memmove(msg,"### Déconnexion de ",20);

                    int nb = strlen(pseudo);
                    nb = nb - 3;

                    memmove(npseu,pseudo,nb);
                    msg = strcat(msg,npseu);
                    msg = strcat(msg,". ###\n");

                    memmove(pseudo,npseu,strlen(npseu));
                }

                int j;
                for (j=0; j<sockets->taille; ++j)
                {
                    write(sockets->tab[j],msg, strlen(msg));
                }
            }

    

            printf("Messages envoyés. \n");

        }

    }

    deconnexion(sockets,ind);
    printf("### Déconnexion de : %s",npseu);
    printf(". ###\n");


    return;
}
/*------------------------------------------------------*/
/*------------------------------------------------------*/
main(int argc, char **argv)
{

    int socket_descriptor, /* descripteur de socket */
        nouv_socket_descriptor, /* [nouveau] descripteur de socket */
        longueur_adresse_courante; /* longueur d'adresse courante d'un client */

    inttab sockets;
    inttab* psockets = &sockets;
    int taillemax = 2;
    sockets.taille = 0;
    sockets.tab = (int*)malloc(taillemax * sizeof(int));

    //tableau de chaines de 15 caractères max
    sockets.pseudos = malloc(taillemax * 15 * sizeof(char));

    pthread_t thread;

    sockaddr_in adresse_locale, /* structure d'adresse locale*/
        adresse_client_courant; /* adresse client courant */
    hostent* ptr_hote; /* les infos recuperees sur la machine hote */
    servent* ptr_service; /* les infos recuperees sur le service de la machine */

    char machine[TAILLE_MAX_NOM+1]; /* nom de la machine locale */

    gethostname(machine,TAILLE_MAX_NOM); /* recuperation du nom de la machine */

    /* recuperation de la structure d'adresse en utilisant le nom */
    if ((ptr_hote = gethostbyname(machine)) == NULL)
    {
        perror("erreur : impossible de trouver le serveur a partir de son nom.");
        exit(1);
    }

    /* initialisation de la structure adresse_locale avec les infos recuperees */

    /* copie de ptr_hote vers adresse_locale */
    bcopy((char*)ptr_hote->h_addr, (char*)&adresse_locale.sin_addr,ptr_hote->h_length);
    adresse_locale.sin_family = ptr_hote->h_addrtype; /* ou AF_INET */
    adresse_locale.sin_addr.s_addr = INADDR_ANY; /* ou AF_INET */

    /* 2 facons de definir le service que l'on va utiliser a distance */
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

    printf("numero de port pour la connexion au serveur : %d \n", ntohs(adresse_locale.sin_port) /*ntohs(ptr_service->s_port)*/);

    /* creation de la socket */
    if ((socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("erreur : impossible de creer la socket de connexion avec le client.");
        exit(1);
    }

    /* association du socket socket_descriptor à la structure d'adresse adresse_locale */
    if ((bind(socket_descriptor, (sockaddr*)(&adresse_locale), sizeof(adresse_locale))) < 0)
    {
        perror("erreur : impossible de lier la socket a l'adresse de connexion.");
        exit(1);
    }

    /* initialisation de la file d'ecoute */
    listen(socket_descriptor,1);

    /* attente des connexions et traitement des donnees recues */
    for(;;) {

        longueur_adresse_courante = sizeof(adresse_client_courant);

        /* adresse_client_courant sera renseignée par accept via les infos du connect */
        nouv_socket_descriptor = accept(socket_descriptor, (sockaddr*)(&adresse_client_courant),&longueur_adresse_courante);
        if (nouv_socket_descriptor < 0)
        {
            perror("erreur : impossible d'accepter la connexion avec le client.");
            exit(1);
        }
        else
        {
            if (sockets.taille == taillemax)
            {
                int* nvtab = (int*)malloc(sockets.taille*2 * sizeof(int));
                char** nvpseudo = malloc(sockets.taille*2 * 15* sizeof(char));
                int i;
                for (i=0; i<taillemax; ++i)
                {
                    nvtab[i] = sockets.tab[i];
                    nvpseudo[i] = sockets.pseudos[i];
                }

                sockets.tab = nvtab;
                sockets.pseudos = nvpseudo;
                taillemax = sockets.taille*2;
            }

            sockets.tab[sockets.taille] = nouv_socket_descriptor;
            sockets.taille++;
        }

        /* traitement du message */
        if (pthread_create(&thread,NULL,renvoi,(int *)psockets))
        {
            printf("Erreur pthread_create");
        }


    }
}
