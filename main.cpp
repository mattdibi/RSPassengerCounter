#include <iostream>

#include "RSPCN.cpp"

using namespace std;

void displayHelp()
{
    cout << "*****************************************\n";
    cout << "* COMMAND HELP: \n";
    cout << "* 0 - 4: selecting camera presets\n";
    cout << "* 5: resetting counters\n";
    cout << "* 6: display calibration\n";
    cout << "* 7: display depth view\n";
    cout << "* 8: display raw depth view\n";
    cout << "* 9: displat frame view\n";
    cout << "* 0: exit program\n";
    cout << "*****************************************\n";

    return;
}

int getInput(void)
{
	int ch;
	int ret = -1;

	ch = getchar();

	if (ch >= '0' && ch <= '9')
		ret = ch - '0';

	while ((ch = getchar()) != '\n' && ch != EOF);

	return ret;
}

int main()
{
    bool stop = false;
    int choice;

    RSPCN myRSPCN_0(0);
    // RSPCN myRSPCN_1(1);

    myRSPCN_0.start();
    // myRSPCN_1.start();

    displayHelp();

    do
    {
        cout << "Please enter a valid command:\n>";
        choice = getInput();

        if(choice > 0 && choice < 5)
        {
            cout << "Setting camera presets: " << choice << endl;
            myRSPCN_0.setCameraPresets(choice);
        }
        else if(choice == 5)
        {
            cout << "Resetting counters\n";
            myRSPCN_0.resetCounters();
        }
        else if(choice == 6)
        {
            cout << "Display calibration\n";
            myRSPCN_0.setCalibration(true);
        }
        else if(choice == 7)
        {
            cout << "Display depth view\n";
            myRSPCN_0.setDisplayDepth(true);
        }
        else if(choice == 8)
        {
            cout << "Display raw depth view\n";
            myRSPCN_0.setDisplayRawDepth(true);
        }
        else if(choice == 9)
        {
            cout << "Display frame view\n";
            myRSPCN_0.setDisplayFrame(true);
        }
        else if(choice == 0)
        {
            cout << "Exiting program!\n";

            myRSPCN_0.stop();
            // myRSPCN_1.stop();

            cout << "Capture closed.\n";

            stop = true;
        }
        else
        {
            displayHelp();
        }
    } while(!stop);

    return 0;
}