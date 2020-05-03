#include "Recherche tabou.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <chrono>
#include <time.h>

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
    this->solution_initiale->i_valeur_fonction_objectif = valeurVoisin(*solution_initiale);
    this->nb_iteration_max = nb_iteration_max;
    this->aspiration = aspiration;
}

RechercheTabou::~RechercheTabou()
{
    liste_taboue_jours.clear();
    liste_taboue_jours.shrink_to_fit();
    liste_taboue_shifts.clear();
    liste_taboue_shifts.shrink_to_fit();
}

//La fonction qui lance la rechercheTabou sur l'instance à partir de la solution_initiale
Solution* RechercheTabou::rechercheTabou(chrono::time_point<chrono::system_clock> start)
{
    chrono::time_point<chrono::system_clock> chrono_start, chrono_end;
    srand(time(NULL));
    chrono_start = start;
    chrono_end = chrono::system_clock::now();
    chrono::duration<double> elapsed;
    elapsed = chrono_end - chrono_start;

    Solution solution_courante = *solution_initiale;
    Solution solution_meilleure = *solution_initiale;
    Solution solution_voisine = *solution_initiale;
    Solution solution_meilleure_voisine = *solution_initiale;

    liste_taboue_jours.clear();
    liste_taboue_jours.shrink_to_fit();
    liste_taboue_shifts.clear();
    liste_taboue_shifts.shrink_to_fit();

    int i = 0;
    int tampon = 0;
    int index_employe;
    int index_jour1;
    int index_jour2;
    int index_jour;
    int index_shift;

    vector<int> mouvement_utilise;
    while (i < nb_iteration_max && elapsed.count() < 180)
    {
        solution_meilleure_voisine = Solution();
        solution_meilleure_voisine.i_valeur_fonction_objectif = 999999999;
        vector<vector<int>> v_i_nb_personne_par_Shift_et_jour = calculNbPersonneShiftJour(solution_courante);
        vector<int> v_i_Nb_shift = calculNbShift(solution_courante);
        int duree_travail = calculDureeTravail(solution_courante);

        // Premier opérateur de voisinage : pour tous les employés, tous les jours, on essaie de changer le shift en un qui améliore la fonction objectif.
        // On effectue d'abord cet opérateur puisqu'il est beaucoup plus rapide que notre second.
        for (index_employe = 0; index_employe < solution_courante.v_v_IdShift_Par_Personne_et_Jour.size(); index_employe++)
        {
            for (index_jour = 0; index_jour < solution_courante.v_v_IdShift_Par_Personne_et_Jour[index_employe].size(); index_jour++)
            {
                for (index_shift = 0; index_shift < instance->get_Nombre_Shift(); index_shift++)
                {
                    vector<int> mouvement_actuel = { index_employe, index_jour, index_shift };
                    if (index_shift != solution_courante.v_v_IdShift_Par_Personne_et_Jour[index_employe][index_jour] && solution_courante.v_v_IdShift_Par_Personne_et_Jour[index_employe][index_jour] != -1 && (!presenceMouvement(mouvement_actuel) || aspiration))
                    {
                        solution_voisine = solution_courante;
                        solution_voisine.v_v_IdShift_Par_Personne_et_Jour[index_employe][index_jour] = index_shift;
                        if (!presenceMouvement(mouvement_actuel))                                                               //Si le mouvement n'est pas dans notre liste taboue
                        {
                            solution_voisine.i_valeur_fonction_objectif = nouvelleValeurVoisinShifts(solution_courante, index_employe, index_jour, index_shift, solution_courante.v_v_IdShift_Par_Personne_et_Jour[index_employe][index_jour], v_i_nb_personne_par_Shift_et_jour);
                            //solution_voisine.i_valeur_fonction_objectif = valeurVoisin(solution_voisine);
                            if (solution_voisine.i_valeur_fonction_objectif < solution_meilleure_voisine.i_valeur_fonction_objectif)         //On teste si la valeur du voisin est meilleure que notre meilleur
                            {                                                                                                                //voisin, pour ne pas perdre de temps à regarder si une moins bonne
                                if (validiteVoisin(solution_voisine, index_employe))                                                         //solution est valide ou non
                                //if (validiteVoisinShift(solution_courante, index_employe, index_jour, solution_courante.v_v_IdShift_Par_Personne_et_Jour[index_employe][index_jour], index_shift, duree_travail, v_i_Nb_shift))
                                //if (solution_voisine.Verification_Solution(instance))
                                {
                                    solution_meilleure_voisine = solution_voisine;
                                    mouvement_utilise = mouvement_actuel;
                                }
                            }
                        }
                        else if (aspiration)                                                                                    //Sinon si le critère d'aspiration est activé
                        {
                            solution_voisine.i_valeur_fonction_objectif = nouvelleValeurVoisinShifts(solution_courante, index_employe, index_jour, index_shift, solution_courante.v_v_IdShift_Par_Personne_et_Jour[index_employe][index_jour], v_i_nb_personne_par_Shift_et_jour);
                            //solution_voisine.i_valeur_fonction_objectif = valeurVoisin(solution_voisine);
                            if (solution_voisine.i_valeur_fonction_objectif < solution_meilleure.i_valeur_fonction_objectif)            //On vérifie si notre mouvement déjà utilisé améliore la meilleure solution
                            {
                                if (validiteVoisin(solution_voisine, index_employe))
                                //if (solution_voisine.Verification_Solution(instance))
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

        //Si le meilleur voisin suite à notre premier opérateur améliore la meilleure solution, on ne va pas utiliser le deuxième opérateur.
        if (solution_meilleure_voisine.i_valeur_fonction_objectif < solution_meilleure.i_valeur_fonction_objectif)
        {
            solution_meilleure = solution_meilleure_voisine;
            i = 0;
            ajouterElementShifts(mouvement_utilise);
            solution_courante = solution_meilleure_voisine;
        }
        else
        {
            //Notre deuxième opérateur de voisinage est le suivant : pour tous les employés, on tente toutes les possibilités de swap entre deux jours.
            //Comme dit en commentaire au dessus du premier opérateur, celui ci prend plus de temps mais trouve plus facilement une solution.
            for (index_employe = 0; index_employe < solution_courante.v_v_IdShift_Par_Personne_et_Jour.size(); index_employe++)
            {
                for (index_jour1 = 0; index_jour1 < solution_courante.v_v_IdShift_Par_Personne_et_Jour[index_employe].size(); index_jour1++)
                {
                    for (index_jour2 = 0; index_jour2 < solution_courante.v_v_IdShift_Par_Personne_et_Jour[index_employe].size(); index_jour2++)
                    {
                        vector<int> mouvement_actuel = { index_employe, index_jour1, index_jour2 };
                        if (index_jour1 != index_jour2 && solution_courante.v_v_IdShift_Par_Personne_et_Jour[index_employe][index_jour1] != solution_courante.v_v_IdShift_Par_Personne_et_Jour[index_employe][index_jour2] && (!presenceMouvementEtInverse(mouvement_actuel) || aspiration))
                        {
                            solution_voisine = solution_courante;
                            solution_voisine.v_v_IdShift_Par_Personne_et_Jour[index_employe][index_jour2] = solution_courante.v_v_IdShift_Par_Personne_et_Jour[index_employe][index_jour1];
                            solution_voisine.v_v_IdShift_Par_Personne_et_Jour[index_employe][index_jour1] = solution_courante.v_v_IdShift_Par_Personne_et_Jour[index_employe][index_jour2];
                            if (!presenceMouvementEtInverse(mouvement_actuel))                                                               //Si le mouvement n'est pas dans notre liste taboue
                            {
                                solution_voisine.i_valeur_fonction_objectif = nouvelleValeurVoisinJours(solution_courante, index_employe, index_jour1, index_jour2, v_i_nb_personne_par_Shift_et_jour);
                                //solution_voisine.i_valeur_fonction_objectif = valeurVoisin(solution_voisine);
                                if (solution_voisine.i_valeur_fonction_objectif < solution_meilleure_voisine.i_valeur_fonction_objectif)         //On teste si la valeur du voisin est meilleure que notre meilleur
                                {                                                                                                               //voisin, pour ne pas perdre de temps à regarder si une moins bonne 
                                                                                                                                                // solution est correcte.
                                    if (validiteVoisinJours(solution_voisine, index_employe))
                                    //if (validiteVoisin(solution_voisine, index_employe))                                            
                                    //if (solution_voisine.Verification_Solution(instance))
                                    {
                                        solution_meilleure_voisine = solution_voisine;
                                        mouvement_utilise = mouvement_actuel;
                                    }
                                }
                            }
                            else if (aspiration)                                                                                    //Sinon si le critère d'aspiration est activé
                            {
                                solution_voisine.i_valeur_fonction_objectif = nouvelleValeurVoisinJours(solution_courante, index_employe, index_jour1, index_jour2, v_i_nb_personne_par_Shift_et_jour);
                                if (solution_voisine.i_valeur_fonction_objectif < solution_meilleure.i_valeur_fonction_objectif)                 //On vérifie si notre mouvement déjà utilisé améliore la meilleure solution
                                {
                                    //if (validiteVoisin(solution_voisine, index_employe))
                                    if (validiteVoisinJours(solution_voisine, index_employe))
                                    //if (solution_voisine.Verification_Solution(instance))
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
            ajouterElementJours(mouvement_utilise);
            if (solution_courante.i_valeur_fonction_objectif < solution_meilleure.i_valeur_fonction_objectif)
            {
                solution_meilleure = solution_courante;
                i = 0;
                liste_taboue_shifts.clear();
                liste_taboue_shifts.shrink_to_fit();
            }
        }

        i++;
        cout << "Meilleure valeur de solution voisine a cette iteration : " << solution_meilleure_voisine.i_valeur_fonction_objectif;
        cout << "\n" << i;
        chrono_end = chrono::system_clock::now();
        elapsed = chrono_end - chrono_start;
	}
    delete solution_initiale;
    cout << "Voici la meilleure solution : \n";
    printVoisin(solution_meilleure);
	return new Solution(solution_meilleure);
}



//Permet d'ajouter un élément dans la liste taboue des jours, fonctionne comme une FIFO
void RechercheTabou::ajouterElementJours(vector<int> tabou)
{
	if (liste_taboue_jours.size() == taille_liste_taboue)
	{
		liste_taboue_jours.pop_back();
	}
	liste_taboue_jours.insert(liste_taboue_jours.begin(), tabou);
}

//Permet d'ajouter un élément dans la liste taboue des shifts, fonctionne comme une FIFO
void RechercheTabou::ajouterElementShifts(vector<int> tabou)
{
    if (liste_taboue_shifts.size() == taille_liste_taboue)
    {
        liste_taboue_shifts.pop_back();
    }
    liste_taboue_shifts.insert(liste_taboue_shifts.begin(), tabou);
}

//renvoie true si le mouvement ou le mouvement inverse est présent dans la liste taboue jours
bool RechercheTabou::presenceMouvementEtInverse(vector<int> mouvement)
{
    bool present = false;
    vector<int> mouvement_inverse = { mouvement[0], mouvement[2], mouvement[1] };

    if (find(liste_taboue_jours.begin(), liste_taboue_jours.end(), mouvement) != liste_taboue_jours.end() || find(liste_taboue_jours.begin(), liste_taboue_jours.end(), mouvement_inverse) != liste_taboue_jours.end()) 
    {
        present = true;
    }

    return present;
}

//renvoie true sur le mouvement est présent dans la liste taboue shift
bool RechercheTabou::presenceMouvement(vector<int> mouvement)
{
    bool present = false;

    if (find(liste_taboue_shifts.begin(), liste_taboue_shifts.end(), mouvement) != liste_taboue_shifts.end()) 
    {
        present = true;
    }

    return present;
}

//Renvoie la valeur de la fonction objectif de la solution passée en argument
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
    return i_fc_obj;
}

//Renvoie la valeur de la solution objectif de la solution passée en argument, version qui réduit la complexité pour l'opérateur de swap des jours
int RechercheTabou::nouvelleValeurVoisinJours(Solution voisin, int index_employe, int index_jour1, int index_jour2, vector<vector<int>> v_i_nb_personne_par_Shift_et_jour)
{
    int shift_j1 = voisin.v_v_IdShift_Par_Personne_et_Jour[index_employe][index_jour1];
    int shift_j2 = voisin.v_v_IdShift_Par_Personne_et_Jour[index_employe][index_jour2];
    int i_fc_obj = voisin.i_valeur_fonction_objectif;

    if (shift_j1 != -1)
    {
        //Vérification du nombre de personne par shift et par jour 
        if (v_i_nb_personne_par_Shift_et_jour[shift_j1][index_jour1] <= instance->get_Nbre_Personne_Requis_Jour_Shift(index_jour1, shift_j1))    //Si on manquait déjà de gens sur shift_j1 au j1
            i_fc_obj = i_fc_obj + instance->get_Poids_Personne_En_Moins_Jour_Shift(index_jour1, shift_j1);                                      //Alors on manque encore plus

        if (v_i_nb_personne_par_Shift_et_jour[shift_j1][index_jour1] > instance->get_Nbre_Personne_Requis_Jour_Shift(index_jour1, shift_j1))    //Si on a trop de gens sur shift_j1 au j1
            i_fc_obj = i_fc_obj - instance->get_Poids_Personne_En_Plus_Jour_Shift(index_jour1, shift_j1);                                      //Alors on est content, y'en a un de moins

        if (v_i_nb_personne_par_Shift_et_jour[shift_j1][index_jour2] < instance->get_Nbre_Personne_Requis_Jour_Shift(index_jour2, shift_j1))    //Si on manquait déjà de gens sur shift_j1 au j2
            i_fc_obj = i_fc_obj - instance->get_Poids_Personne_En_Moins_Jour_Shift(index_jour2, shift_j1);                                      //Alors on est content, y'en a un de plus

        if (v_i_nb_personne_par_Shift_et_jour[shift_j1][index_jour2] >= instance->get_Nbre_Personne_Requis_Jour_Shift(index_jour2, shift_j1))    //Si on a trop de gens sur shift_j1 au j2
            i_fc_obj = i_fc_obj + instance->get_Poids_Personne_En_Plus_Jour_Shift(index_jour2, shift_j1);                                      //Alors on a une personne en trop de plus

        i_fc_obj = i_fc_obj - instance->get_Poids_Refus_Pers_Jour_Shift(index_employe, index_jour1, shift_j1);
        i_fc_obj = i_fc_obj + instance->get_Poids_Refus_Pers_Jour_Shift(index_employe, index_jour2, shift_j1);
    }
   
    if (shift_j2 != -1)
    {
        //Vérification du nombre de personne par shift et par jour 
        if (v_i_nb_personne_par_Shift_et_jour[shift_j2][index_jour2] <= instance->get_Nbre_Personne_Requis_Jour_Shift(index_jour2, shift_j2))    //Si on manquait déjà de gens sur shift_j2 au j2
            i_fc_obj = i_fc_obj + instance->get_Poids_Personne_En_Moins_Jour_Shift(index_jour2, shift_j2);                                      //Alors on manque encore plus

        if (v_i_nb_personne_par_Shift_et_jour[shift_j2][index_jour2] > instance->get_Nbre_Personne_Requis_Jour_Shift(index_jour2, shift_j2))    //Si on a trop de gens sur shift_j2 au j2
            i_fc_obj = i_fc_obj - instance->get_Poids_Personne_En_Plus_Jour_Shift(index_jour2, shift_j2);                                      //Alors on est content, y'en a un de moins

        if (v_i_nb_personne_par_Shift_et_jour[shift_j2][index_jour1] < instance->get_Nbre_Personne_Requis_Jour_Shift(index_jour1, shift_j2))    //Si on manquait déjà de gens sur shift_j2 au j1
            i_fc_obj = i_fc_obj - instance->get_Poids_Personne_En_Moins_Jour_Shift(index_jour1, shift_j2);                                      //Alors on est content, y'en a un de plus

        if (v_i_nb_personne_par_Shift_et_jour[shift_j2][index_jour1] >= instance->get_Nbre_Personne_Requis_Jour_Shift(index_jour1, shift_j2))    //Si on a trop de gens sur shift_j2 au j1
            i_fc_obj = i_fc_obj + instance->get_Poids_Personne_En_Plus_Jour_Shift(index_jour1, shift_j2);                                      //Alors on a une personne en trop de plus

        i_fc_obj = i_fc_obj - instance->get_Poids_Refus_Pers_Jour_Shift(index_employe, index_jour2, shift_j2);
        i_fc_obj = i_fc_obj + instance->get_Poids_Refus_Pers_Jour_Shift(index_employe, index_jour1, shift_j2);
    }

    //Vérification de si les employés ont bien leurs shifts préférés ou non
    for (int k = 0; k < instance->get_Nombre_Shift(); k++)
    {
        if (shift_j1 != k)
        {
            i_fc_obj = i_fc_obj + instance->get_Poids_Affectation_Pers_Jour_Shift(index_employe, index_jour2, k);
            i_fc_obj = i_fc_obj - instance->get_Poids_Affectation_Pers_Jour_Shift(index_employe, index_jour1, k);
        }
        if (shift_j2 != k)
        {
            i_fc_obj = i_fc_obj + instance->get_Poids_Affectation_Pers_Jour_Shift(index_employe, index_jour1, k);
            i_fc_obj = i_fc_obj - instance->get_Poids_Affectation_Pers_Jour_Shift(index_employe, index_jour2, k);
        }
    }

    return i_fc_obj;
}

//Renvoie la valeur de la solution objectif de la solution passée en argument, version qui réduit la complexité pour l'opérateur de swap des shifts
int RechercheTabou::nouvelleValeurVoisinShifts(Solution voisin, int index_employe, int index_jour, int index_shift, int index_ancien_shift, vector<vector<int>> v_i_nb_personne_par_Shift_et_jour)
{
    int ancien_shift_j = voisin.v_v_IdShift_Par_Personne_et_Jour[index_employe][index_jour];
    int i_fc_obj = voisin.i_valeur_fonction_objectif;


    //Vérification du nombre de personne par shift et par jour 
    if (v_i_nb_personne_par_Shift_et_jour[ancien_shift_j][index_jour] <= instance->get_Nbre_Personne_Requis_Jour_Shift(index_jour, ancien_shift_j))    //Si on manquait déjà de gens sur ancien_shift_j au j
        i_fc_obj = i_fc_obj + instance->get_Poids_Personne_En_Moins_Jour_Shift(index_jour, ancien_shift_j);                                                  //Alors on manque encore plus

    if (v_i_nb_personne_par_Shift_et_jour[ancien_shift_j][index_jour] > instance->get_Nbre_Personne_Requis_Jour_Shift(index_jour, ancien_shift_j))    //Si on a trop de gens sur ancien_shift_j au j
        i_fc_obj = i_fc_obj - instance->get_Poids_Personne_En_Plus_Jour_Shift(index_jour, ancien_shift_j);                                      //Alors on est content, y'en a un de moins

    if (v_i_nb_personne_par_Shift_et_jour[index_shift][index_jour] < instance->get_Nbre_Personne_Requis_Jour_Shift(index_jour, index_shift))    //Si on manquait déjà de gens sur index_shift au j
        i_fc_obj = i_fc_obj - instance->get_Poids_Personne_En_Moins_Jour_Shift(index_jour, index_shift);                                      //Alors on est content, y'en a un de plus

    if (v_i_nb_personne_par_Shift_et_jour[index_shift][index_jour] >= instance->get_Nbre_Personne_Requis_Jour_Shift(index_jour, index_shift))    //Si on a trop de gens sur index_shift au j
        i_fc_obj = i_fc_obj + instance->get_Poids_Personne_En_Plus_Jour_Shift(index_jour, index_shift);                                      //Alors on a une personne en trop de plus

    i_fc_obj = i_fc_obj - instance->get_Poids_Refus_Pers_Jour_Shift(index_employe, index_jour, ancien_shift_j);
    i_fc_obj = i_fc_obj + instance->get_Poids_Refus_Pers_Jour_Shift(index_employe, index_jour, index_shift);

    //Vérification de si les employés ont bien leurs shifts préférés ou non
    for (int k = 0; k < instance->get_Nombre_Shift(); k++)
    {
        if (ancien_shift_j != k)
        {
            i_fc_obj = i_fc_obj - instance->get_Poids_Affectation_Pers_Jour_Shift(index_employe, index_jour, k);
        }
        if (index_shift != k)
        {
            i_fc_obj = i_fc_obj + instance->get_Poids_Affectation_Pers_Jour_Shift(index_employe, index_jour, k);
        }
    }

    return i_fc_obj;
}

//Cette fonction renvoie un vector contenant le nombre de personne par shift et par jour de la solution passée de argument.
vector<vector<int>> RechercheTabou::calculNbPersonneShiftJour(Solution voisin)
{
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

    return v_i_nb_personne_par_Shift_et_jour;
}

//Renvoie false si toutes les contraintes de la nouvelle solution ne sont pas respectées, true sinon
bool RechercheTabou::validiteVoisin(Solution voisin, int numero_employe)
{
    int j = 0;
	bool validite = true;
    int i_shift_consecutif = 0;
    int i_conge_consecutif = 0;
    int i_duree_travail = 0;
    int i_nb_WE = 0;
    vector<int> v_i_Nb_shift(instance->get_Nombre_Shift(), 0);

    while (j < voisin.v_v_IdShift_Par_Personne_et_Jour[numero_employe].size() && validite != false)
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

            //Vérification du nombre de congés consécutifs minimums assignables à notre employe
            if (i_conge_consecutif != 0)
            {
                if (i_conge_consecutif < instance->get_Personne_Jour_OFF_Consecutif_Min(numero_employe) && j > instance->get_Personne_Jour_OFF_Consecutif_Min(numero_employe))
                {
                    validite = false;
                }
                i_conge_consecutif = 0;
            }
        }
        else
        {
            i_conge_consecutif++;

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

//Renvoie false si toutes les contraintes de la nouvelle solution ne sont pas respectées, true sinon, version améliorée en terme de complexité pour l'opérateur des Shifts, ne fonctionne malheureusement pas 
bool RechercheTabou::validiteVoisinShift(Solution voisin, int numero_employe, int index_jour, int ancien_shift, int nouveau_shift, int duree_travail, vector<int> v_i_Nb_shift)
{
    bool validite = true;

    v_i_Nb_shift[nouveau_shift] = v_i_Nb_shift[nouveau_shift] + 1;
    duree_travail = duree_travail + instance->get_Shift_Duree(nouveau_shift) - instance->get_Shift_Duree(ancien_shift);

    //Vérification des successions des Shifts
    if (index_jour > 0)
    {
        if (voisin.v_v_IdShift_Par_Personne_et_Jour[numero_employe][index_jour - 1] != -1)
        {
            if (!instance->is_possible_Shift_Succede(voisin.v_v_IdShift_Par_Personne_et_Jour[numero_employe][index_jour - 1], nouveau_shift))
            {
                validite = false;
            }
        }
    }
    if (index_jour < voisin.v_v_IdShift_Par_Personne_et_Jour[numero_employe].size() - 1)
    {
        if (voisin.v_v_IdShift_Par_Personne_et_Jour[numero_employe][index_jour + 1] != -1)
        {
            if (!instance->is_possible_Shift_Succede(nouveau_shift, voisin.v_v_IdShift_Par_Personne_et_Jour[numero_employe][index_jour + 1]))
            {
                validite = false;
            }
        }
    }

    //Vérification de la durée totale maximale et minimale de notre employe
    if (duree_travail > instance->get_Personne_Duree_total_Max(numero_employe))
    {
        validite = false;
    }
    if (duree_travail < instance->get_Personne_Duree_total_Min(numero_employe))
    {
        validite = false;
    }

    //Vérification du nombre maximal de shifts de notre employe
    if (v_i_Nb_shift[nouveau_shift] > instance->get_Personne_Shift_Nbre_Max(numero_employe, nouveau_shift))
    {
        validite = false;
    }

    return validite;
}

//Renvoie false si toutes les contraintes de la nouvelle solution ne sont pas respectées, true sinon, version améliorée en terme de complexité pour l'opérateur des jours
bool RechercheTabou::validiteVoisinJours(Solution voisin, int numero_employe)
{
    int j = 0;
    bool validite = true;
    int i_shift_consecutif = 0;
    int i_conge_consecutif = 0;
    int i_nb_WE = 0;

    while (j < voisin.v_v_IdShift_Par_Personne_et_Jour[numero_employe].size() && validite != false)
    {
        if (voisin.v_v_IdShift_Par_Personne_et_Jour[numero_employe][j] != -1)
        {
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

            //Vérification du nombre de congés consécutifs minimums assignables à chaque personne
            if (i_conge_consecutif != 0)
            {
                if (i_conge_consecutif < instance->get_Personne_Jour_OFF_Consecutif_Min(numero_employe) && j > instance->get_Personne_Jour_OFF_Consecutif_Min(numero_employe))
                {
                    validite = false;
                }
                i_conge_consecutif = 0;
            }
        }
        else
        {
            i_conge_consecutif++;
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
    //Vérification du nombre de WE (samedi ou/et dimanche) de travail maximal pour notre employe
    if (i_nb_WE > instance->get_Personne_Nbre_WE_Max(numero_employe))
    {
        validite = false;
    }

    return validite;
}

int RechercheTabou::calculDureeTravail(Solution solution)
{
    int i_duree_travail = 0;
    for (int i = 0; i < solution.v_v_IdShift_Par_Personne_et_Jour.size(); i++)
    {
        for (int j = 0; j < solution.v_v_IdShift_Par_Personne_et_Jour[i].size(); j++)
        {
            if (solution.v_v_IdShift_Par_Personne_et_Jour[i][j] != -1)
            {
                i_duree_travail = i_duree_travail + instance->get_Shift_Duree(solution.v_v_IdShift_Par_Personne_et_Jour[i][j]);
            }
        }
    }
    return i_duree_travail;
}

vector<int> RechercheTabou::calculNbShift(Solution solution)
{
    vector<int> v_i_Nb_shift(instance->get_Nombre_Shift(), 0);

    for (int i = 0; i < solution.v_v_IdShift_Par_Personne_et_Jour.size(); i++)
    {
        for (int j = 0; j < solution.v_v_IdShift_Par_Personne_et_Jour[i].size(); j++)
        {
            if (solution.v_v_IdShift_Par_Personne_et_Jour[i][j] != -1)
            {
                v_i_Nb_shift[solution.v_v_IdShift_Par_Personne_et_Jour[i][j]] = v_i_Nb_shift[solution.v_v_IdShift_Par_Personne_et_Jour[i][j]] + 1;
            }
        }
    } 
    return v_i_Nb_shift;
}

//Permet de print une solution dans la console
void RechercheTabou::printVoisin(Solution voisin)
{
    cout << "\n Voici le print d'une solution : \n";
    for (int i = 0; i < voisin.v_v_IdShift_Par_Personne_et_Jour.size(); i++)
    {
        cout << "Personne n° " << i << "\n";
        for (int j = 0; j < voisin.v_v_IdShift_Par_Personne_et_Jour[i].size(); j++)
        {
            cout << voisin.v_v_IdShift_Par_Personne_et_Jour[i][j];
        }
        cout << "\n";
    }
    cout << "Sa valeur est " << voisin.i_valeur_fonction_objectif << "\n";
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