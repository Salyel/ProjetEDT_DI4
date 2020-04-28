#define CHEMIN_DOSSIER_DONNEES "D:/Programmes Visual/ProjetEDT_DI4/ProjetEDT_DI4/Format Etudiant Public/"
//#define CHEMIN_DOSSIER_DONNEES "C:/Users/Florent/Documents/Cours/S8/Optimisation Discrete/ProjetEDT_DI4/Format Etudiant Public/"
#define NOM_FICHIER_LISTE_FICHIER_DONNEES "data.txt"
#define NOM_FICHIER_LISTE_SORTIE "sortie.txt"

#include <iostream>
#include <fstream>
#include <chrono>
#include <algorithm>
#include "Instance.hpp"
#include "Solution.hpp"
#include "Heuristique.h"

using namespace std;

int Resolution(Instance * instance);

int main(int argc, const char * argv[])
{
    //Bloc de commentaire pour tester des trucs sur les solutions
    /*
    Solution test;
    Solution test2;

    vector<vector<int>> nombrax;
    vector<int> inte_nombrax;
    inte_nombrax.push_back(1);
    inte_nombrax.push_back(-1);
    inte_nombrax.push_back(1);
    inte_nombrax.push_back(1);
    nombrax.push_back(inte_nombrax);

    test.v_v_IdShift_Par_Personne_et_Jour = nombrax;
    test.i_valeur_fonction_objectif = 5;

    test2 = test;

    for (int i = 0; i < 1; i++) {
        for (int j = 0; j < 4; j++) {
            cout << test.v_v_IdShift_Par_Personne_et_Jour[i][j];
        }
        cout << "\n";
    }
    for (int i = 0; i < 1; i++) {
        for (int j = 0; j < 4; j++) {
            cout << test2.v_v_IdShift_Par_Personne_et_Jour[i][j];
        }
        cout << "\n";
    }

    test.v_v_IdShift_Par_Personne_et_Jour[0].pop_back();

    for (int i = 0; i < 1; i++) {
        for (int j = 0; j < test.v_v_IdShift_Par_Personne_et_Jour[0].size(); j++) {
            cout << test.v_v_IdShift_Par_Personne_et_Jour[i][j];
        }
        cout << "\n";
    }
    for (int i = 0; i < 1; i++) {
        for (int j = 0; j < test2.v_v_IdShift_Par_Personne_et_Jour[0].size(); j++) {
            cout << test2.v_v_IdShift_Par_Personne_et_Jour[i][j];
        }
        cout << "\n";
    }

    test.~Solution();

    for (int i = 0; i < 1; i++) {
        for (int j = 0; j < test2.v_v_IdShift_Par_Personne_et_Jour[0].size(); j++) {
            cout << test2.v_v_IdShift_Par_Personne_et_Jour[i][j];
        }
        cout << "\n";
    }
    */
    
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
    int i_val_Retour_Fct_obj=0;
    Solution* uneSolution = new Solution();
    
/* EXEMPLE D UNE SOLUTION : */
/*    uneSolution->v_v_IdShift_Par_Personne_et_Jour.push_back(vector<int> ()={-1,1,3,-1,-1,-1,-1,-1,1,1,3,3,-1,-1,1,1,3,-1,-1,1,1,1,1,1,-1,-1,1,3});
    uneSolution->v_v_IdShift_Par_Personne_et_Jour.push_back(vector<int> ()={3,3,3,-1,-1,0,0,2,2,-1,-1,0,0,2,-1,-1,0,0,3,-1,-1,-1,0,2,3,3,-1,-1});
    uneSolution->v_v_IdShift_Par_Personne_et_Jour.push_back(vector<int> ()={3,3,-1,-1,3,3,3,3,3,-1,-1,-1,-1,-1,0,0,0,3,3,-1,-1,0,3,-1,-1,0,0,0});
    uneSolution->v_v_IdShift_Par_Personne_et_Jour.push_back(vector<int> ()={-1,-1,-1,4,4,4,4,-1,-1,2,3,-1,-1,-1,1,2,1,1,3,-1,-1,1,3,-1,-1,1,1,2});
    uneSolution->v_v_IdShift_Par_Personne_et_Jour.push_back(vector<int> ()={-1,-1,-1,0,0,-1,-1,3,3,3,3,3,-1,-1,3,3,-1,-1,-1,3,3,3,4,4,-1,-1,4,4});
    uneSolution->v_v_IdShift_Par_Personne_et_Jour.push_back(vector<int> ()={3,-1,-1,-1,4,4,4,4,4,-1,-1,-1,1,1,1,1,1,-1,-1,-1,-1,3,3,3,3,3,-1,-1});
    uneSolution->v_v_IdShift_Par_Personne_et_Jour.push_back(vector<int> ()={0,-1,-1,-1,2,2,2,2,4,-1,-1,-1,2,2,2,-1,-1,3,3,-1,-1,3,3,3,3,3,-1,-1});
    uneSolution->v_v_IdShift_Par_Personne_et_Jour.push_back(vector<int> ()={-1,1,1,1,2,-1,-1,1,1,-1,-1,-1,2,1,2,-1,-1,1,1,-1,-1,2,1,2,-1,-1,2,1});
    uneSolution->v_v_IdShift_Par_Personne_et_Jour.push_back(vector<int> ()={-1,0,1,1,1,-1,-1,-1,0,0,0,4,-1,-1,-1,0,0,-1,-1,1,1,-1,-1,-1,0,1,1,1});
    uneSolution->v_v_IdShift_Par_Personne_et_Jour.push_back(vector<int> ()={3,3,4,-1,-1,1,1,1,-1,-1,1,1,-1,-1,4,4,4,-1,-1,1,3,3,-1,-1,1,4,-1,-1});
    uneSolution->v_v_IdShift_Par_Personne_et_Jour.push_back(vector<int> ()={-1,1,1,-1,-1,-1,-1,-1,1,1,1,1,-1,-1,-1,-1,4,4,4,4,4,-1,-1,1,1,1,1,1});
    uneSolution->v_v_IdShift_Par_Personne_et_Jour.push_back(vector<int> ()={2,4,4,-1,-1,-1,-1,-1,-1,0,0,0,-1,-1,0,0,0,-1,-1,2,2,2,2,2,-1,-1,1,1});
    uneSolution->v_v_IdShift_Par_Personne_et_Jour.push_back(vector<int> ()={0,3,3,-1,-1,-1,-1,0,0,3,-1,-1,3,3,3,3,3,-1,-1,0,0,0,0,1,-1,-1,-1,-1});
    uneSolution->v_v_IdShift_Par_Personne_et_Jour.push_back(vector<int> ()={-1,-1,3,3,3,3,3,-1,-1,3,3,3,-1,-1,3,3,3,3,-1,-1,-1,-1,-1,-1,3,3,3,3});
    uneSolution->v_v_IdShift_Par_Personne_et_Jour.push_back(vector<int> ()={3,3,3,3,3,-1,-1,-1,-1,0,0,2,2,2,-1,-1,0,0,2,-1,-1,2,2,2,-1,-1,0,0});
    uneSolution->v_v_IdShift_Par_Personne_et_Jour.push_back(vector<int> ()={4,-1,-1,3,3,-1,-1,-1,2,3,4,-1,-1,-1,0,0,2,-1,-1,1,1,1,1,2,-1,-1,0,0});
    uneSolution->v_v_IdShift_Par_Personne_et_Jour.push_back(vector<int> ()={0,0,0,0,0,-1,-1,0,0,0,-1,-1,3,3,3,3,3,-1,-1,-1,-1,-1,0,3,-1,-1,0,0});
    uneSolution->v_v_IdShift_Par_Personne_et_Jour.push_back(vector<int> ()={1,2,2,2,-1,-1,-1,0,0,-1,-1,0,1,2,1,1,-1,-1,0,0,0,0,1,-1,-1,-1,-1,-1});
    uneSolution->v_v_IdShift_Par_Personne_et_Jour.push_back(vector<int> ()={2,2,2,-1,-1,3,3,3,3,-1,-1,-1,2,1,1,1,1,-1,-1,-1,-1,1,2,3,3,4,-1,-1});
    uneSolution->v_v_IdShift_Par_Personne_et_Jour.push_back(vector<int> ()={-1,2,2,-1,-1,2,2,-1,-1,-1,2,2,3,3,3,-1,-1,2,2,-1,-1,3,3,3,3,3,-1,-1});
    uneSolution->v_v_IdShift_Par_Personne_et_Jour.push_back(vector<int> ()={-1,-1,-1,3,3,-1,-1,3,3,3,3,3,-1,-1,-1,-1,2,2,2,2,2,-1,-1,-1,0,0,2,2});
    uneSolution->v_v_IdShift_Par_Personne_et_Jour.push_back(vector<int> ()={2,-1,-1,3,3,3,3,3,-1,-1,1,1,-1,-1,2,2,2,2,1,-1,-1,2,2,-1,-1,2,3,3});
    uneSolution->v_v_IdShift_Par_Personne_et_Jour.push_back(vector<int> ()={-1,-1,0,4,4,4,4,-1,-1,-1,-1,-1,1,1,1,2,4,-1,-1,-1,-1,2,1,2,2,1,-1,-1});
    uneSolution->v_v_IdShift_Par_Personne_et_Jour.push_back(vector<int> ()={0,0,1,1,-1,-1,-1,0,0,0,-1,-1,0,0,-1,-1,0,0,0,0,1,-1,-1,-1,0,0,-1,-1});
    uneSolution->v_v_IdShift_Par_Personne_et_Jour.push_back(vector<int> ()={-1,2,3,3,-1,-1,-1,1,2,1,1,-1,-1,-1,0,1,-1,-1,0,2,2,3,-1,-1,0,2,3,3});
    uneSolution->v_v_IdShift_Par_Personne_et_Jour.push_back(vector<int> ()={3,-1,-1,-1,-1,2,1,2,3,4,-1,-1,-1,-1,0,0,0,0,0,-1,-1,3,4,-1,-1,2,1,1});
    uneSolution->v_v_IdShift_Par_Personne_et_Jour.push_back(vector<int> ()={1,1,-1,-1,1,1,1,-1,-1,1,1,1,1,3,-1,-1,1,1,1,-1,-1,-1,1,1,1,3,-1,-1});
    uneSolution->v_v_IdShift_Par_Personne_et_Jour.push_back(vector<int> ()={-1,0,0,0,0,-1,-1,4,4,-1,-1,2,2,4,-1,-1,2,2,2,2,2,-1,-1,1,2,-1,-1,-1});
    uneSolution->v_v_IdShift_Par_Personne_et_Jour.push_back(vector<int> ()={1,4,-1,-1,-1,0,0,0,0,1,-1,-1,-1,-1,3,3,-1,-1,1,4,4,-1,-1,0,0,1,-1,-1});
    uneSolution->v_v_IdShift_Par_Personne_et_Jour.push_back(vector<int> ()={-1,-1,2,2,2,-1,-1,-1,0,2,2,2,-1,-1,3,3,-1,-1,0,0,0,0,0,-1,-1,0,0,0});
    uneSolution->v_v_IdShift_Par_Personne_et_Jour.push_back(vector<int> ()={-1,1,1,3,-1,-1,-1,1,1,2,3,-1,-1,-1,-1,-1,3,3,3,3,3,-1,-1,2,2,2,2,2});
    uneSolution->v_v_IdShift_Par_Personne_et_Jour.push_back(vector<int> ()={-1,-1,0,0,0,-1,-1,-1,0,0,2,3,-1,-1,2,2,-1,-1,2,2,2,2,2,-1,-1,0,0,0});
    uneSolution->v_v_IdShift_Par_Personne_et_Jour.push_back(vector<int> ()={0,0,0,0,0,-1,-1,-1,-1,-1,0,0,0,0,0,-1,-1,0,0,0,0,-1,-1,0,0,0,-1,-1});
    uneSolution->v_v_IdShift_Par_Personne_et_Jour.push_back(vector<int> ()={0,-1,-1,2,2,3,3,3,-1,-1,2,2,-1,-1,2,2,-1,-1,0,0,0,0,3,-1,-1,-1,-1,-1});
    uneSolution->v_v_IdShift_Par_Personne_et_Jour.push_back(vector<int> ()={0,-1,-1,1,1,-1,-1,0,4,-1,-1,0,3,3,-1,-1,3,3,3,3,3,-1,-1,3,3,3,-1,-1});
    uneSolution->v_v_IdShift_Par_Personne_et_Jour.push_back(vector<int> ()={2,-1,-1,-1,-1,1,2,1,2,-1,-1,0,3,3,-1,-1,0,1,3,-1,-1,4,4,4,4,-1,-1,-1});
    uneSolution->v_v_IdShift_Par_Personne_et_Jour.push_back(vector<int> ()={3,3,-1,-1,1,1,1,2,2,-1,-1,-1,-1,-1,0,0,3,3,-1,-1,-1,-1,-1,-1,0,0,2,1});
    uneSolution->v_v_IdShift_Par_Personne_et_Jour.push_back(vector<int> ()={2,2,-1,-1,1,1,1,2,1,-1,-1,1,1,1,4,-1,-1,1,1,-1,-1,1,2,-1,-1,-1,-1,-1});
    uneSolution->v_v_IdShift_Par_Personne_et_Jour.push_back(vector<int> ()={-1,-1,0,0,0,0,0,-1,-1,-1,0,0,-1,-1,0,0,0,0,4,-1,-1,-1,0,0,-1,-1,0,0});
    uneSolution->v_v_IdShift_Par_Personne_et_Jour.push_back(vector<int> ()={2,2,2,-1,-1,0,1,-1,-1,-1,2,3,3,3,-1,-1,2,2,1,-1,-1,0,0,0,0,2,-1,-1});
    
    uneSolution->i_valeur_fonction_objectif=4631;*/
 /* * * * * * * * * * * * */
    
    uneSolution = Heuristique::unnamed_heuristique(instance);

    for (int i = 0; i < instance->get_Nombre_Personne(); i++) {
        for (int j = 0; j < instance->get_Nombre_Jour(); j++) {
            cout << uneSolution->v_v_IdShift_Par_Personne_et_Jour[i][j];
        }
        cout << "\n";
    }
    

    uneSolution->Verification_Solution(instance);
    
    i_val_Retour_Fct_obj=uneSolution->i_valeur_fonction_objectif;
    delete uneSolution;
    return i_val_Retour_Fct_obj;
}

