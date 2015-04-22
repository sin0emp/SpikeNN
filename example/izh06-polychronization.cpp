/********************************************************************************************
*  This example creates and runs the network designed by E. M. Izhikevich in 2006:          *
*  - "Polychronization: computation with spikes." Neural computation 18.2 (2006): 245-282.  *
*********************************************************************************************/

#include <Network.h>

int main(int argc, char **argv) {
   Network net(0.2);
   int l = net.addLayer();
   
   net.addNeuron<IzhikevichNeuron>(l, 800, EXCITATORY, new IzhikevichParameters(0.02f, 0.2f, -65,8));
   net.addNeuron<IzhikevichNeuron>(l, 200, INHIBITORY, new IzhikevichParameters(0.1f, 0.2f, -65,2));
   
   net.setSTDPParameters(l, 0.9f, 0.1f, 0.12f, 1.0f, 1000);
   net.setCurrentParameters(l, 20, 20);
   net.setBoundingParameters(l, 10, 10, 6, 6, 5, 5, 1, 20);
   net.makeConnection(l, l, 100, 6, 5, CHOOSE_RANDOM, 1);
   net.setInputPattern(l, ONE_MAX_CURRENT);
   net.setIInhibitoryLearningFlag(l, false);
   net.logLayerActivity(l);
   
   net.runNetwork(3600000);
   
   return 0;
}
