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

	//r�cup�ration de donn�es de l'instance
	int i_Nb_Employe = instance->get_Nombre_Personne();
	int i_Nb_Jour = instance->get_Nombre_Jour();
	int i_Nb_Shift = instance->get_Nombre_Shift();

	//initialisation du vecteur de solution et d'un vecteur repr�sentant le nombre de personnes actuellement affect�es � chaque shift
	s->v_v_IdShift_Par_Personne_et_Jour = vector<vector<int>>(i_Nb_Employe, vector<int>());
	vector<vector<int>> nb_Personne_Par_Jour_Par_Shift = vector<vector<int>>(i_Nb_Jour, vector<int>(i_Nb_Shift, 0));


	for (int e = 0; e < i_Nb_Employe; e++)
	{
		//on commence par determiner quels jours sont travailles et quels jours sont des conges
		vector<int> v_Horizon_Employe = Heuristique::jours_Travailles_Par_Personne(e);

		//puis on determine quelle est la meilleure shift a mettre pour chaque jour travailler

		int duree_Travail_Min = instance->get_Personne_Duree_total_Min(e), duree_Travail_Max = instance->get_Personne_Duree_total_Max(e), duree_Travail_Actuelle = 0;
		vector<int> nb_Shift_Par_Type = vector<int>(i_Nb_Shift, 0);

		//pour chaque jour de l'horizon
		for (int j = 0; j < i_Nb_Jour; j++)
		{
			//si on a pas etabli que le jour serait un jour de repos
			if(v_Horizon_Employe[j] != -1) {

				//on etablit la liste des shifts "ajoutables"
				vector<int> v_Shift_Ajoutables;
				for (int shift = 0; shift < i_Nb_Shift; shift++)
				{
					//est-ce que la shift peut suivre la shift precedente
					bool shift_Succede = false;
					if (j == 0) 
						shift_Succede = true;
					else
					{
						int shift_Prec = v_Horizon_Employe[j - 1];
						if (shift_Prec == -1)
							shift_Succede = true;
						else if (instance->is_possible_Shift_Succede(shift_Prec, shift))
							shift_Succede = true;
					}
				
					if (duree_Travail_Actuelle + instance->get_Shift_Duree(shift) <= duree_Travail_Max //est-ce que ajouter la shift ne fait pas depasser la duree de travail
						&& shift_Succede //cf ci-dessus
						&& nb_Shift_Par_Type[shift] + 1 <= instance->get_Personne_Shift_Nbre_Max(e, shift)) // est-ce qu'on a pas atteint le nombre de shift max de ce type

						v_Shift_Ajoutables.push_back(shift);
				}

				int nb_Shift_Ajoutables = v_Shift_Ajoutables.size();
				int shift_A_Ajouter = -1;
				//si il y a une seule shift ajoutable
				if (nb_Shift_Ajoutables == 1)
				{
					shift_A_Ajouter = v_Shift_Ajoutables[0];
				}
				//si il y a plus d'une shift ajoutable
				else if (nb_Shift_Ajoutables > 1)
				{
					//on cherche si l'employe a une shift preferee
					for (int i = 0; i < nb_Shift_Ajoutables && shift_A_Ajouter == -1; i++)
					{
						if (instance->get_Poids_Affectation_Pers_Jour_Shift(e, j, v_Shift_Ajoutables[i]) > 0)
							shift_A_Ajouter = v_Shift_Ajoutables[i];
					}

					//si on a pas trouve de shift preferee
					if (shift_A_Ajouter == -1)
					{
						int nb_Manquant_Max = -999999999;
						//on cherche la shift sur laquelle il manque le plus de personnes
						for (int i = 0; i < nb_Shift_Ajoutables; i++)
						{
							int nb_Manquant = instance->get_Nbre_Personne_Requis_Jour_Shift(j, v_Shift_Ajoutables[i]) - nb_Personne_Par_Jour_Par_Shift[j][v_Shift_Ajoutables[i]];
							if ( nb_Manquant >= nb_Manquant_Max)
							{
								nb_Manquant_Max = nb_Manquant;
								shift_A_Ajouter = v_Shift_Ajoutables[i];
							}
						}
					}
				}

				//si on a trouve une shit a ajouter
				if (shift_A_Ajouter != -1)
				{
					//on affecte la shift
					v_Horizon_Employe[j] = shift_A_Ajouter;
					//et on met a jour les autres donnees liees
					duree_Travail_Actuelle += instance->get_Shift_Duree(shift_A_Ajouter);
					nb_Personne_Par_Jour_Par_Shift[j][shift_A_Ajouter]++;
					nb_Shift_Par_Type[shift_A_Ajouter]++;
				}
				//si il n'y en a aucune
				else
				{
					v_Horizon_Employe[j] = -1;
				}
			}
		}


		/*for (int k = 0; k < i_Nb_Jour; k++)
			cout << v_Horizon_Employe[k] << " ";
		cout << "\n";*/

		//ajout du vecteur dans la solution
		s->v_v_IdShift_Par_Personne_et_Jour[e] = v_Horizon_Employe;
	}

	return s;
}



vector<int> Heuristique::jours_Travailles_Par_Personne(int id_Employe)
{
	//initialisation des variables / r�cup�ration des donn�es de l'instance
	int i_Nb_Jour = instance->get_Nombre_Jour();
	int i_Nb_Shift_Min = instance->get_Personne_Nbre_Shift_Consecutif_Min(id_Employe);
	int i_Nb_Shift_Max = instance->get_Personne_Nbre_Shift_Consecutif_Max(id_Employe);
	int i_Nb_Conge_Min = instance->get_Personne_Jour_OFF_Consecutif_Min(id_Employe);
	int i_Nb_We_Travail_Max = instance->get_Personne_Nbre_WE_Max(id_Employe);

	//initialisation du vecteur � retourner
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
		//si un jour de conge est demande et qu'il n'est pas deja a -1 (�a peut arriver quand on a plusieurs conge a la suite)
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

	//tant qu'on est pas � la fin de l'horizon 
	while (j < i_Nb_Jour)
	{

		//on va au prochain jour travaill�
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
				//cout << "UN\n";
				//si on est au tout debut  de l'horizon
				if (j == 0)
				{
					//on laisse tel quel
					while (v_Travail_Par_Jour[j] == 0 && j < i_Nb_Jour)
						j++;
				}
				//si on est a la toute fin de l'horizon
				else if (j > i_Nb_Jour - i_Nb_Shift_Min)
				{
					//on laisse tel quel
					while (j < i_Nb_Jour)
						j++;
				}
				//sinon
				else
				{
					//on bouche le trou avec des -1
					while (v_Travail_Par_Jour[j] == 0)
					{
						v_Travail_Par_Jour[j] = -1;
						j++;
					}
				}
			}

			//CAS 2 : entre les nombres de shift consecutif max et min
			else if (i_Nb_Jour_Affilee <= i_Nb_Shift_Max)
			{
				//cout << "DEUX\n";
				//parfait on laisse comme �a
				j += i_Nb_Jour_Affilee;
			}

			//CAS 3 : plus que le nombre de shift consecutif max mais pas assez pour pouvoir le s�parer en deux suite de shit de taille min
			else if (i_Nb_Jour_Affilee < 2*i_Nb_Shift_Min + i_Nb_Conge_Min)
			{
				//cout << "TROIS\n";
				//on rajoute des conges � la position j jusqu'� ce que la suite de 0 qui suit soit de la bonne taille
				for (int i = 0; i < i_Nb_Jour_Affilee - i_Nb_Shift_Max; i++)
				{
					v_Travail_Par_Jour[j] = -1;
					j++;
				}
			}

			//CAS 4 : assez pour pouvoir rajouter un conge entier mais oblige d'avoir une suite de shift de taille minimum
			else if (i_Nb_Jour_Affilee < i_Nb_Shift_Max + i_Nb_Shift_Min + i_Nb_Conge_Min)
			{
				//cout << "QUATRE\n";
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
				//cout << "CINQ\n";
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
		//si on arrive au bout du vecteur on s'arr�te
		if ((dir && i >= i_Nb_Jour) || (!dir && i < 0))
			fini = true;
		//m�me chose si on tombe sur -1
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

