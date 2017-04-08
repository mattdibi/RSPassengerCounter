import java.io.File;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.ShortBuffer;

import org.bytedeco.javacv.*;
import org.bytedeco.javacpp.*;

import org.bytedeco.javacpp.RealSense;
import org.bytedeco.javacpp.RealSense.context;
import org.bytedeco.javacpp.RealSense.device;

import static org.bytedeco.javacpp.opencv_core.*;
import static org.bytedeco.javacpp.opencv_imgproc.*;
import static org.bytedeco.javacpp.opencv_imgcodecs.*;

public class Main {

    private static context context = null;
    private static device device = null;

    public static void main(String[] args) {

        context = new context();
        System.out.println("Devices found: " + context.get_device_count());

        device = context.get_device(0);
        System.out.println("Using device 0, an " + device.get_name().getString());
        System.out.println("Using firmware version: " +  device.get_firmware_version().getString()); 
        System.out.println("Usb port id: " + device.get_usb_port_id().getString());


        device.enable_stream(RealSense.color, 640, 480, RealSense.rgb8, 60);
        device.enable_stream(RealSense.depth, 640, 480, RealSense.z16, 60);
        device.start();

        OpenCVFrameConverter.ToIplImage converter = new OpenCVFrameConverter.ToIplImage();
        CanvasFrame frame = new CanvasFrame("Depth Stream",1); 

        // while(true) will begin here
        while(true) {
            device.wait_for_frames();

            IplImage depthImage = new IplImage();
            IplImage colorImage = new IplImage();

            depthImage = grabDepthImage();
            colorImage = grabColorImage();

            //cvSaveImage("depth.jpg", depthImage);
            //cvSaveImage("color.jpg", colorImage);

            // displayImage in a frame
            frame.showImage(converter.convert(depthImage));
        }
        //return;

    }
    
    public static IplImage grabColorImage() {

        Pointer rawVideoImageData = new Pointer((Pointer) null);
        IplImage rawVideoImage = null;
        IplImage returnImage = null;

        int iplDepth = IPL_DEPTH_8U, channels = 3;

        rawVideoImageData = device.get_frame_data(RealSense.color);
        int deviceWidth = device.get_stream_width(RealSense.color);
        int deviceHeight = device.get_stream_height(RealSense.color);

        if (rawVideoImage == null || rawVideoImage.width() != deviceWidth || rawVideoImage.height() != deviceHeight) {
            rawVideoImage = IplImage.createHeader(deviceWidth, deviceHeight, iplDepth, channels);
        }

        cvSetData(rawVideoImage, rawVideoImageData, deviceWidth * channels * iplDepth / 8);

        return returnImage;
    }

    public static IplImage grabDepthImage() {

        Pointer rawDepthImageData = new Pointer((Pointer) null);
        IplImage rawDepthImage = null;

        rawDepthImageData = device.get_frame_data(RealSense.depth);

        int iplDepth = IPL_DEPTH_16U, channels = 1;
        int deviceWidth = device.get_stream_width(RealSense.depth);
        int deviceHeight = device.get_stream_height(RealSense.depth);

        if (rawDepthImage == null || rawDepthImage.width() != deviceWidth || rawDepthImage.height() != deviceHeight) {
            rawDepthImage = IplImage.createHeader(deviceWidth, deviceHeight, iplDepth, channels);
        }

        cvSetData(rawDepthImage, rawDepthImageData, deviceWidth * channels * iplDepth / 8);

        if (iplDepth > 8 && !ByteOrder.nativeOrder().equals(ByteOrder.BIG_ENDIAN)) {
            ByteBuffer bb = rawDepthImage.getByteBuffer();
            ShortBuffer in = bb.order(ByteOrder.BIG_ENDIAN).asShortBuffer();
            ShortBuffer out = bb.order(ByteOrder.LITTLE_ENDIAN).asShortBuffer();
            out.put(in);
        }

        return rawDepthImage;
    }

}


