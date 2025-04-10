import React, { useEffect, useState } from "react";
import { initializeApp } from "firebase/app";
import {
  getDatabase,
  ref,
  onValue
} from "firebase/database";
import { MapContainer, TileLayer, Marker, Popup } from "react-leaflet";
import "leaflet/dist/leaflet.css";
import L from "leaflet";

// Fix missing marker icons in Leaflet
delete L.Icon.Default.prototype._getIconUrl;
L.Icon.Default.mergeOptions({
  iconRetinaUrl: "https://unpkg.com/leaflet@1.9.4/dist/images/marker-icon-2x.png",
  iconUrl: "https://unpkg.com/leaflet@1.9.4/dist/images/marker-icon.png",
  shadowUrl: "https://unpkg.com/leaflet@1.9.4/dist/images/marker-shadow.png"
});

// Firebase config
const firebaseConfig = {
  apiKey: "AIzaSyA99e-vzF-T536HYE-T7mehji30e5By8UM",
  authDomain: "neo6mtesting123.firebaseapp.com",
  databaseURL: "https://neo6mtesting123-default-rtdb.asia-southeast1.firebasedatabase.app",
  projectId: "neo6mtesting123",
  storageBucket: "neo6mtesting123.appspot.com",
  messagingSenderId: "574805192263",
  appId: "1:574805192263:web:cb88dd7c9f3682b3f18b62"
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);
const database = getDatabase(app);

const SmartMetalDetector = () => {
  const [location, setLocation] = useState({ latitude: 0, longitude: 0 });

  useEffect(() => {
    const locationRef = ref(database, "location");

    const unsubscribe = onValue(locationRef, (snapshot) => {
      const data = snapshot.val();
      if (data && data.latitude && data.longitude) {
        setLocation({
          latitude: data.latitude,
          longitude: data.longitude
        });
      }
    });

    return () => unsubscribe();
  }, []);

  const backgroundImage = "/cprime-myths-about-embedded-systems-development.jpeg";

  return (
    <div
      style={{
        padding: "2rem",
        fontFamily: "Arial, sans-serif",
        backgroundImage: `url(${backgroundImage})`,
        backgroundSize: "cover",
        backgroundPosition: "center",
        minHeight: "100vh",
        width: "100vw",
      }}
    >
      <h1>Smart Metal Detector</h1>
      <p><strong>Latitude:</strong> {location.latitude}</p>
      <p><strong>Longitude:</strong> {location.longitude}</p>

      <MapContainer
        center={[location.latitude, location.longitude]}
        zoom={20}
        style={{ height: "400px", width: "100%", marginTop: "1rem" }}
      >
        <TileLayer
          attribution='Tiles &copy; Esri &mdash; Source: Esri, i-cubed, USDA, USGS, AEX, GeoEye, Getmapping, Aerogrid, IGN, IGP, UPR-EGP, and the GIS User Community'
          url="https://server.arcgisonline.com/ArcGIS/rest/services/World_Imagery/MapServer/tile/{z}/{y}/{x}"
        />
        <Marker position={[location.latitude, location.longitude]}>
          <Popup>Metal Detected Location</Popup>
        </Marker>
      </MapContainer>
    </div>
  );
};

export default SmartMetalDetector;
