
#define KLAWIATURA 1
#define PLIK 2

#include <iostream>
#include <math.h>
#include <fstream>
#include <string>
using namespace std;

struct Indeks
{
	int klucz; 
	int strona;
};

struct Rekord
{
	int klucz; 
	double napiecie, natezenie;
	int wskaznik; 

	Rekord()
	{
		klucz = -1;
		napiecie = 0.0;
		natezenie = 0.0;
		wskaznik = -1;
	}
};


const char plikDane[] = { "dane.txt" }; 
const char plikIndeksy [] = { "indeksy.txt" }; 
const int rozmiarStrony = 5;      
const double alfa = 0.5; 
int rekordowPrimary = 0; 
int rekordowOverflow = 0; 
int stron = 1; 
const double wspolczynnikOverflow = 0.2;

void wyswietlIndeks();
void stworzIndeks(int &, int &);
int szukajIndeks(int , int &);
void sortuj(Rekord[], int );
void dodaj(double , double , int &, int &);
void wartosciPoczatkowe();
void odczytaj(int);
void wyswietlPlik();
void reorganizuj(int &, int &);
bool usun(int, int &, int &);
void aktualizuj(int, double, double);

int maxPrimary()
{
	return rozmiarStrony*stron;
}

int maxOverflow()
{
	return maxPrimary()*wspolczynnikOverflow;
}

int max()
{
	return maxPrimary() + maxOverflow();
}

void czyscBufor(Rekord *bufor, int size)
{
	for (int i = 0; i < size; i++)
	{
		bufor[i].klucz = -1;
		bufor[i].napiecie = 0.0;
		bufor[i].natezenie = 0.0;
		bufor[i].wskaznik = -1;
	}
}

void przepiszBufor(Rekord *buforNew, Rekord *bufor, int size)
{
	for (int i = 0; i < size; i++)
	{
		buforNew[i].klucz = bufor[i].klucz;
		buforNew[i].napiecie = bufor[i].napiecie;
		buforNew[i].natezenie = bufor[i].natezenie;
		buforNew[i].wskaznik = bufor[i].wskaznik;
	}
}

bool przetwarzajInstrukcje(char instrukcja, int wejscie, int klucz=0,double napiecie=0, double natezenie=0)
{
	int odczytow=0, zapisow=0;

	bool przetP = true;

	switch (instrukcja)
	{
	case 'd':
		if (wejscie == KLAWIATURA)
		{
			cout << "Podaj napiecie:\n";
			cin >> napiecie;
			cout << "Podaj natezenie:\n";
			cin >> natezenie;

			//int val[15]=wczytajRekord();
		}

		dodaj(napiecie, natezenie, odczytow, zapisow);
		cout << "DODANO REKORD\nOdczytow = " << odczytow << " Zapisow = " << zapisow << endl;
		break;

	case 'a':
		if (wejscie == KLAWIATURA)
		{
			cout << "Podaj klucz rekordu, ktory chcesz zmienic:\n";
			cin >> klucz;
			cout << "Podaj napiecie:\n";
			cin >> napiecie;
			cout << "Podaj natezenie:\n";
			cin >> natezenie;
		}

		aktualizuj(klucz, napiecie, natezenie);
		cout << "ZAKTUALIZOWANO REKORD\nOdczytow = " << odczytow << " Zapisow = " << zapisow << endl;
		break;

	case 'u':
		cout << "Podaj klucz:\n";
		if(wejscie==KLAWIATURA)
		cin >> klucz;
		cout << "USUWANIE REKORD\n";
		if (!usun(klucz, odczytow, zapisow))
			cout << "Nie ma rekordu z podanym kluczem\n";
		cout << "Odczytow = " << odczytow << " Zapisow = " << zapisow << endl;
		break;
	case 'o':
		cout << "Podaj klucz:\n";
		if(wejscie==KLAWIATURA)
			cin >> klucz;
		odczytaj(klucz);
		break;
	case 'r': 
		reorganizuj(odczytow, zapisow);
		cout << "ZREORGANIZOWANO\n";
		cout << "Odczytow = " << odczytow << " Zapisow = " << zapisow << endl;
		break;
	case 'p':
		wyswietlPlik();
		break;
	case 'i':
		wyswietlIndeks();
		break;
	case 'k':
		przetP = false;
		break;
	default:
		cout << "Nie wybrano ¿adnego dzia³ania\n";
		break;
	}

	return przetP;
}

void odczytaj(int klucz)
{
	int odczytow = 0, wskaznik, i;
	cout << "ODCZYTYWANIE\n";
	int strona = szukajIndeks(klucz, odczytow);
	Rekord *bufor = new Rekord[rozmiarStrony];
	FILE *plik = fopen(plikDane, "rb"); 
	fseek(plik, strona*sizeof(Rekord), SEEK_SET); 
	int elementow;
	elementow=fread(bufor, sizeof(Rekord), rozmiarStrony, plik); 
	odczytow++;

	for (i = 0; i < elementow; i++)
	{
		if (bufor[i].klucz < klucz)
			wskaznik = bufor[i].wskaznik;
		else
			break;
	}

	if (bufor[i].klucz == klucz)
		cout << bufor[i].klucz << " " << bufor[i].napiecie << " " << bufor[i].natezenie << endl;
	else if (wskaznik == -1)
		cout << "Nie ma rekordu o podanym kluczu\n";
	else
	{
		fseek(plik, maxPrimary()*sizeof(Rekord), SEEK_SET);
		while ((elementow=fread(bufor, sizeof(Rekord), rozmiarStrony, plik)) > 0)                //////////////// czy stronami czy maxOverflow lepiej????????????????
		{
			odczytow++;
			for (i = 0; i < elementow; i++)
			{
				if (bufor[i].klucz < klucz)
					wskaznik = bufor[i].wskaznik;
				else
					break;
			}
		}
		if (bufor[i].klucz == klucz)
			cout<<" over " << bufor[i].klucz << " " << bufor[i].napiecie << " " << bufor[i].natezenie << endl;
		else
			cout << "Nie ma rekordu o podanym kluczu\n";
	}
	fclose(plik); 
	delete [] bufor; 
	cout << "Odczytow = " << odczytow << endl;
}

void wartosciPoczatkowe()                                                         
{
	Rekord *bufor = new Rekord[rozmiarStrony]; 
	FILE *plik = fopen(plikDane, "wb"); 
	int odczytow = 0, zapisow = 0;

	int i = (int)ceil(((double)(max()))/(double)rozmiarStrony);
	while (i--)
	{
		fwrite(bufor, sizeof(Rekord), rozmiarStrony, plik);
		zapisow++;
	}
	delete [] bufor; 
	fclose(plik);
	stworzIndeks(odczytow, zapisow); 

	cout << "USTAWIENIE POCZATKOWYCH WARTOSCI\n";
	cout << "Odczytow = " << odczytow << " Zapisow = " << zapisow << endl;

	dodaj(0.5, 1.0, odczytow, zapisow);
}

void dodaj(double napiecie, double natezenie, int &odczytow, int &zapisow) 
{
	if (rekordowOverflow == maxOverflow() || rekordowPrimary == maxPrimary())
		reorganizuj(odczytow, zapisow);

	int klucz = (int)(napiecie/natezenie); 
	if (klucz < 0)
	{
		cout << "Nie mozna wstawic rekordu o kluczu mniejszym od 1\n";
		return;
	}
	else if (klucz == 0)
	{
		cout << "Rekord o podanym kluczu juz istnieje\n";
		return;
	}
	int wskaznik, i, j, strona; 
	double pomoc, zmianaIndeksow = false;

	Rekord *bufor = new Rekord[rozmiarStrony]; 
	Rekord *buforTmp = new Rekord[rozmiarStrony];
	int where;
	FILE *plikOdczytu = fopen(plikDane, "rb"); 
	FILE *plikZapisu = fopen("dane2", "wb"); 
	strona = szukajIndeks(klucz, odczytow); 
	int elementow;
	for (i = 0; i < strona; i++)
	{
		fread(bufor, sizeof(Rekord), rozmiarStrony, plikOdczytu);
		odczytow++;
		fwrite(bufor, sizeof(Rekord), rozmiarStrony, plikZapisu);
		zapisow++;
	}
	
	fread(bufor, sizeof(Rekord), rozmiarStrony, plikOdczytu);
	odczytow++;

	if (bufor[rozmiarStrony-1].klucz == -1)
	{
		for (i = 0; i < rozmiarStrony; i++)
		{
			if (bufor[i].klucz == klucz)
			{
				cout << "Rekord o podanym kluczu juz istnieje\n";
				fclose(plikOdczytu);
				fclose(plikZapisu);
				remove("dane2");
				delete [] bufor;
				return;
			}
			else if (bufor[i].klucz == -1)
			{
				if (i == 0) zmianaIndeksow = true;
				bufor[i].klucz = klucz;
				bufor[i].napiecie = napiecie;
				bufor[i].natezenie = natezenie;
				bufor[i].wskaznik = -1;
				rekordowPrimary++;
				sortuj(bufor, i);
				fwrite(bufor, sizeof(Rekord), rozmiarStrony, plikZapisu);
				zapisow++;

				for (i = strona+1; i < stron; i++)
				{
					fread(bufor, sizeof(Rekord), rozmiarStrony, plikOdczytu);
					odczytow++;
					fwrite(bufor, sizeof(Rekord), rozmiarStrony, plikZapisu);        ///////////////////////////////////////////////////////////////////////////////// zapisuja sie strony do "strona" ale czy dalej?
					zapisow++;
				}
				// przepisujemy overflow
				while ((elementow = fread(bufor, sizeof(Rekord), rozmiarStrony, plikOdczytu)) > 0)                //////////////// czy stronami czy maxOverflow lepiej????????????????
				{
					odczytow++;
					fwrite(bufor, sizeof(Rekord), elementow, plikZapisu);
					zapisow++;
				}

				delete [] bufor; 
				fclose(plikOdczytu); 
				fclose(plikZapisu);
				remove(plikDane); 
				rename("dane2", plikDane); 
				break; 
			}
		}
		if (zmianaIndeksow) stworzIndeks(odczytow, zapisow);
	}
	else     ////////////////////////////////////////////////////////////////////////////////  czy sorawdzamy zawsze czy dodawany klucz nie jest w overflow? sk¹d wiemy ¿e overflow //      mi sie wydaje ze to oznacza ze nie ma wolnego miejsca
	{
		for (i = 0; i < rozmiarStrony; i++)
		{
			if (bufor[i].klucz == klucz)
			{
				cout << "Rekord o podanym kluczu juz istnieje\n";
				fclose(plikOdczytu);
				fclose(plikZapisu);
				remove("dane2");
				delete [] bufor;
				return;
			}
			else if (bufor[i].klucz > klucz)
				break;
		}
		i--;
		wskaznik = bufor[i].wskaznik;
		if (bufor[i].wskaznik == -1 || bufor[i].wskaznik > klucz) 
			bufor[i].wskaznik = klucz;
		buforTmp = bufor;         // ogarnac przepisanie
		przepiszBufor(buforTmp, bufor, rozmiarStrony);
								  //     fwrite(bufor, sizeof(Rekord), rozmiarStrony, plikZapisu);  ////////////////// czy nie podwojne zapisywanie
	//	zapisow++;
		fseek(plikOdczytu, 0, SEEK_SET);
		for (i = 0; i < strona; i++)
		{
			fread(bufor, sizeof(Rekord), rozmiarStrony, plikOdczytu);
			odczytow++;
			fwrite(bufor, sizeof(Rekord), rozmiarStrony, plikZapisu);
			zapisow++;
		}

		fread(bufor, sizeof(Rekord), rozmiarStrony, plikOdczytu);
		odczytow++;
		fwrite(buforTmp, sizeof(Rekord), rozmiarStrony, plikZapisu);
		zapisow++;
		delete [] bufor;
		bufor = new Rekord[maxOverflow()];
		elementow=fread(bufor, sizeof(Rekord), maxOverflow(), plikOdczytu);
		odczytow++;

		for (j = 0; j < elementow; j++)
		{
			if (bufor[j].klucz == -1)
			{
				bufor[j].klucz = klucz;
				bufor[j].napiecie = napiecie;
				bufor[j].natezenie = natezenie;
				bufor[j].wskaznik = wskaznik;
				sortuj(bufor, j);
				break;
			}
			else if (bufor[j].klucz == wskaznik)
			{
				if (bufor[j].klucz < klucz)
				{
					if (bufor[j].wskaznik == -1)
					{
						bufor[j].wskaznik = klucz;
						wskaznik = -1;
					}
					else if (bufor[j].wskaznik > klucz)
					{
						wskaznik = bufor[j].wskaznik;
						bufor[j].wskaznik = klucz;
					}
					else 
					{
						wskaznik = bufor[j].wskaznik;
					}
				}
				else 
				{
					wskaznik = bufor[j].klucz;
				}
			}
		}
		rekordowOverflow++; 
		fwrite(bufor, sizeof(Rekord), maxOverflow(), plikZapisu); 
		zapisow++; 
		delete [] bufor; 
		delete[] buforTmp;
		fclose(plikOdczytu);
		fclose(plikZapisu);
		remove(plikDane); 
		rename("dane2", plikDane); 
	}
}

void sortuj(Rekord bufor[], int ile) 
{
	Rekord tmp;
	bool zamiana;

	for (int j = ile; j >= 0; j--)
	{
		zamiana = false;
		for (int i = 0; i < j; i++) 
		{
			if (bufor[i].klucz > bufor[i+1].klucz) 
			{
				zamiana = true;
				tmp = bufor[i];
				bufor[i] = bufor[i+1];
				bufor[i+1] = tmp;
			}
		}
		if (!zamiana) break; 
	}
}

int szukajIndeks(int klucz, int &odczytow) 
{
	Indeks *bufor = new Indeks[rozmiarStrony];
	FILE *plik = fopen(plikIndeksy, "rb"); 
	int i = stron, j, k, l, strona = -1;   

	while (i>0)
	{
		l = fread(bufor, sizeof(Indeks), rozmiarStrony, plik);
		i--;
		odczytow++;
		for (k = 0; k < l; k++)
		{
			if (bufor[k].klucz > klucz)
			{
				strona = bufor[k].strona-1;
				break;
			}
			else 
				strona = -1;
		}
		if (strona != -1)
			break; 
	}
	fclose(plik); 
	if (strona == -1) 
		strona = bufor[l-1].strona;                               // jesli primary area puste? o co chodzi?
	delete [] bufor; 
	return strona; 
}

void stworzIndeks(int &odczytow, int &zapisow) 
{
	int i = stron, j, k; 
	Rekord *bufor1 = new Rekord[rozmiarStrony]; 
	Indeks *bufor2 = new Indeks[rozmiarStrony]; 
	FILE *plik1 = fopen(plikDane, "rb"); 
	FILE *plik2 = fopen(plikIndeksy, "wb");
	
	for (j = 0, k = 0; j < i; j++, k++)
	{
		if (k == rozmiarStrony)
		{
			fwrite(bufor2, sizeof(Indeks), k, plik2);             
			zapisow++;
			k = 0;
		}
		fread(bufor1, sizeof(Rekord), rozmiarStrony, plik1);
		odczytow++;
		bufor2[k].klucz = bufor1[0].klucz;
		bufor2[k].strona = j;
	}
	fwrite(bufor2, sizeof(Indeks), k, plik2);
	zapisow++;

	fclose(plik1);
	fclose(plik2);
	delete [] bufor1;
	delete [] bufor2; 
}

void wyswietlIndeks() 
{
	int odczytow = 0, k, l; 
	Indeks *bufor = new Indeks[rozmiarStrony]; 
	FILE *plik = fopen(plikIndeksy, "rb");
	cout << "WYSWIETLENIE INDEKSU\n";
	while (true)
	{
		l = fread(bufor, sizeof(Indeks), rozmiarStrony, plik);
		odczytow++;
		if (l == 0) break;
		for (k = 0; k < l; k++)
			cout << bufor[k].klucz << " " << bufor[k].strona << endl;
	}
	fclose(plik); 
	delete [] bufor; 
	cout << "Odczytow = " << odczytow << endl; 
}

void wyswietlPlik()
{
	int odczytow = 0, i = stron, j, k, l = 0, wskaznik;      ///////////////// czy i jest poprawne zawsze? reorganizacja
	Rekord *buforPrimary = new Rekord[rozmiarStrony]; 
	Rekord *buforOverflow = new Rekord [maxOverflow()];
	FILE *plik = fopen(plikDane, "rb"); 
	
	fseek(plik, maxPrimary()*sizeof(Rekord), SEEK_SET);  
	int elementow=fread(buforOverflow, sizeof(Rekord), maxOverflow(), plik);
	odczytow++;

	fseek(plik, 0, SEEK_SET); 
	cout << "WYSWIETLENIE PLIKU\n";
	for(j = 0; j < i; j++)
	{
		fread(buforPrimary, sizeof(Rekord), rozmiarStrony, plik); 
		odczytow++;
		for (k = 0; k < rozmiarStrony; k++) 
		{
			cout << buforPrimary[k].klucz << " " << buforPrimary[k].napiecie << " " << buforPrimary[k].natezenie << endl;
			wskaznik = buforPrimary[k].wskaznik;
			l = 0;            ///////////////////////////////////  zerujemy l??????
			while (l < maxOverflow() && buforOverflow[l].klucz != -1) 
			{
				if (buforOverflow[l].klucz == wskaznik)
				{
					cout << "OVER" << buforOverflow[l].klucz << " " << buforOverflow[l].napiecie << " " << buforOverflow[l].natezenie << endl;
					wskaznik = buforOverflow[l].wskaznik;
				}
				l++;
			}
		}
	}

	fclose(plik);
	delete [] buforPrimary;
	delete [] buforOverflow;
	cout << "Odczytow = " << odczytow << endl;
}

void reorganizuj(int &odczytow, int &zapisow)
{
	if (maxOverflow() > rekordowOverflow)
	{
		stron++;
		return;
	}

	int licznik = 0, i = stron, j, k, l = 0, wskaznik, stron = 0; 
	Rekord *buforPrimary = new Rekord[rozmiarStrony]; 
	Rekord *buforOverflow = new Rekord[maxOverflow()]; 
	Rekord *buforZapisu = new Rekord[rozmiarStrony]; 
	FILE *plikOdczytu = fopen(plikDane, "rb");
	FILE *plikZapisu = fopen("dane2", "wb"); 

	fseek(plikOdczytu, maxPrimary()*sizeof(Rekord), SEEK_SET);
	fread(buforOverflow, sizeof(Rekord), maxOverflow(), plikOdczytu);
	odczytow++;
	fseek(plikOdczytu, 0, SEEK_SET);
	int elementow;
	for(j = 0; j < i; j++)
	{
		elementow=fread(buforPrimary, sizeof(Rekord), rozmiarStrony, plikOdczytu); 
		odczytow++;
		for (k = 0; k < elementow; k++) 
		{
			if (licznik == (int)(rozmiarStrony*alfa)) 
			{
				for (int m = 0; m < licznik; m++)  //////////////////////////////////////////////////////////////////// czy nie powinno sie zerowac od licznika do rozmiar strony?

					buforZapisu[m].wskaznik = -1;	

				fwrite(buforZapisu, sizeof(Rekord), rozmiarStrony, plikZapisu);
				czyscBufor(buforZapisu, rozmiarStrony);
				zapisow++;
				licznik = 0;
				stron++;
			}
			buforZapisu[licznik] = buforPrimary[k];
			licznik++;
			wskaznik = buforPrimary[k].wskaznik;
			l = 0;    ////////////////////////////////////////////???
			while (wskaznik != -1 && l < maxOverflow())
			{
				if (licznik == rozmiarStrony*alfa) 
				{
					for (int m = 0; m < licznik; m++)
						//////////////////////////////////////////////////////////the same^^^
						buforZapisu[m].wskaznik = -1;
					fwrite(buforZapisu, sizeof(Rekord), rozmiarStrony, plikZapisu);
					czyscBufor(buforZapisu, rozmiarStrony);
					zapisow++;
					licznik = 0;
					stron++;
				}
				buforZapisu[licznik] = buforOverflow[l]; 
				licznik++;
				wskaznik = buforOverflow[l].wskaznik;
				l++;
			}
		}
	}
	if (licznik > 0) 
	{
		for (int m = 0; m < licznik; m++)        //////////////////////////////////////////////////////////// the same^^^^^
			buforZapisu[m].wskaznik = -1;
		fwrite(buforZapisu, sizeof(Rekord), rozmiarStrony, plikZapisu);
		zapisow++;
		stron++;
	}
	delete [] buforPrimary;
	delete [] buforOverflow;
	delete [] buforZapisu;
	fclose(plikOdczytu);
	fclose(plikZapisu);

	remove(plikDane); 
	rename("dane2", plikDane);
	rekordowPrimary += rekordowOverflow;
	rekordowOverflow = 0;
	int stronN = ceil(rekordowPrimary / (int)(rozmiarStrony*alfa));
	stron = stronN;


	stworzIndeks(odczytow, zapisow); 
}

bool usun(int klucz, int &odczytow, int &zapisow)
{
	if (klucz == 0)
	{
		cout << "Nie mozna usunac rekordu z kluczem 0\n";
		return false;
	}
	int i, j, wskaznik, strona = szukajIndeks(klucz, odczytow); 
	bool usunieto = false;

	Rekord *buforPrimary = new Rekord[rozmiarStrony]; 
	Rekord *buforOverflow = new Rekord[rozmiarStrony];
	FILE *plikOdczytu = fopen(plikDane, "rb"); 
	FILE *plikZapisu = fopen("dane2", "wb"); 
	
	fseek(plikOdczytu, maxPrimary()*sizeof(Rekord), SEEK_SET);
	fread(buforOverflow, sizeof(Rekord), maxOverflow(), plikOdczytu);
	odczytow++;
	fseek(plikOdczytu, 0, SEEK_SET);

	for (i = 0; i < strona; i++)                                            ///////////////////////czy przepisuja sie wszystkie strony?  'strona' ni¿ej!!!!!!!!!!!
	{
		fread(buforPrimary, sizeof(Rekord), rozmiarStrony, plikOdczytu);
		odczytow++;
		fwrite(buforPrimary, sizeof(Rekord), rozmiarStrony, plikZapisu);
		zapisow++;
	}
	
	fread(buforPrimary, sizeof(Rekord), rozmiarStrony, plikOdczytu);
	odczytow++;
	wskaznik = -1;

	for (i = 0; i < rozmiarStrony; i++)
	{
		if (buforPrimary[i].klucz == klucz) 
		{
			if (buforPrimary[i].wskaznik == -1) 
			{
				for ( ; i < rozmiarStrony-1; i++)
					buforPrimary[i] = buforPrimary[i+1];
				buforPrimary[i].klucz = buforPrimary[i].wskaznik = -1;
				buforPrimary[i].napiecie = buforPrimary[i].natezenie = 0.0;
				usunieto = true;
				rekordowPrimary--;
			}
			else 
			{
				                                                                        //////////wskaznik=buforPrimary[i].wskaznik;/////////////////////////////
				for (j = 0; j < maxOverflow(); j++)
				{
					if (buforOverflow[j].klucz == wskaznik) 
					{
						buforPrimary[i] = buforOverflow[j];
						for (i++; i < rozmiarStrony; i++)
							if (buforPrimary[i].klucz == -1) break;
						sortuj(buforPrimary, i);
						for ( ; j < maxOverflow()-1; j++)
							buforOverflow[j] = buforOverflow[j+1];
						buforOverflow[j].klucz = buforOverflow[j].wskaznik = -1;
						buforOverflow[j].napiecie = buforOverflow[j].natezenie = 0.0;
						usunieto = true;
						rekordowOverflow--;
						break;
					}
				}
			}
			break;
		}
		else if (buforPrimary[i].klucz > klucz)
		{
			if (wskaznik != -1) 
			{
				j = 0;
				while (j < maxOverflow() && wskaznik != -1)
				{
					if (buforOverflow[j].klucz == klucz)
					{
						buforPrimary[i-1].wskaznik = buforOverflow[j].wskaznik;
						for ( ; j < maxOverflow()-1; j++)
							buforOverflow[j] = buforOverflow[j+1];
						buforOverflow[j].klucz = buforOverflow[j].wskaznik = -1;
						buforOverflow[j].napiecie = buforOverflow[j].natezenie = 0.0;
						usunieto = true;
						rekordowOverflow--;
					}
					else
						wskaznik = buforOverflow[j].wskaznik;
					j++;
				}
			}
		}
		wskaznik = buforPrimary[i].wskaznik;
	}

	fwrite(buforPrimary, sizeof(Rekord), rozmiarStrony, plikZapisu); 
	zapisow++;
		int elementow;
	for (i = strona+1; i < stron; i++)                          //////////////prawid³owa zapisywanie reszty!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	{
		elementow=fread(buforPrimary, sizeof(Rekord), rozmiarStrony, plikOdczytu);
		odczytow++;
		fwrite(buforPrimary, sizeof(Rekord), elementow, plikZapisu);
		zapisow++;
	}
	//rekordowPrimary--;                          //////////////////////////////////czy nie drugi raz zmniejszanie

	// przepisujemy overflow
	fwrite(buforOverflow, sizeof(Rekord), maxOverflow(), plikZapisu); 
	zapisow++; 
		
	delete [] buforPrimary; 	
	delete [] buforOverflow; 
	fclose(plikOdczytu); 
	fclose(plikZapisu); 
	remove(plikDane); 
	rename("dane2", plikDane); 

	return usunieto;
}

void aktualizuj(int klucz, double napiecie, double natezenie)
{
	cout << "AKTUALIZACJA\n";
	int odczytow = 0, zapisow = 0;
	if (usun(klucz, odczytow, zapisow)) 
		dodaj(napiecie, natezenie, odczytow, zapisow); 
	else 
		cout << "Nie ma rekordu z podanym kluczem\n";
	cout << "Odczytow = " << odczytow << " Zapisow = " << zapisow << endl;
}


int main()
{
	wartosciPoczatkowe();

	bool przet = true;
	char instrukcja;
	int napiecie, natezenie, klucz;
	cout << "Skad wczytywac dane?\nk - klawiatura\np - plik\n";
	cin >> instrukcja;
	if (instrukcja == 'k')
	{
		while (przet)
		{
			cout << "Co chcesz zrobic?\n" << "d - dodawanie rekordu\n" << "a - aktualizacja rekordu\n"
				<< "u - usuwanie rekordu\n" << "o - odczyt rekordu\n" << "r - reorganizacja\n"
				<< "p - przegladaj plik\n" << "i - przegladaj indeks\n" << "k - koniec\n";
			cin >> instrukcja;
			przet = przetwarzajInstrukcje(instrukcja, KLAWIATURA);
		}
	}
	else
	{
		string nazwa;
		char sciezkaDoPliku[80];
		cout << "Podaj sciezke do pliku:\n";
		cin >> nazwa;

		ifstream plik;
		plik.open(nazwa.c_str(), ios::in);

		while (przet)
		{
			plik >> instrukcja;
			if (instrukcja == 'd')
			{
				plik >> napiecie >> natezenie;
				przet = przetwarzajInstrukcje(instrukcja, PLIK, 0, napiecie, natezenie);
			}
			else if (instrukcja == 'a')
			{
				plik >> klucz >> napiecie >> natezenie;
				przet = przetwarzajInstrukcje(instrukcja, PLIK, klucz, napiecie, natezenie);
			}
			else if (instrukcja == 'u')
			{

				plik >> klucz;
				przet = przetwarzajInstrukcje(instrukcja, PLIK, klucz);
			}
			else if (instrukcja == 'o')
			{

				plik >> klucz;
				przet = przetwarzajInstrukcje(instrukcja, PLIK, klucz);
			}
			else
				przet = przetwarzajInstrukcje(instrukcja, PLIK);

		}
		plik.close();
	}
	system("pause");
	return 0;
}
