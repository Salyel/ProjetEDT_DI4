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
		vector<int> nb_Shift_Par_Type;

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
		***** NOM :  echange_Jour_Travail()
		*******************************************
		***** Pour un jour de travail d'un employe donne, cherche un jour de conge a echanger avec ce dernier
		*******************************************
		***** Entr�e :  v_Horizon_Employe : le vecteur des shift par jour pour un employe
						id_Employe : l'indice de l'employe correspondant
						id_Jour : le jour de travail que l'on cherche a passer en jour de conge
						meme_Shift : est-ce que la shift du nouveau jour de travail doit etre la meme que la shift du jour qui va etre passe en conge
		***** Sortie :  bool : est-ce qu'on a reussi a faire l'echange ou non
		*******************************************/
		bool echange_Jour_Travail(vector<int>& v_Horizon_Employe, int id_Employe, int id_Jour, bool meme_Shift);

		/******************************************
		***** NOM :  nb_Shift_Affilee()
		*******************************************
		***** Parcourt un vecteur de 0 et de -1 et compte le nombre de 0 d'affilee � partir d'une certaine position
		*******************************************
		***** Entr�es : v : le vecteur � parcourir
		*****			pos : la position de d�part dans le vecteur
		*****			dir : le sens dans lequel on parcourt le vecteur, true si on va vers la fin du vecteur, false si on va vers le d�but
		***** Sortie : int : le nombre de 0 d'affilee � partir de la position donn�e dans le vecteur donn�
		*******************************************/
		int nb_Shift_Affilee(vector<int> v, int pos, bool dir);

		/******************************************
		***** NOM :  improved_Shift_Succede()
		*******************************************
		***** indique si un shift peut �tre place entre deux autres (prend en compte les conges)
		*******************************************
		***** Entr�es : shift_Prev : la shift precedente
		*****			shift : la shift qu'on veut ajouter
		*****			shift_Suiv : la shift suivante
		***** Sortie : bool : vrai si shift peut suivre shift_Prev ET shift_Suiv peut suivre shift, faux sinon
		*******************************************/
		bool improved_Shift_Succede(int shift_Prev, int shift, int shift_Suiv);

};


#endif