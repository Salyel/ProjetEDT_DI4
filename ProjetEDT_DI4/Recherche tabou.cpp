#include "Recherche tabou.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>


RechercheTabou::RechercheTabou(Instance* instance, int taille_liste_taboue, Solution* solution_initiale, int nb_iteration_max, bool aspiration)
{
	this->instance = instance;
	this->taille_liste_taboue = taille_liste_taboue;
	this->solution_initiale = new Solution(*solution_initiale);
	this->nb_iteration_max = nb_iteration_max;
	this->aspiration = aspiration;
}

RechercheTabou::~RechercheTabou()
{
	liste_taboue.clear();
	liste_taboue.shrink_to_fit();
}


Solution* RechercheTabou::rechercheTabou()
{
	Solution solution_courante = *solution_initiale;
	Solution solution_meilleure = *solution_initiale;
	Solution solution_voisine = Solution();
	Solution solution_meilleure_voisine = Solution();

	int i = 0;
    int index_employe;
    int index_jour1;
    int index_jour2;

	while (i < nb_iteration_max)
	{
        //Notre op�rateur de voisinage est le suivant : pour tous les employ�s, on tente toutes les possibilit�s de swap entre deux jours
        for (index_employe = 0; index_employe < solution_courante.v_v_IdShift_Par_Personne_et_Jour.size(); index_employe++)
        {
            for (index_jour1 = 0; index_jour1 < solution_courante.v_v_IdShift_Par_Personne_et_Jour[index_employe].size(); index_jour1++)
            {
                for (index_jour2 = 0; index_jour2 < solution_courante.v_v_IdShift_Par_Personne_et_Jour[index_employe].size(); index_jour2++)
                {
                    vector<int> mouvement_actuel = {index_employe, index_jour1, index_jour2};
                    if ((index_jour1 != index_jour2 && !presenceMouvement(mouvement_actuel)) || aspiration)
                    {
                        //Je dois coder ici
                    }
                }
            }
        }
	}
	return new Solution();
}

//Permet d'ajouter un �l�ment dans la liste taboue, fonctionne comme une FIFO
void RechercheTabou::ajouterElement(vector<int> tabou)
{
	if (liste_taboue.size() == taille_liste_taboue)
	{
		liste_taboue.pop_back();
	}
	liste_taboue.insert(liste_taboue.begin(), tabou);
}

//renvoie true si le mouvement ou le mouvement inverse est pr�sent dans la liste taboue
bool RechercheTabou::presenceMouvement(vector<int> mouvement)
{
    bool present = false;
    vector<int> mouvement_inverse = { mouvement[0], mouvement[2], mouvement[1] };

    if (find(liste_taboue.begin(), liste_taboue.end(), mouvement) != liste_taboue.end() || find(liste_taboue.begin(), liste_taboue.end(), mouvement_inverse) != liste_taboue.end()) // pas s�r de cette ligne
    {
        present = true;
    }

    return present;
}

int RechercheTabou::valeurVoisin(Solution voisin)
{
    //V�rification de la fonction objectif
    int i_fc_obj = 0;
    vector<vector<int>> v_i_nb_personne_par_Shift_et_jour(instance->get_Nombre_Shift(), vector<int>(instance->get_Nombre_Jour(), 0));

    //R�cup�ration du nombre de personne par shift et par jour
    for (int i = 0; i < voisin.v_v_IdShift_Par_Personne_et_Jour.size(); i++)
    {
        for (int j = 0; j < voisin.v_v_IdShift_Par_Personne_et_Jour[i].size(); j++)
        {
            if (voisin.v_v_IdShift_Par_Personne_et_Jour[i][j] != -1)
            {
                v_i_nb_personne_par_Shift_et_jour[voisin.v_v_IdShift_Par_Personne_et_Jour[i][j]][j] = v_i_nb_personne_par_Shift_et_jour[voisin.v_v_IdShift_Par_Personne_et_Jour[i][j]][j] + 1;
            }
        }
    }

    //V�rification de si les employ�s ont bien leurs shifts pr�f�r�s ou non
    for (int i = 0; i < voisin.v_v_IdShift_Par_Personne_et_Jour.size(); i++)
    {
        for (int j = 0; j < voisin.v_v_IdShift_Par_Personne_et_Jour[i].size(); j++)
        {
            if (voisin.v_v_IdShift_Par_Personne_et_Jour[i][j] != -1)
            {
                i_fc_obj = i_fc_obj + instance->get_Poids_Refus_Pers_Jour_Shift(i, j, voisin.v_v_IdShift_Par_Personne_et_Jour[i][j]);
            }
            for (int k = 0; k < instance->get_Nombre_Shift(); k++)
            {
                if (voisin.v_v_IdShift_Par_Personne_et_Jour[i][j] != k)
                    i_fc_obj = i_fc_obj + instance->get_Poids_Affectation_Pers_Jour_Shift(i, j, k);
            }
        }
    }
    //V�rification du nombre de personne par shift et par jour 
    for (int k = 0; k < instance->get_Nombre_Shift(); k++)
    {
        for (int j = 0; j < instance->get_Nombre_Jour(); j++)
        {
            if (v_i_nb_personne_par_Shift_et_jour[k][j] < instance->get_Nbre_Personne_Requis_Jour_Shift(j, k))
                i_fc_obj = i_fc_obj + instance->get_Poids_Personne_En_Moins_Jour_Shift(j, k) * (instance->get_Nbre_Personne_Requis_Jour_Shift(j, k) - v_i_nb_personne_par_Shift_et_jour[k][j]);
            if (v_i_nb_personne_par_Shift_et_jour[k][j] > instance->get_Nbre_Personne_Requis_Jour_Shift(j, k))
                i_fc_obj = i_fc_obj + instance->get_Poids_Personne_En_Plus_Jour_Shift(j, k) * (v_i_nb_personne_par_Shift_et_jour[k][j] - instance->get_Nbre_Personne_Requis_Jour_Shift(j, k));
        }
    }
    return i_fc_obj;
}

//On calcule la valeur avant de v�rifier si c'est valide, si sa valeur n'est pas inf�rieure � la meilleure valeur des voisins actuels : pas besoin de v�rifier si c'est valide.
//Renvoie false si toutes les contraintes de la nouvelle solution ne sont pas respect�es, true sinon
bool RechercheTabou::validiteVoisin(Solution voisin, int numero_employe)
{
    int j = 0;
	int validite = true;
    int i_shift_consecutif = 0;
    int i_duree_travail = 0;
    int i_nb_WE = 0;
    vector<int> v_i_Nb_shift(instance->get_Nombre_Shift(), 0);

    while (j < voisin.v_v_IdShift_Par_Personne_et_Jour[numero_employe].size() && validite != -1)
    {
        if (voisin.v_v_IdShift_Par_Personne_et_Jour[numero_employe][j] != -1)
        {
            v_i_Nb_shift[voisin.v_v_IdShift_Par_Personne_et_Jour[numero_employe][j]] = v_i_Nb_shift[voisin.v_v_IdShift_Par_Personne_et_Jour[numero_employe][j]] + 1;
            i_duree_travail = i_duree_travail + instance->get_Shift_Duree(voisin.v_v_IdShift_Par_Personne_et_Jour[numero_employe][j]);
            i_shift_consecutif++;
            if ((j % 7) == 5)
                i_nb_WE++;
            if (((j % 7) == 6) && (voisin.v_v_IdShift_Par_Personne_et_Jour[numero_employe][j - 1] == -1))
                i_nb_WE++;
            //V�rification du nombre de shifts cons�cutifs maximum assignable � notre employe
            if (i_shift_consecutif > instance->get_Personne_Nbre_Shift_Consecutif_Max(numero_employe))
            {
                validite = false;
            }

            //V�rification des jours de cong�s de notre employe
            if (!instance->is_Available_Personne_Jour(numero_employe, j))
            {
                validite = false;
            }
        }
        else
        {
            //V�rification du nombre de shifts cons�cutifs minimum assignable � notre employe
            if ((i_shift_consecutif < instance->get_Personne_Nbre_Shift_Consecutif_Min(numero_employe)) && (i_shift_consecutif != 0) && ((j - instance->get_Personne_Nbre_Shift_Consecutif_Min(numero_employe)) > 0))
            {
                validite = false;
            }
            i_shift_consecutif = 0;
        }

        //V�rification des successions des Shifts
        if (j != (voisin.v_v_IdShift_Par_Personne_et_Jour[numero_employe].size() - 1))
        {
            if ((voisin.v_v_IdShift_Par_Personne_et_Jour[numero_employe][j] != -1) && (voisin.v_v_IdShift_Par_Personne_et_Jour[numero_employe][j + 1] != -1))
            {
                if (!instance->is_possible_Shift_Succede(voisin.v_v_IdShift_Par_Personne_et_Jour[numero_employe][j], voisin.v_v_IdShift_Par_Personne_et_Jour[numero_employe][j + 1]))
                {
                    validite = false;
                }
            }
        }
        j++;
    }
    //V�rification de la dur�e totale maximale et minimale de notre employe
    if (i_duree_travail > instance->get_Personne_Duree_total_Max(numero_employe))
    {
        validite = false;
    }
    if (i_duree_travail < instance->get_Personne_Duree_total_Min(numero_employe))
    {
        validite = false;
    }
    //V�rification du nombre de WE (samedi ou/et dimanche) de travail maximal pour notre employe
    if (i_nb_WE > instance->get_Personne_Nbre_WE_Max(numero_employe))
    {
        validite = false;
    }
    //V�rification du nombre maximal de shifts de notre employe
    if (v_i_Nb_shift[j] > instance->get_Personne_Shift_Nbre_Max(numero_employe, j))
    {
        validite = false;
    }

	return validite;
}

void RechercheTabou::setInstance(Instance* instance)
{
    this->instance = instance;
}

void RechercheTabou::setSolutionInitiale(Solution* solution)
{
    this->solution_initiale = new Solution(*solution);
}