import java.io.File;

import org.bytedeco.javacv.*;
import org.bytedeco.javacpp.*;

import org.bytedeco.javacpp.RealSense;
import org.bytedeco.javacpp.RealSense.context;
import org.bytedeco.javacpp.RealSense.device;

import static org.bytedeco.javacpp.opencv_core.*;
import static org.bytedeco.javacpp.opencv_imgproc.*;
import static org.bytedeco.javacpp.opencv_imgcodecs.*;

public class Main {

    public static void main(String[] args) {

        try{
            RealSenseFrameGrabber grabber = RealSenseFrameGrabber.createDefault(0);

            String[] devDescription;
            devDescription = grabber.getDeviceDescriptions();

            int size = devDescription.length;

            System.out.println("Device description: ");

            for(int i = 0; i < size; i++)
            {
                System.out.println(devDescription[i]);
            }

            OpenCVFrameConverter.ToIplImage converter = new OpenCVFrameConverter.ToIplImage();

            grabber.start();

            try{
                Thread.sleep(1000); //Needed for camera initialization
            }catch(InterruptedException e) {
                System.out.println("Main interrupted");
            }
            
            System.out.println("Grabbing frame");

            // Create image window named "My Image".
            CanvasFrame canvas = new CanvasFrame("My Image", CanvasFrame.getDefaultGamma()/grabber.getGamma());
            Frame grabbedImage = grabber.grab();

            while((grabbedImage = grabber.grab()) != null)
            {
                canvas.showImage(grabbedImage);
            }

        } catch (FrameGrabber.Exception e) {
            System.out.println("FrameGrabber exception thrown: " + e);
        }
        
    }
}


