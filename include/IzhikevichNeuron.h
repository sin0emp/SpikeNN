#ifndef IZHIKEVICHNEURON_H
#define IZHIKEVICHNEURON_H

#ifndef MODULE_EXPORT
   #if defined(_WIN32) || defined(_WIN_64)
      #ifdef SpikeNN_EXPORTS
         #define MODULE_EXPORT  __declspec(dllexport)   // export DLL information
      #else
         #define MODULE_EXPORT  __declspec(dllimport)   // import DLL information
      #endif
   #else
      #define MODULE_EXPORT
   #endif
#endif

#include "Neuron.h"
#include "GlobalVars.h"

class Network;
class Layer;

class MODULE_EXPORT IzhikevichNeuron : public Neuron
{
public:
   //default parameters describes a regular spike: a = 0.02, b = 0.2, c = -65, d = 2
   //default parameters describes a fast spike:
   IzhikevichNeuron(Layer* layer, int ID, ChannelType type = EXCITATORY, ParameterContainer* params = 0);

   void setParameters(float a, float b, float c, float d);
   void setParameters(ParameterContainer params);
   virtual float updatePotential();
   virtual void rest();
   
private:
   //model parameters
   float mA, mB, mC, mD;

   //model variables
   float mV, mU;
};

//structure for the izhikech model's parameters used for initialization
struct IzhikevichParameters : public ParameterContainer
{
   IzhikevichParameters()
   {
      //default parameters describes a regular spike
      mParams.push_back(0.02f); //default a
      mParams.push_back(0.2f);  //default b
      mParams.push_back(-65);  //default c
      mParams.push_back(8);    //default d
   }

   IzhikevichParameters(float a, float b, float c, float d)
   {
      mParams.push_back(a);
      mParams.push_back(b);
      mParams.push_back(c);
      mParams.push_back(d);
   }
};

#endif