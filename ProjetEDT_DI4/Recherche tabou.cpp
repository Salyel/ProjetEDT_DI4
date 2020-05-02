#include "Recherche tabou.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

/* Constructeur de la classe RechercheTabou, les arguments taille_liste_tabue, nb_iteration_max et aspiration permettent de modifier le fonctionnement de l'algorithme.
 * int taille_liste_taboue : la taille de notre liste taboue, correspondant au nombre de mouvements différents que nous stockons dans notre liste
 * int nb_iteration_max : le nombre d'itération de l'algorithme sans amélioration avant l'arrêt
 * bool aspiration : si aspiration est égal à true, on peut choisir un voisin même si le mouvement correspondant est présent dans la liste taboue si et seulement si ce voisin est meilleur que la meilleure solution
 */
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

//La fonction qui lance la rechercheTabou sur l'instance à partir de la solution_initiale
Solution* RechercheTabou::rechercheTabou()
{
    Solution solution_courante = *solution_initiale;
    Solution solution_meilleure = *solution_initiale;
    Solution solution_voisine = *solution_initiale;
    Solution solution_meilleure_voisine = Solution();

    int i = 0;
    int tampon = 0;
    int index_employe;
    int index_jour1;
    int index_jour2;

    vector<int> mouvement_utilise;

    while (i < nb_iteration_max)
    {
        //Notre opérateur de voisinage est le suivant : pour tous les employés, on tente toutes les possibilités de swap entre deux jours
        for (index_employe = 0; index_employe < solution_courante.v_v_IdShift_Par_Personne_et_Jour.size(); index_employe++)
        {
            for (index_jour1 = 0; index_jour1 < solution_courante.v_v_IdShift_Par_Personne_et_Jour[index_employe].size(); index_jour1++)
            {
                for (index_jour2 = 0; index_jour2 < solution_courante.v_v_IdShift_Par_Personne_et_Jour[index_employe].size(); index_jour2++)
                {
                    vector<int> mouvement_actuel = { index_employe, index_jour1, index_jour2 };
                    if ((index_jour1 != index_jour2 && !presenceMouvement(mouvement_actuel)) || aspiration)
                    {
                        solution_voisine.v_v_IdShift_Par_Personne_et_Jour[index_employe][index_jour2] = solution_courante.v_v_IdShift_Par_Personne_et_Jour[index_employe][index_jour1];
                        solution_voisine.v_v_IdShift_Par_Personne_et_Jour[index_employe][index_jour1] = solution_courante.v_v_IdShift_Par_Personne_et_Jour[index_employe][index_jour2];
                        if (!presenceMouvement(mouvement_actuel))                                                               //Si le mouvement n'est pas dans notre liste taboue
                        {
                            if (valeurVoisin(solution_voisine) < solution_meilleure_voisine.i_valeur_fonction_objectif)         //On teste si la valeur du voisin est meilleure que notre meilleur
                            {                                                                                                   //voisin, pour ne pas perdre de temps à regarder si une moins bonne 
                                if (validiteVoisin(solution_voisine, index_employe))                                            //solution est valide ou non
                                {
                                    solution_meilleure_voisine = solution_voisine;
                                    mouvement_utilise = mouvement_actuel;
                                }
                            }
                        }
                        else if (aspiration)                                                                                    //Sinon si le critère d'aspiration est activé
                        {
                            if (valeurVoisin(solution_voisine) < solution_meilleure.i_valeur_fonction_objectif)                 //On vérifie si notre mouvement déjà utilisé améliore la meilleure solution
                            {
                                if (validiteVoisin(solution_voisine, index_employe))
                                {
                                    solution_meilleure_voisine = solution_voisine;
                                    mouvement_utilise = mouvement_actuel;
                                }
                            }
                        }
                    }
                }
            }
        }
        solution_courante = solution_meilleure_voisine;
        ajouterElement(mouvement_utilise);
        if (solution_courante.i_valeur_fonction_objectif < solution_meilleure.i_valeur_fonction_objectif)
        {
            solution_meilleure = solution_courante;
            i = 0;
        }
        i++;
	}
	return new Solution(solution_meilleure);
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

//renvoie true si le mouvement ou le mouvement inverse est présent dans la liste taboue
bool RechercheTabou::presenceMouvement(vector<int> mouvement)
{
    bool present = false;
    vector<int> mouvement_inverse = { mouvement[0], mouvement[2], mouvement[1] };

    if (find(liste_taboue.begin(), liste_taboue.end(), mouvement) != liste_taboue.end() || find(liste_taboue.begin(), liste_taboue.end(), mouvement_inverse) != liste_taboue.end()) // pas sûr de cette ligne
    {
        present = true;
    }

    return present;
}

int RechercheTabou::valeurVoisin(Solution voisin)
{
    //Vérification de la fonction objectif
    int i_fc_obj = 0;
    vector<vector<int>> v_i_nb_personne_par_Shift_et_jour(instance->get_Nombre_Shift(), vector<int>(instance->get_Nombre_Jour(), 0));

    //Récupération du nombre de personne par shift et par jour
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

    //Vérification de si les employés ont bien leurs shifts préférés ou non
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
    //Vérification du nombre de personne par shift et par jour 
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
    voisin.i_valeur_fonction_objectif = i_fc_obj;
    return i_fc_obj;
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
    for (j = 0; j < instance->get_Nombre_Shift(); j++)
    {
        if (v_i_Nb_shift[j] > instance->get_Personne_Shift_Nbre_Max(numero_employe, j))
        {
            validite = false;
        }
    }

	return validite;
}

//Permet de modifier l'instance stockée dans la classe, utile lorsqu'on lance plusieurs instances d'un seul coup
void RechercheTabou::setInstance(Instance* instance)
{
    this->instance = instance;
}

//Permet de modifier la solution initiale stockée dans la classe, utile lorsqu'on lance plusieurs instances d'un seul coup ou si on veut tester différents heuristiques
void RechercheTabou::setSolutionInitiale(Solution* solution)
{
    this->solution_initiale = new Solution(*solution);
}