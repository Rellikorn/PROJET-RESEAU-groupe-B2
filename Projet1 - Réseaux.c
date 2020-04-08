#include <stdio.h>
#include <stdlib.h> /* pour exit */
#include <sys/types.h>
#include <sys/socket.h>

struct sockaddr
{
	unsigned short int sa_family; //au choix unsigned 
	char sa_data[14]; //en fonction de la famille
};

struct in_addr
{
	unsigned int s_addr;
}; // une adresse Ipv4 (32 bits)

struct sockaddr_in
{
	unsigned short int sin_family; // <- PF_INET
	unsigned short int sin_port; // <- numéro de port
	struct in_addr sin_addr; // <- adresse IPv4
	unsigned char sin_zero[8]; // ajustement pour être compatible avec sockaddr
};

int main(int argc, char const *argv[])
{
	int descripteurSocket;
	//--- Début de l’étape n°1 : 
	// Crée un socket de communication
	descripteurSocket = socket(PF_INET, SOCK_STREAM, 0);
	/* 0 indique que l’on utilisera le protocole par défaut associé à SOCK_STREAM soit TCP */
	// Teste la valeur renvoyée par l’appel système socket()
	
	if(descripteurSocket < 0) /* échec ? */
	{
		perror("socket"); // Affiche le message d’erreu
		exit(-1); // On sort en indiquant un code erreur
	}
	
	//--Fin de l’étape n°1 !
	printf("Socket créée avec succès ! (%d)\n", descripteurSocket);

	// On ferme la ressource avant de quitter 
	close(descripteurSocket);

	int descripteurSocket;
	struct sockaddr_in pointDeRencontreDistant;
	socklen_t longueurAdresse;
	
	// Crée un socket de communication
	descripteurSocket = socket(PF_INET, SOCK_STREAM, 0);

	// Teste la valeur renvoyée par l’appel système socket() 
	if(descripteurSocket < 0)
	{
		perror("socket"); // Affiche le message d’erreur 
		exit(-1); // On sort en indiquant un code erreur
	}
	
	printf("Socket créée avec succès ! (%d)\n", descripteurSocket);
	//--- Début de l’étape n°2 : 
	// Obtient la longueur en octets de la structure 
	sockaddr_in longueurAdresse = sizeof(pointDeRencontreDistant);
	// Initialise à 0 la structure
	sockaddr_in memset(&pointDeRencontreDistant, 0x00, longueurAdresse);
	// Renseigne la structure sockaddr_in avec les informations du serveur distant
	pointDeRencontreDistant.sin_family = PF_INET;
	// On choisit le numéro de port d’écoute du serveur
	pointDeRencontreDistant.sin_port = htons(IPPORT_USERRESERVED); // = 5000 
	// On choisit l’adresse IPv4 du serveur
	inet_aton("127.0.0.1", &pointDeRencontreDistant.sin_addr);
	// à modifier selon ses besoins
	// Débute la connexion vers le processus serveur distant
	
	if((connect(descripteurSocket, (struct sockaddr *)&pointDeRencontreDistant, longueurAdresse)) == -1)
	{
		perror("connect"); // Affiche le message d’erreur
		close(descripteurSocket); // On ferme la ressource avant de quitter 
		exit(-2); // On sort en indiquant un code erreur
	}

	//--- Fin de l’étape n°2 ! 
	printf("Connexion au serveur réussie avec succès !\n");
	// On ferme la ressource avant de quitter close(descripteurSocket); 

	return 0;
}