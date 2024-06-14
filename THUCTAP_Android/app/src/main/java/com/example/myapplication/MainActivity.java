package com.example.myapplication;

import android.annotation.SuppressLint;
import android.os.Bundle;
import android.widget.ToggleButton;
import android.widget.Button;
import androidx.annotation.NonNull;
import androidx.fragment.app.FragmentActivity;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.OnMapReadyCallback;
import com.google.android.gms.maps.SupportMapFragment;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.MarkerOptions;
import com.google.android.gms.maps.CameraUpdateFactory;

public class MainActivity extends FragmentActivity implements OnMapReadyCallback {

    SupportMapFragment map;
    GoogleMap googleMap;
    Button mapTypeButton;

    @SuppressLint({"WrongViewCast", "MissingInflatedId"})
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);


        map = (SupportMapFragment) getSupportFragmentManager().findFragmentById(R.id.mapView);
        assert map != null;
        map.getMapAsync(this);
        mapTypeButton = findViewById(R.id.mapTypeButton);
        mapTypeButton.setOnClickListener(view -> {
            if (googleMap != null) {
                if (googleMap.getMapType() == GoogleMap.MAP_TYPE_NORMAL) {
                    googleMap.setMapType(GoogleMap.MAP_TYPE_HYBRID);
                } else {
                    googleMap.setMapType(GoogleMap.MAP_TYPE_NORMAL);
                }
            }
        });
    }

    @Override
    public void onMapReady(@NonNull GoogleMap map) {
        googleMap = map;

        // Tạo LatLng object với tọa độ cần hiển thị
        LatLng coordinates = new LatLng(20.767037573744744, 106.09887087663647); // Ví dụ: TP.HCM

        // Thêm marker tại tọa độ
        googleMap.addMarker(new MarkerOptions().position(coordinates).title("My Location"));

        // Di chuyển camera đến tọa độ
        googleMap.moveCamera(CameraUpdateFactory.newLatLngZoom(coordinates, 12.0f));
        googleMap.setMapType(GoogleMap.MAP_TYPE_HYBRID);
    }


    @Override
    public void onResume() {
        super.onResume();
        map.onResume();
    }

    @Override
    public void onPause() {
        super.onPause();
        map.onPause();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        map.onDestroy();
    }

    @Override
    public void onLowMemory() {
        super.onLowMemory();
        map.onLowMemory();
    }
}