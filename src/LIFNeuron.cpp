#include "LIFNeuron.h"
#include "Network.h"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <cmath>

LIFNeuron::LIFNeuron(Layer* layer, int ID, ChannelType type, ParameterContainer* params) : Neuron(layer, ID, type)
{
   if (params)
   {
      setParameters(*params);
   }
   else
      setParameters(LIFParameters());
   
   rest();
}

void LIFNeuron::setParameters(float a, float b, float c, float vthresh)
{
   mA = a;
   mB = b;
   mC = c;
   mVThresh = vthresh;
}

void LIFNeuron::setParameters(ParameterContainer params)
{
   mA = params.mParams[0];
   mB = params.mParams[1];
   mC = params.mParams[2];
   mVThresh = params.mParams[3];
}

float LIFNeuron::updatePotential()
{
   if (mV >= mVThresh)
   {
      mV = mC;
      propagateSpike();
   }

   mV += mA - mB*mV + mInputCurrent;

   return mV;
}

void LIFNeuron::rest()
{
   Neuron::rest();
   //default membrane potential is resting point
   mV = mA/mB;
}

template <class Archive>
void LIFNeuron::serialize(Archive &ar, const unsigned int version)
{
   ar & boost::serialization::base_object<Neuron>(*this);
   ar & mA & mB & mC
      & mVThresh & mV;
}

template void LIFNeuron::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive &ar, const unsigned int version);
template void LIFNeuron::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive &ar, const unsigned int version);