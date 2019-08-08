#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <stdio.h>
#include <fstream>
#include <time.h>


#define NR_ORASE 127
#define distanta_MAX 100000
#define NR_TURURI (NR_ORASE * distanta_MAX)
#define NR_FURNICI 127

using namespace std;


struct structuraOras {
	int x, y;
};

struct structuraFurnici {

	int orasCurent, orasUrmacatrer, indexDrum;
	int tabu[NR_ORASE];
	int drum[NR_ORASE];
	double lungimeTur;
};


#define ALFA 1.0
#define BETA  5.0 
#define RHO 0.5 
#define QVAL 100
#define NR_MAXTUR 20
#define MAX_TIME (NR_MAXTUR * NR_ORASE)
#define INIT_PHER (1.0/NR_ORASE)
#define RANK_W  NR_FURNICI/2.0



structuraOras orase[NR_ORASE];
structuraFurnici furnici[NR_FURNICI];
structuraFurnici furniciRang[NR_FURNICI];

double distanta[NR_ORASE][NR_ORASE];

double feromon[NR_ORASE][NR_ORASE];

double costMin = (double)NR_TURURI;
int furnicaMin;



void initializare()
{
	int din, catre, f;

	ifstream f1;

	f1.open("bier127.txt");

	
	for (din = 0; din < NR_ORASE; din++)
	{
		

		f1 >> orase[din].x;
		f1 >> orase[din].y;

		
		
		for (catre = 0; catre < NR_ORASE; catre++)
		{
			distanta[din][catre] = 0.0;
			feromon[din][catre] = INIT_PHER;
		}
	}


	for (din = 0; din < NR_ORASE; din++)
	{
		for (catre = 0; catre < NR_ORASE; catre++)
		{
			if (catre != din && distanta[din][catre] == 0.0)
			{
				int xd = pow(abs(orase[din].x - orase[catre].x), 2);
				int yd = pow(abs(orase[din].y - orase[catre].y), 2);

				distanta[din][catre] = (sqrt(xd + yd));
				distanta[catre][din] = distanta[din][catre];

			}
		}
	}


	

	catre = 0;
	for (f = 0; f < NR_FURNICI; f++)
	{
		if (catre == NR_ORASE)
			catre = 0;

		furnici[f].orasCurent = catre++;

		for (din = 0; din < NR_ORASE; din++)
		{
			furnici[f].tabu[din] = 0;
			furnici[f].drum[din] = -1;
		}

		furnici[f].indexDrum = 1;
		furnici[f].drum[0] = furnici[f].orasCurent;
		furnici[f].orasUrmacatrer = -1;
		furnici[f].lungimeTur = 0;

	    furnici[f].tabu[furnici[f].orasCurent] = 1;

	}
}

void repornireFurnici()
{
	int f, i, catre = 0;

	for (f = 0; f < NR_FURNICI; f++)
	{
		if (furnici[f].lungimeTur < costMin)
		{
			costMin = furnici[f].lungimeTur;
			furnicaMin = f;
		}

		furnici[f].orasUrmacatrer = -1;
		furnici[f].lungimeTur = 0.0;

		for (i = 0; i < NR_ORASE; i++)
		{
			furnici[f].tabu[i] = 0;
			furnici[f].drum[i] = -1;
		}

		if (catre == NR_ORASE)
			catre = 0;

		furnici[f].orasCurent = catre++;

		furnici[f].indexDrum = 1;
		furnici[f].drum[0] = furnici[f].orasCurent;

		furnici[f].tabu[furnici[f].orasCurent] = 1;
	}
}

double cantFeromon(int din, int catre)
{
	return((pow(feromon[din][catre], ALFA) * pow((1.0 / distanta[din][catre]), BETA)));
}

int selectareUrmator(int f)
{
	int din, catre;
	double suma = 0.0;

	din = furnici[f].orasCurent;

	for (catre = 0; catre < NR_ORASE; catre++)
	{
		if (furnici[f].tabu[catre] == 0)
		{
			suma += cantFeromon(din, catre);
		}
	}

	assert(suma!= 0.0);

	do
	{
		double probabilitate;
		catre++;

		if (catre >= NR_ORASE)
			catre = 0;
		if (furnici[f].tabu[catre] == 0)
		{
			probabilitate = cantFeromon(din, catre) / suma;

			
			double x = ((double)rand() / RAND_MAX);
			if (x < probabilitate)
			{
				

				break;
			}
		}
	} while (1);

	return catre;
}

int simularefurnici()
{
	int k;
	int m = 0;

	for (k = 0; k < NR_FURNICI; k++)
	{
		

		if (furnici[k].indexDrum < NR_ORASE)
		{
			furnici[k].orasUrmacatrer = selectareUrmator(k);
			furnici[k].tabu[furnici[k].orasUrmacatrer] = 1;
			furnici[k].drum[furnici[k].indexDrum++] = furnici[k].orasUrmacatrer;

			furnici[k].lungimeTur += distanta[furnici[k].orasCurent][furnici[k].orasUrmacatrer];

			

			if (furnici[k].indexDrum == NR_ORASE)
			{
				furnici[k].lungimeTur += distanta[furnici[k].drum[NR_ORASE - 1]][furnici[k].drum[0]];
			}

			furnici[k].orasCurent = furnici[k].orasUrmacatrer;
			m++;

		}
	}

	return m;
}

void sortareFurnici()
{
	structuraFurnici tempFurnici;
	int f, i, j;
	for (i = 0; i < NR_ORASE; i++)
	{
		furniciRang[i] = furnici[i];
	}
	
	for (i = 0; i < NR_ORASE; i++)
	{
		for (j = i + 1; j < NR_ORASE; j++)
		{
			if (furniciRang[i].lungimeTur >= furniciRang[j].lungimeTur)
			{
				tempFurnici = furniciRang[i];
				furniciRang[i] = furniciRang[j];
				furniciRang[j] = tempFurnici;
			}
		}
	}
}



void actualizareFeromon()
{
	int din, catre, i, f;


	for (din = 0; din < NR_ORASE; din++)
	{
		for (catre = 0; catre < NR_ORASE; catre++)
		{
			if (din != catre)
			{
				feromon[din][catre] *= (1.0 - RHO);

				if (feromon[din][catre] < 0.0)
				{
					feromon[din][catre] = INIT_PHER;
				}
			}
		}
	}

	

	for (f = 0; f < RANK_W - 1; f++)
	{
		for (i = 0; i < NR_ORASE; i++)
		{
			if (i < NR_ORASE - 1)
			{
				din = furniciRang[f].drum[i];
				catre = furniciRang[f].drum[i + 1];
			}
			else
			{
				din = furniciRang[f].drum[i];
				catre = furniciRang[f].drum[0];
			}

			feromon[din][catre] += (RANK_W - f) * (QVAL / furniciRang[f].lungimeTur); 
			feromon[catre][din] = feromon[din][catre];

		}
	}
	
	for (i = 0; i < NR_ORASE; i++)
	{
		if (i < NR_ORASE - 1)
		{
			din = furnici[furnicaMin].drum[i];
			catre = furnici[furnicaMin].drum[i + 1];
		}
		else
		{
			din = furnici[furnicaMin].drum[i];
			catre = furnici[furnicaMin].drum[0];
		}

		feromon[din][catre] += (QVAL / costMin);
		feromon[catre][din] = feromon[din][catre];
	}

	for (din = 0; din < NR_ORASE; din++)
	{
		for (catre = 0; catre < NR_ORASE; catre++)
		{
			feromon[din][catre] *= RHO;
		}
	}

}

void emitDataFile(int furnicaMin)
{
	
	structuraFurnici fcostMin;
	fcostMin = furnici[furnicaMin];
	
	int i;
	cout << "Turul gasit: ";
	for (i = 0; i < NR_ORASE; i++)
	{
		
		cout  << fcostMin.drum[i] + 1 << " ";
	}

	

}



int main()
{
	clock_t tStart = clock();
	
	int curTime = 0;
	cout << "ACO-Rank:";
	cout << "MaxTime=" << MAX_TIME;

	initializare();

	while (curTime++ < MAX_TIME)
	{
		if (simularefurnici() == 0)
		{
			sortareFurnici();
			actualizareFeromon();

			if (curTime != MAX_TIME)
				repornireFurnici();

			cout << "\nTimpul " << curTime << "(" << costMin << ")";

		}
	}

	cout << "\nRank Based ACO cost minim  = " << costMin << endl << endl << endl;

	emitDataFile(furnicaMin);
	cout << "\n" << "Timpul de  rulare: " << ((double)(clock() - tStart) / CLOCKS_PER_SEC);

	return 0;
}