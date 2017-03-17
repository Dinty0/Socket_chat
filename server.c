/*----------------------------------------------
Serveur à lancer avant le client
------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <linux/types.h> /* pour les users */
#include <sys/socket.h>
#include <netdb.h> /* pour hostent, servent */
#include <string.h> /* pour bcopy, ... */
#define nb_MAX_NOM 256

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;
typedef struct hostent hostent;
typedef struct servent servent;

typedef struct map
{
    int* sockets;
    char** pseudos;
    int* status;
    int* cpt_offWords;
    int* warned;
    int nb;
} map;

void deconnexion(map* users,int ind)
{
    int i;
    for (i=ind;i<users->nb-1;++i)
    {
        users->sockets[i]=users->sockets[i+1];    
    }
    //free(users->sockets[users->nb-1]);
    --users->nb;
}

char* replace_offensive_words(char* str, map* users, int indUser)
{
    const char s[2] = " ";
    char *token = (char*) malloc(256*sizeof(char));
    char temp[256];
    int offensive = 0;

    char* final_string = malloc(256 * sizeof(char));

    token = strtok(str, s);

    static const char filename[] = "offensive_words.txt";
    FILE *file = fopen (filename, "r" );

    if (file != NULL)
    {
        char line[256];
        char *offWord = (char*) malloc(256*sizeof(char));
 
        while (token != NULL) 
        {
            strcpy(temp,token);

            while (fgets(line,sizeof line,file) != NULL) 
            {
                memset(offWord,'\0',sizeof(offWord));
                memmove(offWord,line,strlen(line)-1);

                if (strcmp(token,offWord) == 0)
                {
                    offensive = 1;
                    printf("Offensive word found : %s\n",token);

                    ++users->cpt_offWords[indUser];

                    int i;
                    for(i=0;i<strlen(temp);++i)
                    {
                        temp[i] = '*';
                    }              
                }
            }

            rewind(file);

            final_string = strcat(final_string, temp);
            final_string = strcat(final_string, " ");

            token = strtok(NULL, s);
        }

        fclose (file);
    }
    else
    {
      perror("file not found"); 
    }

    if (offensive)
    {
        final_string = strcat(final_string," (corrected)");
    }


    return final_string;

   
}


/*------------------------------------------------------*/

void renvoi (map* users)
{
    printf("Nombre de clients : %d\n",users->nb);

    int client = users->sockets[users->nb-1];
    int ind = users->nb-1;
    //users->status[ind] = 0;
    users->cpt_offWords[ind] =0;
    users->warned[ind] = 0;

    char buffer[256];
    char stockB[256];
    char stockP[256];
    char pseudo[256];
    char npseu[256];

    char* connexion = malloc(256 * sizeof(char));

    int longueur;

    if ((longueur = read(client, buffer, sizeof(buffer))) <= 0)
        return;

    printf("pseudo lu : %s \n", buffer);

    memmove(connexion,"Nouvelle connexion, pseudo : ",30);
    connexion = strcat(connexion,buffer);
    connexion = strcat(connexion,".\n");

    int i;
    for (i=0;i<users->nb;++i)
    {
        write(users->sockets[i],connexion,strlen(connexion));
    }

    strcpy(stockB,buffer);

    users->pseudos[ind] = malloc(15 * sizeof(char));
    strcpy(users->pseudos[ind],buffer);

    strcat(buffer, " : ");
    strcpy(pseudo,buffer);
    strcpy(stockP,buffer);

    while(strcmp(buffer,"/quit" ) < 0 || strcmp(buffer,"/quit") > 0)
    {
        strcpy(pseudo,stockP);
        memset(buffer,'\0', sizeof(buffer));

        longueur = read(client, buffer, sizeof(buffer));

        buffer[strlen(buffer)-1] = '\0';

        printf("allo le monde : %d\n",strcmp(buffer,'\n'));

        if (longueur > 0 || strcmp(buffer,'\n') == 0)
        {
            char* msg = NULL;

            if (strcmp(buffer,"!help") == 0)
            {
                msg = malloc(110 * sizeof(char));
                memmove(msg,"Type </w> <pseudo dest> <private message> to whisper ! # warning : whispers must not be longer than 256 char #",110);
                write(users->sockets[ind],msg,strlen(msg));
            }
            // PRIVATE MESSAGE
            else if (buffer[0]=='/' && buffer[1]=='w' && buffer[2]==' ')
            {
                char key[3];
                char dest[16];
                char* whisper = (char*)malloc(256*sizeof(char));

                sscanf(buffer,"%s %s %*s", key, dest);

                int inddest;
                int k;
                int destfound = 0;
                for (k=0;k<users->nb;++k)
                {
                    if (strcmp(users->pseudos[k],dest) == 0)
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

                    whisper = replace_offensive_words(whisper,users,ind);

                    msg = malloc((20+strlen(users->pseudos[inddest])+strlen(whisper)+1) * sizeof(char));

                    memmove(msg,"(whisper from ",14);

                    msg = strcat(msg,users->pseudos[ind]);
                    msg = strcat(msg,") : ");
                    msg = strcat(msg,whisper);

                    write(users->sockets[inddest],msg,strlen(msg));

                }
                else
                {
                    write(users->sockets[ind],"### Utilisateur non trouve ###",30);
                }

            }
            else
            {
                // MESSAGE TO THE LOBBY
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

                msg = replace_offensive_words(msg,users,ind);

                int j;
                for (j=0; j<users->nb; ++j)
                {
                    write(users->sockets[j],msg, strlen(msg));
                }
            }

    
            printf("Cpt offWord : %d\n", users->cpt_offWords[ind]);
            printf("Messages envoyés. \n");

        }

        // AUTO KICK FOR OFFENSIVE LANGUAGE
        if (users->cpt_offWords[ind] >= 1 && users->cpt_offWords[ind] < 4 && users->warned[ind] != 1)
        {
            char* warning = (char*)malloc(70*sizeof(char));
            warning = "\n### WARNING : Do not use offensive words or you'll get kicked ! ###";
            write(users->sockets[ind],warning, strlen(warning));

            users->warned[ind] = 1;
        }
        if (users->cpt_offWords[ind] == 4 && users->warned[ind] !=2)
        {
            char* warning = (char*)malloc(75*sizeof(char));
            warning = "\n### LAST WARNING : Do not use offensive words or you'll get kicked ! ###";
            write(users->sockets[ind],warning, strlen(warning));

            users->warned[ind] = 2;
        }
        if (users->cpt_offWords[ind] >= 5 && users->warned[ind] == 2)
        {
            char* warning = (char*)malloc(75*sizeof(char));
            warning = "\n### KICKED. (cause : Offensive language) ###";
            write(users->sockets[ind],warning, strlen(warning));

            char* kickan = (char*)malloc((60+strlen(users->pseudos[ind])) *sizeof(char));

            // Preparation of the kick announcement
            memmove(kickan,"\n### ",6);
            kickan = strcat(kickan,users->pseudos[ind]);
            kickan = strcat(kickan," has been kicked. (cause : Offensive language). ###\n");

            printf("kickan : %s",kickan);

            deconnexion(users,ind);

            // Kick announcement
            int m;
            for (m=0; m<users->nb; ++m)
            {
                write(users->sockets[m],kickan, strlen(kickan));
            }

            return;
        }

    }

    deconnexion(users,ind);
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

    map users;
    map* pusers = &users;
    int nbmax = 2;
    users.nb = 0;
    users.sockets = (int*)malloc(nbmax * sizeof(int));

    //socketsleau de chaines de 15 caractères max
    users.pseudos = (char**)malloc(nbmax * 15 * sizeof(char));
    users.cpt_offWords = (int*)malloc(nbmax * sizeof(int));
    users.warned = (int*)malloc(nbmax * sizeof(int));


    pthread_t thread;

    sockaddr_in adresse_locale, /* structure d'adresse locale*/
        adresse_client_courant; /* adresse client courant */
    hostent* ptr_hote; /* les infos recuperees sur la machine hote */
    servent* ptr_service; /* les infos recuperees sur le service de la machine */

    char machine[nb_MAX_NOM+1]; /* nom de la machine locale */

    gethostname(machine,nb_MAX_NOM); /* recuperation du nom de la machine */

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
            if (users.nb == nbmax)
            {
                int* nvsockets = (int*)malloc(users.nb*2 * sizeof(int));
                char** nvpseudo = (char**)malloc(users.nb*2 * 15* sizeof(char));
                int* nvcpt_offWords = (int*)malloc(users.nb*2 * sizeof(int));
                int* nvstatus = (int*)malloc(users.nb*2 * sizeof(int));
                int i;
                for (i=0; i<nbmax; ++i)
                {
                    nvsockets[i] = users.sockets[i];
                    nvpseudo[i] = users.pseudos[i];
                    nvcpt_offWords[i] = users.cpt_offWords[i];
                    nvstatus[i] = users.status[i];
                }

                users.sockets = nvsockets;
                users.pseudos = nvpseudo;
                users.cpt_offWords = nvcpt_offWords;
                users.status = nvstatus;

                nbmax = users.nb*2;
            }

            users.sockets[users.nb] = nouv_socket_descriptor;
            users.nb++;
        }

        /* traitement du message */
        if (pthread_create(&thread,NULL,renvoi,(int *)pusers))
        {
            printf("Erreur pthread_create");
        }


    }
}
