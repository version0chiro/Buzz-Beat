package com.example.neosensespo2draft;

import androidx.appcompat.app.AppCompatActivity;

import android.Manifest;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.os.AsyncTask;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import com.neosensory.neosensoryblessed.NeosensoryBlessed;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.Array;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.Arrays;

import javax.net.ssl.HttpsURLConnection;

import pl.droidsonroids.gif.GifImageView;

public class MainActivity extends AppCompatActivity {
    private final String TAG = MainActivity.class.getSimpleName();

//    spo2 - 80 to 100
//    HR - 60-160
//    HRV - 0-120 ms
//    temp - 28-43 degC

    //Ranges for all differnet motors
    int spo2Start = 80;
    int spo2End=100;

    int hrStart = 60;
    int hrEnd =160;

    int hrvStart=20;
    int hrvEnd = 120;

    int tempStart = 28;

    int tempEnd = 43;

    // range for motor
    int startY = 40;
    int endY = 255;

    // UI components
    //before connect
    Button connect;
    TextView instruction;
    //after connect
    Button disconnect,start;
    TextView label1,label2,label3,label4,cliOutput,label5;
    TextView spooTag,temperatureTag,heartrateTag,hrvTag,trueInsightTag;
    GifImageView gif;
    // Constants
    private static final int ACCESS_LOCATION_REQUEST = 2;
    private static final int NUM_MOTORS = 4;

    //variables for motor roations
    int spooInt,tempInt,hrvInt,hrInt;

    // Access the library to leverage the Neosensory API
    private NeosensoryBlessed blessedNeo = null;


    // Variable to track whether or not the wristband should be vibrating
    private static boolean vibrating = false;
    private static boolean disconnectRequested =
            false; // used for requesting a disconnect within our thread
    Runnable vibratingPattern;
    Thread vibratingPatternThread;

    // class for downloading the json data from the node.js server
    public class  WeatherDownloader extends AsyncTask<String,Void,String>{

        @Override
        protected void onPostExecute(String s) {
            super.onPostExecute(s);
            try{
                JSONObject jsonObject = new JSONObject(s);
                String spo2 = jsonObject.getString("spo2");
                spooTag.setText(spo2);
                spooInt = (int) Float.parseFloat(spo2);
                String hr = jsonObject.getString("hr");
                heartrateTag.setText(hr);
                hrInt= (int) Float.parseFloat(hr);
                String temperature = jsonObject.getString("temperature");
                temperatureTag.setText(temperature);
                tempInt = (int)Float.parseFloat(temperature);
                String insight = jsonObject.getString("hrv");
                hrvTag.setText(insight);
                hrvInt = (int) Float.parseFloat(insight);
                String trueInsight = jsonObject.getString("insight");
                trueInsightTag.setText(trueInsight);


            } catch (JSONException e) {
                e.printStackTrace();
            }
        }

        @Override
        protected String doInBackground(String... strings) {
            String results="";
            URL url;

            HttpsURLConnection urlConnection = null;

            try {
                url = new URL(strings[0]);

                urlConnection = (HttpsURLConnection) url.openConnection();

                InputStream reader = urlConnection.getInputStream();

                int data = reader.read();

                while(data!=-1){
                    char current = (char)data;
                    results += current;
                    data = reader.read();

                }
                return results;
            } catch (MalformedURLException e) {
                e.printStackTrace();
            } catch (IOException e) {
                e.printStackTrace();
            }
            return null;
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        //preconnect ui
        connect = (Button) findViewById(R.id.connectButton);
        instruction = (TextView) findViewById(R.id.instruction);
        //cli
        cliOutput = (TextView) findViewById(R.id.cli_response);
        //labels
        label1 = (TextView) findViewById(R.id.label1);
        label2=(TextView) findViewById(R.id.label2);
        label3=(TextView) findViewById(R.id.label3);
        label4 =(TextView) findViewById(R.id.label4);
        label5 =(TextView) findViewById(R.id.label5);
        //textViews
        spooTag = (TextView) findViewById(R.id.spooTag);
        heartrateTag = (TextView) findViewById(R.id.heartrateTag);
        temperatureTag = (TextView) findViewById(R.id.temperatureTag);
        hrvTag = (TextView) findViewById(R.id.insightTag);
        trueInsightTag = (TextView)findViewById(R.id.trueInsightTag);
        //button post connect
        disconnect = (Button) findViewById(R.id.button2);
        start = (Button) findViewById(R.id.button3);

        //gif
        gif = (GifImageView)findViewById(R.id.gif);

        displayInitialUI();

        NeosensoryBlessed.requestBluetoothOn(this);

        if(checkLocationPermissions()){
            displayInitConnectButton();
        }


        vibratingPattern= new VibratingPattern();
    }

    public double round(double d){
        return Math.floor(d+0.5);
    }

    public  int mapCustom(int yStart,int yend,int inputEnd,int input,int inputStart){
//        double slope= 1.0* (yend-yStart)/(inputEnd-inputStart);
//        double output = yStart + round(slope*(input-inputStart));
        int input_range = inputEnd-inputStart;
        int output_range = yend-yStart;
        int output = (input-inputStart)*output_range/input_range + yStart;
        if (output<0){
            return 0;
        }
        else if(output>yend){
            return yend;
        }
        else{
            return (int) output;
        }
    }
    class VibratingPattern implements Runnable {
        private int minVibration = 40;
        private int currentVibration = minVibration;

        public void run() {

            // loop until the thread is interrupted
            int motorID = 0;
            while (!Thread.currentThread().isInterrupted() && vibrating) {
                try {
                    Log.i("Vibration", String.valueOf(currentVibration) + "in motor "+String.valueOf(motorID));
                    Log.i("Tag",String.valueOf(motorID));
                    Log.i("max amp", String.valueOf(NeosensoryBlessed.MAX_VIBRATION_AMP));

                    Thread.sleep(1000);
                    WeatherDownloader task = new WeatherDownloader();
                    task.execute("https://spo2-registration.herokuapp.com/neoSenseSender");
                    int spo2M=mapCustom(minVibration,endY,spo2End,spooInt,spo2Start);
                    int hrvM=mapCustom(minVibration,endY,hrvEnd,hrvInt,hrvStart);
                    int tempM=mapCustom(minVibration,endY,tempEnd,tempInt,tempStart);
                    int hrM=mapCustom(minVibration,endY,hrEnd,hrInt,hrStart);

                    int[] motorPattern = new int[]{spo2M,hrvM, tempM,hrM};
//                    motorPattern[motorID] = currentVibration;
                    blessedNeo.vibrateMotors(motorPattern);
                    Log.i("array", Arrays.toString(motorPattern));
                    if (currentVibration == 0) {
                        currentVibration = minVibration;
                    }
                } catch (InterruptedException e) {
                    blessedNeo.stopMotors();
                    blessedNeo.resumeDeviceAlgorithm();
                    Log.i(TAG, "Interrupted thread");
                    e.printStackTrace();
                }
            }
            if (disconnectRequested) {
                Log.i(TAG, "Disconnect requested while thread active");
                blessedNeo.stopMotors();
                blessedNeo.resumeDeviceAlgorithm();
                // When disconnecting: it is possible for the device to process the disconnection request
                // prior to processing the request to resume the onboard algorithm, which causes the last
                // sent motor command to "stick"
                try {
                    Thread.sleep(200);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                blessedNeo.disconnectNeoDevice();
                disconnectRequested = false;
            }
        }
    }

    //////////////////////////
    // Cleanup on shutdown //
    /////////////////////////

    @Override
    protected void onDestroy() {
        super.onDestroy();
        unregisterReceiver(BlessedReceiver);
        if (vibrating) {
            vibrating = false;
            disconnectRequested = true;
        }
        blessedNeo = null;
        vibratingPatternThread = null;
    }

    ////////////////////////////////////
    // SDK state change functionality //
    ////////////////////////////////////

    private final BroadcastReceiver BlessedReceiver =
            new BroadcastReceiver() {
                @Override
                public void onReceive(Context context, Intent intent) {
                    if (intent.hasExtra("com.neosensory.neosensoryblessed.CliReadiness")) {
                        // Check the message from NeosensoryBlessed to see if a Neosensory Command Line
                        // Interface
                        // has become ready to accept commands
                        // Prior to calling other API commands we need to accept the Neosensory API ToS
                        if (intent.getBooleanExtra("com.neosensory.neosensoryblessed.CliReadiness", false)) {
                            // request developer level access to the connected Neosensory device
                            blessedNeo.sendDeveloperAPIAuth();
                            // sendDeveloperAPIAuth() will then transmit a message back requiring an explicit
                            // acceptance of Neosensory's Terms of Service located at
                            // https://neosensory.com/legal/dev-terms-service/
                            blessedNeo.acceptApiTerms();
                            Log.i(TAG, String.format("state message: %s", blessedNeo.getNeoCliResponse()));
                            // Assuming successful authorization, set up a button to run the vibrating pattern
                            // thread above
                            displayVibrateButton();
                            displayDisconnectUI();
                        } else {
                            displayReconnectUI();
                        }
                    }

                    if (intent.hasExtra("com.neosensory.neosensoryblessed.CliMessage")) {
                        String notification_value =
                                intent.getStringExtra("com.neosensory.neosensoryblessed.CliMessage");
                        cliOutput.setText(notification_value);
                    }

                    if (intent.hasExtra("com.neosensory.neosensoryblessed.ConnectedState")) {
                        if (intent.getBooleanExtra("com.neosensory.neosensoryblessed.ConnectedState", false)) {
                            Log.i(TAG, "Connected to Buzz");
                        } else {
                            Log.i(TAG, "Disconnected from Buzz");
                        }
                    }
                }
            };



    public void displayVibrateButton() {
        connect.setVisibility(View.GONE);
        cliOutput.setVisibility(View.VISIBLE);
        gif.setVisibility(View.VISIBLE);
        label1.setVisibility(View.VISIBLE);
        label2.setVisibility(View.VISIBLE);
        label3.setVisibility(View.VISIBLE);
        label4.setVisibility(View.VISIBLE);
        label5.setVisibility(View.VISIBLE);
        spooTag.setVisibility(View.VISIBLE);
        heartrateTag.setVisibility(View.VISIBLE);
        hrvTag.setVisibility(View.VISIBLE);
        temperatureTag.setVisibility(View.VISIBLE);
        trueInsightTag.setVisibility(View.VISIBLE);
        instruction.setVisibility(View.GONE);
    }

    private void displayDisconnectUI() {
        disconnect.setVisibility(View.VISIBLE);
        disconnect.setClickable(true);
        start.setVisibility(View.VISIBLE);
        disconnect.setOnClickListener(
                new View.OnClickListener() {
                    public void onClick(View v) {
                        if (!vibrating) {
                            blessedNeo.disconnectNeoDevice();
                        } else {
                            // If motors are vibrating (in the VibratingPattern thread in this case) and we want
                            // to stop them on disconnect, we need to add a sleep/delay as it's possible for the
                            // disconnect to be processed prior to stopping the motors. See the VibratingPattern
                            // definition.
                            disconnectRequested = true;
                            vibrating = false;
                        }
                    }
                });
    }

    private void displayReconnectUI() {
        cliOutput.setVisibility(View.GONE);
        gif.setVisibility(View.GONE);
        label1.setVisibility(View.GONE);
        label2.setVisibility(View.GONE);
        label3.setVisibility(View.GONE);
        label4.setVisibility(View.GONE);
        label5.setVisibility(View.GONE);
        spooTag.setVisibility(View.GONE);
        heartrateTag.setVisibility(View.GONE);
        hrvTag.setVisibility(View.GONE);
        temperatureTag.setVisibility(View.GONE);
        trueInsightTag.setVisibility(View.GONE);

        disconnect.setVisibility(View.GONE);
        start.setVisibility(View.GONE);
        connect.setVisibility(View.VISIBLE);
        instruction.setVisibility(View.VISIBLE);
        connect.setOnClickListener(
                new View.OnClickListener() {
                    public void onClick(View v) {
                        blessedNeo.attemptNeoReconnect();
                        toastMessage("Attempting to reconnect. This may take a few seconds.");
                    }
                });
    }

    private void displayInitConnectButton() {
        connect.setVisibility(View.VISIBLE);
        connect.setClickable(true);
        connect.setOnClickListener(
                new View.OnClickListener() {
                    public void onClick(View v) {
                        initBluetoothHandler();
                    }
                });
    }


    private void displayInitialUI() {
        displayReconnectUI();
        start.setOnClickListener(
                new View.OnClickListener() {
                    public void onClick(View v) {
                        if (!vibrating) {
                            blessedNeo.pauseDeviceAlgorithm();
                            start.setText("Stop");
                            vibrating = true;
                            // run the vibrating pattern loop
                            vibratingPatternThread = new Thread(vibratingPattern);
                            vibratingPatternThread.start();
                        } else {
                            start.setText("Start");
                            vibrating = false;
                            blessedNeo.resumeDeviceAlgorithm();
                        }
                    }
                }
        );
    }


    private void toastMessage(String s) {
        Toast.makeText(this, s, Toast.LENGTH_SHORT).show();
    }

    //////////////////////////////////////////////
    // Bluetooth and permissions initialization //
    //////////////////////////////////////////////

    private void initBluetoothHandler() {
        // Create an instance of the Bluetooth handler. This uses the constructor that will search for
        // and connect to the first available device with "Buzz" in its name. To connect to a specific
        // device with a specific address, you can use the following pattern:  blessedNeo =
        // NeosensoryBlessed.getInstance(getApplicationContext(), <address> e.g."EB:CA:85:38:19:1D",
        // false);
        blessedNeo =
                NeosensoryBlessed.getInstance(getApplicationContext(), new String[] {"Buzz"}, false);
        // register receivers so that NeosensoryBlessed can pass relevant messages and state changes to MainActivity
        registerReceiver(BlessedReceiver, new IntentFilter("BlessedBroadcast"));
    }


    private boolean checkLocationPermissions() {
        int targetSdkVersion = getApplicationInfo().targetSdkVersion;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q
                && targetSdkVersion >= Build.VERSION_CODES.Q) {
            if (getApplicationContext().checkSelfPermission(Manifest.permission.ACCESS_FINE_LOCATION)
                    != PackageManager.PERMISSION_GRANTED) {
                requestPermissions(
                        new String[] {Manifest.permission.ACCESS_FINE_LOCATION}, ACCESS_LOCATION_REQUEST);
                return false;
            } else {
                return true;
            }
        } else {
            if (getApplicationContext().checkSelfPermission(Manifest.permission.ACCESS_COARSE_LOCATION)
                    != PackageManager.PERMISSION_GRANTED) {
                requestPermissions(
                        new String[] {Manifest.permission.ACCESS_COARSE_LOCATION}, ACCESS_LOCATION_REQUEST);
                return false;
            } else {
                return true;
            }
        }
    }

}


