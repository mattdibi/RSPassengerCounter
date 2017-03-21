#include <iostream>

#include "RSPCN.cpp"

using namespace std;

int main()
{
    RSPCN myRSPCN;

    myRSPCN.setCalibration(false);
    myRSPCN.setDisplayDepth(true);
    myRSPCN.setFramerateStabilization(true);
    myRSPCN.setSaveVideo(true);

    myRSPCN.start();

    return 0;
}