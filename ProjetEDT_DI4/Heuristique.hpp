#ifndef Heuristique_hpp
#define Heuristique_hpp

#include "Instance.hpp"
#include "Solution.hpp"
#include <stdio.h>
#include <vector>

using namespace std;

class Heuristique 
{
	private:
		//attributs

		Instance* instance;

	public:

		//méthodes

		//construteur

		/******************************************
		***** NOM :  Heuristique()
		*******************************************
		***** Constructeur par défaut
		*******************************************
		***** Entrée : i : l'instance à résoudre
		***** Sortie : /
		*******************************************/
		Heuristique(Instance* i);

		//setter

		/******************************************
		***** NOM :  set_Instance()
		*******************************************
		***** Setter
		*******************************************
		***** Entrée : i : l'instance à résoudre
		***** Sortie : /
		*******************************************/
		void set_Instance(Instance* i);

		/******************************************
		***** NOM :  resolution_Instance()
		*******************************************
		***** Méthode principale de la classe, à appeler pour résoudre l'instance qui a été fournie
		***** La priorité est avant tout de fournir une solution qui respecte les contraintes
		*******************************************
		***** Entrée : /
		***** Précond : L'instance fournie au préalable doit être correcte et réalisable
		***** Sortie : Solution* : une solution à l'instance donnée
		*******************************************/
		Solution* resolution_Instance();

		/******************************************
		***** NOM :  jours_Travailles_Par_Personne()
		*******************************************
		***** Pour un employe de l'instance, donne un vecteur représentant pour chaque jour de l'horizon si il travaille ou pas
		*******************************************
		***** Entrée : id_Employe : l'indice d'un employe de l'instance
		***** Sortie : vector<int> : Vecteur ou pour chaque jour v[jour] = 0 si l'employe travaille et = -1 s'il ne travaille pas
		*******************************************/
		vector<int> jours_Travailles_Par_Personne(int id_Employe);

		/******************************************
		***** NOM :  nb_Shift_Affilee()
		*******************************************
		***** Parcours un vecteur de 0 et de -1 et compte le nombre de 0 d'affilee à partir d'une certaine position
		*******************************************
		***** Entrées : v : le vecteur à parcourir
		*****			pos : la position de départ dans le vecteur
		*****			dir : le sens dans lequel on parcourt le vecteur, true si on va vers la fin du vecteur, false si on va vers le début
		***** Sortie : int : le nombre de 0 d'affilee à partir de la position donnée dans le vecteur donné
		*******************************************/
		int nb_Shift_Affilee(vector<int> v, int pos, bool dir);
};


#endif