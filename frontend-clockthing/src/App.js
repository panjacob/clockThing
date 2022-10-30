import React from 'react';
import TemperatureWidget from "./components/TemperatureWidget";
import { Container } from "react-bootstrap";
import NavbarMenu from "./components/NavbarMenu";
import Alarms from "./components/Alarms";
import Power from "./components/Power";
import MyChart from './components/MyChart';

function App() {
    return (
        <Container>
            <NavbarMenu />
            <Power />
            <TemperatureWidget />
            <MyChart />
            <Alarms />
        </Container>
    );
}

export default App;
