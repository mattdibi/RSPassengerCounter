#include <iostream>

#include "RSPCN.cpp"

using namespace std;

void displayHelp()
{
    cout << "\n* Command help: *\n";
    cout << "reset: reset counters\n";
    cout << "calib: activate calibration mode\n";
    cout << "exit: exit program\n";

    return;
}

int main()
{
    bool stop = false;
    string input = "";

    RSPCN myRSPCN_0(0);
    RSPCN myRSPCN_1(1);

    myRSPCN_0.start();
    myRSPCN_1.start();

    while(!stop)
    {
        cout << "Please enter a valid command:\n>";
        getline(cin, input);
        cout << "You entered: " << input << endl;

        if(input.compare("reset") == 0)
        {
            cout << "Resetting counters!\n";
            myRSPCN_0.resetCounters();
        }
        else if(input.compare("calib") == 0)
        {
            cout << "Setting calibration on!\n";
            myRSPCN_0.setCalibration(true);
        }
        else if(input.compare("exit") == 0)
        {
            cout << "Exiting program!\n";

            myRSPCN_0.stop();
            myRSPCN_1.stop();

            cout << "Capture closed.\n";

            stop = true;
        }
        else
        {
            displayHelp();
        }
    }

    return 0;
}