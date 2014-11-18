#ifndef LIFNEURON_H
#define LIFNEURON_H

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

namespace boost{ namespace serialization { class access; } namespace archive { class xml_oarchive; } }
class Network;
class Layer;

class MODULE_EXPORT LIFNeuron : public Neuron
{
   friend class boost::serialization::access;
public:
   LIFNeuron(Layer* layer, int ID, ChannelType type = EXCITATORY, ParameterContainer* params = 0);

   void setParameters(float a, float b, float c, float vthresh);
   void setParameters(ParameterContainer params);
   virtual float updatePotential();
   virtual void rest();

private:
   //model parameters
   float mA, mB, mC, mVThresh;

   //model variables
   float mV;

   template <class Archive>
   void serialize(Archive &ar, const unsigned int version);
   LIFNeuron() {}
};

//structure for the izhikech model's parameters used for initialization
struct LIFParameters : public ParameterContainer
{
   LIFParameters()
   {
      //default parameters describes a regular spike
      mParams.push_back(-4.3f); //default a
      mParams.push_back(0.07f);  //default b
      mParams.push_back(-70);  //default c
      mParams.push_back(-50);    //default vthresh
   }

   LIFParameters(float a, float b, float c, float vthresh)
   {
      mParams.push_back(a);
      mParams.push_back(b);
      mParams.push_back(c);
      mParams.push_back(vthresh);
   }
};

#endif