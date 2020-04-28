#ifndef Tabou_hpp
#define Tabou_hpp

#include "Instance.hpp"
#include "Solution.hpp"
#include "Heuristique.h"
#include <stdio.h>
#include <vector>

using namespace std;

class RechercheTabou
{
	private:
		Instance* instance;
		int taille_liste_taboue;
		Solution* solution_initiale;
		int nb_iteration_max;
		bool aspiration;
		vector<vector<int>> liste_taboue;	// [[1, 6, 2, 7, 3]] ---> [[Employé n°1], [Employé n°2], ...] & [[n° employé, jour de travail 1, shift de travail 1, jour de travail 2, shift de travail 2]]

	public:
		RechercheTabou(Instance* instance, int taille_liste_taboue, Solution* solution_initiale, int nb_interation_max, bool aspiration);
		~RechercheTabou();
		Solution* rechercheTabou();
		void ajouterElement(vector<int> tabou);
		int valeurVoisin(Solution voisin);
		bool validiteVoisin(Solution voisin, int numero_employe);
		//void initialisation();
};


#endif