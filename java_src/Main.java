import java.io.File;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.ShortBuffer;

import org.bytedeco.javacv.*;
import org.bytedeco.javacpp.*;
import org.bytedeco.javacpp.indexer.*;

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


        device.enable_stream(RealSense.color, 640, 480, RealSense.rgb8, 30);
        device.enable_stream(RealSense.depth, 640, 480, RealSense.z16, 30);

        device.start();

        OpenCVFrameConverter.ToIplImage converter = new OpenCVFrameConverter.ToIplImage();

        IplImage colorImage = null;
        IplImage depthImage = null;

        CanvasFrame colorFrame = new CanvasFrame("Color Stream",1); 
        CanvasFrame depthFrame = new CanvasFrame("Depth Stream",1); 
        CanvasFrame frameFrame = new CanvasFrame("Frame Stream",1); 

        // Frame capture loop
        while(true) {
            device.wait_for_frames();

            colorImage = grabColorImage();
            depthImage = grabDepthImage();

            // Display stream using Java2D frame 
            colorFrame.showImage(converter.convert(colorImage));
            depthFrame.showImage(converter.convert(depthImage));

            IplImage frameImage = IplImage.create(640, 480, IPL_DEPTH_8U, 1);

            // Convert and threshold image
            getFrameImage(depthImage, frameImage);

            frameFrame.showImage(converter.convert(frameImage));
        }

    }
    
    public static IplImage grabColorImage() {

        Pointer rawVideoImageData = new Pointer((Pointer) null);
        IplImage rawVideoImage = null;

        rawVideoImageData = device.get_frame_data(RealSense.color);

        int iplDepth = IPL_DEPTH_8U, channels = 3;
        int deviceWidth = device.get_stream_width(RealSense.color);
        int deviceHeight = device.get_stream_height(RealSense.color);

        rawVideoImage = IplImage.createHeader(deviceWidth, deviceHeight, iplDepth, channels);

        cvSetData(rawVideoImage, rawVideoImageData, deviceWidth * channels * iplDepth / 8);

        // ack, the camera's endianness doesn't correspond to our machine ...
        // swap bytes of 16-bit images

        // if (iplDepth > 8 && !ByteOrder.nativeOrder().equals(ByteOrder.BIG_ENDIAN)) {
        //     ByteBuffer bb = rawVideoImage.getByteBuffer();
        //     ShortBuffer in = bb.order(ByteOrder.BIG_ENDIAN).asShortBuffer();
        //     ShortBuffer out = bb.order(ByteOrder.LITTLE_ENDIAN).asShortBuffer();
        //     out.put(in);
        // }
        if (channels == 3) {
            cvCvtColor(rawVideoImage, rawVideoImage, CV_BGR2RGB);
        }   

        return rawVideoImage;
    }

    public static IplImage grabDepthImage() {

        Pointer rawDepthImageData = new Pointer((Pointer) null);
        IplImage rawDepthImage = null;

        rawDepthImageData = device.get_frame_data(RealSense.depth);

        int iplDepth = IPL_DEPTH_16U, channels = 1;
        int deviceWidth = device.get_stream_width(RealSense.depth);
        int deviceHeight = device.get_stream_height(RealSense.depth);

        rawDepthImage = IplImage.createHeader(deviceWidth, deviceHeight, iplDepth, channels);

        cvSetData(rawDepthImage, rawDepthImageData, deviceWidth * channels * iplDepth / 8);

        // WARNING: this part of code seems to screw depth data... Needs further investigations

        // if (iplDepth > 8 && !ByteOrder.nativeOrder().equals(ByteOrder.BIG_ENDIAN)) {
        //     ByteBuffer bb = rawDepthImage.getByteBuffer();
        //     ShortBuffer in = bb.order(ByteOrder.BIG_ENDIAN).asShortBuffer();
        //     ShortBuffer out = bb.order(ByteOrder.LITTLE_ENDIAN).asShortBuffer();
        //     out.put(in);
        // }

        return rawDepthImage;
    }

    public static void getFrameImage(IplImage src, IplImage dst) {

        UShortRawIndexer srcIdx = src.createIndexer();
        UByteRawIndexer dstIdx = dst.createIndexer();

        final int rows = src.height();
        final int cols = src.width();

        // Parallel computation: we need speed
        Parallel.loop(0, rows, new Parallel.Looper() { 
        public void loop(int from, int to, int looperID) {

        for(int i = 0; i < rows; i++) {

            for(int j = 0; j < cols; j++) {

                double p = srcIdx.get(i, j, 0);
                

                if(p == 0)
                     p = 65535;

                // TODO: Implement intellingent threshold with distance conversion
                // Threshold
                if(p > 5000)
                    p = 65535;

                dstIdx.put(i, j, 0, 255 - (int)(p * 255.0 / 65535));

            }
        }}});

        srcIdx.release();
        dstIdx.release();

        return ;
    }

}


