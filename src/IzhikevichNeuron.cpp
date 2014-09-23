#include "IzhikevichNeuron.h"
#include "Network.h"
#include <cmath>

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
   mV += 0.5f * (mV * (0.04f * mV + 5) + 140 - mU + mInputCurrent);
   mV += 0.5f * (mV * (0.04f * mV + 5) + 140 - mU + mInputCurrent);

   mU += mA * (mB * mV - mU);

   return mV;
}

void IzhikevichNeuron::rest()
{
   Neuron::rest();
   //default membrane potential is resting point
   mV = (mB - 5 - std::sqrt((5-mB)*(5-mB)-4*0.04f*140))/0.08f;
   mU = mB*mV;
}