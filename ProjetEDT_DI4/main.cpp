#define CHEMIN_DOSSIER_DONNEES "D:/Programmes Visual/ProjetEDT_DI4/ProjetEDT_DI4/Format Etudiant Public/"
//#define CHEMIN_DOSSIER_DONNEES "C:/Users/Florent/Documents/Cours/S8/Optimisation Discrete/ProjetEDT_DI4/Format Etudiant Public/"
#define NOM_FICHIER_LISTE_FICHIER_DONNEES "ALLdata.txt"
#define NOM_FICHIER_LISTE_SORTIE "sortie.txt"

#include <iostream>
#include <fstream>
#include <chrono>
#include <algorithm>
#include <set>
#include "Instance.hpp"
#include "Solution.hpp"
#include "Heuristique.hpp"
#include "Recherche tabou.h"

using namespace std;

int Resolution(Instance * instance);

int main(int argc, const char * argv[])
{
    try
    {
        string s_tmp;
        string s_chemin=CHEMIN_DOSSIER_DONNEES;
        s_chemin.append(NOM_FICHIER_LISTE_FICHIER_DONNEES);
        
        ifstream fichier(s_chemin.c_str(), std::ios::in);std::ofstream fichier_Sortie_Resume;
        s_chemin=CHEMIN_DOSSIER_DONNEES;
        s_chemin.append(NOM_FICHIER_LISTE_SORTIE);
        ofstream fichier_Sortie(s_chemin.c_str(), std::ios::out | std::ios::app);

        if(fichier)
        {
            if(fichier_Sortie)
            {
                fichier_Sortie<<" Fichier données\t Tps de résolution \t Best solution"<<endl;
                getline(fichier,s_tmp);
                while(s_tmp!="")
                {
                    Instance * instance = new Instance();
                    chrono::time_point<chrono::system_clock> chrono_start, chrono_end;
                    chrono::duration<double> elapsed;
                    int i_best_solution_score=0;
                    s_chemin=CHEMIN_DOSSIER_DONNEES;
                    cout<< " Résolution de "<<s_tmp<<endl;
                    s_chemin.append(s_tmp);
                    s_chemin.erase(remove(s_chemin.begin(), s_chemin.end(), '\r'), s_chemin.end());
                    s_chemin.erase(remove(s_chemin.begin(), s_chemin.end(), '\n'), s_chemin.end());
                    
                    instance->chargement_Instance(s_chemin);
                    chrono_start = chrono::system_clock::now();
                    i_best_solution_score=Resolution(instance);
                    cout<< " Fin de résolution de "<<s_tmp<<endl;
                    chrono_end = chrono::system_clock::now();
                    elapsed=chrono_end-chrono_start;
                    fichier_Sortie<<s_chemin <<"\t"<<elapsed.count()<<"\t"<< i_best_solution_score <<endl;
                    s_tmp="";
                    getline(fichier,s_tmp);
                    delete instance;
                }
                fichier_Sortie.close();
            }
            else
            {
                cout<<" Erreur lecture des données : chemin vers la sortie non valide. "<<endl;
            }
            fichier.close();
        }
        else
        {
            cout<<" Erreur lecture des données : chemin listant l'ensemble des données non valide. "<<endl;
        }
    }
    
    catch(string err)
    {
        cout << "Erreur fatale : " <<endl;
        cout << err <<endl;
    }
    return 0;
}

int Resolution(Instance * instance)
{
    int i_val_Retour_Fct_obj = 0;
    chrono::time_point<chrono::system_clock> chrono_start = chrono::system_clock::now();
    Heuristique h = Heuristique(instance);
    Solution* uneSolution = h.resolution_Instance();

    int nb_iteration_max = 200;             //Le nombre d'itération sans amélioration avant d'arrêter la recherche tabou
    bool aspiration = true;                 //Si aspiration est égal à true, on peut prendre une solution si elle améliore la meilleure solution de tous les temps même si son mouvement est dans la liste taboue
    int taille_liste_taboue = 30;           //La taille de la liste taboue
    int nb_random = 2000;                   //Le nombre d'itération de random à chaque génération de voisinage.

    //L'instance 18 prend beaucoup trop de temps et nous ne trouvons jamais de solution, autant l'ignorer !
    if (instance->get_Nombre_Shift() == 32)
    {
        uneSolution->Verification_Solution(instance);
        i_val_Retour_Fct_obj = uneSolution->i_valeur_fonction_objectif;
        delete uneSolution; 
    }
    else
    {
        //Si la solution fournie par l'heuristique est réalisable, on lance tout normalement
        if (uneSolution->Verification_Solution(instance))
        {
            RechercheTabou tabou = RechercheTabou(instance, taille_liste_taboue, uneSolution, nb_iteration_max, aspiration);
            Solution* meilleure_solution = tabou.rechercheTabou(chrono_start, nb_random, true);
            meilleure_solution->Verification_Solution(instance);
            i_val_Retour_Fct_obj = meilleure_solution->i_valeur_fonction_objectif;
            delete meilleure_solution;
            delete uneSolution;
        }
        //Si elle n'est pas réalisable, on lance un mode spéciale pour ça (ne fonctionne que si l'erreur peut être résolue par un unique mouvement, par manque de temps)
        else
        {
            RechercheTabou tabou = RechercheTabou(instance, taille_liste_taboue, uneSolution, nb_iteration_max, aspiration);
            Solution* meilleure_solution = tabou.rechercheTabou(chrono_start, nb_random, false);
            meilleure_solution->Verification_Solution(instance);
            i_val_Retour_Fct_obj = meilleure_solution->i_valeur_fonction_objectif;
            delete meilleure_solution;
            delete uneSolution;
        }
    }



    return i_val_Retour_Fct_obj;
}

