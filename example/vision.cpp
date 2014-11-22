#include <VisualNetwork.h>
#include <iostream>

void firstLayer()
{
   VisualNetwork visNet;
   visNet.setDefaultSTDPParameters(0.99f, 0.002f, 0.002f, 1.0f, 100, 20.0f, 30.0f);
   visNet.setDefaultBoundingParameters(3, 20, 0.5f, 2.5f, 10, 10, 1, 20);
   int input = visNet.addLayer<IzhikevichNeuron>(Point2D(144, 144), INPUT_LAYER);
   visNet.setInputImagesDirectory("C:/Pix/CU3D_2class");

   int s1 = visNet.addReceptiveFieldSuperLayer<IzhikevichNeuron>(input, 6, Point2D(10,10), Point2D(4,4), new IzhikevichParameters(0.02, 0.2, -65, 6));

   visNet.runNetwork(200000);
   VisualNetwork::saveNetwork(visNet, "SL1.sav");
}

void secondLayer()
{
   VisualNetwork* visNet = VisualNetwork::loadNetwork("SL1.sav");
   visNet->setLearningLock(true);
   visNet->setDefaultBoundingParameters(4, 20, 1.5, 3.5, 10, 10, 1, 20);
   int s2 = visNet->addReceptiveFieldFromSuperLayer<IzhikevichNeuron>(0, 6, Point2D(4, 4), Point2D(2, 2));
   visNet->runNetwork(200000);
   VisualNetwork::saveNetwork(*visNet, "SL2.sav");
   delete visNet;
}

void thirdLayer()
{
   VisualNetwork* visNet = VisualNetwork::loadNetwork("SL2.sav");
   visNet->setLearningLock(true);
   visNet->setDefaultBoundingParameters(4, 20, 1.5, 3.5, 10, 10, 1, 20);
   int s3 = visNet->addReceptiveFieldFromSuperLayer<IzhikevichNeuron>(1, 6, Point2D(4, 4), Point2D(2, 2));
   visNet->runNetwork(200000);
   VisualNetwork::saveNetwork(*visNet, "SL3.sav");
   delete visNet;
}

void fourthLayer()
{
   VisualNetwork* visNet = VisualNetwork::loadNetwork("SL3.sav");
   visNet->setLearningLock(true);
   visNet->setDefaultBoundingParameters(4, 20, 2, 4, 10, 10, 1, 20);
   int s3 = visNet->addReceptiveFieldFromSuperLayer<IzhikevichNeuron>(2, 6, Point2D(5, 5), Point2D(2, 2));
   visNet->runNetwork(200000);
   VisualNetwork::saveNetwork(*visNet, "SL4.sav");
   delete visNet;
}

void test()
{
   VisualNetwork* visNet = VisualNetwork::loadNetwork("SL4.sav");
   visNet->setLearningLock(true);

   //visNet->logLayerActivity(1);
   //visNet->logLayerActivity(2);
   //visNet->logLayerActivity(3);
   //visNet->logLayerActivity(4);
   //visNet->logLayerActivity(5);
   //visNet->logLayerActivity(6);

   //visNet->logLayerActivity(7);
   //visNet->logLayerActivity(8);
   //visNet->logLayerActivity(9);
   //visNet->logLayerActivity(10);
   //visNet->logLayerActivity(11);
   //visNet->logLayerActivity(12);

   //visNet->logLayerActivity(13);
   //visNet->logLayerActivity(14);
   //visNet->logLayerActivity(15);
   //visNet->logLayerActivity(16);
   //visNet->logLayerActivity(17);
   //visNet->logLayerActivity(18);

   visNet->logLayerActivity(19);
   visNet->logLayerActivity(20);
   visNet->logLayerActivity(21);
   visNet->logLayerActivity(22);
   visNet->logLayerActivity(23);
   visNet->logLayerActivity(24);

   visNet->runNetwork(60000);
   delete visNet;
}

int main()
{
   _setmaxstdio(2000);
   test();

   return 0;
}
