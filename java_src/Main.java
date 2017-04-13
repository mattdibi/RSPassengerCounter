import java.io.*;
import java.util.Scanner;

public class Main {

    public static void main(String[] args) {

        char c;
        Scanner s= new Scanner(System.in);
        boolean stop = false;
        RSPCN myRSPCN = new RSPCN(0);

        System.out.println( "Device name: " + myRSPCN.getDeviceName() );
        System.out.println( "Device serial: " + myRSPCN.getDeviceSerial() );
        System.out.println( "Device firmware: " + myRSPCN.getDeviceFirmware() );

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
                    // displayHelp();
                    break;
            }

        } while(!stop);

        return;
    }

}
