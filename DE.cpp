#include "Entete.h"
#pragma comment (lib,"EvoDiffDLL.lib")  
//%%%%%%%%%%%%%%%%%%%%%%%%% IMPORTANT: %%%%%%%%%%%%%%%%%%%%%%%%% 
//Les fichiers de la DLL (DiffEvolutionDLL.dll et DiffEvolutionDLL.lib) doivent se trouver dans le m�me r�pertoire que l'ex�cutable (.exe) et 
//dans le r�pertoire courant du projet pour une ex�cution � l'aide du compilateur.
//Indiquer les arguments du programme dans les propri�t�s du projet - d�bogage - arguements.
//Sinon, utiliser le r�pertoire execution.

//*****************************************************************************************
// Prototype des fonctions se trouvant dans la DLL 
//*****************************************************************************************
//DESCRIPTION: Dimension des vecteurs de la population & initialisation des solutions avec des valeurs entre Xmin et Xmax.
extern "C" _declspec(dllimport) void InitialisationPopulation(std::vector<tSolution> &unePop, tProblem unProb, tAlgoDE &unDE);

//DESCRIPTION: S�LECTION: On retient le meilleur vecteur entre le vecteur trial (essai) et le vecteur target (cible) pour prendre la place de ce dernier dans la population
//PARAMETRES: unTarget-Vecteur cible qui pourra �tre remplac� par le vecteur trial si ce dernier pr�sente une meilleure fonction objectif, iTarget-Indice dans la population du vecteur cible 
//(n�cessaire � la MAJ de la Best), unTrial-Vecteur d'essai, uneBest-Meilleure solution depuis le d�but de l'algorithme, iBest-Indice dans la population de la meilleure solution qui pourra �tre modifi�
//unProb-D�finition du probleme, unDE-D�finition de l'algorithme
extern "C" _declspec(dllimport) void Selection(tSolution &unTarget, int iTarget, tSolution unTrial, tSolution uneBest, int &iBest, tProblem unProb, tAlgoDE unDE);

//DESCRIPTION: Renvoie un double al�atoire entre a et b
extern "C" _declspec(dllimport) double AleaDouble(double a, double b);

//DESCRIPTION: Fonction qui affiche le d�tail des solutions (de Debut jusqu'a Fin-1) de la population
extern "C" _declspec(dllimport) void AfficherSolutions(std::vector<tSolution> unePop, int Debut, int Fin, int Iter, tProblem unProb);

//DESCRITPION: Fonction qui affiche une solution (bool Detail: avec ou sans d�tail)
extern "C" _declspec(dllimport) void AfficherUneSolution(tSolution P, int Iter, tProblem unProb, bool Detail);
//DESCRIPTION: Fonction qui affiche une solution dans un fichier de sortie (bool Detail: avec ou sans d�tail)
extern "C" _declspec(dllimport) void AfficherUneSolutionFichier(tSolution Sol, int Iter, tProblem unProb, bool Detail, ofstream &Fichier);

//DESCRIPTION: Fonction affichant les r�sultats de l'algorithme
extern "C" _declspec(dllimport) void AfficherResultats(tSolution uneBest, tProblem unProb, tAlgoDE unDE);
//DESCRIPTION: Fonction affichant les r�sultats de l'algorithme dans un fichier texte
extern "C" _declspec(dllimport) void AfficherResultatsFichier(tSolution uneBest, tProblem unProb, tAlgoDE unDE, std::string FileName);

//**Liberation de la m�moire allou�e dynamiquement
extern "C" _declspec(dllimport) void LibererMemoireFinPgm(std::vector<tSolution> &unePop, tProblem unProb, tAlgoDE unDE);

//*****************************************************************************************
// Prototype des fonctions locales
//*****************************************************************************************
void InitialisationDomaineVariable(tProblem &unProb);
void EvaluationSolution(tSolution &Sol, tProblem unProb, tAlgoDE &unDE);
void Mutation(std::vector<tSolution> unePop, int iTarget, int iBest, tSolution &unMutant, tProblem unProb, tAlgoDE unDE);
void Croisement(tSolution unTarget, tSolution unMutant, tSolution &unTrial, tProblem unProb, tAlgoDE &unDE);

//******************************************************************************************
// Fonction main
//******************************************************************************************
int main(int NbParam, char *Param[])
{
	tProblem LeProb;					//**D�finition de l'instance de probl�me
	tAlgoDE LeDE;						//**D�finition des param�tres de l'algorithme
	std::vector<tSolution> Pop;			//**Ensemble de solutions 
	tSolution Mutant, Trial;			//**Vecteurs: mutant(donneur) et essai
	int NoBest;							//**Indice dans la Pop de la Meilleure solution depuis le d�but de l'algorithme
	int i;
	
		//**Lecture des param�tres
	LeDE.NP				= atoi(Param[1]);
	LeDE.F				= atof(Param[2]);
	LeDE.CR				= atof(Param[3]);
	LeDE.NB_EVAL_MAX	= atoi(Param[4]);
	LeDE.Iter			= 0;
	LeDE.CptEval		= 0;
	
	srand((unsigned)time(NULL));							//**Precise un germe pour le generateur aleatoire
	cout.setf(ios::fixed | ios::showpoint);

	//**Choix de la strat�gie de mutation/croisement
	LeDE.TypeMut = RAND1;									//**Sp�cifie le type de Mutation (s�lection solutions + #perturbations) - Voir Entete.h
	LeDE.TypeCr = EXP;										//**Sp�cifie le type de croisement  - Voir Entete.h
	
	//**Sp�cifications du probl�me � traiter
	LeProb.Fonction = ALPINE;								//**Sp�cifie le probl�me trait�  - Voir Entete.h
	InitialisationDomaineVariable(LeProb);

	//**Dimension du vecteur de la population, initialisation des solutions avec des valeurs entre Xmin et Xmax
	InitialisationPopulation(Pop, LeProb, LeDE);			//**NB: Pop est un vecteur de 0 � NP - 1
	//**Evaluation des solutions et conservation de la meilleure solution
	for (i = 0; i < LeDE.NP; i++)
	{
		EvaluationSolution(Pop[i], LeProb, LeDE);
		//Conservation de la meilleure solution initiale
		if (i == 0)
			NoBest = i;
		else
			if (Pop[i].FctObj <= Pop[NoBest].FctObj)
				NoBest = i;
	}

	//Dimension des vecteurs: mutant et essai
	Mutant.X.resize(LeProb.D);
	Trial.X.resize(LeProb.D);								
	
	//AfficherSolutions(Pop, 0, LeDE.NP, LeDE.Iter, LeProb);
	AfficherUneSolution(Pop[NoBest], LeDE.Iter, LeProb, false);

	//**Boucle principale de l'algorithme
	while (LeDE.CptEval < LeDE.NB_EVAL_MAX) 	//**NE PAS ENLEVER/MODIFIER LA CONDITION SUR LE NOMBRE D'�VALUATION
	{
		LeDE.Iter++;
		
		for (i = 0; i < LeDE.NP; i++)
		{
			//**MUTATION: Cr�ation du vecteur mutant
			Mutation(Pop, i, NoBest, Mutant, LeProb, LeDE);

			//**CROISEMENT: Cr�ation du vecteur trial (essai)
			Croisement(Pop[i], Mutant, Trial, LeProb, LeDE);

			//**SELECTION: entre le vecteur target(cible) et le vecteur trial (essai)
			Selection(Pop[i], i, Trial, Pop[NoBest], NoBest, LeProb, LeDE);
		}

		//AfficherSolutions(Pop, 0, LeDE.NP, LeDE.Iter, LeProb);
		AfficherUneSolution(Pop[NoBest], LeDE.Iter, LeProb, false);
	};

	AfficherResultats(Pop[NoBest], LeProb, LeDE);		//**NE PAS ENLEVER
	AfficherResultatsFichier(Pop[NoBest], LeProb, LeDE, "Resultats.txt");
	
	LibererMemoireFinPgm(Pop, LeProb, LeDE);

	system("PAUSE");
	return 0;
}

//**-----------------------------------------------------------------------
//**D�termine l'intervalle de recherche selon la fonction choisie
void InitialisationDomaineVariable(tProblem &unProb)
{
	switch(unProb.Fonction)
	{
		case ALPINE:		unProb.Xmin = -10.0;	unProb.Xmax = 10.0;	unProb.D = 200; break;
		case SPHERE:		unProb.Xmin = -5.12;	unProb.Xmax = 5.12; unProb.D = 100; break;
		case EGGHOLDER:		unProb.Xmin = -512;		unProb.Xmax = 512;	unProb.D = 2; break;
		case ROSENBROCK:	unProb.Xmin = -10;		unProb.Xmax = 10;	unProb.D = 10; break;
		default:			unProb.Xmin = 0.0;		unProb.Xmax = 0.0;	unProb.D = 0; break; 
	}
}

//**-----------------------------------------------------------------------
//**Calcul de la fonction objectif d'une solution selon la fonction continue (probl�me) s�lectionn�e
void EvaluationSolution(tSolution &Sol, tProblem unProb, tAlgoDE &unDE)
{
	double valeur = 0.0;
	int d;

	switch (unProb.Fonction)
	{
		case ALPINE: //Alpine: Min 0 en (0,0 ... 0)
			for (d = 0; d < unProb.D; d++)
			{
				valeur += fabs(Sol.X[d] * sin(Sol.X[d]) + 0.1 * Sol.X[d]);
			}
			break;


		case SPHERE: //Sphere: Min 0 en (0,0 ... 0)
			for (d = 0; d < unProb.D; d++)
			{
				valeur +=  pow(Sol.X[d], 2);
			}
			break;

		case EGGHOLDER: //Eggholder: Min -959.6407 en (512,404.2319)
			valeur = -(Sol.X[1]+47) * sin(sqrt(abs(Sol.X[1]+ Sol.X[0]*0.5+47)))
				- Sol.X[0] * sin(sqrt(abs(Sol.X[0] - ( Sol.X[1] + 47))));
			break;

		case ROSENBROCK: //Rosenbrock: Min 0 en (0,0 ... 0)
			for (d = 0; d < unProb.D-1; d++)
			{
				valeur += 100*pow(Sol.X[d+1] - Sol.X[d]* Sol.X[d], 2) + pow(Sol.X[d]-1, 2);
			}
			break;
		default: valeur = FLT_MAX;
	}
	//Ne pas enlever/modifier
	Sol.FctObj = valeur;
	unDE.CptEval++;
}

//**-----------------------------------------------------------------------
//MUTATION: Creation du vecteur mutant � l'aide d'autres vecteurs de la population (ces vecteurs doivent �tre diff�rents).
//PARAMETRES: unePop-Ensemble des solutions, iTarget-Indice dans la population du vecteur cible impliqu� dans la mutation, iBest-Indice dans la population de la meilleure solution
//unMutant-Vecteur mutant qui sera produit et retourn�, unProb-D�finition du probleme, unDE-D�finition de l'algorithme
void Mutation(std::vector<tSolution> unePop, int iTarget, int iBest, tSolution &unMutant, tProblem unProb, tAlgoDE unDE)
{
	int R1, R2, R3, R4, R5,			//indices des solutions choisies al�atoirement
		d;				

	/**********************************************************************************************************************************************/
	/*NB: Pour simplification: ne pas faire la v�rification que la meilleure solution et le vecteur Cible (Target) sont des solutions diff�rentes.*/
	/*    Faire les v�rifications pour les autres vecteurs                                                                                        */
	/**********************************************************************************************************************************************/
	switch (unDE.TypeMut)
	{
		case RAND1:	//le vecteur mutant est cr�e en ajoutant 1 perturbation � l'aide de 3 solutions choisies al�atoirement (1 diff�rence pond�r�e)
			do R1 = rand() % unDE.NP; while (R1 == iTarget);
			do R2 = rand() % unDE.NP; while (R2 == iTarget || R2 == R1);
			do R3 = rand() % unDE.NP; while (R3 == iTarget || R3 == R1 || R3 == R2);
			for (d = 0; d < unProb.D; d++)
				unMutant.X[d] = unePop[R1].X[d] + unDE.F * (unePop[R2].X[d] - unePop[R3].X[d]);
			break;
		case RAND2:	//le vecteur mutant est cr�e en ajoutant 2 perturbations � l'aide de 5 solutions choisies al�atoirement (2 diff�rences pond�r�es)
			do R1 = rand() % unDE.NP; while (R1 == iTarget);
			do R2 = rand() % unDE.NP; while (R2 == iTarget || R2 == R1);
			do R3 = rand() % unDE.NP; while (R3 == iTarget || R3 == R1 || R3 == R2);
			do R4 = rand() % unDE.NP; while (R4 == iTarget || R4 == R1 || R4 == R2 || R4 == R3);
			do R5 = rand() % unDE.NP; while (R5 == iTarget || R5 == R1 || R5 == R2 || R5 == R3 || R5 == R4);
			for (d = 0; d < unProb.D; d++)
				unMutant.X[d] = unePop[R1].X[d] + unDE.F * (unePop[R2].X[d] - unePop[R3].X[d])
												+ unDE.F * (unePop[R4].X[d] - unePop[R5].X[d]);
			break;
		case BEST1: //le vecteur mutant est cr�� en ajoutant une perturbation � Best � travers 1 diff�rence pond�r�e de solutions s�lectionn�es al�atoirement
			break;
	}
	//Confinement d'intervalle pour chaque dimension de la solution
	for (int d = 0; d < unProb.D; d++)
	{
		if (unMutant.X[d] < unProb.Xmin || unMutant.X[d] > unProb.Xmax) unMutant.X[d] = AleaDouble(unProb.Xmin, unProb.Xmax);
	}
}

//**-----------------------------------------------------------------------
//CROISEMENT: �change de genes entre le vecteur Target (cible) et le vecteur Mutant afin de cr�er le vecteur Trial (essai-prog�niture)
//PARAMETRES: unTarget-Vecteur cible, unMutant-Vecteur mutant, unTrial-Vecteur d'essai qui sera produit et retourn�, 
//unProb-D�finition du probleme, unDE-D�finition de l'algorithme
void Croisement(tSolution unTarget, tSolution unMutant, tSolution &unTrial, tProblem unProb, tAlgoDE &unDE)
{
	int n,			//croisement exponentionel: Point de d�part (point coupure) [0,D-1]
		L;			//croisement exponentionel: Nombre de dimensions � copier
	double Alea;

	switch (unDE.TypeCr)
	{
			case BIN:	//Croisement de type binomial (ou uniform)
				break;
			case EXP:	//Croisement de type exponentiel (ou 2-point)
				unTrial = unTarget;  //Copie du vecteur cible dans le vecteur essai
				n = rand() % unProb.D;
				L = 0;
				do
				{
					unTrial.X[n] = unMutant.X[n];
					n = (n+1) % unProb.D; 
					L++;
					Alea = AleaDouble(0, 1);
				} while (Alea<unDE.CR && L < unProb.D);
				break;
	}

	//Evaluation de la fonction objectif - Ne pas enlever
	EvaluationSolution(unTrial, unProb, unDE);
}