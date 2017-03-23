import java.io.*;

public class Main {

    static {
        try {
            System.loadLibrary("RSPCN");
        } catch (UnsatisfiedLinkError e) {
        System.err.println("Native code library failed to load. See the chapter on Dynamic Linking Problems in the SWIG Java documentation for help.\n" + e);
        System.exit(1);
        }
    }

    public static void main(String[] args) {
        char c = 'x';
        boolean stop = false;
        BufferedReader br = new BufferedReader(new InputStreamReader(System.in));

        RSPCN myRSPCN = new RSPCN(0);
        // RSPCN myRSPCN_1 = new RSPCN(1);

        myRSPCN.start();
        // myRSPCN_1.start();

        System.out.println("Enter characters, 'q' to quit.");

        do
        {
            System.out.print("> ");

            try{
                c = (char) br.read();
            } catch (IOException e) {
                System.out.println("Error reading character!");
            }
        
            System.out.println("You entered: " + c);

            if(c == 'q')
            {
                System.out.println("Exiting program.");
                stop = true;
                myRSPCN.stop();
                // myRSPCN_1.stop();
            }
            else if( c == 'r')
            {
                System.out.println("Resetting counters.");
                myRSPCN.resetCounters();
            }
            else if( c == 'f')
            {
                System.out.println("Displaying frame view.");
                myRSPCN.toggleDisplayFrame();
            }
            else if( c == 'c')
            {
                System.out.println("Displaying calibration view.");
                myRSPCN.toggleCalibration();
            }
            else if( c == '2')
            {
                System.out.println("Changing camera presets.");
                myRSPCN.setCameraPresets(2);
            }
            else if (c == 'd')
            {
                System.out.println("Displaying depth view.");
                myRSPCN.toggleDisplayDepth();
            }

        }while(!stop);

    }
}