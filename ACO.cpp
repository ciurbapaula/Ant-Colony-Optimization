#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <fstream>


#define NR_ORASE 127
#define DIST_MAX 100000
#define NR_TURURI (NR_ORASE * DIST_MAX)
#define NR_FURNICI 127

using namespace std;


struct strucuraOras {
	int x, y;
};

struct strucuraFurnici {

	int orasCurent, orasUrmator, indexDrum;
	int tabu[NR_ORASE];
	int drum[NR_ORASE];
	double lungimeTur;
};





#define ALFA 1.0
#define BETA 5.0 
#define RHO 0.5 
#define QVAL 100
#define NR_MAXTUR 20
#define MAX_TIME (NR_MAXTUR * NR_ORASE)
#define FERO_initializare (1.0/NR_ORASE)


strucuraOras orase[NR_ORASE];
strucuraFurnici furnici[NR_FURNICI];

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
			feromon[din][catre] = FERO_initializare;
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

				distanta[din][catre] = sqrt(xd + yd);
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
			furnici[f].drum[din] = 0;
		}

		furnici[f].indexDrum = 1;
		furnici[f].drum[0] = furnici[f].orasCurent;
		furnici[f].orasUrmator = 0;
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

		furnici[f].orasUrmator = 0;
		furnici[f].lungimeTur = 0.0;

		for (i = 0; i < NR_ORASE; i++)
		{
			furnici[f].tabu[i] = 0;
			furnici[f].drum[i] = 0;
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

int simulareFurnici()
{
	int k;
	int moving = 0;

	for (k = 0; k < NR_FURNICI; k++)
	{
		

		if (furnici[k].indexDrum < NR_ORASE)
		{
			furnici[k].orasUrmator = selectareUrmator(k);
			furnici[k].tabu[furnici[k].orasUrmator] = 1;
			furnici[k].drum[furnici[k].indexDrum++] = furnici[k].orasUrmator;

			furnici[k].lungimeTur += distanta[furnici[k].orasCurent][furnici[k].orasUrmator];


			if (furnici[k].indexDrum == NR_ORASE)
			{
				furnici[k].lungimeTur += distanta[furnici[k].drum[NR_ORASE - 1]][furnici[k].drum[0]];
			}

			furnici[k].orasCurent = furnici[k].orasUrmator;
			moving++;
			
		}
	}
	
	return  moving;
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
					feromon[din][catre] = FERO_initializare;
				}
			}
		}
	}

	

	for (f = 0; f < NR_FURNICI; f++)
	{
		for (i = 0; i < NR_ORASE; i++)
		{
			if (i < NR_ORASE - 1)
			{
				din = furnici[f].drum[i];
				catre = furnici[f].drum[i + 1];
			}
			else
			{
				din = furnici[f].drum[i];
				catre = furnici[f].drum[0];
			}

			feromon[din][catre] += (QVAL / furnici[f].lungimeTur);
			feromon[catre][din] = feromon[din][catre];

		}
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
	
	strucuraFurnici costMin;
	costMin = furnici[furnicaMin];
	
	int i;
	cout << "\n" << "Turul gasit :";
	for (i = 0; i < NR_ORASE; i++)
	{
		
		cout <<costMin.drum[i]+1 << " ";
	}

}



int main()
{
	clock_t tStart = clock();
	int curTime = 0;

	cout << "S-ACO:";
	cout << "MaxTime=" << MAX_TIME;


	initializare();

	while (curTime++ < MAX_TIME)
	{
		if (simulareFurnici() == 0)
		{
			actualizareFeromon();

			if (curTime != MAX_TIME)
				repornireFurnici();

			cout << "\n Timpul is " << curTime << "(" << costMin << ")";

		}
	}

	cout << "\nSACO: MIN COST = " << costMin << endl << endl << endl;

	emitDataFile(furnicaMin);
	cout << "\n" << "Timpul de  rulare: " << ((double)(clock() - tStart) / CLOCKS_PER_SEC);

	return 0;
}