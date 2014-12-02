#include <VisualNetwork.h>
#include <iostream>

void firstLayer()
{
   VisualNetwork visNet;
   visNet.setDefaultSTDPParameters(0.99f, 0.002f, 0.002f, 1.0f, 100, 20.0f, 30.0f);
   int input = visNet.addLayer<IzhikevichNeuron>(Point2D(120, 120), INPUT_LAYER);
   visNet.setInputImagesDirectory("C:/Pix/101Object-edited/resize/");

   visNet.setDefaultBoundingParameters(1.7f, 30, 0.0f, 1.0f, 15, 15, 1, 5);
   int s1 = visNet.addSimpleCellSuperLayer<IzhikevichNeuron>(input, 6, Point2D(10,10), Point2D(4,4), new IzhikevichParameters(0.02, 0.2, -65, 6));

   visNet.setDefaultBoundingParameters(20.0f, 0, 20.0f, 20.0f, 0, 0, 0, 0);
   int c1 = visNet.addComplexCellFromSuperLayer<IzhikevichNeuron>(s1, Point2D(3, 3), Point2D(1, 1), new IzhikevichParameters(0.02, 0.2, -65, 6));

   visNet.runNetwork(EPOCH_NUMBER, 10);
   VisualNetwork::saveNetwork(visNet, "SL1.sav");
}

void secondLayer()
{
   VisualNetwork* visNet = VisualNetwork::loadNetwork("SL1.sav");
   visNet->setLearningLock(true);

   visNet->setDefaultBoundingParameters(1.5f, 30, 0.2f, 1.2f, 15, 15, 1, 5);
   int s2 = visNet->addSimpleCellFromSuperLayer<IzhikevichNeuron>(1, 6, Point2D(3, 3), Point2D(2, 2));

   visNet->setDefaultBoundingParameters(20.0f, 0, 20.0f, 20.0f, 0, 0, 0, 0);
   int c2 = visNet->addComplexCellFromSuperLayer<IzhikevichNeuron>(s2, Point2D(3, 3), Point2D(1, 1), new IzhikevichParameters(0.02, 0.2, -65, 6));

   visNet->runNetwork(EPOCH_NUMBER, 10);
   VisualNetwork::saveNetwork(*visNet, "SL2.sav");
   delete visNet;
}

void thirdLayer()
{
   VisualNetwork* visNet = VisualNetwork::loadNetwork("SL2.sav");
   visNet->setLearningLock(true);

   visNet->setDefaultBoundingParameters(1.5f, 30, 0.2f, 1.2f, 15, 15, 1, 10);
   int s3 = visNet->addSimpleCellFromSuperLayer<IzhikevichNeuron>(3, 6, Point2D(4, 4), Point2D(2, 2));

   visNet->setDefaultBoundingParameters(20.0f, 0, 20.0f, 20.0f, 0, 0, 0, 0);
   int c3 = visNet->addComplexCellFromSuperLayer<IzhikevichNeuron>(s3, Point2D(3, 3), Point2D(1, 1), new IzhikevichParameters(0.02, 0.2, -65, 6));

   visNet->runNetwork(EPOCH_NUMBER, 10);
   VisualNetwork::saveNetwork(*visNet, "SL3.sav");
   delete visNet;
}

void fourthLayer()
{
   VisualNetwork* visNet = VisualNetwork::loadNetwork("SL3.sav");
   visNet->setLearningLock(true);

   visNet->setDefaultBoundingParameters(1.5f, 30, 0.2f, 1.2f, 15, 15, 1, 10);
   int s3 = visNet->addSimpleCellFromSuperLayer<IzhikevichNeuron>(5, 6, Point2D(5, 5), Point2D(2, 2));

   visNet->runNetwork(EPOCH_NUMBER, 10);
   VisualNetwork::saveNetwork(*visNet, "SL4.sav");
   delete visNet;
}

void test()
{
   VisualNetwork* visNet = VisualNetwork::loadNetwork("SL3.sav");
   visNet->setLearningLock(true);

   visNet->logSuperLayerActivity(5);

   visNet->runNetwork(EPOCH_NUMBER, 1);
   delete visNet;
}

int main()
{
   //firstLayer();
   //secondLayer();
   //thirdLayer();
   //fourthLayer();
   test();
   
   return 0;
}
