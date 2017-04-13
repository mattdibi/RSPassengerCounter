public class Main {

    public static void main(String[] args) {

        RSPCN myRSPCN = new RSPCN(0);

        System.out.println( "Device name: " + myRSPCN.getDeviceName() );
        System.out.println( "Device serial: " + myRSPCN.getDeviceSerial() );
        System.out.println( "Device firmware: " + myRSPCN.getDeviceFirmware() );

        myRSPCN.start();
        // myRSPCN.stop();

        myRSPCN.setCameraPresets(2);

    }

}
