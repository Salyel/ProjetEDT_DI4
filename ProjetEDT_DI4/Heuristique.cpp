#include "Heuristique.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

Solution* Heuristique::unnamed_heuristique(Instance* instance)
{
	Solution* s = new Solution();

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

		/**
		*
		* Ici ce serait cool de voir quels jours il peut travailler sans ruiner les 5 jours d'affilé avec ses congés
		*
		*/

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

					int id_Meilleure_Shift = 0, valeur_Meilleure; //inutiles pour l'instant, on ne s'en sert pas!
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
				if (!shift_Affectee /*&& nb_Shift_Consecutive < instance->get_Personne_Nbre_Shift_Consecutif_Min(e)*/)
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
				/*else {
					//si il n'était pas en congé le jour précédent
					if (j == 0 || s->v_v_IdShift_Par_Personne_et_Jour[e][j-1] != -1)
					{
						//on s'assure que l'employe restera en conge le minimum necessaire pendant les jours suivant 
						nb_Conge_Restant = instance->get_Personne_Jour_OFF_Consecutif_Min(e);
					}
					//sinon on laisse simplement l'id shift a -1

					//le nombre de shift de travail consécutives repasse à 0;
					nb_Shift_Consecutive = 0;
				}*/
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
				nb_Shift_Consecutive = 0;
				//sinon on laisse simplement l'id shift a -1

				//le nombre de shift de travail consécutives repasse à 0;
				nb_Shift_Consecutive = 0;
			}
			//boucle sur les jours
			j++;
			if (nb_Conge_Restant > 0) nb_Conge_Restant--;
		}
		cout << duree_Actuelle << " " << j << "\n";
	}

	return s;
}
