#ifndef Tabou_hpp
#define Tabou_hpp

#include "Instance.hpp"
#include "Solution.hpp"
#include "Heuristique.hpp"
#include <algorithm>
#include <stdio.h>
#include <vector>
#include <chrono>

using namespace std;

class RechercheTabou
{
	private:
		Instance* instance;
		int taille_liste_taboue;
		Solution* solution_initiale;
		int nb_iteration_max;
		bool aspiration;
		bool realisable = false;
		vector<vector<int>> liste_taboue_jours;	// [[1, 6, 7]] ---> [[Employé n°1], [Employé n°2], ...] & [[n° employé, jour de travail 1, jour de travail 2]]
		vector<vector<int>> liste_taboue_shifts;	// [[1, 6, 7]] ---> [[Employé n°1], [Employé n°2], ...] & [[n° employé, n° du jour, nouveau_shift]]

	public:
		RechercheTabou(Instance* instance, int taille_liste_taboue, Solution* solution_initiale, int nb_interation_max, bool aspiration);
		~RechercheTabou();

		Solution* rechercheTabou(chrono::time_point<chrono::system_clock> start, int random, bool realisable); //Lance la recherche tabou adéquate en fonction du paramètre random
		Solution* rechercheTabouRandom(chrono::time_point<chrono::system_clock> start, int random);
		Solution* rechercheTabouToutParcourir(chrono::time_point<chrono::system_clock> start);

		void ajouterElementJours(vector<int> tabou);
		void ajouterElementShifts(vector<int> tabou);

		bool presenceMouvementEtInverse(vector<int> mouvement);
		bool presenceMouvement(vector<int> mouvement);

		int valeurVoisin(Solution voisin);
		int nouvelleValeurVoisinJours(Solution voisin, int index_employe, int index_jour1, int index_jour2, vector<vector<int>> v_i_nb_personne_par_Shift_et_jour);
		int nouvelleValeurVoisinShifts(Solution voisin, int index_employe, int index_jour, int index_shift, int index_ancien_shift, vector<vector<int>> v_i_nb_personne_par_Shift_et_jour);

		vector<vector<int>> calculNbPersonneShiftJour(Solution voisin);

		bool validiteVoisin(Solution voisin);
		bool validiteVoisinEmploye(Solution voisin, int numero_employe);
		bool validiteVoisinShift(Solution voisin, int numero_employe, int index_jour, int ancien_shift, int nouveau_shift, int duree_travail, vector<int> v_i_Nb_shift);
		bool validiteVoisinJours(Solution voisin, int numero_employe);

		int calculDureeTravail(Solution solution);
		vector<int> calculNbShift(Solution solution);

		void printVoisin(Solution voisin);

		void setInstance(Instance* instance);
		void setSolutionInitiale(Solution* solution);
};


#endif