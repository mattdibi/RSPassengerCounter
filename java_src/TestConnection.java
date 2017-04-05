import org.bytedeco.javacpp.RealSense;
import org.bytedeco.javacpp.RealSense.context;
import org.bytedeco.javacpp.RealSense.device;

import org.opencv.core.Core;                                                                                                                                                                                                              
import org.opencv.core.Mat;
import org.opencv.core.CvType;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.highgui.Highgui;
import org.opencv.highgui.VideoCapture;

public class TestConnection {

    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }

    public static void main(String[] args) {
        // OpenCV Testing
        System.out.println("Welcome to OpenCV " + Core.VERSION);

        VideoCapture camera = new VideoCapture(0);

        try{
            Thread.sleep(1000); //Needed for camera initialization
        }catch(InterruptedException e) {
            System.out.println("Main interrupted");
        }

        Mat frame = new Mat();
        camera.read(frame); 

        if(!camera.isOpened()){
            System.out.println("Error");
        }

        camera.read(frame);
        System.out.println("Frame Obtained");

        Highgui.imwrite("camera.jpg",frame);

        // librealsense Testing
        context context = new context();
        System.out.println("Devices found: " + context.get_device_count());

        device device = context.get_device(0);
        System.out.println("Using device 0, an " + device.get_name().getString());
        System.out.println("Using firmware version: " +  device.get_firmware_version().getString()); 
        System.out.println("Usb port id: " + device.get_usb_port_id().getString());

        device.enable_stream(RealSense.depth, 640, 480, RealSense.z16, 60);
        device.start();

        // System.out.println("Test get_frame_data(): " + device.get_frame_data(RealSense.depth).getStringBytes());

        Size sz = new Size(640,480);

        Mat depth = new Mat(sz, CvType.CV_16U ,device.get_frame_data(RealSense.depth));
        // depth = device.get_frame_data(RealSense.depth); 

        // Display depth vision.
        Highgui.imwrite("depth.jpg", depth);
    }
}
