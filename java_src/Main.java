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
            grabber.tryLoad();

            OpenCVFrameConverter.ToIplImage converter = new OpenCVFrameConverter.ToIplImage();

            grabber.start();

            try{
                Thread.sleep(1000); //Needed for camera initialization
            }catch(InterruptedException e) {
                System.out.println("Main interrupted");
            }
            
            System.out.println("Grabbing frame");

            // Create window named "Color"
            CanvasFrame canvas = new CanvasFrame("Color", CanvasFrame.getDefaultGamma()/grabber.getGamma());

            Frame grabbedImage = grabber.grab();
            int deviceWidth = grabber.getDepthImageWidth();
            int deviceHeight = grabber.getDepthImageHeight(); 

            while((grabbedImage = grabber.grab()) != null)
            {
                canvas.showImage(grabbedImage);

                IplImage depthImage = grabber.grabDepth();
                
                // Mat depthMat = new Mat(depthImage);
                // display(depthMat, "depthMat");

                //cvSaveImage("depth.jpg", depthImage);
            }

        } catch (FrameGrabber.Exception e) {
            System.out.println("FrameGrabber exception thrown: " + e);
        }
        
        return;
    }

    static void display(Mat image, String caption) {
        // Create image window named "My Image".
        final CanvasFrame canvas = new CanvasFrame(caption, 1.0);

        // Convert from OpenCV Mat to Java Buffered image for display
        final OpenCVFrameConverter converter = new OpenCVFrameConverter.ToMat();

        // Show image on window.
        canvas.showImage(converter.convert(image));
    }

}


