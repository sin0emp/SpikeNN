#include "GlobalVars.h"

std::vector<std::size_t> SHUFFLE(std::size_t size)
{
   std::vector<std::size_t> shuffled;
   for (std::size_t i = 0; i < size; ++i)
      shuffled.push_back(i);

   for (std::size_t i = 0; i < size - 1; ++i)
   {
      std::size_t k = i + rand() % (size - i);
      size_t hold = shuffled[i];
      shuffled[i] = shuffled[k];
      shuffled[k] = hold;
   }

   return shuffled;
}
