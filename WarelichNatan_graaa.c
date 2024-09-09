#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include <string.h>
#include <strings.h>

#include <sys/socket.h>
#include <sys/types.h>

#define MAXROZMIAR 1024

int poznanie_sie = 0;
int gracz1 = 0;
int gracz2 = 0;
int rozgrywana = 0;
char nazwa_oponenta[1024];


void *pozyskaj_adres(struct sockaddr *sa) {
	
	/*pozyskujemy tylko adresy IPv4*/
    return &(((struct sockaddr_in *)sa)->sin_addr);
    
}

void informacje_klient_host(struct sockaddr *ich_adres, char* nazwa) {
	
    char przedstawiany_adres[INET6_ADDRSTRLEN];
    inet_ntop(ich_adres->sa_family, pozyskaj_adres(ich_adres), przedstawiany_adres, sizeof(przedstawiany_adres));
    if(strcmp(nazwa, "4W5JnLN9Zz") == 0)
    {
		printf("\ntwoim przeciwnikiem jest: %s\n \n", przedstawiany_adres);
		strcpy(nazwa_oponenta, przedstawiany_adres);
	}
	else
	{
		printf("\ntwoim przeciwnikiem jest: %s\n \n", nazwa);
		strcpy(nazwa_oponenta, nazwa);
		
	}
}

int koniec(char *msg, int sockfd) {
	close(sockfd);
	perror(msg);
    exit(0);
   
}

int losowanie(){
	int liczba;
	srand(time(NULL));
	
	liczba = rand()% 10 + 1;
	return liczba;
	
}


void komunikacja(int sockfdhost, char **argv, int argc) {
    char wiadomosc[MAXROZMIAR];
    int operacja;
	int liczba;

    /*tutaj rozpoczynamy nasze polaczenie z hostem*/
    
    int sockfd;
    struct addrinfo wskazowki, *serwer_info, *p;
    int wynik_gadri;
  
    memset(&wskazowki, 0, sizeof(wskazowki));
    wskazowki.ai_family = AF_INET;
    wskazowki.ai_socktype = SOCK_DGRAM;
    
    if ((wynik_gadri = getaddrinfo(argv[1], argv[2], &wskazowki, &serwer_info)) != 0) {
        koniec("getaddrinfo klient", -1);
    }
    
    for (p = serwer_info; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("klient socket");
            continue;
        }
        break; 
    }
    if (p == NULL) {
        koniec("nie udalo polaczyc sie z serwerem", sockfd);
    }
    freeaddrinfo(serwer_info);
    
    /*przechodzimy do komunikacji z hostem*/
    
    memset(wiadomosc, 0, sizeof(wiadomosc));
	
    if (poznanie_sie == 0) {

		printf("Witamy w grze do 50 wariant B!\n");
		printf("napisz koniec aby zakonczyc gre\n");
		printf("napisz wynik, aby wyswietlic wynik\n");
		
		/*wyslanie nazwy uzytkownika lub klucza w przypadku jej braku*/
		if(argc > 3)
		{
			strcpy(wiadomosc, argv[3]);
		}
		else
		{
			strcpy(wiadomosc, "4W5JnLN9Zz");
		}
		
		if ((operacja = sendto(sockfd, wiadomosc, sizeof(wiadomosc), 0, p->ai_addr, p->ai_addrlen)) == -1) {
            koniec("sendto klient, dane osobowe", sockfdhost);
        }
        memset(wiadomosc, 0, sizeof(wiadomosc));
        /*odbieranie danych o serwerze*/
		if ((operacja = recvfrom(sockfdhost, wiadomosc, MAXROZMIAR - 1, 0, NULL, NULL)) == -1) 
		{
			koniec("recvfrom klient, dane osobowe", sockfdhost);
		}
        if (wiadomosc[strlen(wiadomosc) - 1] == '1')
        {
			/*tutaj wykrylismy ze doszlo do polaczenia*/
			poznanie_sie++;
			wiadomosc[strlen(wiadomosc) - 1] = '\0';
			
		}
        informacje_klient_host(p->ai_addr, wiadomosc);
        poznanie_sie++;
        
        if(poznanie_sie == 1)
        {
			struct addrinfo wskazowki, *serwer_info, *p;
			memset(&wskazowki, 0, sizeof(wskazowki));
			wskazowki.ai_family = AF_INET;
			wskazowki.ai_socktype = SOCK_DGRAM;
				
			if ((wynik_gadri = getaddrinfo(argv[1], argv[2], &wskazowki, &serwer_info)) != 0) 
			{
				koniec("getaddrinfo klient", sockfdhost);
			}
				
			for (p = serwer_info; p != NULL; p = p->ai_next) {
				if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) 
				{
					perror("klient socket");
					continue;
					}
				break; 
			}
			if (p == NULL) 
			{
				koniec("nie udalo polaczyc sie z serwerem", sockfd);
			}
			freeaddrinfo(serwer_info);
			
			
			printf("wysylam nick po raz drugi");
			/*tutaj dodajemy znak "1" aby i wysylamy do drugiej strony "oznake zycia"*/
			if(argc > 3)
			{
				strcpy(wiadomosc, argv[3]);
			}
			else
			{
				strcpy(wiadomosc, "4W5JnLN9Zz");
			}
			char tmp[sizeof(wiadomosc) + 1];
			strcpy(tmp, wiadomosc);
			tmp[strlen(wiadomosc) - 1] = '1';
			if ((operacja = sendto(sockfd, tmp, sizeof(tmp), 0, p->ai_addr, p->ai_addrlen)) == -1) 
			{
				koniec("sendto klient, dane osobowe", sockfdhost);
				
			}
		}

    }
    
    /*wyjasnienie: robimy tutaj while w while poniewaz nie wiemy ile rozgrywek bedzie,
     * a ture ma zaczac gracz drugi po pierwszej rozgrywce, 
     * program zakonczy sie tylko i wylacznie w momencie wpisania "koniec"*/
    while(1)
    {
		while(poznanie_sie % 2 == 0)
		{
			memset(wiadomosc, 0, sizeof(wiadomosc));
			   /*odebranie liczby losowej*/     
			if ((operacja = recvfrom(sockfdhost, wiadomosc, MAXROZMIAR - 1, 0, NULL, NULL)) == -1) 
			{
				koniec("recvfrom klient, liczba losowa", sockfdhost);
			}
			printf("polaczono z hostem, wylosowana liczba to: %s\n", wiadomosc);
			rozgrywana += atoi(wiadomosc);
			memset(wiadomosc, 0, sizeof(wiadomosc));
			printf("oczekiwanie az twoj przeciwnik poda liczbe\n");
			
			/*otrzymanie pierwszej liczby*/
			if ((operacja = recvfrom(sockfdhost, wiadomosc, MAXROZMIAR - 1, 0, NULL, NULL)) == -1) 
			{
				/*h1 - host pierwszy, g2 - gracz drugi*/
				koniec("h1g2 recvfrom klient, liczba losowa", sockfdhost);
			}
			if(strcmp(wiadomosc, "koniec") == 0 || strcmp(wiadomosc, "koniec\n") == 0)
			{
				/*wyjasnienie: tutaj raczej oczywiste nasz host wyszedl, klient sobie nie zhostuje gry*/
				printf("host wyszedl, byc moze ragequit...\n");
				close(sockfd);
				exit(EXIT_SUCCESS);
				
			}
			if((atoi(wiadomosc)) >= 50){
				printf("koniec rundy, wygrywasz te runde\n");
				gracz2++;
				break;
			}
			printf("twoj oponent %s podal liczbe: %d\n", nazwa_oponenta, atoi(wiadomosc));
			rozgrywana += atoi(wiadomosc) - rozgrywana;
			memset(wiadomosc, 0, sizeof(wiadomosc));
			
			/*ponizej mamy petle while ktora zabezpiecza nas przed przypadkami takimi jak:
			 * - celowe wielokrotne podawanie nieprawidlowych danych, 
			 * - celowe wielokrotne podawanie wynik
			 * petla ta zawsze sie odbedzie gdyz resetujemy nasze memset*/
			while(1)
			{
				printf("twoja kolej podaj liczbe: \n");
				if ((operacja = read(0, wiadomosc, MAXROZMIAR)) == -1) 
				{
					koniec("h1g2 Błąd czytania, liczba/wynik/koniec", sockfdhost); 
				}
				
				/*wyjasnienie: w programie czesto sprawdzamy samo wyrazenie jak i wyrazenie z znakiem konca linii
				 * spowodowane jest to tym iz jezeli uzytkownik napisze wynik/koniec przed swoim ruchem to program
				 * przyjmie takowy napis bez "\n"*/
				if(strcmp(wiadomosc, "koniec") == 0 || strcmp(wiadomosc, "koniec\n") == 0)
				{
					if ((operacja = sendto(sockfd, wiadomosc, MAXROZMIAR - 1, 0, p->ai_addr, p->ai_addrlen)) == -1) 
					{
						koniec("h1g2 sendto klient, koniec", sockfdhost);
					}
					close(sockfd);
					exit(EXIT_SUCCESS);
				}
				
				while(strcmp(wiadomosc, "wynik") == 0 || strcmp(wiadomosc, "wynik\n") == 0)
				{
					printf("ty: %d, %s: %d\n", gracz2, nazwa_oponenta, gracz1);
					if ((operacja = read(0, wiadomosc, MAXROZMIAR)) == -1) 
					{
					koniec("h1g2 Błąd czytania, wynik", sockfdhost); 
					}
				}
				
				if((atoi(wiadomosc) - rozgrywana) > 10 || (atoi(wiadomosc) - rozgrywana) < 1)
				{
					printf("podano nieprawidlowe dane, sprobuj ponownie\n");
				}
				else
				{
					break;
				}
			}
			rozgrywana = atoi(wiadomosc) - rozgrywana;
			
			if ((operacja = sendto(sockfd, wiadomosc, MAXROZMIAR - 1, 0, p->ai_addr, p->ai_addrlen)) == -1) 
			{
				koniec("h1g2 sendto klient, podana liczba", sockfdhost);
			}
			if(atoi(wiadomosc) >= 50){
				printf("koniec rundy, wygrywa: %s", nazwa_oponenta);
				gracz1++;
				break;
			}
			printf("przeslano liczbe do gracza: %s, oczekiwanie na odpowiedz...\n", nazwa_oponenta);		
			
			
		}
		
		printf("\nrozpoczynamy kolejna runde\n \n");
		
		
		while(poznanie_sie % 2 == 1)
		{
			memset(wiadomosc, 0, sizeof(wiadomosc));
			/*losowanie liczby i wyslanie jej do przeciwnika*/
			liczba = losowanie();
			sprintf(wiadomosc, "%d", liczba);
			if((operacja = sendto(sockfd, wiadomosc, strlen(wiadomosc), 0, p->ai_addr, p->ai_addrlen)) == -1)
			{
				koniec("sendto host, liczba losowa", sockfdhost);

			}
			printf("rozpoczynamy rozgrywke wylosowana liczba to: %d\n", liczba);
			rozgrywana += liczba;			
			
			memset(wiadomosc, 0, sizeof(wiadomosc));
			while(1)
			{
				printf("twoja kolej podaj liczbe: \n");
				if ((operacja = read(0, wiadomosc, MAXROZMIAR)) == -1) 
				{
					/*analogicznie h2 - host drugi, g1 - gracz pierwszy*/
					koniec("h2g1 Błąd czytania, liczba/wynik/koniec", sockfdhost); 
				}
				
				/*wyjasnienie: w programie czesto sprawdzamy samo wyrazenie jak i wyrazenie z znakiem konca linii
				 * spowodowane jest to tym iz jezeli uzytkownik napisze wynik/koniec przed swoim ruchem to program
				 * przyjmie takowy napis bez "\n"*/
				if(strcmp(wiadomosc, "koniec") == 0 || strcmp(wiadomosc, "koniec\n") == 0)
				{
					if((operacja = sendto(sockfd, wiadomosc, strlen(wiadomosc), 0, p->ai_addr, p->ai_addrlen)) == -1)
					{
						koniec("h1g2 sendto klient, koniec", sockfdhost);
					}
					close(sockfd);
					exit(EXIT_SUCCESS);
				}
				
				while(strcmp(wiadomosc, "wynik") == 0 || strcmp(wiadomosc, "wynik\n") == 0)
				{
					printf("ty: %d, %s: %d\n", gracz2, nazwa_oponenta, gracz1);
					if ((operacja = read(0, wiadomosc, MAXROZMIAR)) == -1) 
					{
					koniec("h2g1 Błąd czytania, wynik", sockfdhost); 
					}
				}
				
				if((atoi(wiadomosc) - rozgrywana) > 10 || (atoi(wiadomosc) - rozgrywana) < 1)
				{
					printf("podano nieprawidlowe dane, sprobuj ponownie\n");
				}
				else
				{
					break;
				}
			}
			
			rozgrywana = atoi(wiadomosc) - rozgrywana;
			
			if((operacja = sendto(sockfd, wiadomosc, strlen(wiadomosc), 0, p->ai_addr, p->ai_addrlen)) == -1)
			{
				koniec("h2g1 sendto klient, dane osobowe", sockfdhost);
			}
			
			if(atoi(wiadomosc) >= 50){
				printf("koniec rundy, wygrywa: %s", nazwa_oponenta);
				gracz1++;
				break;
			}
			printf("przeslano liczbe do gracza: %s, oczekiwanie na odpowiedz...\n", nazwa_oponenta);		
			
			memset(wiadomosc, 0, sizeof(wiadomosc));
			
			if ((operacja = recvfrom(sockfd, wiadomosc, MAXROZMIAR - 1	, 0, NULL, NULL)) == -1)
			{
				koniec("h1g2 recvfrom klient, liczba losowa", sockfdhost);
			}
			if(strcmp(wiadomosc, "koniec") == 0 || strcmp(wiadomosc, "koniec\n") == 0)
			{
				/*wyjasnienie: tutaj raczej oczywiste nasz host wyszedl, klient sobie nie zhostuje gry*/
				printf("host wyszedl, byc moze ragequit...\n");
				poznanie_sie = 0;
				rozgrywana = 0;
			}
			if((atoi(wiadomosc)) >= 50){
				printf("koniec rundy, wygrywasz\n");
				gracz2++;
				break;
			}
			printf("twoj oponent %s podal liczbe: %d\n", nazwa_oponenta, atoi(wiadomosc));
			rozgrywana += atoi(wiadomosc) - rozgrywana;
				
		}
		printf("\nrozpoczynamy kolejna runde\n \n");
	}
    close(sockfd);
}


int utworzenie_hosta(char** argv, int argc) {
    struct addrinfo wskazowki, *serwer_info, *p;
	int wynik_gadri;
    int sockfd = -1;

    memset(&wskazowki, 0, sizeof(wskazowki));
    wskazowki.ai_family = AF_INET; /*wymuszenie dzialania na IPv4*/
    wskazowki.ai_socktype = SOCK_DGRAM; /*dzialamy na serwerze UDP*/
    wskazowki.ai_flags = AI_PASSIVE; /*adresem serwera bedzie nasz adres*/
    
    if ((wynik_gadri = getaddrinfo(NULL, argv[2], &wskazowki, &serwer_info)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(wynik_gadri));
        return -1;
    }

    for (p = serwer_info; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("listener: socket");
            continue;
        }
		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			koniec("bind", sockfd);
		}
        break;
    }
	if(p == NULL)
	{
		/*nieudalo sie utworzyc serwera
		 * czyli prawdopodobnie takowy juz istnieje*/
		return -1;
	}
    freeaddrinfo(serwer_info);
    
    
    return sockfd;
}

int stworzenie_serwera(char *addr, char *port, char** argv, int argc) {
	
    int sockfd;
	sockfd = utworzenie_hosta(argv, argc);

    if (sockfd == -1) {
        return -1;
    }

    return sockfd;
}


int main(int argc, char **argv) {

    int sockfd_host;
	int sprawdzajaca;
	
    if (argc < 3 || argc > 4) {
        fprintf(stderr, "zastosowanie: ./gra *ip* *port* *nazwa uzytkownika*\n");
        exit(EXIT_FAILURE);
    }
    /* niechcemy aby ktos zrobil serwer na portach w stylu 80, 443 itp*/
    sprawdzajaca = atoi(argv[2]);
    if(sprawdzajaca < 1024 || sprawdzajaca > 65535){
		fprintf(stderr, "fajnie by bylo gdyby port byl z przedzialu [1024, 65535]\n");
        exit(EXIT_FAILURE);
	}

    sockfd_host = stworzenie_serwera(argv[1], argv[2], argv, argc);

    while (1){
		
		komunikacja(sockfd_host, argv, argc);
		
	}
    return 0;
}
