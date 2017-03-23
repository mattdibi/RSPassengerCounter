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

        RSPCN myRSPCN = new RSPCN(0);

        myRSPCN.start();

        for(;;)
        {

        }

    }
}