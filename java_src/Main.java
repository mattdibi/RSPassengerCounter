import java.io.*;
import java.util.Vector;
import java.util.Scanner;
import org.bytedeco.javacpp.RealSense.*;

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

        context context = new context();
        Vector<RSPCN> counters = new Vector<RSPCN>(1,1);

        char c;
        Scanner s= new Scanner(System.in);
        boolean stop = false;
        boolean dispH = false;

        int devCount = context.get_device_count();
        System.out.println( "Device count: " + devCount);

        for(int i = 0; i < devCount; i++) {
            counters.add(new RSPCN(context, i));
            System.out.println( );
            System.out.println( "Device number: " + i);
            System.out.println( "Device name: " + counters.elementAt(i).getDeviceName() );
            System.out.println( "Device serial: " + counters.elementAt(i).getDeviceSerial() );
            System.out.println( "Device firmware: " + counters.elementAt(i).getDeviceFirmware() );
        }

        System.out.println( );

        System.out.println( "Choose execution mode:" );
        System.out.println( "M: bare metal mode." );
        System.out.println( "N: normal mode." );
        System.out.println( "V: video recording mode." );
        System.out.println( "B: video recording in bare metal mode." );

        System.out.print("> ");
        c = s.next().charAt(0);
        // TODO: Add input control

        for(int i = 0; i < devCount; i++) {
            if(c == 'M') {
                counters.elementAt(i).setBareMetalMode(true);
                System.out.println("Bare metal mode activated");
            }
            else if(c == 'V') {
                counters.elementAt(i).setVideoRecordMode(true);
                System.out.println("Video recording mode activated");
            }
            else if(c == 'B') {
                counters.elementAt(i).setBareMetalMode(true);
                counters.elementAt(i).setVideoRecordMode(true);
                System.out.println("Video recording in bare metal mode activated");
            }
            else {
                System.out.println("Normal mode activated");
            }
            counters.elementAt(i).start();
        }

        System.out.println( "Insert command: " );
        do {
            System.out.print("> ");

            c = s.next().charAt(0);
            // TODO: Add input control

            System.out.println("You entered: " + c);

            for(int i = 0; i < devCount; i++) {
                switch(c) {
                    case('q'): 
                        System.out.println( "Closing device: " + i );
                        counters.elementAt(i).stop();
                        stop = true;
                        break;

                    case('r'): 
                        System.out.println( "Resetting counters device: " + i );
                        counters.elementAt(i).resetCounters();
                        break;
                    
                    case ('c'): 
                        System.out.println( );
                        System.out.println("Device: " + i);
                        System.out.println("Count in  > " + counters.elementAt(i).getCnt_in());
                        System.out.println("Count out > " + counters.elementAt(i).getCnt_out());
                        System.out.println("Current balance > " + (counters.elementAt(i).getCnt_in() - counters.elementAt(i).getCnt_out()));
                        break;
                        
                    case ('p'): 
                        System.out.println( "Set preset device: " + i);
                        System.out.print("New preset > ");
                        int preset = s.nextInt();
                        //TODO: Add input control
                        counters.elementAt(i).setCameraPresets(preset);
                        break;

                    case ('t'): 
                        System.out.println( "Set threshold device: " + i);
                        System.out.print("New threshold > ");
                        int centimeters = s.nextInt();
                        //TODO: Add input control
                        counters.elementAt(i).setThresholdCentimeters(centimeters);
                        break;
                    
                    case ('a') :
                        System.out.println( "Set passenger age device: " + i);
                        System.out.print("New age > ");
                        int age = s.nextInt();
                        //TODO: Add input control
                        counters.elementAt(i).setMaxPassengerAge(age);
                        break;

                    case ('A') :
                        System.out.println( "Set area threshold device: " + i);
                        System.out.print("New area > ");
                        int area = s.nextInt();
                        //TODO: Add input control
                        counters.elementAt(i).setAreaThreshold(area);
                        break;
                    
                    case ('b') :
                        System.out.println( "Set blur device: " + i);
                        System.out.print("New blur > ");
                        int blur = s.nextInt();
                        //TODO: Add input control
                        counters.elementAt(i).setBlurSize(blur);
                        break;
                    
                    case ('x') :
                        System.out.println( "Set xNear device: " + i);
                        System.out.print("New xNear > ");
                        int xNear = s.nextInt();
                        //TODO: Add input control
                        counters.elementAt(i).setXNear(xNear);
                        break;
                    
                    case ('y') :
                        System.out.println( "Set yNear device: " + i);
                        System.out.print("New yNear > ");
                        int yNear = s.nextInt();
                        //TODO: Add input control
                        counters.elementAt(i).setYNear(yNear);
                        break;
                    
                    default:
                        dispH = true;
                        break;
                }
            }
            
            if(dispH) {
                displayHelp();
                dispH = false;
            }

        } while(!stop);

        return;
    }

}
