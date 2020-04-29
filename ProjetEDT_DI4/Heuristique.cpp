#include "Heuristique.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>


Heuristique::Heuristique(Instance* i)
{
	instance = i;
}

void Heuristique::set_Instance(Instance* i)
{
	instance = i;
}

Solution* Heuristique::resolution_Instance()
{
	Solution* s = new Solution();

	int nb_personne = instance->get_Nombre_Personne();
	for (int i = 0; i < nb_personne; i++)
	{
		cout << i + 1 << "\n";
		jours_Travailles_Par_Personne(i);
	}

	/*

	//toutes les valeurs de la matrice sont initialisées à -1, donc comme des congés
	s->v_v_IdShift_Par_Personne_et_Jour = vector<vector<int>>(instance->get_Nombre_Personne(), vector<int>(instance->get_Nombre_Jour(), -1));
	vector<vector<int>> nb_Personne_Par_Jour_Et_Shift = vector<vector<int>>(instance->get_Nombre_Jour(), vector<int>(instance->get_Nombre_Shift(), 0));

	int i_Nb_Employe = instance->get_Nombre_Personne();
	int i_Nb_Jour = instance->get_Nombre_Jour();
	int i_Nb_Shift = instance->get_Nombre_Shift();

	//pour chaque employé
	for (int e = 0; e < i_Nb_Employe; e++)
	{
		int duree_Totale_Min = instance->get_Personne_Duree_total_Min(e), duree_Totale_Max = instance->get_Personne_Duree_total_Max(e), duree_Actuelle = 0;
		int nb_Shift_Consecutive = 0, weekend_travail = 0, nb_Conge_Restant = 0;
		vector<int> nb_Shift_Par_Type = vector<int>(instance->get_Nombre_Shift(), 0);

		//on parcourt les journees une a une
		int j = 0;
		//tant qu'il n'a pas son temps de travail minimum
		while (duree_Actuelle < duree_Totale_Min && j < i_Nb_Jour)
		{
			//si l'employe ne depasse son nombre de jour de travail max d'affilee 
			//si il n'a pas de jour de conge reserve ou n'a pas ete mis en conge
			//si il ne depasse pas son nombre de WE de travail max
			if (nb_Shift_Consecutive + 1 <= instance->get_Personne_Nbre_Shift_Consecutif_Max(e)
				&& instance->is_Available_Personne_Jour(e,j) && nb_Conge_Restant == 0
				&& (j % 7 < 5 || weekend_travail + 1 <= instance->get_Personne_Nbre_WE_Max(e)))
			{
				//cela veut dire qu'il peut travailler sur cette journee

				//represente si on a deja affecte une shift pour cette journee
				bool shift_Affectee = false;

				int id_Shift = 0, valeur_Shift = 0;
				//on cherche si l'employe a une shift preferee pour ce jour
				while (valeur_Shift == 0 && id_Shift < i_Nb_Shift)
				{
					valeur_Shift = instance->get_Poids_Affectation_Pers_Jour_Shift(e, j, id_Shift);
					if (valeur_Shift == 0) id_Shift++;
				}

				//si on en a trouve une
				if (valeur_Shift != 0)
				{
					//on verifie que les shift peuvent se suivre
					bool shift_Succede = false;
					if (j != 0) {
						int shift_Prec = s->v_v_IdShift_Par_Personne_et_Jour[e][j-1];
						if (shift_Prec == -1)
							shift_Succede = true;
						else if (instance->is_possible_Shift_Succede(shift_Prec, id_Shift))
							shift_Succede = true;
					}
					else
						shift_Succede = true;

					//si ajouter la shift ne depasse pas le temps de travail max
					//si la shift peut succeder a celle de la veille
					//si on ne depasse le nombre de shift max d'un type
					if (duree_Actuelle + instance->get_Shift_Duree(id_Shift) <= duree_Totale_Max
						&& shift_Succede
						&& nb_Shift_Par_Type[id_Shift] + 1 <= instance->get_Personne_Shift_Nbre_Max(e, id_Shift))
					{
						//on affecte la shift et on met a jour les autres donnees liees
						s->v_v_IdShift_Par_Personne_et_Jour[e][j] = id_Shift;

						duree_Actuelle += instance->get_Shift_Duree(id_Shift);
						nb_Personne_Par_Jour_Et_Shift[j][id_Shift]++;
						nb_Shift_Par_Type[id_Shift]++;
						nb_Shift_Consecutive ++;
						if (j % 7 >= 5) weekend_travail++;
						shift_Affectee = true;
					}
				}

				//sinon on cherche la shift du jour sur laquelle il manque le plus de personnes
				if (!shift_Affectee)
				{

					//int id_Meilleure_Shift = 0, valeur_Meilleure;
					id_Shift = 0;
					valeur_Shift = 0;

					for (int s = 0; s < i_Nb_Shift; s++)
					{
						int nb_Personne_Manquante_Shift = instance->get_Nbre_Personne_Requis_Jour_Shift(j, s) - nb_Personne_Par_Jour_Et_Shift[j][s];
						if (valeur_Shift < nb_Personne_Manquante_Shift)
						{
							valeur_Shift = nb_Personne_Manquante_Shift;
							id_Shift = s;
						}
					}

					//si il y a au moins une shift dans laquelle il manque quelqu'un
					if (valeur_Shift != 0) 
					{
						//on verifie que les shift peuvent se suivre
						bool shift_Succede = false;
						if (j != 0) {
							int shift_Prec = s->v_v_IdShift_Par_Personne_et_Jour[e][j-1];
							if (shift_Prec == -1)
								shift_Succede = true;
							else if (instance->is_possible_Shift_Succede(shift_Prec, id_Shift))
								shift_Succede = true;
						}
						else
							shift_Succede = true;

						//si ajouter la shift ne depasse pas le temps de travail max
						//si la shift peut succeder a celle de la veille
						//si on ne depasse le nombre de shift max d'un type
						if (duree_Actuelle + instance->get_Shift_Duree(id_Shift) <= duree_Totale_Max
							&& shift_Succede
							&& nb_Shift_Par_Type[id_Shift] + 1 <= instance->get_Personne_Shift_Nbre_Max(e, id_Shift))
						{
							//on affecte la shift et on met a jour les autres donnees liees
							s->v_v_IdShift_Par_Personne_et_Jour[e][j] = id_Shift;
							duree_Actuelle += instance->get_Shift_Duree(id_Shift);
							nb_Personne_Par_Jour_Et_Shift[j][id_Shift]++;
							nb_Shift_Par_Type[id_Shift]++;
							nb_Shift_Consecutive++;
							if (j % 7 >= 5) weekend_travail++;
							shift_Affectee = true;
						}
					}
				}

				//si toutes les shift sont remplies et qu'il doit forcement travailler aujourd'hui on le met sur une shift aleatoire
				if (!shift_Affectee && nb_Shift_Consecutive < instance->get_Personne_Nbre_Shift_Consecutif_Min(e))
				{
					id_Shift = rand() % i_Nb_Shift;
					s->v_v_IdShift_Par_Personne_et_Jour[e][j] = id_Shift;
					duree_Actuelle += instance->get_Shift_Duree(id_Shift);
					nb_Personne_Par_Jour_Et_Shift[j][id_Shift]++;
					nb_Shift_Par_Type[id_Shift]++;
					nb_Shift_Consecutive++;
					if (j % 7 >= 5) weekend_travail++;
					shift_Affectee = true;
				}
				//sinon on le met en congé
				else {
					//si il n'était pas en congé le jour précédent
					if (j == 0 || s->v_v_IdShift_Par_Personne_et_Jour[e][j-1] != -1)
					{
						//on s'assure que l'employe restera en conge le minimum necessaire pendant les jours suivant 
						nb_Conge_Restant = instance->get_Personne_Jour_OFF_Consecutif_Min(e);
					}
					//sinon on laisse simplement l'id shift a -1

					//le nombre de shift de travail consécutives repasse à 0;
					nb_Shift_Consecutive = 0;
				}
			}
			//si il ne peut pas travailler ce jour on le met en congé
			else 
			{
				//si il n'était pas en congé le jour précédent
				if (j == 0 || s->v_v_IdShift_Par_Personne_et_Jour[e][j-1] != -1)
				{
					//on s'assure que l'employe restera en conge le minimum necessaire pendant les jours suivant 
					nb_Conge_Restant = instance->get_Personne_Jour_OFF_Consecutif_Min(e);
				}
				//sinon on laisse simplement l'id shift a -1

				//le nombre de shift de travail consécutives repasse à 0;
				nb_Shift_Consecutive = 0;
			}
			//boucle sur les jours
			j++;
			if (nb_Conge_Restant > 0) nb_Conge_Restant--;
		}
	}*/

	return s;
}



vector<int> Heuristique::jours_Travailles_Par_Personne(int id_Employe)
{
	//initialisation des variables / récupération des données de l'instance
	int i_Nb_Jour = instance->get_Nombre_Jour();
	int i_Nb_Shift_Min = instance->get_Personne_Nbre_Shift_Consecutif_Min(id_Employe);
	int i_Nb_Shift_Max = instance->get_Personne_Nbre_Shift_Consecutif_Max(id_Employe);
	int i_Nb_Conge_Min = instance->get_Personne_Jour_OFF_Consecutif_Min(id_Employe);
	int i_Nb_We_Travail_Max = instance->get_Personne_Nbre_WE_Max(id_Employe);

	//initialisation du vecteur à retourner
	vector<int> v_Travail_Par_Jour = vector<int>(i_Nb_Jour, 0);

	//le but est d'assigner des differents jours OFF necessaire au respect des contraintes de conges, de We et de shift consecutifs min/max
	//on va progresser des contraintes les moins flexibles aux plus flexibles

	int j = 0;

	/*                                           */
	//premiere etape : les jours de conges reserves
	/*                                           */

	//pour chaque jour
	for (j = 0; j < i_Nb_Jour; j++)
	{
		//si un jour de conge est demande et qu'il n'est pas deja a -1 (ça peut arriver quand on a plusieurs conge a la suite)
		if (!instance->is_Available_Personne_Jour(id_Employe, j) && v_Travail_Par_Jour[j] != -1)
		{
			//on met le jour en conge
			v_Travail_Par_Jour[j] = -1;

			// ATTENTION NE PREND EN COMPTE QUE jour_off_min = 2

			if (nb_Shift_Affilee(v_Travail_Par_Jour, j - 1, false) > i_Nb_Shift_Min && j != 0)
				v_Travail_Par_Jour[j - 1] = -1;
			else if (j < i_Nb_Jour && j + 1 < i_Nb_Jour)
				v_Travail_Par_Jour[j + 1] = -1;
		}
	}

	/*                                    */
	//deuxieme etape : les WE non travailles
	/*                                    */

	//au debut le nombre WE travailles est egal au nombre de semaines dans l'horizon
	int i_Nb_WE_Travail = i_Nb_Jour / 7;

	//pour chaque samedi
	for (j = 5; j < i_Nb_Jour; j += 7)
	{
		//si on encore trop de WE travailles
		if (i_Nb_WE_Travail > i_Nb_We_Travail_Max)
		{
			//si rajouter le week end ne brise pas la contrainte de shift min consecutif
			if (nb_Shift_Affilee(v_Travail_Par_Jour, j - 1, false) >= i_Nb_Shift_Min
				&& (nb_Shift_Affilee(v_Travail_Par_Jour, j + 2, true) >= i_Nb_Shift_Min || j + 2 >= i_Nb_Jour))
			{
				//on met le samedi et le dimanche comme non travailles
				v_Travail_Par_Jour[j] = -1;
				v_Travail_Par_Jour[j + 1] = -1;
				//et on diminue le nombre de WE travailles
				i_Nb_WE_Travail--;
			}
		}
	}

	/*                                                                                                  */
	//troisieme etape : les jours OFF necessaires pour avoir le bon nombre de shift de travail consecutifs
	/*                                                                                                  */

	j = 0;

	//tant qu'on est pas à la fin de l'horizon 
	while (j < i_Nb_Jour)
	{
		//on va au prochain jour travaillé
		if (v_Travail_Par_Jour[j] == -1)
			j++;
		else
		{
			//on compte combien de shift consecutif il y a jusqu'au prochain jour OFF
			int i_Nb_Jour_Affilee = nb_Shift_Affilee(v_Travail_Par_Jour, j, true);

			//a partir de la on a 5 cas differents

			//CAS 1 : moins de jours que le nombre de shift consecutif min
			if (i_Nb_Jour_Affilee < i_Nb_Shift_Min)
			{
				//si on est pas au tout debut ou a la toute fin de l'horizon
				if (j > 0 && j <= i_Nb_Jour - i_Nb_Shift_Min)
				{
					//on bouche le trou avec des -1
					while (v_Travail_Par_Jour[j] == 0)
					{
						v_Travail_Par_Jour[j] = -1;
						j++;
					}
				}
				//sinon
				else
				{
					//on laisse tel quel
					while (v_Travail_Par_Jour[j] == 0 && j < i_Nb_Jour)
						j++;
				}
			}

			//CAS 2 : entre les nombres de shift consecutif max et min
			else if (i_Nb_Jour_Affilee <= i_Nb_Shift_Max)
			{
				//parfait on laisse comme ça
				j += i_Nb_Jour_Affilee;
			}

			//CAS 3 : plus que le nombre de shift consecutif max mais pas assez pour pouvoir le séparer en deux suite de shit de taille min
			else if (i_Nb_Jour_Affilee < 2*i_Nb_Shift_Min + i_Nb_Conge_Min)
			{
				//on rajoute des conges à la position j jusqu'à ce que la suite de 0 qui suit soit de la bonne taille
				for (int i = 0; i < i_Nb_Jour_Affilee - i_Nb_Shift_Max; i++)
				{
					v_Travail_Par_Jour[j] = -1;
					j++;
				}
			}

			//CAS 4 : assez pour pouvoir rajouter un conge entier mais oblige d'avoir une suite de shift de taille minimum
			else if (i_Nb_Jour_Affilee < i_Nb_Shift_Max + i_Nb_Shift_Min + i_Nb_Conge_Min)
			{
				//on laisse une suite de shift de taille min
				j += i_Nb_Shift_Min;
				//on met un suite de jours OFF de taille min
				for (int i = 0; i < i_Nb_Conge_Min; i++)
				{
					v_Travail_Par_Jour[j] = -1;
					j++;
				}
			}

			//CAS 5 : assez pour avoir une suite de shift de taille maximum
			else
			{
				//on laisse une suite de shift de taille max
				j += i_Nb_Shift_Max;
				//on met une suite de jours OFF de taille min
				for (int i = 0; i < i_Nb_Conge_Min; i++)
				{
					v_Travail_Par_Jour[j] = -1;
					j++;
				}
			}


		}
	}

	/*for (int i = 0; i < i_Nb_Jour; i++)
	{
		cout << v_Travail_Par_Jour[i] << " ";
	}
	cout << "\n";
	cout << "\n";
	cout << "\n";*/

	return v_Travail_Par_Jour;
}

int Heuristique::nb_Shift_Affilee(vector<int> v, int pos, bool dir)
{
	//initialisation des variables
	int i = pos, res = 0;
	int i_Nb_Jour = instance->get_Nombre_Jour();
	bool fini = false;

	while (!fini)
	{
		//si on arrive au bout du vecteur on s'arrête
		if ((dir && i >= i_Nb_Jour) || (!dir && i < 0))
			fini = true;
		//même chose si on tombe sur -1
		else if (v[i] == -1)
			fini = true;
		//sinon update la valeur de retour et la position
		else {
			res++;
			dir ? i++ : i--;
		}
	}

	return res;
}

