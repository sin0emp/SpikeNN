/********************************************************************************************
*  This example creates and runs the network designed by E. M. Izhikevich in 2007:          *
*  - "Solving the distal reward problem through linkage of STDP anddopamine signaling.      *
*    " Cerebral cortex 17.10 (2007): 2443-2452.                                             *
*********************************************************************************************/

#include <Network.h>

bool firstSynapse(int id)
{
   return id == 0;
}

int main()
{
   Network net;
   int l = net.addLayer();
   net.addNeuron<IzhikevichNeuron>(l, 800, EXCITATORY, new IzhikevichParameters(0.02f, 0.2f, -65,8));
   net.addNeuron<IzhikevichNeuron>(l, 200, INHIBITORY, new IzhikevichParameters(0.1f, 0.2f, -65,2));

   //daspnet settings
   net.setSTDPParameters(l, 0.99f, 0.1f, 0.15f, 1.0f, 100);
   net.setCurrentParameters(l, -6.5, 6.5);
   net.setBoundingParameters(l, 4, 4, 0, 4, 0, 4, 0, 20);
   net.makeConnection(l, l, 100, 1.0f, 1.0f, CHOOSE_RANDOM, 1);
   IzhikevichDAHandler* dah = new IzhikevichDAHandler();
   dah->setSynapse(net.getSynapse(0));
   net.addDAModule(l, dah);
   net.logSynapseWeight(&firstSynapse);
   net.setInputPattern(l, ALL_RANDOM_CURRENT);
   net.setIInhibitoryLearningFlag(l, false);
   net.runNetwork(3600000);

   return 0;
}