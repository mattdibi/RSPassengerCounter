import java.io.*;
import java.util.Scanner;

public class Main {

    public static void displayHelp()
    {
        System.out.print("*****************************************\n");
        System.out.print("* COMMAND HELP: \n");
        System.out.print("* q: exit program\n");
        System.out.print("* r: resetting counters\n");
        System.out.print("* c: get passenger count\n");
        System.out.print("* p: set camera preset\n");
        System.out.print("* t: set threshold centimeters\n");
        System.out.print("* a: set max passenger age\n");
        System.out.print("* A: set area threshold\n");
        System.out.print("* b: set blur kernel size\n");
        System.out.print("* x: set xNear\n");
        System.out.print("* y: set yNear\n");
        System.out.print("* h: display this help message\n");
        System.out.print("*****************************************\n");

        return;
    }

    public static void main(String[] args) {

        char c;
        Scanner s= new Scanner(System.in);
        boolean stop = false;
        RSPCN myRSPCN = new RSPCN(0);

        System.out.println( "Device name: " + myRSPCN.getDeviceName() );
        System.out.println( "Device serial: " + myRSPCN.getDeviceSerial() );
        System.out.println( "Device firmware: " + myRSPCN.getDeviceFirmware() );

        System.out.println( );

        System.out.println( "Choose execution mode:" );
        System.out.println( "M: bare metal mode." );
        System.out.println( "N: normal mode." );
        System.out.println( "V: video recording mode." );
        System.out.println( "B: video recording in bare metal mode." );

        System.out.print("> ");
        c = s.next().charAt(0);

        if(c == 'M') {
            myRSPCN.setBareMetalMode(true);
            System.out.println("Bare metal mode activated");
        }
        else if(c == 'V') {
            myRSPCN.setVideoRecordMode(true);
            System.out.println("Video recording mode activated");
        }
        else if(c == 'B') {
            myRSPCN.setBareMetalMode(true);
            myRSPCN.setVideoRecordMode(true);
            System.out.println("Video recording in bare metal mode activated");
        }
        else {
            System.out.println("Normal mode activated");
        }

        myRSPCN.start();

        System.out.println( "Insert command: " );

        do {
            System.out.print("> ");

            c = s.next().charAt(0);

            System.out.println("You entered: " + c);

            switch(c) {
                case('q'): 
                    System.out.println( "Exiting program" );
                    myRSPCN.stop();
                    stop = true;
                    break;

                case('r'): 
                    System.out.println( "Resetting counters" );
                    myRSPCN.resetCounters();
                    break;
                
                case ('c'): 
                    System.out.println( "Current count: ");
                    System.out.println("Count in  > " + myRSPCN.getCnt_in());
                    System.out.println("Count out > " + myRSPCN.getCnt_out());
                    System.out.println("Current balance > " + (myRSPCN.getCnt_in() - myRSPCN.getCnt_out()));
                    break;
                    
                case ('p'): 
                    System.out.println( "Set preset: ");
                    System.out.print("New preset > ");
                    int preset = s.nextInt();
                    myRSPCN.setCameraPresets(preset);
                    break;

                case ('t'): 
                    System.out.println( "Set threshold: ");
                    System.out.print("New threshold > ");
                    int centimeters = s.nextInt();
                    myRSPCN.setThresholdCentimeters(centimeters);
                    break;
                
                case ('a') :
                    System.out.println( "Set passenger age: ");
                    System.out.print("New age > ");
                    int age = s.nextInt();
                    myRSPCN.setMaxPassengerAge(age);
                    break;

                case ('A') :
                    System.out.println( "Set area threshold: ");
                    System.out.print("New area > ");
                    int area = s.nextInt();
                    myRSPCN.setAreaThreshold(area);
                    break;
                
                case ('b') :
                    System.out.println( "Set blur: ");
                    System.out.print("New blur > ");
                    int blur = s.nextInt();
                    myRSPCN.setBlurSize(blur);
                    break;
                
                case ('x') :
                    System.out.println( "Set xNear: ");
                    System.out.print("New xNear > ");
                    int xNear = s.nextInt();
                    myRSPCN.setXNear(xNear);
                    break;
                
                case ('y') :
                    System.out.println( "Set yNear: ");
                    System.out.print("New yNear > ");
                    int yNear = s.nextInt();
                    myRSPCN.setYNear(yNear);
                    break;
                
                default:
                    displayHelp();
                    break;
            }

        } while(!stop);

        return;
    }

}
