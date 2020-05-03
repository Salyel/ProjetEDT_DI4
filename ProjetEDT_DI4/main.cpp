#define CHEMIN_DOSSIER_DONNEES "D:/Programmes Visual/ProjetEDT_DI4/ProjetEDT_DI4/Format Etudiant Public/"
//#define CHEMIN_DOSSIER_DONNEES "C:/Users/Florent/Documents/Cours/S8/Optimisation Discrete/ProjetEDT_DI4/Format Etudiant Public/"
#define NOM_FICHIER_LISTE_FICHIER_DONNEES "data.txt"
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

  /*  uneSolution->Verification_Solution(instance);
    i_val_Retour_Fct_obj = uneSolution->i_valeur_fonction_objectif;
    delete uneSolution; */

    RechercheTabou tabou = RechercheTabou(instance, 30, uneSolution, 30, true);
    Solution* meilleure_solution = tabou.rechercheTabou(chrono_start);
    meilleure_solution->Verification_Solution(instance);
    i_val_Retour_Fct_obj=meilleure_solution->i_valeur_fonction_objectif;
    delete meilleure_solution;
    delete uneSolution;

    return i_val_Retour_Fct_obj;
}

