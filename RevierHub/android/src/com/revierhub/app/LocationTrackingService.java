package com.revierhub.app;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.Build;
import android.os.IBinder;

import androidx.core.content.ContextCompat;

public class LocationTrackingService extends Service {
    private static final String CHANNEL_ID = "location_tracking";
    private static final int NOTIFICATION_ID = 1;

    public static void start(Context context) {
        Intent intent = new Intent(context, LocationTrackingService.class);
        ContextCompat.startForegroundService(context, intent);
    }

    @Override
    public void onCreate() {
        super.onCreate();
        createNotificationChannel();
        startForeground(NOTIFICATION_ID, createNotification());
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        return START_NOT_STICKY;
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    private void createNotificationChannel() {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.O) {
            return;
        }

        NotificationChannel channel = new NotificationChannel(
            CHANNEL_ID,
            "Location tracking",
            NotificationManager.IMPORTANCE_LOW);
        channel.setDescription("Keeps trail recording active while RevierHub is in the background.");

        NotificationManager notificationManager =
            getSystemService(NotificationManager.class);
        notificationManager.createNotificationChannel(channel);
    }

    private Notification createNotification() {
        return new Notification.Builder(this, CHANNEL_ID)
            .setContentTitle("RevierHub is recording your trail")
            .setContentText("Location tracking remains active in the background.")
            .setSmallIcon(android.R.drawable.ic_menu_mylocation)
            .setOngoing(true)
            .build();
    }
}
