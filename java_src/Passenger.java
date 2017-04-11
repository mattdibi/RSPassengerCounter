import java.util.Vector;
import java.util.Random;

import org.bytedeco.javacv.*;
import org.bytedeco.javacpp.*;
import static org.bytedeco.javacpp.opencv_core.*;

public class Passenger {
    private static final int MAX_TRACK_LENGTH = 4;

    private CvPoint center;
    private Vector<CvPoint> tracks = new Vector<CvPoint>(1,1);
    
    private int pid;
    private int age;
    private Scalar trackColor;

    // Constructor
    Passenger(int id, CvPoint c, int newAge) {
        pid = id;
        center = c;
        age = newAge;

        CvPoint newCenter = new CvPoint(c.x(), c.y());
        tracks.add(newCenter);
        
        Random randomGenerator = new Random();
        Scalar randColor = new Scalar(randomGenerator.nextInt(255),randomGenerator.nextInt(255),randomGenerator.nextInt(255), 255);
        trackColor = randColor;

    }

    // Selectors
    public int getPid() {return pid;}
    
    public CvPoint getCenter() {return center;}

    public int getAge() {return age;}

    public Vector<CvPoint> getTracks() {return tracks;}
    public Scalar getTrackColor() {return trackColor;}

    public CvPoint getCurrentPoint() {return tracks.elementAt(tracks.size() - 1);}
    public CvPoint getLastPoint() {return tracks.elementAt(tracks.size() - 2);}

    // Methods
    public void updateCoords(CvPoint newCenter) {
        center = newCenter;

        tracks.add(newCenter);

        if(tracks.size() > MAX_TRACK_LENGTH) {

            // Remove oldest track record
            tracks.remove(0);
        }

        age = 0;

        return;
    }
    
    public void updateAge() {
        age++;
        return;
    }
}
