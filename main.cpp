#include <iostream>
#include <time.h>

#define PRZESUNIECIE_PLANSZY_DOL 2
#define PRZESUNIECIE_PLANSZY_PRAWO 3
#define WIERSZE_KONSOLI 25
#define KOLUMNY_KONSOLI 80
#define KOLKO_KRZYZYK "1"
#define PENTAGO "2"
#define ILE_UNDO 4
#define PRZEGLADANIE_GRY "1"
#define KONIEC_GRY "2"
#define LIMIT_POL 36

using namespace std;

struct ramkaPlanszy {
	char goraLewo;
	char goraPrawo;
	char dolLewo;
	char dolPrawo;
	char srodekGora;
	char srodekDol;
	char srodekLewo;
	char srodekPrawo;
	char srodek;
	char wiersz;
	char kolumna;
};

struct komendy {
	char komenda_1_0;
	char komenda_1_1;
	char komenda_2_0;
	char komenda_2_1;
};

bool czyWygrana(char tab[6][6], char znak);
bool sprawdzaniePoziomo(char tab[6][6], char znak);
bool sprawdzaniePionowo(char tab[6][6], char znak);
bool sprawdzanieUkosnie(char tab[6][6], char znak);
int ktoraKolumna(int liczba);
int ktoryWiersz(int liczba);
bool czyPoleZajete(char tab[6][6], int kolumna, int wiersz);
void wypelnijPlanszePredefiniowana(char tab[6][6], char znak_1, char znak_2);
void kopiowaniePlanszy(char plansza_do_gry[6][6], char plansza_do_skopiowania[6][6]);
void wypiszPlansze(char plansza_do_gry[6][6], ramkaPlanszy ramka);
void przesunPlanszeDol(int n);
void przesunPlanszePrawo(int n);
void zapisywanieRuchowUndo(char undoTablica[ILE_UNDO + 1][6][6], char plansza[6][6]);
void undo(char undoTablica[ILE_UNDO + 1][6][6], char plansza[6][6], int ile_ruchow_undo);

int main()
{
	ramkaPlanszy ramka;
	ramka.goraLewo = 0xC9;
	ramka.goraPrawo = 0xBB;
	ramka.dolLewo = 0xC8;
	ramka.dolPrawo = 0xBC;
	ramka.srodek = 0xCE;
	ramka.srodekGora = 0xCB;
	ramka.srodekDol = 0xCA;
	ramka.srodekLewo = 0xCC;
	ramka.srodekPrawo = 0xB9;
	ramka.kolumna = 0xBA;
	ramka.wiersz = 0xCD;

	string imie_1, imie_2;

	char znak_1, znak_2, znak_pomocniczy;
	char znak_wpisywany[2];
	char plansza[6][6];
	char plansza_predefiniowana[6][6];
	char plansze_undo[ILE_UNDO + 1][6][6];
	char plansze_przeglad_rozgrywki[38][6][6];
	komendy ruchy[37];

	int kolejka = 1;
	int wypisywanie = 0;

	string rozgrywka = "rozgrywka";
	string tryb_gry = "tryb gry";

	int gracze[2] = { 0, 1 };
	int gracz;
	char cwiartka;
	char pole;
	char obrot;
	int pole_liczba;
	int przesuniecie_wiersza;
	int przesuniecie_kolumny;
	int wiersz;
	int kolumna;
	int liczba_zajetych_pol = 0;
	int przerwa_pauza = 0;
	int przerwa_opis_programu = 0;
	int przerwa_komenda = 0;
	int ile_ruchow_undo = 0;
	int warunek_komenda_2 = 0;
	int powtarzanie_komendy = 0;

	int limit_czasu_wpisywany;
	clock_t czas_gracza;
	clock_t limit_czasu;
	clock_t limit_czasu_1;
	clock_t limit_czasu_2;
	clock_t czas;
	clock_t start;
	clock_t stop;
	clock_t pauza_start = 0;
	clock_t pauza_stop = 0;
	clock_t pauza_czas = 0;

	bool status_pola;
	int wiersze_konsoli = WIERSZE_KONSOLI;
	int kolumny_konsoli = KOLUMNY_KONSOLI;

	bool status_gry_1 = false;
	bool status_gry_2 = false;
	bool remis = false;

	//---Obsluga programu---//
	char komenda_1[3];
	char komenda_2[3];
	char komenda_3[2];

	//---Bazowe wypelnianie plansz---//
	for (int i = 0; i < 6; ++i) {
		for (int j = 0; j < 6; ++j) {
			plansza[i][j] = ' ';
			plansza_predefiniowana[i][j] = ' ';
			plansze_przeglad_rozgrywki[0][i][j] = ' ';
		}
	}

	for (int i = 0; i < ILE_UNDO; ++i) {
		for (int j = 0; j < 6; ++j) {
			for (int k = 0; k < 6; ++k) {
				plansze_undo[i][j][k] = '?';
			}
		}
	}

	cout << "Imie gracza 1: ";
	cin >> imie_1;
	cout << "Zeton gracza 1: ";
	cin >> znak_wpisywany;
	while (znak_wpisywany[1] != '\0') {
		cout << "Wpisz poprawny znak: ";
		cin >> znak_wpisywany;
	}
	znak_1 = znak_wpisywany[0];
	cout << endl;

	cout << "Imie gracza 2: ";
	cin >> imie_2;
	cout << "Zeton gracza 2: ";
	cin >> znak_wpisywany;
	while (znak_wpisywany[1] != '\0' || znak_wpisywany[0] == znak_1) {
		cout << "Wpisz poprawny znak: ";
		cin >> znak_wpisywany;
	}
	znak_2 = znak_wpisywany[0];
	cout << endl;

	if (imie_1 == imie_2) {
		imie_1 += "_1";
		imie_2 += "_2";
	}

	cout << "Podaj limit czasu (w sekundach): ";
	cin >> limit_czasu_wpisywany;
	while (cin.fail() || limit_czasu_wpisywany <= 0) {
		cout << "Podaj limit czasu (w sekundach): ";
		cin.clear();
		cin.ignore(256, '\n');
		cin >> limit_czasu_wpisywany;
	}
	cout << endl;

	//---Limity czasu dla danego gracza---///
	limit_czasu = limit_czasu_wpisywany * CLOCKS_PER_SEC;
	limit_czasu_1 = limit_czasu;
	limit_czasu_2 = limit_czasu;

	while (tryb_gry != KOLKO_KRZYZYK && tryb_gry != PENTAGO) {
		cout << "Jezeli chcesz zagrac w Kolko i Krzyzyk, wpisz 1" << endl;
		cout << "Jezeli chcesz zagrac w Pentago, wpisz 2" << endl;
		cin >> tryb_gry;
	}

	cout << endl;

	if (tryb_gry == KOLKO_KRZYZYK) cout << "Gra w Kolko i Krzyzyk" << endl;
	if (tryb_gry == PENTAGO) cout << "Gra w Pentago" << endl;

	//-----Gra-----//

	while (!(status_gry_1) && !(status_gry_2) && !(remis))
	{
		przerwa_komenda = 0;
		gracz = gracze[kolejka % 2];

		if (wypisywanie == 0) {
			wypiszPlansze(plansza, ramka);
			cout << endl << endl;
		}

		if (wypisywanie == 0) {
			cout << "Kolejka " << kolejka << " - ";
			if (gracz == 1) cout << imie_1 << endl;
			if (gracz == 0) cout << imie_2 << endl;
		}

		if (wypisywanie == 0) {
			cout << "Pozostaly czas - ";
			if (gracz == 1) {
				czas_gracza = limit_czasu_1 / CLOCKS_PER_SEC;
				if (czas_gracza % 60 < 10)
					cout << czas_gracza / 60 << ":0" << czas_gracza % 60 << endl;
				else cout << czas_gracza / 60 << ":" << czas_gracza % 60 << endl;
			}

			if (gracz == 0) {
				czas_gracza = limit_czasu_2 / CLOCKS_PER_SEC;
				if (czas_gracza % 60 < 10)
					cout << czas_gracza / 60 << ":0" << czas_gracza % 60 << endl;
				else cout << czas_gracza / 60 << ":" << czas_gracza % 60 << endl;
			}
		}

		start = clock();

		cout << "Wpisz komende: ";
		cin >> komenda_1;
		if (tryb_gry == PENTAGO) cin >> komenda_2;
		warunek_komenda_2 = 0;


		switch (komenda_1[0]) {
		case 'q':
		case 'w':
		case 'a':
		case 's':
		{
			cwiartka = komenda_1[0];
			if (komenda_1[1] >= '0' && komenda_1[1] <= '9') {
				pole = komenda_1[1];
				pole_liczba = (int)(pole - '0');
				przerwa_komenda = 1;
			}
			else {
				cout << "Zla intrukcja, prosze o podanie poprawnej." << endl;
				wypisywanie = 1;
				break;
			}

			//---Odwolywanie sie do poszczegolnej cwiartki---//
			if (pole > '0' && pole < '10' && komenda_1[2] == '\0') {
				if (cwiartka == 'q') {
					przesuniecie_wiersza = 0;
					przesuniecie_kolumny = 0;
				}
				if (cwiartka == 'w') {
					przesuniecie_wiersza = 0;
					przesuniecie_kolumny = 3;
				}
				if (cwiartka == 'a') {
					przesuniecie_wiersza = 3;
					przesuniecie_kolumny = 0;
				}
				if (cwiartka == 's') {
					przesuniecie_wiersza = 3;
					przesuniecie_kolumny = 3;
				}

				wiersz = ktoryWiersz(pole_liczba) + przesuniecie_wiersza;
				kolumna = ktoraKolumna(pole_liczba) + przesuniecie_kolumny;
				status_pola = czyPoleZajete(plansza, kolumna, wiersz);

				if (status_pola) {
					cout << "To pole jest zajete, prosze wybrac inne." << endl;
					wypisywanie = 1;
					break;
				}

				else {
					zapisywanieRuchowUndo(plansze_undo, plansza);

					//---Wstawianie znaku do planszy---//
					if (tryb_gry == KOLKO_KRZYZYK) {
						if (gracz == 1) plansza[wiersz][kolumna] = znak_1;
						if (gracz == 0) plansza[wiersz][kolumna] = znak_2;

						stop = clock();
					}

					//---Obracanie planszy---//
					if (tryb_gry == PENTAGO && komenda_1[2] == '\0') {
						int warunek_petli = 0;
						while (warunek_petli == 0) {
							if (warunek_komenda_2 != 0) cin >> komenda_2;
							switch (komenda_2[0]) {
							case 'q':
							case 'w':
							case 'a':
							case 's': {
								cwiartka = komenda_2[0];
								obrot = komenda_2[1];

								if (cwiartka == 'q') {
									przesuniecie_wiersza = 0;
									przesuniecie_kolumny = 0;
								}
								if (cwiartka == 'w') {
									przesuniecie_wiersza = 0;
									przesuniecie_kolumny = 3;
								}
								if (cwiartka == 'a') {
									przesuniecie_wiersza = 3;
									przesuniecie_kolumny = 0;
								}
								if (cwiartka == 's') {
									przesuniecie_wiersza = 3;
									przesuniecie_kolumny = 3;
								}
								int tablica_pomocnicza[3][3];
								if (obrot == 'z' || obrot == 'x') {
									if (gracz == 1) plansza[wiersz][kolumna] = znak_1;
									if (gracz == 0) plansza[wiersz][kolumna] = znak_2;

									//---Przypisywanie wartosci do planszy pomocniczej---//
									for (int i = 0; i < 3; ++i) {
										for (int j = 0; j < 3; ++j) {
											tablica_pomocnicza[i][j] = plansza[i + przesuniecie_wiersza][j + przesuniecie_kolumny];
										}
									}

									//---Transpozycja---//
									for (int i = 0; i < 3; ++i) {
										for (int j = i + 1; j < 3; ++j) {
											swap(tablica_pomocnicza[i][j], tablica_pomocnicza[j][i]);
										}
									}

									if (obrot == 'z') {
										for (int i = 0; i < 3; ++i) {
											swap(tablica_pomocnicza[i][0], tablica_pomocnicza[i][2]);
											warunek_petli = 1;
											warunek_komenda_2 = 0;
											powtarzanie_komendy = 0;

											stop = clock();
										}
									}

									else if (obrot == 'x') {
										for (int i = 0; i < 3; ++i) {
											swap(tablica_pomocnicza[0][i], tablica_pomocnicza[2][i]);
											warunek_petli = 1;
											warunek_komenda_2 = 0;
											powtarzanie_komendy = 0;

											stop = clock();
										}
									}
								}
								else {
									cout << "Zla intrukcja, prosze o podanie poprawnej." << endl;
									powtarzanie_komendy = 1;
									warunek_komenda_2 = 1;
									warunek_petli = 1;
									wypisywanie = 1;
									break;
								}

								//---Powrot do planszy docelowej---//
								for (int i = 0; i < 3; ++i) {
									for (int j = 0; j < 3; ++j) {
										plansza[i + przesuniecie_wiersza][j + przesuniecie_kolumny] = tablica_pomocnicza[i][j];
									}
								}
								break;
							}
							default: {
								cout << "Zla intrukcja, prosze o podanie poprawnej." << endl;
								powtarzanie_komendy = 1;
								wypisywanie = 1;
								warunek_petli = 1;
								break;
							}
							}
						}
					}
					status_gry_1 = czyWygrana(plansza, znak_1);
					status_gry_2 = czyWygrana(plansza, znak_2);

					czas = (stop - start);

					if (gracz == 1) {
						limit_czasu_1 -= czas;
						limit_czasu_1 += pauza_czas;
						pauza_czas = 0;
						pauza_start = 0;
						pauza_stop = 0;
						if (limit_czasu_1 <= 0) {
							cout << "Czas gracza 1 minal, wygrywa gracz 2!";
							return 0;
						}
					}

					if (gracz == 0) {
						limit_czasu_2 -= czas;
						limit_czasu_1 += pauza_czas;
						pauza_czas = 0;
						pauza_start = 0;
						pauza_stop = 0;
						if (limit_czasu_2 <= 0) {
							cout << "Czas gracza 2 minal, wygrywa gracz 1!";
							return 0;
						}

					}

					//---Przypisywanie planszy do tablicy z przegladem rozgrywki---//
					if (powtarzanie_komendy != 1) {
						wypisywanie = 0;

						ruchy[kolejka].komenda_1_0 = komenda_1[0];
						ruchy[kolejka].komenda_1_1 = komenda_1[1];
						if (tryb_gry == PENTAGO) {
							ruchy[kolejka].komenda_2_0 = komenda_2[0];
							ruchy[kolejka].komenda_2_1 = komenda_2[1];
						}

						for (int i = 0; i < 6; ++i) {
							for (int j = 0; j < 6; ++j) {
								plansze_przeglad_rozgrywki[kolejka][i][j] = plansza[i][j];
							}
						}

						++kolejka;

						if (kolejka == 37) {
							wypiszPlansze(plansza, ramka);
							cout << endl;
							cout << "Nikt nie wygral!";
							remis = true;
						}
					}
				}
			}
			else cout << "Zostalo podane zle pole, prosze wybrac inne." << endl;
			break;
		}
		case 'p': {
			pauza_start = clock();
			wypisywanie = 1;
			string margines(57, '-');
			for (int i = 0; i < wiersze_konsoli + 6; ++i) {
				if (i == 16) cout << margines << "PAUZA" << margines << endl;
				cout << endl;
			}
			przerwa_pauza = 1;
			string komenda_pauza;
			while (przerwa_pauza == 1) {
				cin >> komenda_pauza;
				if (komenda_pauza == "p") przerwa_pauza = 0;
			}
			wypisywanie = 0;
			pauza_stop = clock();
			pauza_czas = pauza_start - pauza_stop;

			break;
		}
		case 'u': {
			cout << "Ile ruchow chcesz cofnac?" << endl;
			cin >> ile_ruchow_undo;
			while (cin.fail() || ile_ruchow_undo < 0) {
				cout << "Ile ruchow chcesz cofnac?" << endl;
				cin.clear();
				cin.ignore(256, '\n');
				cin >> ile_ruchow_undo;
			}
			while (ile_ruchow_undo > ILE_UNDO || ile_ruchow_undo >= kolejka || ile_ruchow_undo <= 0) {
				cout << "Brak pamieci, podaj poprawna wartosc." << endl;
				cin >> ile_ruchow_undo;
			}
			undo(plansze_undo, plansza, ile_ruchow_undo - 1);
			if (kolejka - ile_ruchow_undo > 0) kolejka = kolejka - ile_ruchow_undo;
			break;
		}
		case 'o':
		{
			wypelnijPlanszePredefiniowana(plansza_predefiniowana, znak_1, znak_2);
			kopiowaniePlanszy(plansza, plansza_predefiniowana);
			kolejka = 17;
			break;
		}
		case 'm': {
			cout << "Podaj nowy zeton: ";
			cin >> znak_wpisywany;
			if (gracz == 1) znak_pomocniczy = znak_1;
			if (gracz == 0) znak_pomocniczy = znak_2;
			while (znak_wpisywany[1] != '\0' || znak_wpisywany[0] == znak_pomocniczy) {
				cout << "Wpisz poprawny znak: ";
				cin >> znak_wpisywany;
			}
			for (int i = 0; i < 6; ++i) {
				for (int j = 0; j < 6; ++j) {
					if (plansza[i][j] == znak_pomocniczy) plansza[i][j] = znak_wpisywany[0];
				}
			}
			if (gracz == 1) znak_1 = znak_wpisywany[0];
			if (gracz == 0) znak_2 = znak_wpisywany[0];
			break;
		}
		case 'h': {
			cout << endl;
			cout << "Autor - Aleksandra Boruczkowska" << endl << endl;
			cout << "q,w,a,s - wybor czesci planszy odpowiednio: lewej gornej, prawej gornej, lewej dolnej, prawej dolnej" << endl;
			cout << "1..9 - wybor pola na czesci planszy jak na klawiaturze numerycznej tj. 1 to dolne lewe pole" << endl;
			cout << "z, x - obrot odpowiednio : zgodnie z ruchem wskazowek zegara, odwrotnie do ruchu wskazowek" << endl;
			cout << "p - pauza wejscie / wyjscie" << endl;
			cout << "u - undo" << endl;
			cout << "o - wczytanie predefiniowanej planszy" << endl;
			cout << "m - wejscie do opcji" << endl;
			cout << "h - opis programu wejscie / wyjscie" << endl << endl;
			cout << "Ruch to najpierw wybor czesci planszy i pola(np.q5), a nastepnie wybor czesci planszy i obrotu (np.az)." << endl;
			przerwa_opis_programu = 1;
			string komenda_opis_programu;
			while (przerwa_opis_programu == 1) {
				cin >> komenda_opis_programu;
				if (komenda_opis_programu == "h") przerwa_opis_programu = 0;
			}
			break;
		}
		default: {
			cout << "Zla intrukcja, prosze o podanie poprawnej." << endl;
			wypisywanie = 1;
		}
		}

	}
	if (!(remis)) wypiszPlansze(plansza, ramka);

	cout << endl << endl;

	if (status_gry_1 && status_gry_2) {
		cout << "Remis.";
	}
	else if (status_gry_1) cout << "Brawo, " << imie_1 << " wygrales/as!";
	else if (status_gry_2) cout << "Brawo, " << imie_2 << " wygrales/as!";

	while (rozgrywka != PRZEGLADANIE_GRY && rozgrywka != KONIEC_GRY) {
		cout << endl << endl;
		cout << "Jesli chcesz przejrzec rozgrywke, wpisz 1." << endl;
		cout << "Jesli chcesz zakonczyc gre, wpisz 2." << endl;
		cin >> rozgrywka;
	}

	cout << endl;

	//---Przegladanie rozgrywki---//
	if (rozgrywka == KONIEC_GRY) {
		return 0;
	}
	if (rozgrywka == PRZEGLADANIE_GRY) {
		char  plansza_pomocnicza[6][6];
		int index = 0;
		bool czy_wypisywac = true;

		for (int i = 0; i < 6; ++i) {
			for (int j = 0; j < 6; ++j) {
				plansza_pomocnicza[i][j] = plansze_przeglad_rozgrywki[index][i][j];
			}
		}
		wypiszPlansze(plansza_pomocnicza, ramka);
		cout << endl << endl;

		while (czy_wypisywac && index <= LIMIT_POL) {
			cout << "Aby przejsc do kolejnego ruchu, wpisz n." << endl;
			cout << "Aby przejsc do poprzedniego ruchu, wpisz b." << endl;
			cout << "Aby zakonczyc przegladanie, wpisz m." << endl;
			cin >> komenda_3;
			while (!(komenda_3[0] == 'n' || komenda_3[0] == 'b' || komenda_3[0] == 'm') || komenda_3[1] != '\0') {
				cout << "Zla komenda, prosze o podanie poprawnej. ";
				cin >> komenda_3;
			}

			if (komenda_3[0] == 'n') {
				if (index < kolejka - 1) {
					++index;
					for (int i = 0; i < 6; ++i) {
						for (int j = 0; j < 6; ++j) {
							plansza_pomocnicza[i][j] = plansze_przeglad_rozgrywki[index][i][j];
						}
					}
					gracz = gracze[index % 2];
					if (gracz == 1) {
						cout << "Gracz: " << imie_1 << endl;
						cout << "Zeton: " << znak_1 << endl;
					}

					if (gracz == 0) {
						cout << "Gracz: " << imie_2 << endl;
						cout << "Zeton: " << znak_2 << endl;
					}

					cout << "Ruch: " << ruchy[index].komenda_1_0 << ruchy[index].komenda_1_1;
					if (tryb_gry == PENTAGO) {
						cout << " " << ruchy[index].komenda_2_0 << ruchy[index].komenda_2_1;
						cout << endl;
					}

					wypiszPlansze(plansza_pomocnicza, ramka);
					cout << endl << endl;
				}
				else cout << "Gra zakonczona, brak nastepnych ruchow." << endl;
			}

			if (komenda_3[0] == 'b') {
				if (index >= 1 && index < kolejka) {
					--index;
					for (int i = 0; i < 6; ++i) {
						for (int j = 0; j < 6; ++j) {
							plansza_pomocnicza[i][j] = plansze_przeglad_rozgrywki[index][i][j];
						}
					}
					gracz = gracze[index % 2];
					if (gracz == 1) {
						cout << "Gracz: " << imie_1 << endl;
						cout << "Zeton: " << znak_1 << endl;
					}

					if (gracz == 0) {
						cout << "Gracz: " << imie_2 << endl;
						cout << "Zeton: " << znak_2 << endl;
					}

					cout << "Ruch: " << ruchy[index].komenda_1_0 << ruchy[index].komenda_1_1;
					if (tryb_gry == PENTAGO) {
						cout << " " << ruchy[index].komenda_2_0 << ruchy[index].komenda_2_1;
						cout << endl;
					}

					wypiszPlansze(plansza_pomocnicza, ramka);
					cout << endl << endl;
				}
				else cout << "Nie mozesz sie cofnac." << endl;
			}

			if (komenda_3[0] == 'm') {
				return 0;
			}
		}
	}
}

bool czyWygrana(char tab[6][6], char znak) {
	bool poziomo = sprawdzaniePoziomo(tab, znak);
	bool pionowo = sprawdzaniePionowo(tab, znak);
	bool ukosnie = sprawdzanieUkosnie(tab, znak);
	if (poziomo || pionowo || ukosnie) return 1;
	else return 0;
}

bool sprawdzaniePoziomo(char tab[6][6], char znak) {
	int czy5 = 0;
	for (int k = 0; k < 2; ++k) {
		for (int i = k; i < 5 + k; ++i) {
			czy5 = 0;
			for (int j = 0; j < 5; ++j) {
				if (tab[i][j] == znak) ++czy5;
			}
			if (czy5 == 5) return true;
		}
	}
	for (int k = 0; k < 2; ++k) {
		for (int i = k; i < 5 + k; ++i) {
			czy5 = 0;
			for (int j = 1; j < 6; ++j) {
				if (tab[i][j] == znak) ++czy5;
			}
			if (czy5 == 5) return true;
		}
	}
	return false;
}

bool sprawdzaniePionowo(char tab[6][6], char znak) {
	int czy5 = 0;
	for (int k = 0; k < 2; ++k) {
		for (int j = 0; j < 5; ++j) {
			czy5 = 0;
			for (int i = k; i < 5 + k; ++i) {
				if (tab[i][j] == znak) ++czy5;
			}
			if (czy5 == 5) return true;
		}
	}
	for (int k = 0; k < 2; ++k) {
		for (int j = 1; j < 6; ++j) {
			czy5 = 0;
			for (int i = k; i < 5 + k; ++i) {
				if (tab[i][j] == znak) ++czy5;
			}
			if (czy5 == 5) return true;
		}
	}
	return false;
}

bool sprawdzanieUkosnie(char tab[6][6], char znak) {
	int czy5 = 0;
	for (int k = 0; k < 2; ++k) {
		czy5 = 0;
		for (int i = k; i < 5 + k; ++i) {
			if (tab[i][i] == znak) ++czy5;
		}
		if (czy5 == 5) return 1;
	}

	czy5 = 0;
	for (int i = 0; i < 5; ++i) {
		if (tab[i][i + 1] == znak) ++czy5;
	}
	if (czy5 == 5) return true;

	czy5 = 0;
	for (int i = 1; i < 6; ++i) {
		if (tab[i][i - 1] == znak) ++czy5;
	}
	if (czy5 == 5) return true;

	for (int k = 0; k < 2; ++k) {
		czy5 = 0;
		for (int i = 5 - k; i > 0 - k; --i) {
			czy5 = 0;
			for (int j = 0 + k; j < 5 + k; ++j) {
				if (tab[i][j] == znak) ++czy5;
			}
			if (czy5 == 5) return 1;
		}
	}

	czy5 = 0;
	int x = 0;
	for (int i = 4; i >= 0; --i) {
		if (tab[i][x] == znak) ++czy5;
		++x;
	}
	if (czy5 == 5) return true;

	czy5 = 0;
	x = 1;
	for (int i = 5; i > 0; --i) {
		if (tab[i][x] == znak) ++czy5;
		++x;
	}
	if (czy5 == 5) return true;
	return false;
}

int ktoraKolumna(int liczba) {
	int kolumna = liczba % 3 - 1;
	if (kolumna == -1) kolumna = 2;
	return kolumna;
}

int ktoryWiersz(int liczba) {
	int wiersz = 2 - (liczba / 3);
	if (liczba % 3 == 0) wiersz = 3 - (liczba / 3);
	return wiersz;
}

bool czyPoleZajete(char tab[6][6], int kolumna, int wiersz) {
	if (tab[wiersz][kolumna] == ' ')
		return false;
	else
		return true;
}

void wypelnijPlanszePredefiniowana(char tab[6][6], char znak_1, char znak_2) {
	tab[0][1] = znak_2;
	tab[0][4] = znak_1;
	tab[1][0] = znak_1;
	tab[1][1] = znak_1;
	tab[1][2] = znak_2;
	tab[1][3] = znak_1;
	tab[1][4] = znak_2;
	tab[2][1] = znak_1;
	tab[3][0] = znak_1;
	tab[3][1] = znak_1;
	tab[4][0] = znak_2;
	tab[4][2] = znak_1;
	tab[4][4] = znak_2;
	tab[5][1] = znak_2;
	tab[5][4] = znak_2;
	tab[5][5] = znak_2;
}

void kopiowaniePlanszy(char plansza_do_gry[6][6], char plansza_do_skopiowania[6][6]) {
	for (int i = 0; i < 6; ++i) {
		for (int j = 0; j < 6; ++j) {
			plansza_do_gry[i][j] = plansza_do_skopiowania[i][j];
		}
	}
}

void wypiszPlansze(char plansza_do_gry[6][6], ramkaPlanszy ramka) {
	przesunPlanszeDol(PRZESUNIECIE_PLANSZY_DOL);
	przesunPlanszePrawo(PRZESUNIECIE_PLANSZY_PRAWO);
	for (int i = 0; i < 9; ++i) {
		if (i == 0) cout << ramka.goraLewo;
		else if (i == 8) cout << ramka.goraPrawo;
		else if (i == 4) cout << ramka.wiersz << ramka.srodekGora << ramka.wiersz;
		else cout << ramka.wiersz << ramka.wiersz;
	}
	cout << endl;

	for (int i = 0; i < 6; ++i) {
		przesunPlanszePrawo(PRZESUNIECIE_PLANSZY_PRAWO);
		if (i == 3) {
			cout << ramka.srodekLewo;
			for (int k = 0; k < 6; ++k) {
				if (k == 3) cout << ramka.wiersz << ramka.srodek << ramka.wiersz;
				cout << ramka.wiersz << ramka.wiersz;
			}
			cout << ramka.srodekPrawo;
			cout << endl;
			przesunPlanszePrawo(PRZESUNIECIE_PLANSZY_PRAWO);
		}
		cout << ramka.kolumna << " ";
		for (int j = 0; j < 6; ++j) {
			if (j == 3) {
				cout << ramka.kolumna << " ";
			}
			cout << plansza_do_gry[i][j] << " ";
		}
		cout << ramka.kolumna;
		cout << endl;
	}

	przesunPlanszePrawo(PRZESUNIECIE_PLANSZY_PRAWO);
	for (int i = 0; i < 9; ++i) {
		if (i == 0) cout << ramka.dolLewo;
		else if (i == 8) cout << ramka.dolPrawo;
		else if (i == 4) cout << ramka.wiersz << ramka.srodekDol << ramka.wiersz;
		else cout << ramka.wiersz << ramka.wiersz;
	}

}

void przesunPlanszeDol(int n) {
	for (int i = 0; i < n; ++i) {
		cout << endl;
	}
}

void przesunPlanszePrawo(int n) {
	for (int i = 0; i < n; ++i) {
		cout << " ";
	}
}

void zapisywanieRuchowUndo(char undoTablica[ILE_UNDO + 1][6][6], char plansza[6][6]) {
	for (int i = ILE_UNDO; i > 0; --i) {
		for (int j = 0; j < 6; ++j) {
			for (int k = 0; k < 6; ++k) {
				undoTablica[i][j][k] = undoTablica[i - 1][j][k];
			}
		}
	}

	for (int i = 0; i < 6; ++i) {
		for (int j = 0; j < 6; ++j) {
			undoTablica[0][i][j] = plansza[i][j];
		}
	}
}

void undo(char undoTablica[ILE_UNDO + 1][6][6], char plansza[6][6], int ile_ruchow_undo) {
	if (undoTablica[ile_ruchow_undo][0][0] != '?') {
		for (int i = 0; i < 6; ++i) {
			for (int j = 0; j < 6; ++j) {
				plansza[i][j] = undoTablica[ile_ruchow_undo][i][j];
				undoTablica[0][i][j] = undoTablica[ile_ruchow_undo][i][j];
			}
		}
		int i;
		for (i = 1; i + ile_ruchow_undo <= ILE_UNDO; ++i) {
			for (int j = 0; j < 6; ++j) {
				for (int k = 0; k < 6; ++k) {
					undoTablica[i][j][k] = undoTablica[i + ile_ruchow_undo][j][k];
				}
			}
		}
		for (int m = i; m <= ILE_UNDO; ++m) {
			for (int j = 0; j < 6; ++j) {
				for (int k = 0; k < 6; ++k) {
					undoTablica[m][j][k] = '?';
				}
			}
		}
	}
	else cout << "Brak pamieci.";
}
