#include "IzhikevichNeuron.h"
#include "Network.h"
#include <cmath>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>

BOOST_CLASS_EXPORT(IzhikevichNeuron);

IzhikevichNeuron::IzhikevichNeuron(Layer* layer, int ID, ChannelType type, ParameterContainer* params) : Neuron(layer, ID, type)
{
   if (params)
   {
      setParameters(*params);
   }
   else
      setParameters(IzhikevichParameters());

   rest();
}

void IzhikevichNeuron::setParameters(float a, float b, float c, float d)
{
   mA = a;
   mB = b;
   mC = c;
   mD = d;
}

void IzhikevichNeuron::setParameters(ParameterContainer params)
{
   mA = params.mParams[0];
   mB = params.mParams[1];
   mC = params.mParams[2];
   mD = params.mParams[3];
}

float IzhikevichNeuron::updatePotential()
{
   if (mV >= 30)
   {
      mV = mC;
      mU += mD;
      propagateSpike();
   }

   //for better approximation, time step is assumed to be 0.5
   //time step = 0.2 => better approximation??
   //for (int i=0; i<5; ++i)
   //{
   //   mV += 0.2f * (mV * (0.04f * mV + 5) + 140 - mU + mInputCurrent);
   //   mU += 0.2f * (mA * (mB * mV - mU));
   //}

   mV += 0.5f * (mV * (0.04f * mV + 5) + 140 - mU + mInputCurrent);
   mV += 0.5f * (mV * (0.04f * mV + 5) + 140 - mU + mInputCurrent);

   mU += (mA * (mB * mV - mU));

   return mV;
}

void IzhikevichNeuron::rest()
{
   Neuron::rest();
   //default membrane potential is resting point
   mV = (mB - 5 - std::sqrt((5-mB)*(5-mB)-4*0.04f*140))/0.08f;
   mU = mB*mV;
}

template <class Archive>
void IzhikevichNeuron::serialize(Archive &ar, const unsigned int version)
{
   ar & boost::serialization::base_object<Neuron>(*this);
   ar & mA & mB & mC
      & mD;
}

template void IzhikevichNeuron::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive &ar, const unsigned int version);
template void IzhikevichNeuron::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive &ar, const unsigned int version);
