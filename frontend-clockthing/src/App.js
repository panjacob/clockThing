import React, { useEffect, useState } from 'react';
import TemperatureWidget from "./components/TemperatureWidget";
import { Container } from "react-bootstrap";
import NavbarMenu from "./components/NavbarMenu";
import Alarms from "./components/Alarms";
import Power from "./components/Power";
import MyChart from './components/MyChart';
import { ThermometerHalf, PieChartFill, Alarm } from 'react-bootstrap-icons';

const btnO = "btn btn-outline-secondary"
const btn = "btn btn-secondary"

function App() {
    const [ menu, setMenu ] = useState(1);

    const menuValues = {
        1: <><Power /><TemperatureWidget /></>,
        2: <MyChart />,
        3: <Alarms />,
    }

    return (
        <Container>
            <NavbarMenu />
            <div class="btn-group w-100 px-0 mx-0 pt-2 mb-5 btn-group-lg" role="group">
                <button type="button" class={menu == 1 ? btn : btnO} onClick={() => setMenu(1)}><ThermometerHalf /></button>
                <button type="button" class={menu == 2 ? btn : btnO} onClick={() => setMenu(2)}><PieChartFill /></button>
                <button type="button" class={menu == 3 ? btn : btnO} onClick={() => setMenu(3)}><Alarm /></button>
            </div>
        {menuValues[menu]}

        

        </Container>
    );
}

export default App;
