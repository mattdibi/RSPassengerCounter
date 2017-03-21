#include <iostream>

#include "RSPCN.cpp"

using namespace std;

int main()
{
    RSPCN myRSPCN;

    myRSPCN.setCalibration(false);
    myRSPCN.setDisplayDepth(true);
    myRSPCN.setFramerateStabilization(true);
    myRSPCN.setSaveVideo(false);

    myRSPCN.start();

    return 0;
}