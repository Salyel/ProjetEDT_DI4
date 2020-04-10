#ifndef Heuristique_hpp
#define Heuristique_hpp

#include "Instance.hpp"
#include "Solution.hpp"
#include <stdio.h>
#include <vector>

using namespace std;

class Heuristique 
{
	public:
		static Solution* unnamed_heuristique(Instance* instance);
};


#endif