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

//Pour l'instant, je pars du principe que Solution->i_valeur_fonction_objectif est bien rempli alors que ce n'est pas le cas. Il faudra calculer la valeur de la 
// fonction objectif quand on pourra le faire ! 
Solution* RechercheTabou::rechercheTabou()
{
	Solution solution_courante = *solution_initiale;
	Solution solution_meilleure = *solution_initiale;
	Solution solution_voisine = Solution();
	Solution solution_meilleure_voisine = Solution();
	int i = 0;

	while (i < nb_iteration_max)
	{

	}
	return new Solution();
}

//Permet d'ajouter un élément dans la liste taboue, fonctionne comme une FIFO
void RechercheTabou::ajouterElement(vector<int> tabou)
{
	if (liste_taboue.size() == taille_liste_taboue)
	{
		liste_taboue.pop_back();
	}
	liste_taboue.insert(liste_taboue.begin(), tabou);
}

int RechercheTabou::valeurVoisin(Solution voisin)
{
    //Vérification de la fonction objectif
   /* i_fc_obj = 0;
    for (i = 0; i < v_v_IdShift_Par_Personne_et_Jour.size(); i++)
    {
        for (j = 0; j < v_v_IdShift_Par_Personne_et_Jour[i].size(); j++)
        {
            if (v_v_IdShift_Par_Personne_et_Jour[i][j] != -1)
            {
                i_fc_obj = i_fc_obj + instance->get_Poids_Refus_Pers_Jour_Shift(i, j, v_v_IdShift_Par_Personne_et_Jour[i][j]);
            }
            for (k = 0; k < instance->get_Nombre_Shift(); k++)
            {
                if (v_v_IdShift_Par_Personne_et_Jour[i][j] != k)
                    i_fc_obj = i_fc_obj + instance->get_Poids_Affectation_Pers_Jour_Shift(i, j, k);
            }
        }
    }
    for (k = 0; k < instance->get_Nombre_Shift(); k++)
    {
        for (j = 0; j < instance->get_Nombre_Jour(); j++)
        {
            if (v_i_nb_personne_par_Shift_et_jour[k][j] < instance->get_Nbre_Personne_Requis_Jour_Shift(j, k))
                i_fc_obj = i_fc_obj + instance->get_Poids_Personne_En_Moins_Jour_Shift(j, k) * (instance->get_Nbre_Personne_Requis_Jour_Shift(j, k) - v_i_nb_personne_par_Shift_et_jour[k][j]);
            if (v_i_nb_personne_par_Shift_et_jour[k][j] > instance->get_Nbre_Personne_Requis_Jour_Shift(j, k))
                i_fc_obj = i_fc_obj + instance->get_Poids_Personne_En_Plus_Jour_Shift(j, k) * (v_i_nb_personne_par_Shift_et_jour[k][j] - instance->get_Nbre_Personne_Requis_Jour_Shift(j, k));
        }
    }*/
}

//On calcule la valeur avant de vérifier si c'est valide, si sa valeur n'est pas inférieure à la meilleure valeur des voisins actuels : pas besoin de vérifier si c'est valide.
//Renvoie false si toutes les contraintes de la nouvelle solution ne sont pas respectées, true sinon
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
            //Vérification du nombre de shifts consécutifs maximum assignable à notre employe
            if (i_shift_consecutif > instance->get_Personne_Nbre_Shift_Consecutif_Max(numero_employe))
            {
                validite = false;
            }

            //Vérification des jours de congés de notre employe
            if (!instance->is_Available_Personne_Jour(numero_employe, j))
            {
                validite = false;
            }
        }
        else
        {
            //Vérification du nombre de shifts consécutifs minimum assignable à notre employe
            if ((i_shift_consecutif < instance->get_Personne_Nbre_Shift_Consecutif_Min(numero_employe)) && (i_shift_consecutif != 0) && ((j - instance->get_Personne_Nbre_Shift_Consecutif_Min(numero_employe)) > 0))
            {
                validite = false;
            }
            i_shift_consecutif = 0;
        }

        //Vérification des successions des Shifts
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
    //Vérification de la durée totale maximale et minimale de notre employe
    if (i_duree_travail > instance->get_Personne_Duree_total_Max(numero_employe))
    {
        validite = false;
    }
    if (i_duree_travail < instance->get_Personne_Duree_total_Min(numero_employe))
    {
        validite = false;
    }
    //Vérification du nombre de WE (samedi ou/et dimanche) de travail maximal pour notre employe
    if (i_nb_WE > instance->get_Personne_Nbre_WE_Max(numero_employe))
    {
        validite = false;
    }
    //Vérification du nombre maximal de shifts de notre employe
    if (v_i_Nb_shift[j] > instance->get_Personne_Shift_Nbre_Max(numero_employe, j))
    {
        validite = false;
    }

	return validite;
}