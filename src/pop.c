#include "../inc/pop.h"

void gestionPOP(const u_char* paquet, const int offset, int tailleHeader){
	// On se place après l'entête TCP
	u_int8_t* pointeurPOP = (u_int8_t*) paquet + offset;
	char* type = "";

	titreProto("POP", ROUGE);

	// Cherche si une commande a été envoyée
	if (memcmp(pointeurPOP, USER, sizeofSansSenti(USER)) == 0)
		type = USER;
	else if	(memcmp(pointeurPOP, PASS, sizeofSansSenti(PASS)) == 0)
		type = PASS;
	else if (memcmp(pointeurPOP, STAT, sizeofSansSenti(STAT)) == 0)
		type = STAT;
	else if (memcmp(pointeurPOP, LIST, sizeofSansSenti(LIST)) == 0)
		type = LIST;
	else if (memcmp(pointeurPOP, UIDL, sizeofSansSenti(UIDL)) == 0)
		type = UIDL;
	else if (memcmp(pointeurPOP, RETR, sizeofSansSenti(RETR)) == 0)
		type = RETR;
	else if (memcmp(pointeurPOP, DELE, sizeofSansSenti(DELE)) == 0)
		type = DELE;
	else if (memcmp(pointeurPOP, TOP, sizeofSansSenti(TOP)) == 0)
		type = TOP;
	else if (memcmp(pointeurPOP, LAST, sizeofSansSenti(LAST)) == 0)
		type = LAST;
	else if (memcmp(pointeurPOP, RSET, sizeofSansSenti(RSET)) == 0)
		type = RSET;
	else if (memcmp(pointeurPOP, NOOP, sizeofSansSenti(NOOP)) == 0)
		type = NOOP;
	else if (memcmp(pointeurPOP, QUIT, sizeofSansSenti(QUIT)) == 0)
		type = QUIT;
	else if (memcmp(pointeurPOP, OK, sizeofSansSenti(OK)) == 0)
		type = OK;

	if (niveauVerbo > SYNTHETIQUE){
		// N'affiche pas le "\r\n" à la fin (d'où le "- 2")
		for (int i = 0; i < tailleHeader - 2; i++)
			printf("%c", *pointeurPOP++);
	}
	else{
		if (strlen(type) > 0)
			printf("%s", type);
		else
			printf("%s", DATA);
	}
}