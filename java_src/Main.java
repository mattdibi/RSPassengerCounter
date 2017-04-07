import org.bytedeco.javacv.*;

import org.bytedeco.javacpp.BytePointer;
import org.bytedeco.javacpp.Loader;
import org.bytedeco.javacpp.Pointer;

import org.bytedeco.javacpp.RealSense;
import org.bytedeco.javacpp.RealSense.context;
import org.bytedeco.javacpp.RealSense.device;

import static org.bytedeco.javacpp.opencv_core.*;
import static org.bytedeco.javacpp.opencv_imgproc.*;
import static org.bytedeco.javacpp.opencv_calib3d.*;
import static org.bytedeco.javacpp.opencv_objdetect.*;

public class Main {

    public static void main(String[] args) {

        try{
            RealSenseFrameGrabber grabber = RealSenseFrameGrabber.createDefault(0);
        } catch (FrameGrabber.Exception e) {
            System.out.println("FrameGrabber exception thrown: " + e);
        }
        
        try{
            String[] devDescription;
            devDescription = RealSenseFrameGrabber.getDeviceDescriptions();

            int size = devDescription.length;

            System.out.println("Device description: ");

            for(int i = 0; i < size; i++)
            {
                System.out.println(devDescription[i]);
            }


        } catch (FrameGrabber.Exception e) {
            System.out.println("FrameGrabber exception thrown: " + e);
        }
        
//        grabber.enableDepthStream();
//
//        try{
//            grabber.start();
//        } catch (FrameGrabber.Exception e) {
//            System.out.println("FrameGrabber exception thrown: " + e);
//        }
//


        System.out.println("END");
    }
}


