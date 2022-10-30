#include "../inc/dns.h"

void verifTaille(const int retourTaille, const size_t element){
	if (retourTaille < 0 || ((size_t) retourTaille) >= element){
		fflush(stdout);
		fprintf(stderr, "\n%s|Error| snprintf%s\n", ROUGE, RESET);
		exit(EXIT_FAILURE);
	}
}

void affichageDuree(const unsigned int dureeSecondes){
	unsigned int h, m, s;

	h = dureeSecondes / 3600;
	m = (dureeSecondes - (3600 * h)) / 60;
	s = dureeSecondes - (3600 * h) - (m * 60);

	if (h > 0)
		printf("(%d hours, %d minutes, %d seconds)", h, m, s);
	else if (m > 0)
		printf("(%d minutes, %d seconds)", m, s);
	else
		printf("(%d seconds)", s);
}

void affichageType(const unsigned int type){
	printf("\n\tType : ");
		switch(type){
			/* A */
			case A:
				printf("A (Host adress)");
				break;

			/* NS */
			case NS:
				printf("NS (Authoritative name server)");
				break;

			/* CNAME */
			case CNAME:
				printf("CNAME (Canonical NAME for an alias)");
				break;

			/* SOA */
			case SOA:
				printf("SOA (Start of a zone of authority)");
				break;

			/* WKS */
			case WKS:
				printf("WKS (Well known service description)");
				break;

			/* PTR */
			case PTR:
				printf("PTR (Domain name pointer)");
				break;

			/* HINFO */
			case HINFO:
				printf("HINFO (Host information)");
				break;

			/* MINFO */
			case MINFO:
				printf("MINFO (Mailbox or mail list information)");
				break;

			/* MX */
			case MX:
				printf("MX (Mail exchange)");
				break;

			/* TXT */
			case TXT:
				printf("TXT (Text strings)");
				break;

			/* Inconnu */
			default:
				printf("Unsupported");
				break;
		}
		printf(" (%d)", type);
}

void affichageClasse(const unsigned int classe){
	printf("\n\tClass : ");
		switch(classe){
			/* IN */
			case IN:
				printf("IN");
				break;

			/* CS */
			case CS:
				printf("CS");
				break;

			/* CH */
			case CH:
				printf("CH");
				break;

			/* HS */
			case HS:
				printf("HS");
				break;

			/* Inconnu */
			default:
				printf("Unknown");
				break;
		}
		printf(" (0x%04x)", classe);
}

unsigned int recupereNiemeBit(const unsigned int nombre,
	const unsigned int nieme
){
	return (nombre >> ((TAILLEBIT-1)-nieme)) & 1;
}

void affichageBinaire(const unsigned int nombre,
	const unsigned int nieme, const unsigned int nbrContigu
){
	printf("\n");
	for (int i = 0; i < TAILLEBIT; i++){
		if (i == nieme){
			printf("%d", recupereNiemeBit(nombre, nieme));
			for (int j = 1; j < nbrContigu; j++){
				printf("%d", recupereNiemeBit(nombre, nieme+j));
				i++;
			}
		}
		else
			printf(".");
	}
}

void gestionDNS(const u_char* paquet, const int size_udp){
	// On se place après l'entête UDP
	u_int8_t* pointeurDns =  (u_int8_t*) paquet + size_udp;
	unsigned int hexUn, hexDeux, hexTrois, hexQuatre, concatHex;
	unsigned int bitUn, bitDeux, bitTrois, bitQuatre, concatBit, retourBit;
	unsigned int nbrQuestions, nbrReponses;

	char nomDomaine[TAILLEMAX];

	titreViolet("DNS");
	printf(JAUNE);

	hexUn = *pointeurDns++; 				// Récupère le premier hexa
	hexDeux = *pointeurDns++;				// Récupère le second hexa
	concatHex = (hexUn << 8) | (hexDeux);	// Concatène les deux
	printf("Transaction ID : 0x%04x", concatHex);

	hexUn = *pointeurDns++;
	hexDeux = *pointeurDns++;
	concatHex = (hexUn << 8) | (hexDeux);
	printf("\nFlags : 0x%04x", concatHex);
	int niemeBit = 0;

	printf("\n");
	for (int i = niemeBit; i < TAILLEBIT; i++){
		printf("%d ", recupereNiemeBit(concatHex, i));
	}

	// Response
	affichageBinaire(concatHex, niemeBit, 1);
	unsigned int typeReponse = recupereNiemeBit(concatHex, niemeBit);
	printf("\tResponse : ");
	if (typeReponse == REPONSE)
		printf("Message is a response");
	else
		printf("Message is a query");

	// Op code
	bitUn = recupereNiemeBit(concatHex, ++niemeBit);
	affichageBinaire(concatHex, niemeBit, 4);
	bitDeux = recupereNiemeBit(concatHex, ++niemeBit);
	bitTrois = recupereNiemeBit(concatHex, ++niemeBit);
	bitQuatre = recupereNiemeBit(concatHex, ++niemeBit);
	printf("\tOp code : ");
	concatBit = (bitUn << 3) | (bitDeux << 2) | (bitTrois << 1) | (bitQuatre);

	switch(concatBit){
		/* Query */
		case QUERY:
			printf("Standard query");
			break;

		/* Iquery */
		case IQUERY:
			printf("Inverse query");
			break;

		/* Status */
		case STATUS:
			printf("Server status request");
			break;

		/* Inconnu */
		default:
			printf("Unknown");
			break;
	}
	printf(" (%d)", concatBit);

	// Authoritative
	if (typeReponse == REPONSE){
		retourBit = recupereNiemeBit(concatHex, ++niemeBit);
		affichageBinaire(concatHex, niemeBit, 1);
		printf("\tAuthoritative : ");
		if (retourBit)
			printf("Server is an authority for domain");
		else
			printf("Server is not an authority for domain");
	}
	else
		++niemeBit;

	// Truncated
	retourBit = recupereNiemeBit(concatHex, ++niemeBit);
	affichageBinaire(concatHex, niemeBit, 1);
	printf("\tTruncated : ");
	if (retourBit)
		printf("Message is truncated");
	else
		printf("Message is not truncated");

	// Recursion desired
	retourBit = recupereNiemeBit(concatHex, ++niemeBit);
	affichageBinaire(concatHex, niemeBit, 1);
	printf("\tRecursion desired : ");
	if (retourBit)
		printf("Do query recursively");
	else
		printf("Don't query recursively");

	// Recursion available
	retourBit = recupereNiemeBit(concatHex, ++niemeBit);
	affichageBinaire(concatHex, niemeBit, 1);
	printf("\tRecursion available : ");
	if (retourBit)
		printf("Server can do recursive queries");
	else
		printf("Server can't do recursive queries");

	// Z (Reserved)
	bitUn = recupereNiemeBit(concatHex, ++niemeBit);
	affichageBinaire(concatHex, niemeBit, 3);
	bitDeux = recupereNiemeBit(concatHex, ++niemeBit);
	bitTrois = recupereNiemeBit(concatHex, ++niemeBit);
	printf("\tZ : ");
	concatBit = (bitUn << 3) | (bitDeux << 2) | (bitTrois << 1) | (bitQuatre);

	if (concatBit == ALLNULL)
		printf("Reserved (%d)", concatBit);

	// Reply code
	if (typeReponse == REPONSE){
		bitUn = recupereNiemeBit(concatHex, ++niemeBit);
		affichageBinaire(concatHex, niemeBit, 4);
		bitDeux = recupereNiemeBit(concatHex, ++niemeBit);
		bitTrois = recupereNiemeBit(concatHex, ++niemeBit);
		bitQuatre = recupereNiemeBit(concatHex, ++niemeBit);
		printf("\tReply code : ");
		concatBit = (bitUn << 3) | (bitDeux << 2) | (bitTrois << 1) | (bitQuatre);

		switch(concatBit){
			/* No error */
			case NOERR:
				printf("No error");
				break;

			/* Format error */
			case FORMERR:
				printf("Format error");
				break;

			/* Server failure */
			case FAILERR:
				printf("Server failure ");
				break;

			/* Name error */
			case NAMEERR:
				printf("Name error");
				break;

			/* Not implemented */
			case NOTIMPL:
				printf("Not implemented");
				break;

			/* Refused */
			case REFUSED:
				printf("Refused");
				break;

			/* Inconnu */
			default:
				printf("Unknown");
				break;
		}
		printf(" (%d)", concatBit);
	}

	hexUn = *pointeurDns++;
	hexDeux = *pointeurDns++;
	concatHex = (hexUn << 8) | (hexDeux);
	printf("\nQuestions : %d", concatHex);
	nbrQuestions = (int) concatHex;

	hexUn = *pointeurDns++;
	hexDeux = *pointeurDns++;
	concatHex = (hexUn << 8) | (hexDeux);
	printf("\nAnswer RRs : %d", concatHex);
	nbrReponses = concatHex;

	hexUn = *pointeurDns++;
	hexDeux = *pointeurDns++;
	concatHex = (hexUn << 8) | (hexDeux);
	printf("\nAuthority RRs : %d", concatHex);

	hexUn = *pointeurDns++;
	hexDeux = *pointeurDns++;
	concatHex = (hexUn << 8) | (hexDeux);
	printf("\nAdditional RRs : %d", concatHex);

	// S'il y a des "queries"
	if (nbrQuestions > 0){
		printf("\nQueries :");

		while (nbrQuestions > 0){
			nbrQuestions--;
			printf("\n\tName : ");
			u_int8_t hexa = *pointeurDns++;
			int tailleNom = 0, nbrLabels = 1, i = 0, retourTaille = 0;
			while(1){
				hexa = *pointeurDns++;

				if (hexa == FIN)
					break;
				if (hexa == POINT1 || hexa == POINT2){
					printf(".");
					nbrLabels++;

					retourTaille = snprintf(nomDomaine, sizeof(nomDomaine),
						"%s.", nomDomaine);
					verifTaille(retourTaille, sizeof(nomDomaine));
				}
				else{
					printf("%c", hexa);
					retourTaille = snprintf(nomDomaine, sizeof(nomDomaine),
						"%s%c", nomDomaine, hexa);
					verifTaille(retourTaille, sizeof(nomDomaine));
				}
				tailleNom++;
			}
			printf("\n\t[Name length] : %d", tailleNom);
			printf("\n\t[Label count] : %d", nbrLabels);

			// Type
			hexUn = *pointeurDns++;
			hexDeux = *pointeurDns++;
			concatHex = (hexUn << 8) | (hexDeux);
			affichageType(concatHex);

			// Classe
			hexUn = *pointeurDns++;
			hexDeux = *pointeurDns++;
			concatHex = (hexUn << 8) | (hexDeux);
			affichageClasse(concatHex);

			printf("\n");
		}
	}

	// S'il y a des "answers"
	if (nbrReponses > 0){
		printf("\nAnswers :");

		while (nbrReponses > 0){
			nbrReponses--;

			unsigned int type;
			hexUn = *pointeurDns++;
			hexDeux = *pointeurDns++;
			concatHex = (hexUn << 8) | (hexDeux);
			printf("\n\tName : ");
			if (concatHex == AFFICHEA || concatHex == AFFICHECNAME)
				printf("%s", nomDomaine);
			else
				printf("Unknown (0x%04x)", concatHex);

			// Type
			hexUn = *pointeurDns++;
			hexDeux = *pointeurDns++;
			concatHex = (hexUn << 8) | (hexDeux);
			type = concatHex;
			affichageType(type);

			// Classe
			hexUn = *pointeurDns++;
			hexDeux = *pointeurDns++;
			concatHex = (hexUn << 8) | (hexDeux);
			affichageClasse(concatHex);

			// Time to live
			hexUn = *pointeurDns++;
			hexDeux = *pointeurDns++;
			hexTrois = *pointeurDns++;
			hexQuatre = *pointeurDns++;
			concatHex = (hexUn << 24) | (hexDeux << 16) | (hexTrois << 8) |
				(hexQuatre);
			printf("\n\tTime to live : %d ", concatHex);
			affichageDuree(concatHex);

			// Data length
			hexUn = *pointeurDns++;
			hexDeux = *pointeurDns++;
			concatHex = (hexUn << 8) | (hexDeux);
			printf("\n\tData length : %d", concatHex);

			// Adress
			printf("\n\tAddress : ");
			if (type == CNAME){
				// Vide le nom de domaine précédement enregistré
				memset(nomDomaine, 0, sizeof(nomDomaine));

				unsigned int hexa = *pointeurDns++;

				int retourTaille = 0;
				for (int i = 0; i < concatHex; i++){
					hexa = *pointeurDns++;

					if (hexa == FIN)
						break;
					if (hexa == POINT1 || hexa == POINT2){
						printf(".");
						retourTaille = snprintf(nomDomaine, sizeof(nomDomaine),
							"%s.", nomDomaine);
						verifTaille(retourTaille, sizeof(nomDomaine));
					}
					else{
						printf("%c", hexa);
						retourTaille = snprintf(nomDomaine, sizeof(nomDomaine),
							"%s%c", nomDomaine, hexa);
						verifTaille(retourTaille, sizeof(nomDomaine));
					}
				}
			}
			else{
				affichageIP(pointeurDns, concatHex);
				pointeurDns += concatHex;
			}

			printf("\n");
		}
	}
}