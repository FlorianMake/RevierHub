package com.revierhub.app;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;

public final class OrientationProvider implements SensorEventListener {
    private static OrientationProvider instance;

    private final SensorManager sensorManager;
    private final Sensor rotationSensor;

    private OrientationProvider(Context context) {
        sensorManager = (SensorManager) context.getSystemService(Context.SENSOR_SERVICE);
        rotationSensor = sensorManager.getDefaultSensor(Sensor.TYPE_ROTATION_VECTOR);
    }

    public static void start(Context context) {
        if (instance != null) {
            return;
        }

        instance = new OrientationProvider(context.getApplicationContext());
        if (instance.rotationSensor != null) {
            instance.sensorManager.registerListener(
                instance,
                instance.rotationSensor,
                SensorManager.SENSOR_DELAY_UI);
        }
    }

    @Override
    public void onSensorChanged(SensorEvent event) {
        float[] rotationMatrix = new float[9];
        float[] orientation = new float[3];
        SensorManager.getRotationMatrixFromVector(rotationMatrix, event.values);
        SensorManager.getOrientation(rotationMatrix, orientation);

        double heading = Math.toDegrees(orientation[0]);
        if (heading < 0) {
            heading += 360;
        }
        onHeadingChanged(heading);
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {
    }

    private static native void onHeadingChanged(double heading);
}
