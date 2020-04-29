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

		//m�thodes

		//construteur

		/******************************************
		***** NOM :  Heuristique()
		*******************************************
		***** Constructeur par d�faut
		*******************************************
		***** Entr�e : i : l'instance � r�soudre
		***** Sortie : /
		*******************************************/
		Heuristique(Instance* i);

		//setter

		/******************************************
		***** NOM :  set_Instance()
		*******************************************
		***** Setter
		*******************************************
		***** Entr�e : i : l'instance � r�soudre
		***** Sortie : /
		*******************************************/
		void set_Instance(Instance* i);

		/******************************************
		***** NOM :  resolution_Instance()
		*******************************************
		***** M�thode principale de la classe, � appeler pour r�soudre l'instance qui a �t� fournie
		***** La priorit� est avant tout de fournir une solution qui respecte les contraintes
		*******************************************
		***** Entr�e : /
		***** Pr�cond : L'instance fournie au pr�alable doit �tre correcte et r�alisable
		***** Sortie : Solution* : une solution � l'instance donn�e
		*******************************************/
		Solution* resolution_Instance();

		/******************************************
		***** NOM :  jours_Travailles_Par_Personne()
		*******************************************
		***** Pour un employe de l'instance, donne un vecteur repr�sentant pour chaque jour de l'horizon si il travaille ou pas
		*******************************************
		***** Entr�e : id_Employe : l'indice d'un employe de l'instance
		***** Sortie : vector<int> : Vecteur ou pour chaque jour v[jour] = 0 si l'employe travaille et = -1 s'il ne travaille pas
		*******************************************/
		vector<int> jours_Travailles_Par_Personne(int id_Employe);

		/******************************************
		***** NOM :  nb_Shift_Affilee()
		*******************************************
		***** Parcours un vecteur de 0 et de -1 et compte le nombre de 0 d'affilee � partir d'une certaine position
		*******************************************
		***** Entr�es : v : le vecteur � parcourir
		*****			pos : la position de d�part dans le vecteur
		*****			dir : le sens dans lequel on parcourt le vecteur, true si on va vers la fin du vecteur, false si on va vers le d�but
		***** Sortie : int : le nombre de 0 d'affilee � partir de la position donn�e dans le vecteur donn�
		*******************************************/
		int nb_Shift_Affilee(vector<int> v, int pos, bool dir);
};


#endif