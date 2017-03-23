#include <iostream>

#include "RSPCN.cpp"

using namespace std;

void displayHelp()
{
    cout << "*****************************************\n";
    cout << "* COMMAND HELP: \n";
    cout << "* 0 - 5: selecting camera presets\n";
    cout << "* r: resetting counters\n";
    cout << "* c: toggle display calibration\n";
    cout << "* d: toggle display depth view\n";
    cout << "* D: toggle display raw depth view\n";
    cout << "* f: toggle display frame view\n";
    cout << "* s: toggle frame rate stabilization\n";
    cout << "* q: exit program\n";
    cout << "*****************************************\n";

    return;
}

int main()
{
    bool stop = false;
    char choice;

    RSPCN myRSPCN_0(0);
    // RSPCN myRSPCN_1(1);

    myRSPCN_0.start();
    // myRSPCN_1.start();

    displayHelp();

    do
    {
        cout << "Please enter a valid command:\n>";
        choice = getchar();
        cout << "You entered: " << choice << endl;

        if(choice >= '0' && choice <= '5')
        {
            cout << "Setting camera presets: " << choice << endl;
            myRSPCN_0.setCameraPresets((int)(choice - '0'));
        }
        else
        {
            switch(choice)
            {
                case('h'):
                    displayHelp();
                    break;

                case('r'):
                    cout << "Resetting counters\n";
                    myRSPCN_0.resetCounters();
                    break;
                
                case('c'):
                    cout << "Toggle calibration\n";
                    myRSPCN_0.toggleCalibration();
                    break;

                case('d'):
                    cout << "Toggle depth view\n";
                    myRSPCN_0.toggleDisplayDepth();;
                    break;

                case('D'):
                    cout << "Toggle raw depth view\n";
                    myRSPCN_0.toggleDisplayRawDepth();;
                    break;

                case('f'):
                    cout << "Toggle frame view\n";
                    myRSPCN_0.toggleDisplayFrame();
                    break;
                
                case('s'):
                    cout << "Toggle frame rate stabilization\n";
                    myRSPCN_0.toggleFrameRateStabilization();
                    break;
                
                case('q'):
                    cout << "Exiting program!\n";
                    myRSPCN_0.stop();
                    // myRSPCN_1.stop();
                    cout << "Capture closed.\n";
                    stop = true;
                    break;

                default:
                    displayHelp();
                    break;
            }
        }

    } while(!stop);

    return 0;
}