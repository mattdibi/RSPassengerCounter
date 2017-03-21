#include <iostream>

#include "RSPCN.cpp"

using namespace std;

int main()
{
    RSPCN myRSPCN(0);
    RSPCN myRSPCN1(1);

    cout << myRSPCN.getDeviceName() << endl;

    myRSPCN.setCalibration(true);
    myRSPCN.setDisplayDepth(false);
    myRSPCN.setDisplayRawDepth(false);
    myRSPCN.setFramerateStabilization(true);
    myRSPCN.setSaveVideo(false);

    myRSPCN1.setCalibration(true);
    myRSPCN1.setDisplayDepth(false);
    myRSPCN1.setDisplayRawDepth(false);
    myRSPCN1.setFramerateStabilization(true);
    myRSPCN1.setSaveVideo(false);

    myRSPCN.start();
    myRSPCN1.start();
    

    string input = "";

    // How to get a string/sentence with spaces
    while(1)
    {
        cout << "Please enter a valid command:\n>";
        getline(cin, input);
        cout << "You entered: " << input << endl;

        if(input.compare("reset") == 0)
        {
            cout << "Resetting counters!\n";
            myRSPCN.resetCounters();
        }
    }

    return 0;
}