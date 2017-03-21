#include <iostream>

#include "RSPCN.cpp"

using namespace std;

int main()
{
    RSPCN myRSPCN;

    cout << myRSPCN.getDeviceName() << endl;

    myRSPCN.setCalibration(true);
    myRSPCN.setDisplayDepth(false);
    myRSPCN.setDisplayRawDepth(false);
    myRSPCN.setFramerateStabilization(true);
    myRSPCN.setSaveVideo(false);

    myRSPCN.start();

    return 0;
}