#include "../inc/utile.h"

void titreOptions(const char* message, const int niveauVerboSauvegarde){
	// Force l'affichage du titre encadré
	niveauVerbo = 3;

	titreTrame(message);
	printf("\n\n");

	// Remets la verbosité sur le niveau par défaut
	niveauVerbo = niveauVerboSauvegarde;
}

void titreTrame(const char* message){
	// Titre "synthétique" et "complet"
	if (niveauVerbo > CONCIS){
		printf("\n%s", CYAN);

		// 8 espaces de chaque côté du message + 2 '#'
		for (unsigned long i = 0; i < strlen(message) + 18; i++)
			printf("#");

		printf("\n#        %s        #\n", message);

		for (unsigned long i = 0; i < strlen(message) + 18; i++)
			printf("#");

		printf("%s", RESET);
	}

	// Titre "concis"
	else
		printf("%s[%s]%s", CYAN, message, RESET);
}

void titreProto(const char* message, char* couleur){
	if (niveauVerbo > CONCIS)
		printf("\n\n");
	else
		printf("%s ", JAUNE);

	printf("%s", couleur);

	// Titre "complet"
	if (niveauVerbo == COMPLET){
		printf("\n");

		// 1 espaces de chaque côté du message + 2 "*"
		for (unsigned long i = 0; i < strlen(message) + 4; i++)
			printf("*");

		printf("\n* %s *\n", message);

		for (unsigned long i = 0; i < strlen(message) + 4; i++)
			printf("*");

		printf("\n");
	}

	// Titre "synthétique"
	else if (niveauVerbo == SYNTHETIQUE)
		printf("*** %s ***", message);

	// Titre "concis"
	else
		printf("[%s] ", message);

	if (niveauVerbo > CONCIS)
		printf("\n");
}

void sautLigneOuSeparateur(void){
	if (niveauVerbo == COMPLET)
		printf("\n");
	else
		printf(" | ");
}

void verifTaille(const int retourTaille, const size_t tailleBuffer){
	if (retourTaille < 0 || ((size_t) retourTaille) > tailleBuffer){
		fflush(stdout);
		fprintf(stderr, "\n%s|Error| snprintf%s\n", ROUGE, RESET);
		exit(EXIT_FAILURE);
	}
}

void affichageAdresseMAC(const u_int8_t* pointeur){
	// Entre l'adresse dans la structure
	struct ether_addr adresse;

	for (unsigned int i = 0; i < ETHER_ADDR_LEN; i++){
		#if __APPLE__
			adresse.octet[i] = *pointeur++;
		#else
			adresse.ether_addr_octet[i] = *pointeur++;
		#endif
	}

	printf("%s", ether_ntoa(&adresse));
}

void affichageAdresseIPv4(const u_int8_t* pointeur, const u_int8_t longueur){
	// Copie l'adresse dans une structure IPv4
	struct in_addr adresse;
	memcpy((void*)&adresse, pointeur, longueur);

	// Converti en string l'adresse du réseau
	char buffAddrIPv4[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &adresse, buffAddrIPv4, INET_ADDRSTRLEN);

	printf("%s", buffAddrIPv4);
}

void affichageAdresseIPv6(const u_int8_t* pointeur, const u_int8_t longueur){
	// Copie l'adresse dans une structure IPv6
	struct in6_addr adresse;
	memcpy((void*)&adresse, pointeur, longueur);

	// Converti en string l'adresse du réseau
	char buffAddrIPv6[INET6_ADDRSTRLEN] = "";
	inet_ntop(AF_INET6, &adresse, buffAddrIPv6, INET6_ADDRSTRLEN);

	printf("%s", buffAddrIPv6);
}

unsigned int affichageNomDomaine(const u_int8_t* pointeur,
	const unsigned int longueur
){
	unsigned int hexa = *pointeur++, nbrIncrPtr = 0;
	int retourTaille = 0;
	char nomDomaine[255] = "";

	// Boucle sur la taille de données récupérée précédemment
	for (nbrIncrPtr = 0; nbrIncrPtr < longueur; nbrIncrPtr++){
		hexa = *pointeur++;
		int offset;

		if (hexa == FIN)
			break;

		// Code ASCII spécial
		if (hexa == CODE_ASCII){
			nbrIncrPtr += 2;
			break;
		}
		else if (hexa < CODE_CONTROLE && nbrIncrPtr > 0){
			offset = strlen(nomDomaine);
			retourTaille = snprintf(nomDomaine + offset,
				sizeof(nomDomaine) - offset, ".");
			verifTaille(retourTaille, sizeof(nomDomaine));
		}
		else{
			offset = strlen(nomDomaine);
			retourTaille = snprintf(nomDomaine + offset,
				sizeof(nomDomaine) - offset, "%c", hexa);
			verifTaille(retourTaille, sizeof(nomDomaine));
		}
	}

	printf("%s", nomDomaine);
	return nbrIncrPtr;
}

void affichageDureeConvertie(unsigned int dureeSecondes){
	unsigned int j, h, m, s;

	// Jours
	j = dureeSecondes / SEC_DANS_JOUR;
	dureeSecondes -= j * SEC_DANS_JOUR;

	// Heures
	h = dureeSecondes / SEC_DANS_HEURE;
	dureeSecondes -= h * SEC_DANS_HEURE;

	// Minutes
	m = dureeSecondes / SEC_DANS_MIN;
	dureeSecondes -= m * SEC_DANS_MIN;

	// Secondes
	s = dureeSecondes;

	if (j > 0 || h > 0 || m > 0 || s > 0){
		// Jours
		if (j > 0){
			printf("%d day", j);

			// Ajoute un "s" si il y a plusieurs jours
			if (j > 1)
				printf("s");

			if (h > 0 || m > 0 || s > 0)
				printf(", ");
		}

		// Heures
		if (h > 0){
			printf("%d hour", h);
			if (h > 1)
				printf("s");
			if (m > 0 || s > 0)
				printf(", ");
		}

		// Minutes
		if (m > 0){
			printf("%d minute", m);
			if (m > 1)
				printf("s");
			if (s > 0)
				printf(", ");
		}

		// Secondes
		if (s > 0){
			printf("%d second", s);
			if (s > 1)
				printf("s");
		}
	}
	else
		printf("%d seconds", s);
}

int caraCtrl(char caractere){
	// Caractère imprimable
	if (isprint(caractere)){
		printf("%c", caractere);
		return 0;
	}

	switch (caractere){
		case '\n':
			printf("\\n");
			return 1;

		case '\r':
			printf("\\r");
			return 0;

		default:
			printf("\\%03x", caractere);
			return 0;
	}
}

void afficheSrcDstPorts(const unsigned portSrc, const unsigned portDst){
	if (niveauVerbo == COMPLET)
		printf("Source port: ");
	else
		printf("Src: ");
	printf("%u", portSrc);
	sautLigneOuSeparateur();

	if (niveauVerbo == COMPLET)
		printf("Destination port: ");
	else
		printf("Dst: ");
	printf("%u", portDst);
}

void afficheSrcDstAddrIP(const char* ipSrc, const char* ipDst){
	if (niveauVerbo == COMPLET)
		printf("Source address: ");
	else
		printf("Src: ");
	printf("%s", ipSrc);
	sautLigneOuSeparateur();

	if (niveauVerbo == COMPLET)
		printf("Destination address: ");
	else
		printf("Dst: ");
	printf("%s", ipDst);
}