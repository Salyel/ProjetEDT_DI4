#include "Heuristique.hpp"
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

	//récupération des données de l'instance
	int i_Nb_Employe = instance->get_Nombre_Personne();
	int i_Nb_Jour = instance->get_Nombre_Jour();
	int i_Nb_Shift = instance->get_Nombre_Shift();

	//initialisation du vecteur de solution et d'un vecteur représentant le nombre de personnes actuellement affectées à chaque shift
	s->v_v_IdShift_Par_Personne_et_Jour = vector<vector<int>>(i_Nb_Employe, vector<int>());
	vector<vector<int>> nb_Personne_Par_Jour_Par_Shift = vector<vector<int>>(i_Nb_Jour, vector<int>(i_Nb_Shift, 0));

	//pour chaque employe
	for (int e = 0; e < i_Nb_Employe; e++)
	{
		// ETAPE 1 : on détermine quels jours sont travailles et quels jours sont des conges

		vector<int> v_Horizon_Employe = Heuristique::jours_Travailles_Par_Personne(e);

		// ETAPE 2 : pour chaque jour on determine quelle est la meilleure shift a attribuer

		//initialisation variables
		int duree_Travail_Min = instance->get_Personne_Duree_total_Min(e), duree_Travail_Max = instance->get_Personne_Duree_total_Max(e), duree_Travail_Actuelle = 0;
		nb_Shift_Par_Type = vector<int>(i_Nb_Shift, 0);

		//pour chaque jour de l'horizon
		for (int j = 0; j < i_Nb_Jour; j++)
		{
			//si on a pas etabli que le jour serait un jour de repos
			if(v_Horizon_Employe[j] != -1) {

				//on etablit la liste des shifts "ajoutables"
				vector<int> v_Shift_Ajoutables;
				for (int shift = 0; shift < i_Nb_Shift; shift++)
				{
					//on ne s'occupe pas de la shift suivante, et pas de la shift precedente si on est au jour 0
					int shift_Prec = -1, shift_Suiv = -1;
					if (j > 0)
						shift_Prec = v_Horizon_Employe[j-1];
				
					//liste des conditions pour que des shifts puissent se suivre
					if (duree_Travail_Actuelle + instance->get_Shift_Duree(shift) <= duree_Travail_Max //est-ce que ajouter la shift ne fait pas depasser la duree de travail
						&& improved_Shift_Succede(shift_Prec, shift, shift_Suiv) //est-ce que la shit peut suivre la shift precedente
						&& nb_Shift_Par_Type[shift] + 1 <= instance->get_Personne_Shift_Nbre_Max(e, shift)) // est-ce qu'on a pas atteint le nombre de shift max de ce type

						v_Shift_Ajoutables.push_back(shift);
				}

				
				int shift_A_Ajouter = -1; //la valeur par défaut, au cas ou on ne trouve pas de shift a ajouter est -1
				int nb_Shift_Ajoutables = v_Shift_Ajoutables.size(); //on regarde le nombre de shift ajoutables

				//si il y a une seule shift ajoutable
				if (nb_Shift_Ajoutables == 1)
					shift_A_Ajouter = v_Shift_Ajoutables[0];
				//si il y a plus d'une shift ajoutable
				else if (nb_Shift_Ajoutables > 1)
				{
					//on cherche si l'employe a une shift preferee ce jour la
					for (int i = 0; i < nb_Shift_Ajoutables && shift_A_Ajouter == -1; i++)
					{
						if (instance->get_Poids_Affectation_Pers_Jour_Shift(e, j, v_Shift_Ajoutables[i]) > 0)
							shift_A_Ajouter = v_Shift_Ajoutables[i];
					}

					//si on a pas trouve de shift preferee
					if (shift_A_Ajouter == -1)
					{
						int valeur_Shift_Max = -999999;
						//on cherche la """meilleure""" shift selon deux critères :
						for (int i = 0; i < nb_Shift_Ajoutables; i++)
						{
							//le nombre de personne manquante pour atteindre le nombre cible de la shift
							int nb_Manquant = instance->get_Nbre_Personne_Requis_Jour_Shift(j, v_Shift_Ajoutables[i]) - nb_Personne_Par_Jour_Par_Shift[j][v_Shift_Ajoutables[i]];
							//le nombre de shift de ce type que l'employe peut faire avant d'atteindre sa valeur max
							int nb_Shift_Par_Type_Restant = instance->get_Personne_Shift_Nbre_Max(e, v_Shift_Ajoutables[i]) - nb_Shift_Par_Type[v_Shift_Ajoutables[i]];

							int valeur_Shift = nb_Manquant + nb_Shift_Par_Type_Restant;
							if (valeur_Shift >= valeur_Shift_Max)
							{
								valeur_Shift_Max = valeur_Shift;
								shift_A_Ajouter = v_Shift_Ajoutables[i];
							}
						}
					}
				}

				//si on a trouve une shift a ajouter
				if (shift_A_Ajouter != -1)
				{
					//on met a jour les donnees liees a l'ajout de la shift
					duree_Travail_Actuelle += instance->get_Shift_Duree(shift_A_Ajouter);
					nb_Personne_Par_Jour_Par_Shift[j][shift_A_Ajouter]++;
					nb_Shift_Par_Type[shift_A_Ajouter]++;
				}
				//dans tous les cas on met à jour la valeur de la shift
				v_Horizon_Employe[j] = shift_A_Ajouter;
			}
		}
		
		// ETAPE 3 : La rectification

		//suite a l'etape precedente, certaines shift prevues comme travaillees ont dues etre mises en conges pour deux raisons :
		//- temps de travail max atteint
		//- nbre de shift max d'un ou plusieurs types atteint

		//cela a cree des inconsistances au niveau des shift consecutives minimum qui doivent être rectifiee
		//pour ce faire on va echanger des jours de travail avec des jours de conges jusqu'à ce que la solution soit correcte

		//initialisation de variables;
		int i_Nb_Shift_Min = instance->get_Personne_Nbre_Shift_Consecutif_Min(e);
		bool solution_Correcte = false;
		int ite = 0;

		//tant qu'on a pas fait une iteration ou la solution est completement correcte ou au bout de 10 iterations (afin d'eviter les boucles infinies)
		while (!solution_Correcte && ite < 10)
		{
			solution_Correcte = true; //on considere la solution comme correcte par defaut
			for (int j = 1; j < i_Nb_Jour; j++)
			{
				//chaque fois qu'on arrive sur un jour de conge
				if (v_Horizon_Employe[j] == -1)
				{
					int nb_Shift_Prec = nb_Shift_Affilee(v_Horizon_Employe, j - 1, false), nb_Shift_Suiv = nb_Shift_Affilee(v_Horizon_Employe, j + 1, true);
					//si le nombre de shift de travail le precedant ou le suivant sont incorrects
					if ((nb_Shift_Prec > 0 && nb_Shift_Prec < i_Nb_Shift_Min && j > i_Nb_Shift_Min - 1)
						|| (nb_Shift_Suiv > 0 && nb_Shift_Suiv < i_Nb_Shift_Min && v_Horizon_Employe[j - 1] != -1 && j < i_Nb_Jour - i_Nb_Shift_Min))
					{
						solution_Correcte = false; //la solution est consideree comme fausse fausse
						
						//on tente de corriger, le nouveau jour de travail peut avoir une shift differente
						bool corrige = echange_Jour_Travail(v_Horizon_Employe, e, j, false);

						//si ça a echoue, on tente de corriger en remplaçant forcement par la meme shift
						//ça veut dire qu'on a pas a prendre en compte le nbre de shift max par type et donc d'explorer plus d'options
						if (!corrige)
							echange_Jour_Travail(v_Horizon_Employe, e, j, true);

					}
				}
			}

			ite++; //pour empêcher les boucles infinies
		}

		//ajout du vecteur dans la solution
		s->v_v_IdShift_Par_Personne_et_Jour[e] = v_Horizon_Employe;
	}

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

	// ETAPE 1 : les jours de conges reserves

	//pour chaque jour
	for (j = 0; j < i_Nb_Jour; j++)
	{
		//si un jour de conge est demande et qu'il n'est pas deja a -1 (ça peut arriver quand on a plusieurs conge a la suite)
		if (!instance->is_Available_Personne_Jour(id_Employe, j) && v_Travail_Par_Jour[j] != -1)
		{
			//tout le bloc de code suivant sert a determiner si les conges necessaires a respecter le nbre de conge off min seront avant ou apres le jour j
			//on elimine donc les cas ou ce n'est pas necessaire de decider
			if ((j > 0 ? v_Travail_Par_Jour[j - 1] != -1 : false) && i_Nb_Conge_Min > 1)
			{
				int dir;
				//si la personne est aussi en conge le lendemain on place les conges apres
				if (j + 1 < i_Nb_Jour ? !instance->is_Available_Personne_Jour(id_Employe, j + 1) : false)
					dir = 1;
				//si on est au tout debut de l'instance on plae les conges avant
				else if (j - i_Nb_Conge_Min < -1)
					dir = -1;
				//si on est a la toute fin de l'instance on place les conges apres
				else if (j + i_Nb_Conge_Min >= i_Nb_Jour)
					dir = 1;
				//si placer les conges apres permet de recouvrir un WE, on place les conges apres
				else if ((j % 7) + i_Nb_Conge_Min >= 7  && nb_Shift_Affilee(v_Travail_Par_Jour, j, true) >= i_Nb_Shift_Min + i_Nb_Conge_Min)
					dir = 1;
				//si placer les conges avant permet de recouvrir un WE, on place les conges avant
				else if (j > 0 && ((j % 7) - i_Nb_Conge_Min <= -2 || (j - i_Nb_Conge_Min) % 7 == 6) && nb_Shift_Affilee(v_Travail_Par_Jour, j, false) >= i_Nb_Shift_Min + i_Nb_Conge_Min)
					dir = -1;
				//on arrive dans un cas un peu par défaut, si il y a assez de place pour mettre les conges avant, on le fait
				else if (nb_Shift_Affilee(v_Travail_Par_Jour, j, false) >= i_Nb_Shift_Min + i_Nb_Conge_Min)
					dir = -1;
				//sinon on place les conges apres
				else 
					dir = 1;

				//on met les jours precedents/suivants en conge
				int i = j + dir;
				while (i != j + (dir * i_Nb_Conge_Min) && i >= 0 && i < i_Nb_Jour)
				{
					v_Travail_Par_Jour[i] = -1;
					i += dir;
				}
			}
			//on met le jour en conge
			v_Travail_Par_Jour[j] = -1;
		}
	}

	// ETAPE 2 : les WE non travailles

	//au debut le nombre WE travailles est egal au nombre de semaines dans l'horizon
	int i_Nb_WE_Travail = i_Nb_Jour / 7;

	//si il y a deja des week end en conge on les retire
	for (j = 6; j < i_Nb_Jour; j += 7)
	{
		//si il ya deja de WE qui sont en conge on les retire
		if (v_Travail_Par_Jour[j] == -1 && v_Travail_Par_Jour[j - 1] == -1)
		{
			i_Nb_WE_Travail--;
		}
	}

	for (j = 6; j < i_Nb_Jour; j += 7)
	{
		//si on encore trop de WE travailles
		if (i_Nb_WE_Travail > i_Nb_We_Travail_Max)
		{
			//si le WE n'est pas deja en conge
			if(v_Travail_Par_Jour[j] == 0 || v_Travail_Par_Jour[j - 1] == 0)
			{
				//la taille min d'un WE est de 2 meme si le nb de shift consecutif min est de 1
				int taille_WE_Min = i_Nb_Conge_Min >= 2 ? i_Nb_Conge_Min : 2;

				//doit on mettre les conges avant ou apres
				int dir = 0;
				//si on peut les faire coincider avec d'autres conges avant on les met avant
				if (nb_Shift_Affilee(v_Travail_Par_Jour, j, false) <= taille_WE_Min && nb_Shift_Affilee(v_Travail_Par_Jour, j+1, true) >= i_Nb_Shift_Min)
					dir = -1;
				//si on peut les faire coincider avec d'autres conges après on les met après
				else if (nb_Shift_Affilee(v_Travail_Par_Jour, j - 1, true) <= taille_WE_Min && nb_Shift_Affilee(v_Travail_Par_Jour, j - 2, false) >= i_Nb_Shift_Min)
					dir = 1;
				//sinon si on peut les mettre avant on les met avant
				else if (nb_Shift_Affilee(v_Travail_Par_Jour, j, false) >= taille_WE_Min + i_Nb_Shift_Min && nb_Shift_Affilee(v_Travail_Par_Jour, j + 1, true) >= i_Nb_Shift_Min)
					dir = -1;
				//sinon si on peut les mettre après on les met après
				else if (nb_Shift_Affilee(v_Travail_Par_Jour, j - 1, true) >= taille_WE_Min + i_Nb_Shift_Min && nb_Shift_Affilee(v_Travail_Par_Jour, j - 2, false) >= i_Nb_Shift_Min)
					dir = 1;

				//si dir = 0 on ne peut pas mettre de WE a cet endroit
				if (dir != 0)
				{
					//si on les met avant on commence par dimanche, sinon on commence par samedi
					int i = dir == -1 ? j : j - 1;
					while (i != j + (dir * taille_WE_Min) && i >= 0 && i < i_Nb_Jour)
					{
						v_Travail_Par_Jour[i] = -1;
						i += dir;
					}
					//on a un WE de travail en moins
					i_Nb_WE_Travail--;
				}
			}
		}
	}


	// ETAPE 3 : les jours OFF nececssaires pour avoir le bon nombre de shift de travail consecutifs 

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
				//parfait on laisse comme ça
				j += i_Nb_Jour_Affilee;
			}

			//CAS 3 : plus que le nombre de shift consecutif max mais pas assez pour pouvoir le séparer en deux suites de shift de taille min
			else if (i_Nb_Jour_Affilee < i_Nb_Shift_Max + i_Nb_Conge_Min)
			{
				//on rajoute des conges à la position j jusqu'à ce que la suite de 0 qui suit soit de la bonne taille
				for (int i = 0; i < i_Nb_Jour_Affilee - i_Nb_Shift_Max; i++)
				{
					v_Travail_Par_Jour[j] = -1;
					j++;
				}
			}

			//CAS 4 : assez pour pouvoir rajouter un conge entier mais oblige d'avoir une suite de shift de taille mini
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

	return v_Travail_Par_Jour;
}

bool Heuristique::echange_Jour_Travail(vector<int>& v_Horizon_Employe, int id_Employe, int id_Jour, bool meme_Shift)
{
	//recuperation de donnees de l'instance
	int i_Nb_Shift = instance->get_Nombre_Shift();
	int i_Nb_Jour = instance->get_Nombre_Jour();
	int i_Nb_Shift_Min = instance->get_Personne_Nbre_Shift_Consecutif_Min(id_Employe);

	//initialisation de la valeur de retour
	bool corrige = false;

	//on etablit la liste des shift ajoutables
	vector<int> v_Shift_Ajoutables;
	for (int shift = 0; shift < i_Nb_Shift; shift++)
	{
		int shift_Prec = -1, shift_Suiv = -1;
		
		//si on est au jour 0 on ne prend pas en compte la shift precedente
		if (id_Jour > 0)
			shift_Prec = v_Horizon_Employe[id_Jour - 1];

		//si on est au jour nb_Jour - 1 on ne prend pas en compte la shift suivante
		if (id_Jour < i_Nb_Jour - 1)
			shift_Suiv = v_Horizon_Employe[id_Jour + 1];

		if (improved_Shift_Succede(shift_Prec, shift, shift_Suiv)//dans tous les cas on verifie que les shifts se suivent
			&& (meme_Shift || nb_Shift_Par_Type[shift] + 1 <= instance->get_Personne_Shift_Nbre_Max(id_Employe, shift)))//si meme_Shift est vrai, pas la peine de verifier le nbre de shift max d'un type
			v_Shift_Ajoutables.push_back(shift);
	}
	
	//on regarde le nombre de shift ajoutable
	int nb_Shift_Ajoutables = v_Shift_Ajoutables.size();
	//si il y en a au moins une
	if (nb_Shift_Ajoutables > 0)
	{
		int k = 0;
		while (k < i_Nb_Jour && !corrige)
		{
			//tant qu'on est sur des conges on passe
			if (v_Horizon_Employe[k] == -1)
				k++;
			//une fois qu'on a trouve un jour travaille
			else
			{
				//si il y a suffisamment de shift consecutif pour qu'on puisse en enlever un
				int n = nb_Shift_Affilee(v_Horizon_Employe, k, true);
				if (n > i_Nb_Shift_Min)
				{
					int i = 0;
					//on parcours les shift une à une tant qu'on a pas corrige
					while (i < nb_Shift_Ajoutables && !corrige)
					{
						//si meme_Shift est vrai, on a qu'a verifier qu'on a la meme shift que celle qu'on veut ajouter
						//sinon on doit verifier que l'ajout ne fera pas depasser le temps de travail max
						if ((meme_Shift && v_Horizon_Employe[k] == v_Shift_Ajoutables[i])
							|| (!meme_Shift && instance->get_Shift_Duree(v_Horizon_Employe[k]) >= instance->get_Shift_Duree(v_Shift_Ajoutables[i])))
						{
							//si meme_Shift est faux on update les nb de shift de chaque type par personne
							if (!meme_Shift)
							{
								nb_Shift_Par_Type[v_Horizon_Employe[k]]--;
								nb_Shift_Par_Type[v_Shift_Ajoutables[i]]++;
							}

							//on fait "l'echange"
							v_Horizon_Employe[k] = -1;
							v_Horizon_Employe[id_Jour] = v_Shift_Ajoutables[i];

							//on met corrige a true pour indiquer qu'on a reussi
							corrige = true;
						}
						//si jamais on ne peut pas echanger avec le premier jour de la suite de shift on essaie avec le dernier, d'ou le k + n - 1
						else if ((meme_Shift && v_Horizon_Employe[k + n - 1] == v_Shift_Ajoutables[i])
							|| (!meme_Shift && instance->get_Shift_Duree(v_Horizon_Employe[k + n - 1]) >= instance->get_Shift_Duree(v_Shift_Ajoutables[i])))
						{
							//si meme_Shift est faux on update les nb de shift de chaque type par personne
							if (!meme_Shift)
							{
								nb_Shift_Par_Type[v_Horizon_Employe[k + n - 1]]--;
								nb_Shift_Par_Type[v_Shift_Ajoutables[i]]++;
							}

							//on fait "l'echange"
							v_Horizon_Employe[k + n - 1] = -1;
							v_Horizon_Employe[id_Jour] = v_Shift_Ajoutables[i];

							//on met corrige a true pour indiquer qu'on a reussi
							corrige = true;
						}
						i++;
					}
				}
				//on sort de la suite de shift
				k += n;
			}
		}
	}

	//on renvoie corrige pour dire si on a reussi a faire l'echange ou pas
	return corrige;
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

bool Heuristique::improved_Shift_Succede(int shift_Prev, int shift, int shift_Suiv)
{
	//par defaut on renvoie true
	bool res = true;

	//si la shift precedente vaut -1 on renvoie true car n'importe quelle shift peut la suivre
	if (shift_Prev > -1)
		//sinon on reutilise la methode donnee
		if (!instance->is_possible_Shift_Succede(shift_Prev, shift))
			res = false;

	//si la shift suivante vaut -1 on renvoie true car n'importe quelle shift peut la preceder
	if (shift_Suiv > -1)
		//sinon on reutiise la methode donnee
		if (!instance->is_possible_Shift_Succede(shift, shift_Suiv))
			res = false;

	return res;
}

