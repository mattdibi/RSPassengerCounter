import java.io.*;
import java.util.Scanner;

public class Main {

    static {
        try {
            System.loadLibrary("RSPCN");
        } catch (UnsatisfiedLinkError e) {
        System.err.println("Native code library failed to load. See the chapter on Dynamic Linking Problems in the SWIG Java documentation for help.\n" + e);
        System.exit(1);
        }
    }

    public static void displayHelp()
    {
        System.out.print("*****************************************\n");
        System.out.print("* COMMAND HELP: \n");
        System.out.print("* 0 - 5: selecting camera presets\n");
        System.out.print("* r: resetting counters\n");
        System.out.print("* c: toggle display calibration\n");
        System.out.print("* d: toggle display depth view\n");
        System.out.print("* D: toggle display raw depth view\n");
        System.out.print("* f: toggle display frame view\n");
        System.out.print("* s: toggle frame rate stabilization\n");
        System.out.print("* q: exit program\n");
        System.out.print("* h: display this help message\n");
        System.out.print("*****************************************\n");

        return;
    }

    public static void main(String[] args) {
        
        char c;
        boolean stop = false;
        Scanner s= new Scanner(System.in);

        RSPCN myRSPCN = new RSPCN(0);
        // RSPCN myRSPCN_1 = new RSPCN(1);

        myRSPCN.start();
        // myRSPCN_1.start();

        System.out.println("Enter characters, 'q' to quit.");

        do
        {
            System.out.print("> ");

            c = s.next().charAt(0);
        
            System.out.println("You entered: " + c);

            if(c >= '0' && c <= '5')
            {
                System.out.println("Setting camera presets: ");
                myRSPCN.setCameraPresets((int)(c - '0'));
            }
            else
            {
                switch(c)
                {
                    case('h'):
                        displayHelp();
                        break;

                    case('r'):
                        System.out.println("Resetting counters");
                        myRSPCN.resetCounters();
                        break;
                    
                    case('c'):
                        System.out.println("Toggle calibration");
                        myRSPCN.toggleCalibration();
                        break;

                    case('d'):
                        System.out.println("Toggle depth view");
                        myRSPCN.toggleDisplayDepth();;
                        break;

                    case('D'):
                        System.out.println("Toggle raw depth view");
                        myRSPCN.toggleDisplayRawDepth();;
                        break;

                    case('f'):
                        System.out.println("Toggle frame view");
                        myRSPCN.toggleDisplayFrame();
                        break;
                    
                    case('s'):
                        System.out.println("Toggle frame rate stabilization");
                        myRSPCN.toggleFrameRateStabilization();
                        break;
                    
                    case('q'):
                        System.out.println("Exiting program.");
                        myRSPCN.stop();
                        // myRSPCN_1.stop();
                        System.out.println("Capture closed.");
                        stop = true;
                        break;

                    default:
                        displayHelp();
                        break;
                }
            }

        } while(!stop);

    }
}