#include <VisualNetwork.h>

int main()
{
	VisualNetwork visNet;
	visNet.setBoundingParameters(3, 20, 0.5f, 2.5f, 10, 10, 1, 20);
	visNet.setSTDPParameters(0.99f, 0.002f, 0.002f, 1.0f, 100, 20.0f, 30.0f);
	int input = visNet.addLayer<IzhikevichNeuron>(Point2D(100,100),INPUT_LAYER);
	visNet.addReceptiveFieldSuperLayer<IzhikevichNeuron>(input,4,Point2D(7,7),Point2D(3,3));
	visNet.setInputImagesDirectory("inputPix");
	visNet.logPreSynapseWeights(1,Point2D(10,10));
	visNet.runNetwork(3600000);

	return 0;
}