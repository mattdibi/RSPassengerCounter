import java.io.File;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.ShortBuffer;
import java.util.Vector;

import org.bytedeco.javacv.*;
import org.bytedeco.javacpp.*;
import org.bytedeco.javacpp.indexer.*;

import org.bytedeco.javacpp.RealSense;
import org.bytedeco.javacpp.RealSense.context;
import org.bytedeco.javacpp.RealSense.device;

import static org.bytedeco.javacpp.opencv_core.*;
import static org.bytedeco.javacpp.opencv_imgproc.*;
import static org.bytedeco.javacpp.opencv_imgcodecs.*;
import static org.bytedeco.javacpp.opencv_highgui.*;

public class Main {

    private static context context = null;
    private static device device = null;
    private static Vector<Passenger> passengers = new Vector<Passenger>(1,1);

    private static int cnt_out = 0;
    private static int cnt_in = 0;

    private static int pid = 0;

    private static final int maxPassengerAge = 2;
    private static final int fps = 30;

    private static final int MAX_1PASS_AREA = 60000;
    private static final int MAX_2PASS_AREA = 90000;

    private static int xNear = 40;
    private static int yNear = 90;

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

        OpenCVFrameConverter.ToIplImage converterToIpl = new OpenCVFrameConverter.ToIplImage();

        IplImage colorImage = null;
        IplImage depthImage = null;
        IplImage frameImage = IplImage.create(640, 480, IPL_DEPTH_8U, 1);

        CanvasFrame colorFrame = new CanvasFrame("Color Stream",1); 
        CanvasFrame depthFrame = new CanvasFrame("Depth Stream",1); 
        CanvasFrame frameFrame = new CanvasFrame("Frame Stream",1); 

        CvMemStorage contours = CvMemStorage.create();

        // Frame capture loop
        while(true) {
            device.wait_for_frames();

            // Grab data from RealSense camera
            colorImage = grabColorImage();
            depthImage = grabDepthImage();
            frameImage = grabFrameImage(depthImage);

            // Conversion needed 
            Mat frameMat = new Mat(frameImage);

            // Drawing line
            CvScalar colorred = new CvScalar( 0, 255, 0, 255);
            CvPoint p1 = new CvPoint(0,colorImage.height()/2);
            CvPoint p2 = new CvPoint(colorImage.width(), colorImage.height()/2);
            cvLine( colorImage,
                  p2,       //Starting point of the line
                  p1,       //Ending point of the line
                  colorred, //Color
                  2,        //Thickness
                  8,        //Linetype
                  0);       

            // Blurring image
            // TODO: Use IplImage equivalent
            Size blur_k_size = new Size(4, 4);
            blur(frameMat, frameMat, blur_k_size);

            // Finding contours
            CvSeq hierarchy = new CvSeq(null); // This is where contours will be accessed
            cvFindContours(frameImage, contours, hierarchy, Loader.sizeof(CvContour.class), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

            while (hierarchy != null && !hierarchy.isNull()) {

                if(hierarchy.elem_size() > 0) {

                    // Find polygon that approximate detected contours
                    CvSeq points = cvApproxPoly(hierarchy, Loader.sizeof(CvContour.class), contours, CV_POLY_APPROX_DP, cvContourPerimeter(hierarchy)*0.02, 0);

                    double areaCurrentObject = Math.abs(cvContourArea(points, CV_WHOLE_SEQ, 0)) ;

                    if(areaCurrentObject > 20000) {

                        // Find bounding rectangle of detected shape
                        CvRect br = cvBoundingRect(hierarchy);

                        // Find center of bounding rectangle of detected shape
                        CvPoint rectCenter = new CvPoint( (int)(br.x() + br.width()/2), (int)(br.y() + br.height()/2));

                        // Drawing rectangle
                        int x = br.x(), y = br.y(), w = br.width(), h = br.height();
                        cvRectangle(frameImage, cvPoint(x, y), cvPoint(x+w, y+h), CvScalar.WHITE, 1, CV_AA, 0);
                        cvRectangle(colorImage, cvPoint(x, y), cvPoint(x+w, y+h), CvScalar.GREEN, 1, CV_AA, 0);
                        // Drawing rectangle center
                        cvCircle(frameImage, rectCenter, 5, CvScalar.WHITE, 2, CV_AA, 0);
                        cvCircle(colorImage, rectCenter, 5, CvScalar.RED, 2, CV_AA, 0);

                        boolean newPassenger = true;
                        for(int i = 0; i < passengers.size(); i++) {
                            //If passenger is near a known passenger assume they are the same one
                            if( abs(rectCenter.x() - passengers.elementAt(i).getX()) <= xNear  && 
                                abs(rectCenter.y() - passengers.elementAt(i).getY()) <= yNear  ) {

                                newPassenger = false;
                                passengers.elementAt(i).updateCoords(rectCenter);

                                // -- COUNT
                                if(passengers.elementAt(i).getTracks().size() > 1)
                                {
                                   // Up to down 
                                   if( passengers.elementAt(i).getLastPoint().y() < frameImage.height()/2 &&  passengers.elementAt(i).getCurrentPoint().y() >= frameImage.height()/2  ||
                                       passengers.elementAt(i).getLastPoint().y() <= frameImage.height()/2 &&  passengers.elementAt(i).getCurrentPoint().y() > frameImage.height()/2 ) {

                                        // Counting multiple passenger depending on area size
                                        if (areaCurrentObject > MAX_1PASS_AREA && areaCurrentObject < MAX_2PASS_AREA)
                                            cnt_out += 2;
                                        else if (areaCurrentObject > MAX_2PASS_AREA)
                                            cnt_out += 3;
                                        else
                                            cnt_out++;

                                   }

                                   // Down to up
                                   if( passengers.elementAt(i).getLastPoint().y() > frameImage.height()/2 &&  passengers.elementAt(i).getCurrentPoint().y() <= frameImage.height()/2  ||
                                       passengers.elementAt(i).getLastPoint().y() >= frameImage.height()/2 &&  passengers.elementAt(i).getCurrentPoint().y() < frameImage.height()/2 ) {

                                        // Counting multiple passenger depending on area size
                                        if (areaCurrentObject > MAX_1PASS_AREA && areaCurrentObject < MAX_2PASS_AREA)
                                            cnt_in += 2;
                                        else if (areaCurrentObject > MAX_2PASS_AREA)
                                            cnt_in += 3;
                                        else
                                            cnt_in++;

                                   }


                                }
                                
                                break;
                            }

                        }

                        if(newPassenger) {
                        
                            Passenger pass = new Passenger(pid, rectCenter, 0);
                            passengers.add(pass);
                            pid++;

                        }
                    }
                }

                hierarchy = hierarchy.h_next();
                
            }

            // Draw trajectories and update age
            for(int i = 0; i < passengers.size(); i++) {

                if(passengers.elementAt(i).getTracks().size() > 1) {

                    for(int j = 0; j < passengers.elementAt(i).getTracks().size() - 1 ; j++) {
                        cvLine(colorImage,
                               passengers.elementAt(i).getTracks().elementAt(j),
                               passengers.elementAt(i).getTracks().elementAt(j + 1),
                               passengers.elementAt(i).getTrackColor(),
                               2,
                               8,
                               0);
                    }
                }
                
                passengers.elementAt(i).updateAge();

                if(passengers.elementAt(i).getAge() > (maxPassengerAge * fps)) {
                    passengers.remove(i);
                }
            }

            // Write current count
            CvPoint cntInLoc = new CvPoint(0, colorImage.height() - 30);
            CvPoint cntOutLoc = new CvPoint(0, colorImage.height() - 10);
            CvFont font = cvFont(1,1);

            cvPutText(colorImage, "Count IN:  " + cnt_in , cntInLoc , font, CvScalar.WHITE);
            cvPutText(colorImage, "Count OUT: " + cnt_out, cntOutLoc, font, CvScalar.WHITE);

            // Display streams using Java frame 
            colorFrame.showImage(converterToIpl.convert(colorImage));
            depthFrame.showImage(converterToIpl.convert(depthImage));
            frameFrame.showImage(converterToIpl.convert(frameImage));
            
            // cvSaveImage("color.jpg", colorImage);
            // cvSaveImage("depth.jpg", depthImage);
            // cvSaveImage("frame.jpg", frameImage);
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

        return rawDepthImage;
    }

    public static IplImage grabFrameImage(IplImage src) {

        IplImage dst = IplImage.create(640, 480, IPL_DEPTH_8U, 1);

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
                if(p > 8000)
                    p = 65535;

                dstIdx.put(i, j, 0, 255 - (int)(p * 255.0 / 65535));

            }
        }}});

        srcIdx.release();
        dstIdx.release();

        return dst;
    }

}


