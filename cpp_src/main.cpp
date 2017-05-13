#include <iostream>
#include <chrono>

#include "RSPCN.cpp"

using namespace std;

void displayHelp() {
    cout << "*****************************************\n";
    cout << "* COMMAND HELP: \n";
    cout << "* 0 - 5: selecting camera presets\n";
    cout << "* r: resetting counters\n";
    cout << "* p: get passenger count\n";
    cout << "* c: toggle display color\n";
    cout << "* C: toggle display calibration\n";
    cout << "* d: toggle display depth view\n";
    cout << "* D: toggle display raw depth view\n";
    cout << "* f: toggle display frame view\n";
    cout << "* s: toggle frame rate stabilization\n";
    cout << "* q: exit program\n";
    cout << "* h: display this help message\n";
    cout << "*****************************************\n";

    return;
}

int main(int argc, char * argv[]) {
    bool stop = false;
    bool displayHelpFlag = false;
    char choice;

    vector<RSPCN *> counters;
    context ctx;
    int deviceNumber = ctx.get_device_count();

    for(int i = 0; i < deviceNumber; i++) {
        counters.push_back(new RSPCN(ctx.get_device(i)));
        cout << endl; 
        cout << "Device number: " << i << endl;
        cout << "Device name: " << counters[i]->getDeviceName() << endl;
        cout << "Device serial: " << counters[i]->getDeviceSerial() << endl;
        cout << "Device firmware: " << counters[i]->getDeviceFirmware() << endl;
    }

    if(argc >= 2) {
        if(!strcmp(argv[1], "-s")) {
            for(int i = 0; i < deviceNumber; i++) {
                counters[i]->setSaveVideo(true);
            }
        } else {
            cout << "Unknown option. Defaulted to normal mode." << endl;
        }
    }

    for(int i = 0; i < deviceNumber; i++) {
        counters[i]->start();
    }

    displayHelp();

    do {
        cout << "Please enter a valid command:\n>";
        choice = getchar();
        cout << "You entered: " << choice << endl;

        for(int i = 0; i < deviceNumber; i++) {
            
            if(choice >= '0' && choice <= '5') {
                cout << "Setting camera presets: " << choice << endl;
                counters[i]->setCameraPresets((int)(choice - '0'));
            }
            else {
                cout << "Device : " << i << endl;
                switch(choice) {
                    case('p'):
                        cout << "Current count:\n";
                        cout << "Count in  = " << counters[i]->getCountIn() << endl;
                        cout << "Count out = " << counters[i]->getCountOut() << endl;
                        cout << "Current balance =" << counters[i]->getCountIn() - counters[i]->getCountOut() << endl;
                        break;

                    case('r'):
                        cout << "Resetting counters\n";
                        counters[i]->resetCounters();
                        break;

                    case('c'):
                        cout << "Toggle color\n";
                        counters[i]->toggleDisplayColor();
                        break;
                    
                    case('C'):
                        cout << "Toggle calibration\n";
                        counters[i]->toggleCalibration();
                        break;

                    case('d'):
                        cout << "Toggle depth view\n";
                        counters[i]->toggleDisplayDepth();;
                        break;

                    case('D'):
                        cout << "Toggle raw depth view\n";
                        counters[i]->toggleDisplayRawDepth();;
                        break;

                    case('f'):
                        cout << "Toggle frame view\n";
                        counters[i]->toggleDisplayFrame();
                        break;
                    
                    case('s'):
                        cout << "Toggle frame rate stabilization\n";
                        counters[i]->toggleFrameRateStabilization();
                        break;
                    
                    case('q'):
                        cout << "Exiting program!\n";
                        counters[i]->stop();
                        // Waiting for counter to stop (better implementation: synch threads)
                        std::this_thread::sleep_for (std::chrono::seconds(1));
                        cout << "Capture closed.\n";
                        stop = true;
                        break;

                    default:
                        displayHelpFlag = true;
                        break;
                }
            }
        }

        if(displayHelpFlag) {
            displayHelp();
            displayHelpFlag = false;
        }

        // Consume input
        while ((choice = getchar()) != '\n' && choice != EOF);

    } while(!stop);

    return 0;
}
