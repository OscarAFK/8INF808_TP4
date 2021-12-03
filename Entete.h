#ifndef __ENTETE_H_
#define __ENTETE_H_

#include <cstdio>
#include <cstdlib> 
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <ctime>  
#include <cmath>
#include <vector>
#include <limits>
using namespace std;

enum eProb	{ ALPINE, SPHERE, EGGHOLDER, ROSENBROCK };	//**Ne pas modifer
enum eDefinitionXY { RAND1, RAND2, BEST1 };				//**Ne pas modifer
enum eDefinitionZ { BIN, EXP };							//**Ne pas modifer


struct tProblem									//**D�finition du probl�me
{
	eProb	Fonction;							//**Nom de la fonction ou du probl�me � traiter
	int		D;									//**Nbre de Variables (dimensions)
	double	Xmin;								//**Domaine des variables: valeur minimale 
	double	Xmax;								//**Domaine des variables: valeur maximale
};

struct tSolution								//**D�finition d'une solution: 
{
	std::vector<double>		X;					//**Valeur r�elle pour chacune des dimensions de la solution. NB: Vecteur de 0 � D-1.
	double					FctObj;				//**Valeur de la fonction objectif
};

struct tAlgoDE
{
	int		NP;							//**Taille de la population (nombre de solutions)
	double	CR;							//**Probabilit� de croisement [0,1]
	double	F;							//**Facteur de mutation [0,1]
	eDefinitionXY TypeMut;				//**Choix de la strat�gie de croisement DE/x/y/z avec x: mode de s�lection, y: nombre de diff�rentiations pour la perturbation
	eDefinitionZ TypeCr;				//**Choix de la strat�gie de croisement DE/x/y/z avec z: sch�ma de croissement (exp ou bin) 
	int		Iter;						//**Compteur du nombre de g�n�rations
	int		CptEval;					//**COMPTEUR DU NOMBRE DE SOLUTIONS EVALUEES. SERT POUR CRITERE D'ARRET.
	int		NB_EVAL_MAX;				//**CRITERE D'ARRET: MAXIMUM "NB_EVAL_MAX" EVALUATIONS.
};

#endif